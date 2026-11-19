#pragma once
#include <memory>
#include <ostream>
#include <string>
#include <vector>
#include "BankException.h"
#include "Customer.h"
#include "Transaction.h"

// ABSTRACT base class: cannot be instantiated. Defines the interface that
// every account type must follow (abstraction + polymorphism).
class Account {
private:
    int accNo_;
    Customer holder_;          // composition: an Account HAS-A Customer
    std::string pin_;
    double balance_;
    std::vector<Transaction> history_;   // composition: an Account HAS Transactions

    void record(const std::string& type, double amount, const std::string& note);

public:
    Account(int accNo, Customer holder, std::string pin, double balance = 0.0);
    virtual ~Account() = default;

    // ---- Pure virtual functions (each account type decides its own rules) ----
    virtual std::string getType() const = 0;
    virtual double availableToWithdraw() const = 0;
    virtual double monthEndAdjustment() const = 0;   // +interest or -charge
    virtual std::string policySummary() const = 0;

    // ---- Shared behaviour ----
    void deposit(double amount, const std::string& note = "Cash deposit");
    void withdraw(double amount, const std::string& note = "Cash withdrawal");
    double applyMonthEnd();

    bool verifyPin(const std::string& attempt) const { return pin_ == attempt; }
    void changePin(const std::string& newPin) { pin_ = newPin; }

    int getAccNo() const { return accNo_; }
    double getBalance() const { return balance_; }
    const Customer& getHolder() const { return holder_; }
    Customer& getHolder() { return holder_; }
    const std::string& getPin() const { return pin_; }
    const std::vector<Transaction>& getHistory() const { return history_; }
    void addHistory(Transaction t) { history_.push_back(std::move(t)); }  // used when loading

    // Operator overloading: compare accounts by balance, print with <<
    bool operator<(const Account& other) const { return balance_ < other.balance_; }
    friend std::ostream& operator<<(std::ostream& os, const Account& a);
};

// Factory function: builds the right derived object from its type name.
std::unique_ptr<Account> createAccount(const std::string& type, int accNo,
                                       Customer holder, const std::string& pin,
                                       double balance = 0.0);
