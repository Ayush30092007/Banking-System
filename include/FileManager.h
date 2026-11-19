#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Account.h"

// Responsible ONLY for reading/writing the data file (single responsibility).
class FileManager {
public:
    static void save(const std::string& path,
                     const std::vector<std::unique_ptr<Account>>& accounts,
                     int nextAccNo);
    // Returns false if the file does not exist yet. Throws BankException if corrupted.
    static bool load(const std::string& path,
                     std::vector<std::unique_ptr<Account>>& accounts,
                     int& nextAccNo);
};
