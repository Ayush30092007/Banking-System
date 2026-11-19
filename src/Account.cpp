#include "Account.h"
#include "SavingsAccount.h"
#include "CurrentAccount.h"
#include <cmath>
#include <iomanip>

Account::Account(int accNo, Customer holder, std::string pin, double balance)
    : accNo_(accNo), holder_(std::move(holder)), pin_(std::move(pin)), balance_(balance) {}

void Account::record(const std::string& type, double amount, const std::string& note) {
    history_.emplace_back(type, amount, balance_, note);
}

void Account::deposit(double amount, const std::string& note) {
    if (amount <= 0) throw InvalidAmountException();
    balance_ += amount;
    record("Deposit", amount, note);
}

void Account::withdraw(double amount, const std::string& note) {
    if (amount <= 0) throw InvalidAmountException();
    // availableToWithdraw() is virtual -> each account type applies its own rule
    if (amount > availableToWithdraw() + 1e-9)
        throw InsufficientFundsException("You can withdraw up to " +
                                         Util::money(availableToWithdraw()) + ".");
    balance_ -= amount;
    record("Withdraw", amount, note);
}

double Account::applyMonthEnd() {
    double adj = std::round(monthEndAdjustment() * 100.0) / 100.0;
    if (adj > 0) {
        balance_ += adj;
        record("Interest", adj, "Monthly interest credited");
    } else if (adj < 0) {
        balance_ += adj;
        record("Overdraft Fee", -adj, "Overdraft interest charged");
    }
    return adj;
}

std::ostream& operator<<(std::ostream& os, const Account& a) {
    os << "[" << a.accNo_ << "] " << a.getType() << " | " << a.holder_.getName()
       << " | Balance: " << Util::money(a.balance_);
    return os;
}

std::unique_ptr<Account> createAccount(const std::string& type, int accNo,
                                       Customer holder, const std::string& pin,
                                       double balance) {
    if (type == "Savings")
        return std::make_unique<SavingsAccount>(accNo, std::move(holder), pin, balance);
    if (type == "Current")
        return std::make_unique<CurrentAccount>(accNo, std::move(holder), pin, balance);
    throw BankException("Unknown account type: " + type);
}
