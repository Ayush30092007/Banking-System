#pragma once
#include "Account.h"

// Savings IS-A Account: earns interest, must keep a minimum balance.
class SavingsAccount : public Account {
public:
    static constexpr double INTEREST_RATE = 0.025;   // 2.5% per year
    static constexpr double MIN_BALANCE = 10.0;

    using Account::Account;   // reuse base constructor

    std::string getType() const override { return "Savings"; }
    double availableToWithdraw() const override;
    double monthEndAdjustment() const override;
    std::string policySummary() const override;
};
