#pragma once
#include <string>
#include <vector>
#include "Bank.h"

// Console user interface. Talks to Bank only through its public interface.
class Menu {
private:
    Bank& bank_;
    static const std::string ADMIN_PASSWORD;

    void adminLogin();
    void adminMenu();
    void customerLogin();
    void customerMenu(int accNo);

    void createAccountUI();
    void listAccountsUI();
    void searchUI();
    void updateUI();
    void deleteUI();
    void reportUI();

    static void printTable(const std::vector<const Account*>& list);
    static void printDetails(const Account& a);
    static void printHistory(const Account& a);

public:
    explicit Menu(Bank& bank) : bank_(bank) {}
    void run();
};
