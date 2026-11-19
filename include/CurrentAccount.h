#pragma once
#include "Account.h"

// Current IS-A Account: allows an overdraft, charges interest on it.
class CurrentAccount : public Account {
public:
    static constexpr double OVERDRAFT_LIMIT = 1000.0;
    static constexpr double OVERDRAFT_RATE = 0.12;   // 12% per year

    using Account::Account;

    std::string getType() const override { return "Current"; }
    double availableToWithdraw() const override;
    double monthEndAdjustment() const override;
    std::string policySummary() const override;
};
