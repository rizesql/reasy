#pragma once

#include <string>
#include <ostream>
#include "../core/id.h"
#include "../users/user.h"

namespace workspace {
    using id = tid<"workspaces">;
}

class Workspace {
private:
    workspace::id id_;
    user::id admin_id_;
    std::string name_;

public:
    explicit Workspace(workspace::id id, user::id admin_id, std::string name);

    [[nodiscard]]
    workspace::id id() const noexcept;

    [[nodiscard]]
    user::id admin_id() const noexcept;

    [[nodiscard]]
    std::string_view name() const noexcept;

    friend std::ostream &operator<<(std::ostream &out, Workspace const &workspace) noexcept;
};
