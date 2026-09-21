#include "banking/BankService.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace {

void removeTestFiles(const std::string& directory) {
    std::remove((directory + "/customers.tsv").c_str());
    std::remove((directory + "/accounts.tsv").c_str());
    std::remove((directory + "/transactions.tsv").c_str());
    std::remove((directory + "/events.jsonl").c_str());
}

void assertThrows(const std::function<void()>& fn) {
    bool thrown = false;
    try {
        fn();
    } catch (const std::exception&) {
        thrown = true;
    }
    assert(thrown);
}

} // namespace

int main() {
    const std::string directory = "test-data";
    removeTestFiles(directory);

    banking::BankService bank{banking::FileStorage(directory)};

    int liveUpdates = 0;
    bank.addChangeListener([&liveUpdates](const banking::ChangeEvent& event) {
        assert(event.sequence > 0);
        assert(!event.summary.empty());
        ++liveUpdates;
    });

    banking::CustomerDraft ayush;
    ayush.fullName = "Ayush Kumar";
    ayush.email = "ayush@example.com";
    ayush.phone = "0123456789";
    ayush.address = "Kuala Lumpur";
    const banking::Customer customer = bank.createCustomer(ayush);
    assert(customer.id == "C000001");

    const banking::Account savings = bank.openAccount(customer.id, banking::AccountType::Savings, banking::parseMoney("500.00"));
    assert(savings.number == "1000000001");

    banking::Account updated = bank.deposit(savings.number, banking::parseMoney("125.25"), "salary");
    assert(updated.balanceCents == banking::parseMoney("625.25"));

    updated = bank.withdraw(savings.number, banking::parseMoney("25.25"), "cash");
    assert(updated.balanceCents == banking::parseMoney("600.00"));

    const banking::Account current = bank.openAccount(customer.id, banking::AccountType::Current, 0);
    bank.transfer(savings.number, current.number, banking::parseMoney("100.00"), "move funds");

    const std::vector<banking::Account> accounts = bank.listAccounts();
    assert(accounts.size() == 2);
    assert(accounts[0].balanceCents == banking::parseMoney("500.00"));
    assert(accounts[1].balanceCents == banking::parseMoney("100.00"));

    const std::vector<banking::Transaction> statement = bank.statementFor(savings.number);
    assert(statement.size() == 4);

    assertThrows([&bank, &savings]() {
        bank.withdraw(savings.number, banking::parseMoney("1000.00"), "too much");
    });

    assert(liveUpdates == 6);

    banking::BankService reloaded{banking::FileStorage(directory)};
    assert(reloaded.listCustomers().size() == 1);
    assert(reloaded.listAccounts().size() == 2);
    assert(reloaded.listTransactions().size() == 6);

    removeTestFiles(directory);
    std::cout << "backend_tests passed\n";
    return EXIT_SUCCESS;
}
