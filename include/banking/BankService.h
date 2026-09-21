#ifndef BANKING_BANK_SERVICE_H
#define BANKING_BANK_SERVICE_H

#include "banking/FileStorage.h"

#include <functional>
#include <vector>

namespace banking {

class BankService {
public:
    typedef std::function<void(const ChangeEvent&)> ChangeListener;

    explicit BankService(const FileStorage& storage = FileStorage());

    void reload();
    void addChangeListener(ChangeListener listener);

    std::vector<Customer> listCustomers() const;
    std::vector<Account> listAccounts() const;
    std::vector<Transaction> listTransactions() const;

    Customer createCustomer(const CustomerDraft& draft);
    Customer updateCustomer(const std::string& customerId, const CustomerPatch& patch);

    Account openAccount(const std::string& customerId, AccountType type, Money initialDepositCents);
    Account deposit(const std::string& accountNumber, Money amountCents, const std::string& note);
    Account withdraw(const std::string& accountNumber, Money amountCents, const std::string& note);
    void transfer(const std::string& fromAccountNumber,
                  const std::string& toAccountNumber,
                  Money amountCents,
                  const std::string& note);
    Account closeAccount(const std::string& accountNumber);

    std::vector<Transaction> statementFor(const std::string& accountNumber) const;

private:
    FileStorage storage_;
    DatabaseSnapshot data_;
    std::vector<ChangeListener> listeners_;
    std::uint64_t nextEventSequence_;
    int nextCustomerNumber_;
    int nextAccountNumber_;
    int nextTransactionNumber_;

    void persistAndPublish(const std::string& action,
                           const std::string& entity,
                           const std::string& entityId,
                           const std::string& summary);

    Customer& requireCustomer(const std::string& customerId);
    Account& requireAccount(const std::string& accountNumber);
    const Account& requireAccount(const std::string& accountNumber) const;

    void requirePositiveAmount(Money amountCents) const;
    void requireActive(const Account& account) const;
    void syncCounters();

    std::string generateCustomerId();
    std::string generateAccountNumber();
    std::string generateTransactionId();
    void addTransaction(const std::string& accountNumber,
                        TransactionType type,
                        Money amountCents,
                        const std::string& note,
                        const std::string& relatedAccountNumber);
};

} // namespace banking

#endif // BANKING_BANK_SERVICE_H
