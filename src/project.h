//
// Created by Rizescu Iulian Stefan on 17.03.2024.
//

#ifndef OOP_PROJECT_H
#define OOP_PROJECT_H

#include <string>
#include <ostream>

class Project {
    unsigned int id_;
    unsigned int workspace_id_;
    std::string name_;

public:
    explicit Project(unsigned int id, unsigned int workspace_id, std::string name) :
        id_(id), workspace_id_(workspace_id), name_(std::move(name)) {}

    [[nodiscard]] auto id() const noexcept { return id_; }

    [[nodiscard]] auto workspace_id() const noexcept { return workspace_id_; };

    [[nodiscard]] std::string_view name() const noexcept { return name_; }

    friend std::ostream &operator<<(std::ostream &out, const Project &project) {
        out << "id_: " << project.id_
            << " workspace_id_: " << project.workspace_id_
            << " name_: " << project.name_;
        return out;
    }
};

#endif //OOP_PROJECT_H
