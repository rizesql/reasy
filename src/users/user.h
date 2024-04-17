#pragma once

#include "core/id.h"
#include <iostream>

namespace user {
    using id = tid<"users">;
}

class User {
private:
    user::id id_;
    std::string name_;
    std::string username_;
    std::string email_;

public:
    explicit User(user::id id, std::string name, std::string username, std::string email);

    [[nodiscard]]
    user::id id() const noexcept;

    [[nodiscard]]
    std::string_view name() const noexcept;

    [[nodiscard]]
    std::string_view username() const noexcept;

    [[nodiscard]]
    std::string_view email() const noexcept;

    friend std::ostream &operator<<(std::ostream &out, User const &user) noexcept;
};
