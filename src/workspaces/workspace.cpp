#include "workspace.h"

Workspace::Workspace(workspace::id id, user::id admin_id, std::string name)
    : id_{id}, admin_id_{admin_id}, name_(std::move(name)) {}

workspace::id Workspace::id() const noexcept { return id_; }

user::id Workspace::admin_id() const noexcept { return admin_id_; }

std::string_view Workspace::name() const noexcept { return name_; }

std::ostream &operator<<(std::ostream &out, const Workspace &workspace) noexcept {
    out << "[Workspace]" << "\n\t"
        << "id: " << workspace.id_.val() << "\n\t"
        << "admin_id: " << workspace.admin_id_.val() << "\n\t"
        << "name: " << workspace.name_;
    return out;
}
