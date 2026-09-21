#include "banking/Models.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace banking {

namespace {

std::string toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

bool allDigits(const std::string& value) {
    for (char ch : value) {
        if (!std::isdigit(static_cast<unsigned char>(ch))) {
            return false;
        }
    }
    return !value.empty();
}

} // namespace

std::string nowIso8601() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t rawTime = std::chrono::system_clock::to_time_t(now);
    std::tm utcTime;

#ifdef _MSC_VER
    gmtime_s(&utcTime, &rawTime);
#elif defined(_WIN32)
    const std::tm* converted = std::gmtime(&rawTime);
    if (converted == nullptr) {
        throw std::runtime_error("Unable to format current time");
    }
    utcTime = *converted;
#else
    gmtime_r(&rawTime, &utcTime);
#endif

    std::ostringstream out;
    out << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%SZ");
    return out.str();
}

std::string trim(const std::string& value) {
    std::string::size_type first = 0;
    while (first < value.size() && std::isspace(static_cast<unsigned char>(value[first]))) {
        ++first;
    }

    std::string::size_type last = value.size();
    while (last > first && std::isspace(static_cast<unsigned char>(value[last - 1]))) {
        --last;
    }

    return value.substr(first, last - first);
}

std::string toString(AccountType type) {
    switch (type) {
        case AccountType::Savings:
            return "Savings";
        case AccountType::Current:
            return "Current";
    }
    return "Savings";
}

std::string toString(AccountStatus status) {
    switch (status) {
        case AccountStatus::Active:
            return "Active";
        case AccountStatus::Frozen:
            return "Frozen";
        case AccountStatus::Closed:
            return "Closed";
    }
    return "Active";
}

std::string toString(TransactionType type) {
    switch (type) {
        case TransactionType::AccountOpened:
            return "AccountOpened";
        case TransactionType::Deposit:
            return "Deposit";
        case TransactionType::Withdrawal:
            return "Withdrawal";
        case TransactionType::TransferIn:
            return "TransferIn";
        case TransactionType::TransferOut:
            return "TransferOut";
        case TransactionType::AccountClosed:
            return "AccountClosed";
    }
    return "Deposit";
}

AccountType accountTypeFromString(const std::string& value) {
    const std::string lowered = toLower(trim(value));
    if (lowered == "savings") {
        return AccountType::Savings;
    }
    if (lowered == "current") {
        return AccountType::Current;
    }
    throw std::invalid_argument("Unknown account type: " + value);
}

AccountStatus accountStatusFromString(const std::string& value) {
    const std::string lowered = toLower(trim(value));
    if (lowered == "active") {
        return AccountStatus::Active;
    }
    if (lowered == "frozen") {
        return AccountStatus::Frozen;
    }
    if (lowered == "closed") {
        return AccountStatus::Closed;
    }
    throw std::invalid_argument("Unknown account status: " + value);
}

TransactionType transactionTypeFromString(const std::string& value) {
    const std::string lowered = toLower(trim(value));
    if (lowered == "accountopened") {
        return TransactionType::AccountOpened;
    }
    if (lowered == "deposit") {
        return TransactionType::Deposit;
    }
    if (lowered == "withdrawal") {
        return TransactionType::Withdrawal;
    }
    if (lowered == "transferin") {
        return TransactionType::TransferIn;
    }
    if (lowered == "transferout") {
        return TransactionType::TransferOut;
    }
    if (lowered == "accountclosed") {
        return TransactionType::AccountClosed;
    }
    throw std::invalid_argument("Unknown transaction type: " + value);
}

Money parseMoney(const std::string& value) {
    std::string text = trim(value);
    if (text.empty()) {
        throw std::invalid_argument("Amount is required");
    }

    bool negative = false;
    if (text[0] == '+' || text[0] == '-') {
        negative = text[0] == '-';
        text = text.substr(1);
    }

    const std::string::size_type dot = text.find('.');
    const std::string whole = dot == std::string::npos ? text : text.substr(0, dot);
    std::string cents = dot == std::string::npos ? "" : text.substr(dot + 1);

    if (!allDigits(whole)) {
        throw std::invalid_argument("Amount must use digits, for example 125.50");
    }
    if (cents.size() > 2 || (!cents.empty() && !allDigits(cents))) {
        throw std::invalid_argument("Amount can have at most two decimal places");
    }

    while (cents.size() < 2) {
        cents.push_back('0');
    }

    Money parsed = static_cast<Money>(std::stoll(whole)) * 100;
    if (!cents.empty()) {
        parsed += static_cast<Money>(std::stoll(cents));
    }

    return negative ? -parsed : parsed;
}

std::string formatMoney(Money cents) {
    const bool negative = cents < 0;
    const Money absolute = negative ? -cents : cents;

    std::ostringstream out;
    if (negative) {
        out << "-";
    }
    out << (absolute / 100) << ".";
    out << std::setw(2) << std::setfill('0') << (absolute % 100);
    return out.str();
}

} // namespace banking
