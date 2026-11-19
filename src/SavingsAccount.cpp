#include "SavingsAccount.h"
#include <algorithm>

double SavingsAccount::availableToWithdraw() const {
    return std::max(0.0, getBalance() - MIN_BALANCE);
}

double SavingsAccount::monthEndAdjustment() const {
    return getBalance() * INTEREST_RATE / 12.0;
}

std::string SavingsAccount::policySummary() const {
    return "2.5% p.a. interest, minimum balance " + Util::money(MIN_BALANCE);
}
