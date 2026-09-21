#ifndef BANKING_MODELS_H
#define BANKING_MODELS_H

#include <cstdint>
#include <string>

namespace banking {

typedef std::int64_t Money;

enum class AccountType {
    Savings,
    Current
};

enum class AccountStatus {
    Active,
    Frozen,
    Closed
};

enum class TransactionType {
    AccountOpened,
    Deposit,
    Withdrawal,
    TransferIn,
    TransferOut,
    AccountClosed
};

struct CustomerDraft {
    std::string fullName;
    std::string email;
    std::string phone;
    std::string address;
};

struct CustomerPatch {
    bool hasFullName = false;
    bool hasEmail = false;
    bool hasPhone = false;
    bool hasAddress = false;

    std::string fullName;
    std::string email;
    std::string phone;
    std::string address;
};

struct Customer {
    std::string id;
    std::string fullName;
    std::string email;
    std::string phone;
    std::string address;
    std::string createdAt;
    std::string updatedAt;
};

struct Account {
    std::string number;
    std::string customerId;
    AccountType type = AccountType::Savings;
    Money balanceCents = 0;
    AccountStatus status = AccountStatus::Active;
    std::string createdAt;
    std::string updatedAt;
};

struct Transaction {
    std::string id;
    std::string accountNumber;
    TransactionType type = TransactionType::Deposit;
    Money amountCents = 0;
    std::string occurredAt;
    std::string note;
    std::string relatedAccountNumber;
};

struct ChangeEvent {
    std::uint64_t sequence = 0;
    std::string occurredAt;
    std::string action;
    std::string entity;
    std::string entityId;
    std::string summary;
};

std::string nowIso8601();
std::string trim(const std::string& value);

std::string toString(AccountType type);
std::string toString(AccountStatus status);
std::string toString(TransactionType type);

AccountType accountTypeFromString(const std::string& value);
AccountStatus accountStatusFromString(const std::string& value);
TransactionType transactionTypeFromString(const std::string& value);

Money parseMoney(const std::string& value);
std::string formatMoney(Money cents);

} // namespace banking

#endif // BANKING_MODELS_H
