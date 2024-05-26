#include "project.h"

Project::Project(project::id id, workspace::id workspace_id, std::string name) :
    id_(id), workspace_id_(workspace_id), name_(std::move(name)) {}

project::id Project::id() const noexcept { return id_; }

workspace::id Project::workspace_id() const noexcept { return workspace_id_; }

std::string_view Project::name() const noexcept { return name_; }

std::ostream &operator<<(std::ostream &out, const Project &project) noexcept {
    out << "[Project]" << "\n\t"
        << "id: " << project.id_.val() << "\n\t"
        << "workspace_id: " << project.workspace_id_.val() << "\n\t"
        << "name: " << project.name_;
    return out;
}
