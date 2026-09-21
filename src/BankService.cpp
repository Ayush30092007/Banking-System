#include "banking/BankService.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace banking {

namespace {

int suffixNumber(const std::string& value, char prefix) {
    if (value.size() < 2 || value[0] != prefix) {
        return 0;
    }

    for (std::size_t i = 1; i < value.size(); ++i) {
        if (value[i] < '0' || value[i] > '9') {
            return 0;
        }
    }

    return std::stoi(value.substr(1));
}

int accountSequenceNumber(const std::string& value) {
    if (value.empty()) {
        return 0;
    }

    for (char ch : value) {
        if (ch < '0' || ch > '9') {
            return 0;
        }
    }

    const long long parsed = std::stoll(value);
    const long long base = 1000000000LL;
    if (parsed <= base) {
        return 0;
    }
    return static_cast<int>(parsed - base);
}

void requireNotBlank(const std::string& value, const std::string& fieldName) {
    if (trim(value).empty()) {
        throw std::invalid_argument(fieldName + " is required");
    }
}

std::string moneySummary(Money amountCents) {
    return "RM " + formatMoney(amountCents);
}

} // namespace

BankService::BankService(const FileStorage& storage)
    : storage_(storage),
      nextEventSequence_(1),
      nextCustomerNumber_(1),
      nextAccountNumber_(1),
      nextTransactionNumber_(1) {
    reload();
}

void BankService::reload() {
    data_ = storage_.load();
    nextEventSequence_ = storage_.lastEventSequence() + 1;
    syncCounters();
}

void BankService::addChangeListener(ChangeListener listener) {
    listeners_.push_back(listener);
}

std::vector<Customer> BankService::listCustomers() const {
    return data_.customers;
}

std::vector<Account> BankService::listAccounts() const {
    return data_.accounts;
}

std::vector<Transaction> BankService::listTransactions() const {
    return data_.transactions;
}

Customer BankService::createCustomer(const CustomerDraft& draft) {
    Customer customer;
    customer.id = generateCustomerId();
    customer.fullName = trim(draft.fullName);
    customer.email = trim(draft.email);
    customer.phone = trim(draft.phone);
    customer.address = trim(draft.address);
    customer.createdAt = nowIso8601();
    customer.updatedAt = customer.createdAt;

    requireNotBlank(customer.fullName, "Customer name");
    requireNotBlank(customer.phone, "Phone number");

    data_.customers.push_back(customer);
    persistAndPublish("created", "customer", customer.id, "Customer " + customer.fullName + " created");
    return customer;
}

Customer BankService::updateCustomer(const std::string& customerId, const CustomerPatch& patch) {
    Customer& customer = requireCustomer(customerId);

    if (patch.hasFullName) {
        customer.fullName = trim(patch.fullName);
        requireNotBlank(customer.fullName, "Customer name");
    }
    if (patch.hasEmail) {
        customer.email = trim(patch.email);
    }
    if (patch.hasPhone) {
        customer.phone = trim(patch.phone);
        requireNotBlank(customer.phone, "Phone number");
    }
    if (patch.hasAddress) {
        customer.address = trim(patch.address);
    }

    customer.updatedAt = nowIso8601();
    const Customer updated = customer;
    persistAndPublish("updated", "customer", updated.id, "Customer " + updated.fullName + " updated");
    return updated;
}

Account BankService::openAccount(const std::string& customerId, AccountType type, Money initialDepositCents) {
    requireCustomer(customerId);
    if (initialDepositCents < 0) {
        throw std::invalid_argument("Initial deposit cannot be negative");
    }

    Account account;
    account.number = generateAccountNumber();
    account.customerId = customerId;
    account.type = type;
    account.balanceCents = initialDepositCents;
    account.status = AccountStatus::Active;
    account.createdAt = nowIso8601();
    account.updatedAt = account.createdAt;

    data_.accounts.push_back(account);
    addTransaction(account.number,
                   TransactionType::AccountOpened,
                   initialDepositCents,
                   "Account opened",
                   "");

    persistAndPublish("created",
                      "account",
                      account.number,
                      "Account " + account.number + " opened with " + moneySummary(initialDepositCents));
    return account;
}

Account BankService::deposit(const std::string& accountNumber, Money amountCents, const std::string& note) {
    requirePositiveAmount(amountCents);
    Account& account = requireAccount(accountNumber);
    requireActive(account);

    account.balanceCents += amountCents;
    account.updatedAt = nowIso8601();
    addTransaction(account.number, TransactionType::Deposit, amountCents, note, "");

    const Account updated = account;
    persistAndPublish("updated",
                      "account",
                      updated.number,
                      "Deposited " + moneySummary(amountCents) + " into " + updated.number);
    return updated;
}

Account BankService::withdraw(const std::string& accountNumber, Money amountCents, const std::string& note) {
    requirePositiveAmount(amountCents);
    Account& account = requireAccount(accountNumber);
    requireActive(account);

    if (account.balanceCents < amountCents) {
        throw std::runtime_error("Insufficient funds");
    }

    account.balanceCents -= amountCents;
    account.updatedAt = nowIso8601();
    addTransaction(account.number, TransactionType::Withdrawal, amountCents, note, "");

    const Account updated = account;
    persistAndPublish("updated",
                      "account",
                      updated.number,
                      "Withdrew " + moneySummary(amountCents) + " from " + updated.number);
    return updated;
}

