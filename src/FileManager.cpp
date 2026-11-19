#include "FileManager.h"
#include <fstream>

void FileManager::save(const std::string& path,
                       const std::vector<std::unique_ptr<Account>>& accounts,
                       int nextAccNo) {
    std::ofstream out(path);
    if (!out) throw BankException("Cannot write to data file: " + path);

    out << "NEXT|" << nextAccNo << "\n";
    for (const auto& a : accounts) {
        const Customer& c = a->getHolder();
        out << "ACC|" << a->getType() << "|" << a->getAccNo() << "|" << c.getName() << "|"
            << c.getIC() << "|" << c.getPhone() << "|" << a->getPin() << "|"
            << Util::fixed(a->getBalance()) << "\n";
        for (const auto& t : a->getHistory()) out << t.serialize() << "\n";
    }
    if (!out) throw BankException("Error while writing data file.");
}

bool FileManager::load(const std::string& path,
                       std::vector<std::unique_ptr<Account>>& accounts,
                       int& nextAccNo) {
    std::ifstream in(path);
    if (!in) return false;   // first run: no file yet

    std::string line;
    int lineNo = 0;
    Account* current = nullptr;
    while (std::getline(in, line)) {
        ++lineNo;
        if (Util::trim(line).empty()) continue;
        auto f = Util::split(line, '|');
        try {
            if (f[0] == "NEXT") {
                nextAccNo = std::stoi(f.at(1));
            } else if (f[0] == "ACC" && f.size() == 8) {
                accounts.push_back(createAccount(f[1], std::stoi(f[2]),
                                                 Customer(f[3], f[4], f[5]), f[6],
                                                 std::stod(f[7])));
                current = accounts.back().get();
            } else if (f[0] == "TXN" && f.size() == 6 && current) {
                current->addHistory(Transaction(f[1], std::stod(f[2]), std::stod(f[3]),
                                                f[4], f[5]));
            } else {
                throw BankException("bad record");
            }
        } catch (const std::exception&) {
            throw BankException("Data file is corrupted at line " + std::to_string(lineNo) + ".");
        }
    }
    return true;
}
