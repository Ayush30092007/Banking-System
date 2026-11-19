#pragma once
#include <stdexcept>
#include <string>

// Base class for all banking errors (custom exception hierarchy).
class BankException : public std::runtime_error {
public:
    explicit BankException(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidAmountException : public BankException {
public:
    InvalidAmountException() : BankException("Amount must be greater than zero.") {}
};

class InsufficientFundsException : public BankException {
public:
    explicit InsufficientFundsException(const std::string& detail)
        : BankException("Insufficient funds. " + detail) {}
};

class AccountNotFoundException : public BankException {
public:
    explicit AccountNotFoundException(int accNo)
        : BankException("Account " + std::to_string(accNo) + " was not found.") {}
};

class DuplicateAccountException : public BankException {
public:
    DuplicateAccountException()
        : BankException("This IC number already has an account of that type.") {}
};
