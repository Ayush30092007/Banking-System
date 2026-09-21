#include "banking/FileStorage.h"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

namespace banking {

namespace {

bool fileExists(const std::string& path) {
    std::ifstream input(path.c_str());
    return input.good();
}

std::string joinPath(const std::string& left, const std::string& right) {
    if (left.empty()) {
        return right;
    }
    const char last = left[left.size() - 1];
    if (last == '/' || last == '\\') {
        return left + right;
    }
    return left + "/" + right;
}

void makeDirectory(const std::string& path) {
    if (path.empty()) {
        return;
    }

#ifdef _WIN32
    _mkdir(path.c_str());
#else
    mkdir(path.c_str(), 0755);
#endif
}

std::string encodeField(const std::string& value) {
    std::string encoded;
    for (char ch : value) {
        switch (ch) {
            case '\\':
                encoded += "\\\\";
                break;
            case '\t':
                encoded += "\\t";
                break;
            case '\n':
                encoded += "\\n";
                break;
            case '\r':
                encoded += "\\r";
                break;
            default:
                encoded.push_back(ch);
                break;
        }
    }
    return encoded;
}

std::vector<std::string> splitTsv(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool escaped = false;

    for (char ch : line) {
        if (escaped) {
            switch (ch) {
                case 't':
                    current.push_back('\t');
                    break;
                case 'n':
                    current.push_back('\n');
                    break;
                case 'r':
                    current.push_back('\r');
                    break;
                case '\\':
                    current.push_back('\\');
                    break;
                default:
                    current.push_back(ch);
                    break;
            }
            escaped = false;
            continue;
        }

        if (ch == '\\') {
            escaped = true;
            continue;
        }

        if (ch == '\t') {
            fields.push_back(current);
            current.clear();
            continue;
        }

        current.push_back(ch);
    }

    if (escaped) {
        current.push_back('\\');
    }

    fields.push_back(current);
    return fields;
}

std::string makeLine(const std::vector<std::string>& fields) {
    std::ostringstream out;
    for (std::size_t i = 0; i < fields.size(); ++i) {
        if (i > 0) {
            out << '\t';
        }
        out << encodeField(fields[i]);
    }
    return out.str();
}

std::string jsonEscape(const std::string& value) {
    std::string escaped;
    for (char ch : value) {
        switch (ch) {
            case '\\':
                escaped += "\\\\";
                break;
            case '"':
                escaped += "\\\"";
                break;
            case '\n':
                escaped += "\\n";
                break;
            case '\r':
                escaped += "\\r";
                break;
            case '\t':
                escaped += "\\t";
                break;
            default:
                escaped.push_back(ch);
                break;
        }
    }
    return escaped;
}

void writeAtomically(const std::string& path, const std::string& contents) {
    const std::string tempPath = path + ".tmp";
    {
        std::ofstream output(tempPath.c_str(), std::ios::out | std::ios::trunc);
        if (!output) {
            throw std::runtime_error("Unable to write " + tempPath);
        }
        output << contents;
    }

    std::remove(path.c_str());
    if (std::rename(tempPath.c_str(), path.c_str()) != 0) {
        std::remove(tempPath.c_str());
        throw std::runtime_error("Unable to replace " + path);
    }
}

} // namespace

FileStorage::FileStorage(const std::string& dataDirectory)
    : dataDirectory_(dataDirectory) {}

DatabaseSnapshot FileStorage::load() const {
    ensureDataDirectory();

    DatabaseSnapshot snapshot;
    std::string line;

    if (fileExists(customersPath())) {
        std::ifstream input(customersPath().c_str());
        while (std::getline(input, line)) {
            if (line.empty()) {
                continue;
            }

            const std::vector<std::string> fields = splitTsv(line);
            if (fields.size() != 7) {
                throw std::runtime_error("Invalid customer row in " + customersPath());
            }

            Customer customer;
            customer.id = fields[0];
            customer.fullName = fields[1];
            customer.email = fields[2];
            customer.phone = fields[3];
            customer.address = fields[4];
            customer.createdAt = fields[5];
            customer.updatedAt = fields[6];
            snapshot.customers.push_back(customer);
        }
    }

    if (fileExists(accountsPath())) {
        std::ifstream input(accountsPath().c_str());
        while (std::getline(input, line)) {
            if (line.empty()) {
                continue;
            }

            const std::vector<std::string> fields = splitTsv(line);
            if (fields.size() != 7) {
                throw std::runtime_error("Invalid account row in " + accountsPath());
            }

            Account account;
            account.number = fields[0];
            account.customerId = fields[1];
            account.type = accountTypeFromString(fields[2]);
            account.balanceCents = static_cast<Money>(std::stoll(fields[3]));
            account.status = accountStatusFromString(fields[4]);
            account.createdAt = fields[5];
            account.updatedAt = fields[6];
            snapshot.accounts.push_back(account);
        }
    }

    if (fileExists(transactionsPath())) {
        std::ifstream input(transactionsPath().c_str());
        while (std::getline(input, line)) {
            if (line.empty()) {
                continue;
            }

            const std::vector<std::string> fields = splitTsv(line);
            if (fields.size() != 7) {
                throw std::runtime_error("Invalid transaction row in " + transactionsPath());
            }

            Transaction transaction;
            transaction.id = fields[0];
            transaction.accountNumber = fields[1];
            transaction.type = transactionTypeFromString(fields[2]);
            transaction.amountCents = static_cast<Money>(std::stoll(fields[3]));
            transaction.occurredAt = fields[4];
            transaction.note = fields[5];
            transaction.relatedAccountNumber = fields[6];
            snapshot.transactions.push_back(transaction);
        }
    }

    return snapshot;
}

void FileStorage::save(const DatabaseSnapshot& snapshot) const {
    ensureDataDirectory();

    std::ostringstream customers;
    for (const Customer& customer : snapshot.customers) {
        customers << makeLine({
            customer.id,
            customer.fullName,
            customer.email,
            customer.phone,
            customer.address,
            customer.createdAt,
            customer.updatedAt
        }) << '\n';
    }

    std::ostringstream accounts;
    for (const Account& account : snapshot.accounts) {
        accounts << makeLine({
            account.number,
            account.customerId,
            toString(account.type),
            std::to_string(account.balanceCents),
            toString(account.status),
            account.createdAt,
            account.updatedAt
        }) << '\n';
    }

    std::ostringstream transactions;
    for (const Transaction& transaction : snapshot.transactions) {
        transactions << makeLine({
            transaction.id,
            transaction.accountNumber,
            toString(transaction.type),
            std::to_string(transaction.amountCents),
            transaction.occurredAt,
            transaction.note,
            transaction.relatedAccountNumber
        }) << '\n';
    }

    writeAtomically(customersPath(), customers.str());
    writeAtomically(accountsPath(), accounts.str());
    writeAtomically(transactionsPath(), transactions.str());
}

void FileStorage::appendEvent(const ChangeEvent& event) const {
    ensureDataDirectory();

    std::ofstream output(eventsPath().c_str(), std::ios::out | std::ios::app);
    if (!output) {
        throw std::runtime_error("Unable to append " + eventsPath());
    }

    output
        << "{\"sequence\":" << event.sequence
        << ",\"occurredAt\":\"" << jsonEscape(event.occurredAt)
        << "\",\"action\":\"" << jsonEscape(event.action)
        << "\",\"entity\":\"" << jsonEscape(event.entity)
        << "\",\"entityId\":\"" << jsonEscape(event.entityId)
        << "\",\"summary\":\"" << jsonEscape(event.summary)
        << "\"}\n";
}

std::uint64_t FileStorage::lastEventSequence() const {
    if (!fileExists(eventsPath())) {
        return 0;
    }

    std::ifstream input(eventsPath().c_str());
    std::string line;
    std::uint64_t sequence = 0;

    while (std::getline(input, line)) {
        const std::string needle = "\"sequence\":";
        const std::string::size_type pos = line.find(needle);
        if (pos == std::string::npos) {
            continue;
        }

        const std::string::size_type start = pos + needle.size();
        std::string::size_type end = start;
        while (end < line.size() && line[end] >= '0' && line[end] <= '9') {
            ++end;
        }

        if (end > start) {
            sequence = static_cast<std::uint64_t>(std::stoull(line.substr(start, end - start)));
        }
    }

    return sequence;
}

const std::string& FileStorage::dataDirectory() const {
    return dataDirectory_;
}

std::string FileStorage::customersPath() const {
    return joinPath(dataDirectory_, "customers.tsv");
}

std::string FileStorage::accountsPath() const {
    return joinPath(dataDirectory_, "accounts.tsv");
}

std::string FileStorage::transactionsPath() const {
    return joinPath(dataDirectory_, "transactions.tsv");
}

std::string FileStorage::eventsPath() const {
    return joinPath(dataDirectory_, "events.jsonl");
}

void FileStorage::ensureDataDirectory() const {
    makeDirectory(dataDirectory_);
}

} // namespace banking
