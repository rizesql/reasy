//
// Created by Rizescu Iulian Stefan on 17.03.2024.
//

#ifndef OOP_WORKSPACE_H
#define OOP_WORKSPACE_H

#include <string>
#include <ostream>

class Workspace {
private:
    unsigned int id_;
    unsigned int admin_id_;
    std::string name_;

public:
    explicit Workspace(unsigned int id, unsigned int admin_id, std::string name)
        : id_{id}, admin_id_{admin_id}, name_(std::move(name)) {}

    [[nodiscard]] auto id() const noexcept { return id_; }

    [[nodiscard]] auto admin_id() const noexcept { return admin_id_; }

    [[nodiscard]] std::string_view name() const noexcept { return name_; }

    friend std::ostream &operator<<(std::ostream &out, Workspace const &workspace) noexcept {
        out << "[Workspace]" << "\n\t"
            << "id: " << workspace.id_ << "\n\t"
            << "admin_id: " << workspace.admin_id_ << "\n\t"
            << "name: " << workspace.name_;
        return out;
    }
};

#endif //OOP_WORKSPACE_H
