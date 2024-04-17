#pragma once

#include "events.h"

using sc = std::chrono::system_clock;

namespace issue::exceptions {
    struct issue_done : std::domain_error {
        issue_done();
    };

    struct changelog_unordered : std::domain_error {
        changelog_unordered();
    };
}


class Issue {
private:
    struct M {
        issue::id id{};
        project::id project_id{};
        user::id creator_id{};

        std::string name{};
        std::string description{};

        std::optional<issue::id> parent_id{std::nullopt};
        std::optional<user::id> assignee_id{std::nullopt};
        Status status{Status::todo};
        Priority priority{Priority::medium};
        sc::time_point created_at{sc::now()};
    };

    M m_;
    issue::events::Changelog changelog_;

    /// An empty constructor is needed for reconstructing an issue from queried events which is an internal
    /// facing api, so this constructor should remain private
    Issue() = default;

    /// A helper method to enforce the invariant that an issue is active before applying events
    void ensure_active() const;

    // region Apply

    void apply(issue::events::CreatedIssue const &evt);

    void apply(issue::events::AssignedTo const &evt);

    void apply(issue::events::RemovedAssignee const &);

    void apply(issue::events::AddedToProject const &evt);

    void apply(issue::events::ChangedPriority const &evt);

    void apply(issue::events::ChangedStatus const &evt);

    // endregion Apply

public:
    explicit Issue(M const &m);

    /// Constructor method to build an issue, given a changelog
    static Issue from_changelog(issue::events::Changelog const &changelog);

    [[nodiscard]]
    issue::id id() const noexcept;

    [[nodiscard]]
    project::id project_id() const noexcept;

    [[nodiscard]]
    user::id creator_id() const noexcept;

    [[nodiscard]]
    std::string name() const noexcept;

    [[nodiscard]]
    std::string description() const noexcept;

    [[nodiscard]]
    std::optional<issue::id> const &parent_id() const noexcept;

    [[nodiscard]]
    std::optional<user::id> const &assignee_id() const noexcept;

    [[nodiscard]]
    Status status() const noexcept;

    [[nodiscard]]
    Priority priority() const noexcept;

    [[nodiscard]]
    issue::events::Changelog const &changelog() const noexcept;

    /// Assign issue to assignee, internally applies `issue::events::AssignedTo`
    void assign_to(user::id const &assignee_id);

    /// Remove assignee from issue, internally applies `issue::events::RemovedAssignee`
    void remove_assignee();

    /// Add issue to a project, internally applies `issue::events::AddedToProject`
    void add_to_project(project::id const &project_id);

    /// Change priority, internally applies `issue::events::ChangedPriority`
    void set_priority(Priority const &priority);

    /// Change status, internally applies `issue::events::ChangedStatus`
    void set_status(Status const &status);
};
