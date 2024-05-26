#pragma once

#include <string>
#include <ostream>
#include "../core/id.h"
#include "../workspaces/workspace.h"

namespace project {
    using id = tid<"projects">;
}

class Project {
public:
    project::id id_;
    workspace::id workspace_id_;
    std::string name_;

public:
    explicit Project(project::id id, workspace::id workspace_id, std::string name);

    [[nodiscard]]
    project::id id() const noexcept;

    [[nodiscard]]
    workspace::id workspace_id() const noexcept;

    [[nodiscard]]
    std::string_view name() const noexcept;

    friend std::ostream &operator<<(std::ostream &out, Project const &project) noexcept;
};

