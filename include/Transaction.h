#pragma once
#include <iomanip>
#include <ostream>
#include <string>
#include "Utils.h"

// One line in an account's history.
class Transaction {
private:
    std::string type_;
    double amount_;
    double balanceAfter_;
    std::string note_;
    std::string timestamp_;

public:
    Transaction(std::string type, double amount, double balanceAfter,
                std::string note, std::string timestamp = Util::timestamp())
        : type_(std::move(type)), amount_(amount), balanceAfter_(balanceAfter),
          note_(std::move(note)), timestamp_(std::move(timestamp)) {}

    const std::string& getType() const { return type_; }
    double getAmount() const { return amount_; }
    double getBalanceAfter() const { return balanceAfter_; }
    const std::string& getNote() const { return note_; }
    const std::string& getTimestamp() const { return timestamp_; }

    bool isCredit() const { return type_ == "Deposit" || type_ == "Interest"; }

    std::string serialize() const {
        return "TXN|" + type_ + "|" + Util::fixed(amount_) + "|" +
               Util::fixed(balanceAfter_) + "|" + note_ + "|" + timestamp_;
    }

    // Operator overloading: print a transaction with <<
    friend std::ostream& operator<<(std::ostream& os, const Transaction& t) {
        os << t.timestamp_ << "  " << std::left << std::setw(14) << t.type_
           << std::right << (t.isCredit() ? "+" : "-") << std::setw(10) << Util::fixed(t.amount_)
           << "  Bal: " << std::setw(10) << Util::fixed(t.balanceAfter_)
           << "  " << t.note_;
        return os;
    }
};
