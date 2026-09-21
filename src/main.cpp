#include "banking/BankService.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using banking::Account;
using banking::AccountType;
using banking::BankService;
using banking::Customer;
using banking::CustomerDraft;
using banking::CustomerPatch;
using banking::Money;
using banking::Transaction;

std::string prompt(const std::string& label) {
    std::cout << label;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

Money promptMoney(const std::string& label) {
    return banking::parseMoney(prompt(label));
}

AccountType promptAccountType() {
    while (true) {
        const std::string value = prompt("Account type [1=Savings, 2=Current]: ");
        if (value == "1") {
            return AccountType::Savings;
        }
        if (value == "2") {
            return AccountType::Current;
        }
        std::cout << "Please choose 1 or 2.\n";
    }
}

void printCustomers(const std::vector<Customer>& customers) {
    std::cout << "\nCustomers\n";
    std::cout << "---------\n";
    if (customers.empty()) {
        std::cout << "No customers yet.\n";
        return;
    }

    for (const Customer& customer : customers) {
        std::cout << customer.id << " | " << customer.fullName
                  << " | " << customer.phone
                  << " | " << customer.email << "\n";
    }
}

void printAccounts(const std::vector<Account>& accounts) {
    std::cout << "\nAccounts\n";
    std::cout << "--------\n";
    if (accounts.empty()) {
        std::cout << "No accounts yet.\n";
        return;
    }

    for (const Account& account : accounts) {
        std::cout << account.number
                  << " | customer " << account.customerId
                  << " | " << banking::toString(account.type)
                  << " | " << banking::toString(account.status)
                  << " | RM " << banking::formatMoney(account.balanceCents)
                  << "\n";
    }
}

void printStatement(const std::vector<Transaction>& transactions) {
    std::cout << "\nStatement\n";
    std::cout << "---------\n";
    if (transactions.empty()) {
        std::cout << "No transactions for this account.\n";
        return;
    }

    for (const Transaction& transaction : transactions) {
        std::cout << transaction.occurredAt
                  << " | " << transaction.id
                  << " | " << banking::toString(transaction.type)
                  << " | RM " << banking::formatMoney(transaction.amountCents);
        if (!transaction.relatedAccountNumber.empty()) {
            std::cout << " | linked " << transaction.relatedAccountNumber;
        }
        if (!transaction.note.empty()) {
            std::cout << " | " << transaction.note;
        }
        std::cout << "\n";
    }
}

void createCustomer(BankService& bank) {
    CustomerDraft draft;
    draft.fullName = prompt("Full name: ");
    draft.email = prompt("Email: ");
    draft.phone = prompt("Phone: ");
    draft.address = prompt("Address: ");

    const Customer customer = bank.createCustomer(draft);
    std::cout << "Created customer " << customer.id << ".\n";
}

void updateCustomer(BankService& bank) {
    const std::string id = prompt("Customer ID: ");
    CustomerPatch patch;

    std::cout << "Leave a field blank to keep the current value.\n";

    const std::string fullName = prompt("New full name: ");
    if (!banking::trim(fullName).empty()) {
        patch.hasFullName = true;
        patch.fullName = fullName;
    }

    const std::string email = prompt("New email: ");
    if (!banking::trim(email).empty()) {
        patch.hasEmail = true;
        patch.email = email;
    }

    const std::string phone = prompt("New phone: ");
    if (!banking::trim(phone).empty()) {
        patch.hasPhone = true;
        patch.phone = phone;
    }

    const std::string address = prompt("New address: ");
    if (!banking::trim(address).empty()) {
        patch.hasAddress = true;
        patch.address = address;
    }

    const Customer customer = bank.updateCustomer(id, patch);
    std::cout << "Updated " << customer.id << ".\n";
}

void openAccount(BankService& bank) {
    const std::string customerId = prompt("Customer ID: ");
    const AccountType type = promptAccountType();
    const Money initialDeposit = promptMoney("Initial deposit: RM ");

    const Account account = bank.openAccount(customerId, type, initialDeposit);
    std::cout << "Opened account " << account.number << ".\n";
}

void deposit(BankService& bank) {
    const std::string accountNumber = prompt("Account number: ");
    const Money amount = promptMoney("Deposit amount: RM ");
    const std::string note = prompt("Note: ");

    const Account account = bank.deposit(accountNumber, amount, note);
    std::cout << "New balance: RM " << banking::formatMoney(account.balanceCents) << ".\n";
}

void withdraw(BankService& bank) {
    const std::string accountNumber = prompt("Account number: ");
    const Money amount = promptMoney("Withdrawal amount: RM ");
    const std::string note = prompt("Note: ");

    const Account account = bank.withdraw(accountNumber, amount, note);
    std::cout << "New balance: RM " << banking::formatMoney(account.balanceCents) << ".\n";
}

void transfer(BankService& bank) {
    const std::string from = prompt("From account: ");
    const std::string to = prompt("To account: ");
    const Money amount = promptMoney("Transfer amount: RM ");
    const std::string note = prompt("Note: ");

    bank.transfer(from, to, amount, note);
    std::cout << "Transfer complete.\n";
}

void closeAccount(BankService& bank) {
    const std::string accountNumber = prompt("Account number: ");
    bank.closeAccount(accountNumber);
    std::cout << "Account closed.\n";
}

void showMenu() {
    std::cout << "\nBanking Backend\n";
    std::cout << "===============\n";
    std::cout << "1. List customers\n";
    std::cout << "2. Create customer\n";
    std::cout << "3. Update customer\n";
    std::cout << "4. List accounts\n";
    std::cout << "5. Open account\n";
    std::cout << "6. Deposit\n";
    std::cout << "7. Withdraw\n";
    std::cout << "8. Transfer\n";
    std::cout << "9. Account statement\n";
    std::cout << "10. Close account\n";
    std::cout << "0. Exit\n";
}

} // namespace

int main(int argc, char* argv[]) {
    const std::string dataDirectory = argc > 1 ? argv[1] : "data";

    try {
        banking::FileStorage storage(dataDirectory);
        BankService bank(storage);

        bank.addChangeListener([](const banking::ChangeEvent& event) {
            std::cout << "\n[live update #" << event.sequence << "] "
                      << event.summary << "\n";
        });

        std::cout << "Data directory: " << dataDirectory << "\n";

        while (true) {
            showMenu();
            const std::string choice = prompt("Choose: ");

            try {
                if (choice == "1") {
                    printCustomers(bank.listCustomers());
                } else if (choice == "2") {
                    createCustomer(bank);
                } else if (choice == "3") {
                    updateCustomer(bank);
                } else if (choice == "4") {
                    printAccounts(bank.listAccounts());
                } else if (choice == "5") {
                    openAccount(bank);
                } else if (choice == "6") {
                    deposit(bank);
                } else if (choice == "7") {
                    withdraw(bank);
                } else if (choice == "8") {
                    transfer(bank);
                } else if (choice == "9") {
                    printStatement(bank.statementFor(prompt("Account number: ")));
                } else if (choice == "10") {
                    closeAccount(bank);
                } else if (choice == "0") {
                    return EXIT_SUCCESS;
                } else {
                    std::cout << "Unknown option.\n";
                }
            } catch (const std::exception& ex) {
                std::cout << "Error: " << ex.what() << "\n";
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << "Fatal: " << ex.what() << "\n";
        return EXIT_FAILURE;
    }
}
