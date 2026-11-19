#include "Menu.h"
#include <iomanip>
#include <iostream>

const std::string Menu::ADMIN_PASSWORD = "admin123";

// ---------------- display helpers ----------------
void Menu::printTable(const std::vector<const Account*>& list) {
    std::cout << "\n" << std::left << std::setw(8) << "AccNo" << std::setw(10) << "Type"
              << std::setw(26) << "Holder" << std::right << std::setw(14) << "Balance (RM)" << "\n"
              << std::string(58, '-') << "\n";
    for (const Account* a : list)
        std::cout << std::left << std::setw(8) << a->getAccNo() << std::setw(10) << a->getType()
                  << std::setw(26) << a->getHolder().getName().substr(0, 25) << std::right
                  << std::setw(14) << Util::fixed(a->getBalance()) << "\n";
}

void Menu::printDetails(const Account& a) {
    std::cout << "\n--- Account Details ---\n"
              << "Account No : " << a.getAccNo() << "\n"
              << "Type       : " << a.getType() << "\n"
              << "Holder     : " << a.getHolder().getName() << "\n"
              << "IC         : " << a.getHolder().getIC() << "\n"
              << "Phone      : " << a.getHolder().getPhone() << "\n"
              << "Balance    : " << Util::money(a.getBalance()) << "\n"
              << "Policy     : " << a.policySummary() << "\n";
}

void Menu::printHistory(const Account& a) {
    std::cout << "\n--- Transaction History (" << a.getAccNo() << ") ---\n";
    if (a.getHistory().empty()) { std::cout << "No transactions yet.\n"; return; }
    for (const auto& t : a.getHistory()) std::cout << t << "\n";
}

// ---------------- main menu ----------------
void Menu::run() {
    int choice;
    do {
        std::cout << "\n========== BANKING SYSTEM ==========\n"
                  << "1. Admin Login\n2. Customer Login\n3. Exit\n";
        choice = Util::readNumber<int>("Choice: ", 1, 3);
        if (choice == 1) adminLogin();
        else if (choice == 2) customerLogin();
    } while (choice != 3);
    std::cout << "Thank you for using the Banking System. Goodbye!\n";
}

void Menu::adminLogin() {
    for (int tries = 1; tries <= 3; ++tries) {
        if (Util::readLine("Admin password: ") == ADMIN_PASSWORD) { adminMenu(); return; }
        std::cout << "Wrong password (" << tries << "/3).\n";
    }
    std::cout << "Too many failed attempts.\n";
}

