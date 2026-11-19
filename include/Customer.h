#pragma once
#include <string>

// Account holder details. Data is private; accessed through getters/setters.
class Customer {
private:
    std::string name_;
    std::string ic_;
    std::string phone_;

public:
    Customer(std::string name, std::string ic, std::string phone)
        : name_(std::move(name)), ic_(std::move(ic)), phone_(std::move(phone)) {}

    const std::string& getName() const { return name_; }
    const std::string& getIC() const { return ic_; }
    const std::string& getPhone() const { return phone_; }

    void setName(const std::string& n) { name_ = n; }
    void setPhone(const std::string& p) { phone_ = p; }
};
