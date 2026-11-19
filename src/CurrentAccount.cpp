#include "CurrentAccount.h"

double CurrentAccount::availableToWithdraw() const {
    return getBalance() + OVERDRAFT_LIMIT;
}

double CurrentAccount::monthEndAdjustment() const {
    return getBalance() < 0 ? getBalance() * OVERDRAFT_RATE / 12.0 : 0.0;
}

std::string CurrentAccount::policySummary() const {
    return "No interest, overdraft up to " + Util::money(OVERDRAFT_LIMIT) +
           " (12% p.a. charge)";
}