void Menu::adminMenu() {
    int choice;
    do {
        std::cout << "\n===== ADMIN MENU =====\n"
                  << "1. Create account\n2. View all accounts\n3. Search account\n"
                  << "4. Update account\n5. Delete account\n"
                  << "6. Account report\n7. Logout\n";
        choice = Util::readNumber<int>("Choice: ", 1, 7);
        try {
            switch (choice) {
                case 1: createAccountUI(); break;
                case 2: listAccountsUI(); break;
                case 3: searchUI(); break;
                case 4: updateUI(); break;
                case 5: deleteUI(); break;
                case 6: reportUI(); break;
                default: break;
            }
        } catch (const BankException& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    } while (choice != 7);
}

// ---------------- admin actions ----------------
void Menu::createAccountUI() {
    std::cout << "\n-- Create Account --\n1. Savings\n2. Current\n";
    int t = Util::readNumber<int>("Account type: ", 1, 2);
    std::string name = Util::readText("Full name: ");
    std::string ic = Util::readIC("IC number: ");
    std::string phone = Util::readPhone("Phone number: ");
    std::string pin = Util::readPin("Set 4-digit PIN: ");
    double dep = Util::readNumber<double>("Opening deposit (min RM 10): ", 0.0, 10000000.0);
    int no = bank_.openAccount(t == 1 ? "Savings" : "Current", Customer(name, ic, phone), pin, dep);
    std::cout << "Account created successfully! Account number: " << no << "\n";
}

void Menu::listAccountsUI() {
    if (bank_.empty()) { std::cout << "No accounts in the system.\n"; return; }
    std::cout << "Sort by: 1. Name  2. Balance (high to low)\n";
    int s = Util::readNumber<int>("Choice: ", 1, 2);
    printTable(bank_.listSorted(s == 2));
}

void Menu::searchUI() {
    if (bank_.empty()) { std::cout << "No accounts in the system.\n"; return; }
    std::cout << "Search by: 1. Account number  2. Holder name\n";
    int s = Util::readNumber<int>("Choice: ", 1, 2);
    if (s == 1) {
        int no = Util::readNumber<int>("Account number: ", 1, 999999999);
        printDetails(bank_.getAccount(no));
    } else {
        auto found = bank_.searchByName(Util::readText("Name keyword: "));
        if (found.empty()) std::cout << "No matching accounts.\n";
        else printTable(found);
    }
}

void Menu::updateUI() {
    int no = Util::readNumber<int>("Account number to update: ", 1, 999999999);
    printDetails(bank_.getAccount(no));
    std::cout << "\n1. Change name & phone\n2. Reset PIN\n3. Cancel\n";
    int c = Util::readNumber<int>("Choice: ", 1, 3);
    if (c == 1) {
        std::string name = Util::readText("New name: ");
        std::string phone = Util::readPhone("New phone: ");
        bank_.updateHolder(no, name, phone);
        std::cout << "Account updated.\n";
    } else if (c == 2) {
        bank_.changePin(no, Util::readPin("New 4-digit PIN: "));
        std::cout << "PIN reset successfully.\n";
    }
}

void Menu::deleteUI() {
    int no = Util::readNumber<int>("Account number to close: ", 1, 999999999);
    printDetails(bank_.getAccount(no));
    if (!Util::confirm("Permanently close this account?")) { std::cout << "Cancelled.\n"; return; }
    double payout = bank_.closeAccount(no);
    std::cout << "Account closed. Cash returned to customer: " << Util::money(payout) << "\n";
}

void Menu::reportUI() {
    int no = Util::readNumber<int>("Account number: ", 1, 999999999);
    const Account& a = bank_.getAccount(no);   // throws if not found
    std::cout << "\n--- Account Report ---";
    printDetails(a);
    printHistory(a);
}

// ---------------- customer ----------------
void Menu::customerLogin() {
    int no = Util::readNumber<int>("Account number: ", 1, 999999999);
    const Account* acc = bank_.findAccount(no);
    for (int tries = 1; tries <= 3; ++tries) {
        std::string pin = Util::readLine("PIN: ");
        if (acc && acc->verifyPin(pin)) {
            std::cout << "\nWelcome, " << acc->getHolder().getName() << "!\n";
            customerMenu(no);
            return;
        }
        std::cout << "Invalid account number or PIN (" << tries << "/3).\n";
    }
    std::cout << "Too many failed attempts. Returning to main menu.\n";
}

void Menu::customerMenu(int accNo) {
    int choice;
    do {
        std::cout << "\n===== CUSTOMER MENU =====\n"
                  << "1. Account details\n2. Deposit\n3. Withdraw\n4. Transfer\n"
                  << "5. Transaction history\n6. Change PIN\n7. Logout\n";
        choice = Util::readNumber<int>("Choice: ", 1, 7);
        try {
            switch (choice) {
                case 1: printDetails(bank_.getAccount(accNo)); break;
                case 2: {
                    double amt = Util::readNumber<double>("Amount: ", 0.0, 10000000.0);
                    bank_.deposit(accNo, amt);
                    std::cout << "Deposit successful. New balance: "
                              << Util::money(bank_.getAccount(accNo).getBalance()) << "\n";
                    break;
                }
                case 3: {
                    double amt = Util::readNumber<double>("Amount: ", 0.0, 10000000.0);
                    bank_.withdraw(accNo, amt);
                    std::cout << "Withdrawal successful. New balance: "
                              << Util::money(bank_.getAccount(accNo).getBalance()) << "\n";
                    break;
                }
                case 4: {
                    int to = Util::readNumber<int>("Recipient account number: ", 1, 999999999);
                    double amt = Util::readNumber<double>("Amount: ", 0.0, 10000000.0);
                    bank_.transfer(accNo, to, amt);
                    std::cout << "Transfer successful. New balance: "
                              << Util::money(bank_.getAccount(accNo).getBalance()) << "\n";
                    break;
                }
                case 5: printHistory(bank_.getAccount(accNo)); break;
                case 6: {
                    std::string oldPin = Util::readLine("Current PIN: ");
                    if (!bank_.getAccount(accNo).verifyPin(oldPin)) {
                        std::cout << "Incorrect PIN.\n";
                        break;
                    }
                    bank_.changePin(accNo, Util::readPin("New 4-digit PIN: "));
                    std::cout << "PIN changed successfully.\n";
                    break;
                }
                default: std::cout << "Logged out.\n"; break;
            }
        } catch (const BankException& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    } while (choice != 7);
}
