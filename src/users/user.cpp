#include "user.h"

User::User(user::id id, std::string name, std::string username, std::string email)
    : id_{id}, name_{std::move(name)}, username_(std::move(username)), email_(std::move(email)) {}

user::id User::id() const noexcept { return id_; }

std::string_view User::name() const noexcept { return name_; }

std::string_view User::username() const noexcept { return username_; }

std::string_view User::email() const noexcept { return email_; }

std::ostream &operator<<(std::ostream &out, const User &user) noexcept {
    out << "[User]" << "\n\t"
        << "id: " << user.id_.val() << "\n\t"
        << "name: " << user.name_ << "\n\t"
        << "username: " << user.username_ << "\n\t"
        << "email: " << user.email_;
    return out;
}
