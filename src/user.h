//
// Created by Rizescu Iulian Stefan on 17.03.2024.
//

#ifndef OOP_USER_H
#define OOP_USER_H

#include <string>

class User {
private:
    unsigned int id_;
    std::string name_;
    std::string username_;
    std::string email_;

public:
    explicit User(unsigned int id, std::string name, std::string username, std::string email)
        : id_{id}, name_{std::move(name)}, username_(std::move(username)), email_(std::move(email)) {}

    [[nodiscard]] auto id() const noexcept { return id_; }

    [[nodiscard]] std::string_view name() const noexcept { return name_; }

    [[nodiscard]] std::string_view username() const noexcept { return username_; }

    [[nodiscard]] std::string_view email() const noexcept { return email_; }

    friend std::ostream &operator<<(std::ostream &out, User const &user) noexcept {
        out << "[User]" << "\n\t"
            << "id: " << user.id_ << "\n\t"
            << "name: " << user.name_ << "\n\t"
            << "username: " << user.username_ << "\n\t"
            << "email: " << user.email_;
        return out;
    }
};

#endif //OOP_USER_H