void BankService::transfer(const std::string& fromAccountNumber,
                           const std::string& toAccountNumber,
                           Money amountCents,
                           const std::string& note) {
    requirePositiveAmount(amountCents);
    if (fromAccountNumber == toAccountNumber) {
        throw std::invalid_argument("Transfer accounts must be different");
    }

    Account& from = requireAccount(fromAccountNumber);
    Account& to = requireAccount(toAccountNumber);
    requireActive(from);
    requireActive(to);

    if (from.balanceCents < amountCents) {
        throw std::runtime_error("Insufficient funds");
    }

    const std::string timestamp = nowIso8601();
    from.balanceCents -= amountCents;
    to.balanceCents += amountCents;
    from.updatedAt = timestamp;
    to.updatedAt = timestamp;

    addTransaction(from.number, TransactionType::TransferOut, amountCents, note, to.number);
    addTransaction(to.number, TransactionType::TransferIn, amountCents, note, from.number);

    persistAndPublish("updated",
                      "transfer",
                      from.number + "->" + to.number,
                      "Transferred " + moneySummary(amountCents) + " from " + from.number + " to " + to.number);
}

Account BankService::closeAccount(const std::string& accountNumber) {
    Account& account = requireAccount(accountNumber);
    requireActive(account);

    if (account.balanceCents != 0) {
        throw std::runtime_error("Account balance must be zero before closing");
    }

    account.status = AccountStatus::Closed;
    account.updatedAt = nowIso8601();
    addTransaction(account.number, TransactionType::AccountClosed, 0, "Account closed", "");

    const Account updated = account;
    persistAndPublish("closed", "account", updated.number, "Account " + updated.number + " closed");
    return updated;
}

std::vector<Transaction> BankService::statementFor(const std::string& accountNumber) const {
    requireAccount(accountNumber);

    std::vector<Transaction> statement;
    for (const Transaction& transaction : data_.transactions) {
        if (transaction.accountNumber == accountNumber) {
            statement.push_back(transaction);
        }
    }
    return statement;
}

void BankService::persistAndPublish(const std::string& action,
                                    const std::string& entity,
                                    const std::string& entityId,
                                    const std::string& summary) {
    storage_.save(data_);

    ChangeEvent event;
    event.sequence = nextEventSequence_++;
    event.occurredAt = nowIso8601();
    event.action = action;
    event.entity = entity;
    event.entityId = entityId;
    event.summary = summary;

    storage_.appendEvent(event);

    for (ChangeListener& listener : listeners_) {
        listener(event);
    }
}

Customer& BankService::requireCustomer(const std::string& customerId) {
    for (Customer& customer : data_.customers) {
        if (customer.id == customerId) {
            return customer;
        }
    }
    throw std::runtime_error("Customer not found: " + customerId);
}

Account& BankService::requireAccount(const std::string& accountNumber) {
    for (Account& account : data_.accounts) {
        if (account.number == accountNumber) {
            return account;
        }
    }
    throw std::runtime_error("Account not found: " + accountNumber);
}

const Account& BankService::requireAccount(const std::string& accountNumber) const {
    for (const Account& account : data_.accounts) {
        if (account.number == accountNumber) {
            return account;
        }
    }
    throw std::runtime_error("Account not found: " + accountNumber);
}

void BankService::requirePositiveAmount(Money amountCents) const {
    if (amountCents <= 0) {
        throw std::invalid_argument("Amount must be greater than zero");
    }
}

void BankService::requireActive(const Account& account) const {
    if (account.status != AccountStatus::Active) {
        throw std::runtime_error("Account " + account.number + " is not active");
    }
}

void BankService::syncCounters() {
    nextCustomerNumber_ = 1;
    nextAccountNumber_ = 1;
    nextTransactionNumber_ = 1;

    for (const Customer& customer : data_.customers) {
        nextCustomerNumber_ = std::max(nextCustomerNumber_, suffixNumber(customer.id, 'C') + 1);
    }
    for (const Account& account : data_.accounts) {
        nextAccountNumber_ = std::max(nextAccountNumber_, accountSequenceNumber(account.number) + 1);
    }
    for (const Transaction& transaction : data_.transactions) {
        nextTransactionNumber_ = std::max(nextTransactionNumber_, suffixNumber(transaction.id, 'T') + 1);
    }
}

std::string BankService::generateCustomerId() {
    std::ostringstream out;
    out << 'C' << std::setw(6) << std::setfill('0') << nextCustomerNumber_++;
    return out.str();
}

std::string BankService::generateAccountNumber() {
    const long long number = 1000000000LL + nextAccountNumber_++;
    return std::to_string(number);
}

std::string BankService::generateTransactionId() {
    std::ostringstream out;
    out << 'T' << std::setw(8) << std::setfill('0') << nextTransactionNumber_++;
    return out.str();
}

void BankService::addTransaction(const std::string& accountNumber,
                                 TransactionType type,
                                 Money amountCents,
                                 const std::string& note,
                                 const std::string& relatedAccountNumber) {
    Transaction transaction;
    transaction.id = generateTransactionId();
    transaction.accountNumber = accountNumber;
    transaction.type = type;
    transaction.amountCents = amountCents;
    transaction.occurredAt = nowIso8601();
    transaction.note = trim(note);
    transaction.relatedAccountNumber = relatedAccountNumber;

    data_.transactions.push_back(transaction);
}

} // namespace banking
