#pragma once

#include <nlohmann/json.hpp>
#include "core/json.h"
#include "users/user.h"
#include "projects/project.h"
#include "status.h"
#include "priority.h"
#include "id.h"

using sc = std::chrono::system_clock;

namespace issue::events {
    struct Metadata {
        user::id created_by{};
        sc::time_point created_at{};
    };

    struct CreatedIssue {
        issue::id id{};
        project::id project_id{};
        user::id creator_id{};

        std::string name{};
        std::string description{};

        std::optional<issue::id> parent_id{std::nullopt};
        std::optional<user::id> assignee_id{std::nullopt};
        Status status{Status::todo};
        Priority priority{Priority::medium};

        Metadata metadata{};
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CreatedIssue, id, project_id, creator_id, name, description, parent_id,
                                       assignee_id, status, priority);


    struct AssignedTo {
        user::id new_assignee_id{};
        Metadata metadata{};
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AssignedTo, new_assignee_id);

    struct RemovedAssignee {
        user::id previous_assignee_id{};
        Metadata metadata{};
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RemovedAssignee, previous_assignee_id);

    struct AddedToProject {
        project::id project_id{};
        Metadata metadata{};
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AddedToProject, project_id);

    struct ChangedPriority {
        Priority initial_priority{};
        Priority new_priority{};
        Metadata metadata{};
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ChangedPriority, initial_priority, new_priority);

    struct ChangedStatus {
        Status initial_status{};
        Status new_status{};
        Metadata metadata{};
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ChangedStatus, initial_status, new_status);

    using Event = std::variant<
        CreatedIssue,
        AssignedTo,
        RemovedAssignee,
        AddedToProject,
        ChangedPriority,
        ChangedStatus>;

    using Changelog = std::vector<Event const>;
}
