#ifndef BANKING_FILE_STORAGE_H
#define BANKING_FILE_STORAGE_H

#include "banking/Models.h"

#include <string>
#include <vector>

namespace banking {

struct DatabaseSnapshot {
    std::vector<Customer> customers;
    std::vector<Account> accounts;
    std::vector<Transaction> transactions;
};

class FileStorage {
public:
    explicit FileStorage(const std::string& dataDirectory = "data");

    DatabaseSnapshot load() const;
    void save(const DatabaseSnapshot& snapshot) const;
    void appendEvent(const ChangeEvent& event) const;
    std::uint64_t lastEventSequence() const;

    const std::string& dataDirectory() const;

private:
    std::string dataDirectory_;

    std::string customersPath() const;
    std::string accountsPath() const;
    std::string transactionsPath() const;
    std::string eventsPath() const;

    void ensureDataDirectory() const;
};

} // namespace banking

#endif // BANKING_FILE_STORAGE_H
