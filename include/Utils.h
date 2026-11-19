#pragma once
#include <algorithm>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Helper functions grouped in a namespace (input validation + formatting).
namespace Util {

constexpr double MIN_OPENING_DEPOSIT = 10.0;
constexpr std::size_t PIN_LENGTH = 4;

// Thrown when stdin is closed (e.g. Ctrl+D) so the program can exit cleanly.
class InputClosedException : public std::runtime_error {
public:
    InputClosedException() : std::runtime_error("Input closed.") {}
};

inline std::string trim(const std::string& s) {
    auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

inline std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}

inline bool allDigits(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(),
                                     [](unsigned char c) { return std::isdigit(c); });
}

// Split by delimiter, keeping empty fields.
inline std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> parts;
    std::size_t start = 0, pos;
    while ((pos = s.find(delim, start)) != std::string::npos) {
        parts.push_back(s.substr(start, pos - start));
        start = pos + 1;
    }
    parts.push_back(s.substr(start));
    return parts;
}

inline std::string timestamp() {
    std::time_t t = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return buf;
}

// Template: format any numeric value with fixed decimals.
template <typename T>
std::string fixed(T value, int decimals = 2) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(decimals) << value;
    return oss.str();
}

inline std::string money(double v) { return "RM " + fixed(v); }

inline std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    if (!std::getline(std::cin, s)) throw InputClosedException();
    return trim(s);
}

// Template: keeps asking until a number within [minV, maxV] is entered.
template <typename T>
T readNumber(const std::string& prompt, T minV, T maxV) {
    while (true) {
        std::istringstream iss(readLine(prompt));
        T value;
        char extra;
        if ((iss >> value) && !(iss >> extra) && value >= minV && value <= maxV)
            return value;
        std::cout << "  Invalid input. Please enter a number between "
                  << minV << " and " << maxV << ".\n";
    }
}

inline std::string readText(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt);
        if (s.empty()) std::cout << "  This field cannot be empty.\n";
        else if (s.find('|') != std::string::npos) std::cout << "  The '|' character is not allowed.\n";
        else return s;
    }
}

inline std::string readPin(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt);
        if (s.size() == PIN_LENGTH && allDigits(s)) return s;
        std::cout << "  PIN must be exactly " << PIN_LENGTH << " digits.\n";
    }
}

inline std::string readIC(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt), digits;
        for (char c : s) if (c != '-') digits += c;
        if (digits.size() == 12 && allDigits(digits)) return digits;
        std::cout << "  IC must contain 12 digits (e.g. 010203-07-1234).\n";
    }
}

inline std::string readPhone(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt);
        if (s.size() >= 9 && s.size() <= 10 && allDigits(s)) return s;
        std::cout << "  Phone must be 9-10 digits, numbers only.\n";
    }
}

inline bool confirm(const std::string& prompt) {
    while (true) {
        std::string s = toLower(readLine(prompt + " (y/n): "));
        if (s == "y") return true;
        if (s == "n") return false;
        std::cout << "  Please type y or n.\n";
    }
}

} // namespace Util
