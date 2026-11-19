#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Account.h"

// Manages all accounts. Provides the CRUD operations.
class Bank {
private:
    std::vector<std::unique_ptr<Account>> accounts_;   // smart pointers => polymorphic storage
    int nextAccNo_ = 1001;
    std::string dataFile_;

    void save() const;

public:
    explicit Bank(std::string dataFile);

    // CREATE
    int openAccount(const std::string& type, const Customer& holder,
                    const std::string& pin, double initialDeposit);

    // READ
    Account* findAccount(int accNo);
    const Account* findAccount(int accNo) const;
    Account& getAccount(int accNo);                       // throws if not found
    std::vector<const Account*> searchByName(const std::string& keyword) const;
    std::vector<const Account*> listSorted(bool byBalance) const;
    bool empty() const { return accounts_.empty(); }
    std::size_t count() const { return accounts_.size(); }

    // UPDATE
    void updateHolder(int accNo, const std::string& name, const std::string& phone);
    void changePin(int accNo, const std::string& newPin);

    // DELETE (returns the cash paid out to the customer)
    double closeAccount(int accNo);

    // TRANSACTIONS
    void deposit(int accNo, double amount);
    void withdraw(int accNo, double amount);
    void transfer(int fromNo, int toNo, double amount);
    double applyMonthEnd();      // returns total adjustments applied

    // REPORT
    std::string report() const;
};
