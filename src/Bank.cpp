#include "Bank.h"
#include "FileManager.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
#include <vector>

Bank::Bank(std::string dataFile) : dataFile_(std::move(dataFile)) {
    try {
        if (FileManager::load(dataFile_, accounts_, nextAccNo_))
            std::cout << "Loaded " << accounts_.size() << " account(s) from " << dataFile_ << ".\n";
        else
            std::cout << "No data file found. Starting with an empty bank.\n";
    } catch (const BankException& e) {
        std::cout << "WARNING: " << e.what() << " Loaded records so far are kept.\n";
    }
}

void Bank::save() const { FileManager::save(dataFile_, accounts_, nextAccNo_); }

// ---------------- CREATE ----------------
int Bank::openAccount(const std::string& type, const Customer& holder,
                      const std::string& pin, double initialDeposit) {
    if (initialDeposit < Util::MIN_OPENING_DEPOSIT)
        throw BankException("Opening deposit must be at least " +
                            Util::money(Util::MIN_OPENING_DEPOSIT) + ".");

    bool duplicate = std::any_of(accounts_.begin(), accounts_.end(), [&](const auto& a) {
        return a->getHolder().getIC() == holder.getIC() && a->getType() == type;
    });
    if (duplicate) throw DuplicateAccountException();

    int accNo = nextAccNo_++;
    auto acc = createAccount(type, accNo, holder, pin, 0.0);
    acc->deposit(initialDeposit, "Initial deposit");
    accounts_.push_back(std::move(acc));
    save();
    return accNo;
}

// ---------------- READ ----------------
Account* Bank::findAccount(int accNo) {
    auto it = std::find_if(accounts_.begin(), accounts_.end(),
                           [accNo](const auto& a) { return a->getAccNo() == accNo; });
    return it == accounts_.end() ? nullptr : it->get();
}

const Account* Bank::findAccount(int accNo) const {
    auto it = std::find_if(accounts_.begin(), accounts_.end(),
                           [accNo](const auto& a) { return a->getAccNo() == accNo; });
    return it == accounts_.end() ? nullptr : it->get();
}

Account& Bank::getAccount(int accNo) {
    Account* a = findAccount(accNo);
    if (!a) throw AccountNotFoundException(accNo);
    return *a;
}

std::vector<const Account*> Bank::searchByName(const std::string& keyword) const {
    std::vector<const Account*> result;
    std::string key = Util::toLower(keyword);
    for (const auto& a : accounts_)
        if (Util::toLower(a->getHolder().getName()).find(key) != std::string::npos)
            result.push_back(a.get());
    return result;
}

std::vector<const Account*> Bank::listSorted(bool byBalance) const {
    std::vector<const Account*> list;
    for (const auto& a : accounts_) list.push_back(a.get());
    if (byBalance)
        std::sort(list.begin(), list.end(),
                  [](const Account* x, const Account* y) { return *y < *x; });  // uses operator<
    else
        std::sort(list.begin(), list.end(), [](const Account* x, const Account* y) {
            return Util::toLower(x->getHolder().getName()) < Util::toLower(y->getHolder().getName());
        });
    return list;
}

// ---------------- UPDATE ----------------
void Bank::updateHolder(int accNo, const std::string& name, const std::string& phone) {
    Account& a = getAccount(accNo);
    a.getHolder().setName(name);
    a.getHolder().setPhone(phone);
    save();
}

void Bank::changePin(int accNo, const std::string& newPin) {
    getAccount(accNo).changePin(newPin);
    save();
}

// ---------------- DELETE ----------------
double Bank::closeAccount(int accNo) {
    auto it = std::find_if(accounts_.begin(), accounts_.end(),
                           [accNo](const auto& a) { return a->getAccNo() == accNo; });
    if (it == accounts_.end()) throw AccountNotFoundException(accNo);
    double balance = (*it)->getBalance();
    if (balance < 0)
        throw BankException("Cannot close an overdrawn account. Customer owes " +
                            Util::money(-balance) + ".");
    accounts_.erase(it);
    save();
    return balance;
}

// ---------------- TRANSACTIONS ----------------
void Bank::deposit(int accNo, double amount) {
    getAccount(accNo).deposit(amount);
    save();
}

void Bank::withdraw(int accNo, double amount) {
    getAccount(accNo).withdraw(amount);
    save();
}

void Bank::transfer(int fromNo, int toNo, double amount) {
    if (fromNo == toNo) throw BankException("Cannot transfer to the same account.");
    Account& from = getAccount(fromNo);
    Account& to = getAccount(toNo);
    from.withdraw(amount, "Transfer to " + std::to_string(toNo));   // throws if not allowed
    to.deposit(amount, "Transfer from " + std::to_string(fromNo));
    save();
}

double Bank::applyMonthEnd() {
    double total = 0;
    for (auto& a : accounts_) total += a->applyMonthEnd();   // polymorphic call
    save();
    return total;
}

// ---------------- REPORT ----------------
std::string Bank::report() const {
    std::ostringstream os;
    double total = std::accumulate(accounts_.begin(), accounts_.end(), 0.0,
                                   [](double sum, const auto& a) { return sum + a->getBalance(); });
    auto countType = [this](const std::string& t) {
        return std::count_if(accounts_.begin(), accounts_.end(),
                             [&](const auto& a) { return a->getType() == t; });
    };
    os << "Total accounts   : " << accounts_.size() << "\n"
       << "  Savings        : " << countType("Savings") << "\n"
       << "  Current        : " << countType("Current") << "\n"
       << "Total balance    : " << Util::money(total) << "\n";
    if (!accounts_.empty()) {
        auto richest = std::max_element(accounts_.begin(), accounts_.end(),
                                        [](const auto& x, const auto& y) { return *x < *y; });
        os << "Highest balance  : " << **richest << "\n";
    }
    return os.str();
}
