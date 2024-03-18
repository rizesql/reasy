//
// Created by Rizescu Iulian Stefan on 17.03.2024.
//

#ifndef OOP_ISSUE_H
#define OOP_ISSUE_H

#include <string>
#include <optional>
#include <chrono>
#include <utility>
#include <variant>
#include <vector>
#include <expected>
#include <ranges>
#include <iostream>

using sc = std::chrono::system_clock;

// region Status

enum class Status {
    backlog, todo, in_progress, done, cancelled
};

std::ostream &operator<<(std::ostream &out, Status const &status) noexcept {
    if (status == Status::backlog) out << "backlog";
    else if (status == Status::todo) out << "todo";
    else if (status == Status::in_progress) out << "in progress";
    else if (status == Status::done) out << "done";
    else if (status == Status::cancelled) out << "cancelled";
    else std::unreachable();

    return out;
}

// endregion

// region Priority

enum class Priority {
    none, urgent, high, medium, low
};

std::ostream &operator<<(std::ostream &out, Priority const &priority) noexcept {
    if (priority == Priority::none) out << "none";
    else if (priority == Priority::urgent) out << "urgent";
    else if (priority == Priority::high) out << "high";
    else if (priority == Priority::medium) out << "medium";
    else if (priority == Priority::low) out << "low";
    else std::unreachable();

    return out;
}

// endregion

class Issue {
private:
    struct M {
        unsigned int id{};
        unsigned int project_id{};
        unsigned int creator_id{};

        std::string name{};
        std::string description{};

        std::optional<unsigned int> parent_id{std::nullopt};
        std::optional<unsigned int> assignee_id{std::nullopt};
        Status status{Status::todo};
        Priority priority{Priority::medium};
        sc::time_point created_at{sc::now()};
    };

public:
    enum Errors {
        issue_done,
        events_unordered
    };

    // region Events

    struct CreatedIssue {
        M m{};

        friend std::ostream &operator<<(std::ostream &out, CreatedIssue const &evt) noexcept {
            out << evt.m.creator_id << " Created Issue";
            return out;
        }
    };

    struct AssignedTo {
        unsigned int new_assignee_id{};
        sc::time_point created_at{sc::now()};

        friend std::ostream &operator<<(std::ostream &out, AssignedTo const &evt) noexcept {
            out << "Issue was assigned to: " << evt.new_assignee_id;
            return out;
        }
    };

    struct RemovedAssignee {
        sc::time_point created_at{sc::now()};

        friend std::ostream &operator<<(std::ostream &out, RemovedAssignee const &) noexcept {
            out << "Removed assignee";
            return out;
        }
    };

    struct AddedToProject {
        unsigned int project_id{};
        sc::time_point created_at{sc::now()};

        friend std::ostream &operator<<(std::ostream &out, AddedToProject const &evt) noexcept {
            out << "Issue was added to project " << evt.project_id;
            return out;
        }
    };

    struct ChangedPriority {
        Priority initial_priority{};
        Priority new_priority{};
        sc::time_point created_at{sc::now()};

        friend std::ostream &operator<<(std::ostream &out, ChangedPriority const &evt) noexcept {
            out << "Changed priority from " << evt.initial_priority << " to " << evt.new_priority;
            return out;
        }
    };

    struct ChangedStatus {
        Status initial_status{};
        Status new_status{};
        sc::time_point created_at{sc::now()};

        friend std::ostream &operator<<(std::ostream &out, ChangedStatus const &evt) noexcept {
            out << "Changed priority from " << evt.initial_status << " to " << evt.new_status;
            return out;
        }
    };

    using Event = std::variant<
        CreatedIssue,
        AssignedTo,
        RemovedAssignee,
        AddedToProject,
        ChangedPriority,
        ChangedStatus>;

    // endregion Events

private:
    using Changelog = std::vector<Event>;

    M m_;
    Changelog changelog_;

    // An empty constructor is needed for reconstructing an issue from queried events which is an internal
    // facing api, so this constructor should remain private
    Issue() = default;

    // region Apply

    std::expected<void, Errors> apply(CreatedIssue const &evt) noexcept {
        if (!changelog_.empty()) {
            return std::unexpected{Errors::events_unordered};
        }

        m_ = evt.m;
        return {};
    }

    std::expected<void, Errors> apply(AssignedTo const &evt) noexcept {
        if (m_.status == Status::done) {
            return std::unexpected{Errors::issue_done};
        }

        // if issue is assigned to same person, ignore the request
        if (m_.assignee_id == evt.new_assignee_id) return {};

        m_.assignee_id = evt.new_assignee_id;
        return {};
    }

    std::expected<void, Errors> apply(RemovedAssignee const &) noexcept {
        if (m_.status == Status::done) {
            return std::unexpected{Errors::issue_done};
        }

        // if issue is already unassigned, ignore the request
        if (!m_.assignee_id) return {};

        m_.assignee_id = std::nullopt;
        return {};
    }

    std::expected<void, Errors> apply(AddedToProject const &evt) noexcept {
        if (m_.status == Status::done) {
            return std::unexpected{Errors::issue_done};
        }

        // if issue is reassigned to the same project, ignore the request
        if (m_.project_id == evt.project_id) return {};

        m_.project_id = evt.project_id;
        return {};
    }

    std::expected<void, Errors> apply(ChangedPriority const &evt) noexcept {
        if (m_.status == Status::done) {
            return std::unexpected{Errors::issue_done};
        }

        m_.priority = evt.new_priority;
        return {};
    }

    std::expected<void, Errors> apply(ChangedStatus const &evt) noexcept {
        m_.status = evt.new_status;

        return {};
    }

    // endregion Apply

public:
    explicit Issue(M const &m) : m_(m), changelog_(std::vector<Event>()) {
        changelog_.emplace_back<CreatedIssue>({m});
    }

    Issue(Issue const &src) : m_(src.m_), changelog_(src.changelog_) {}

    Issue &operator=(Issue const &src) {
        m_ = src.m_;
        changelog_ = src.changelog_;

        return *this;
    }

    ~Issue() = default;

    static std::expected<Issue, Errors> from_activity(Changelog const &changelog) noexcept {
        auto issue = Issue();

        for (auto const &evt: changelog) {
            auto const res = std::visit(
                [&issue](auto const &evt) { return issue.apply(evt); },
                evt);

            if (!res) {
                return std::unexpected{res.error()};
            }
        }

        issue.changelog_ = changelog;

        return issue;
    }

    [[nodiscard]] auto id() const noexcept { return m_.id; }

    [[nodiscard]] auto project_id() const noexcept { return m_.project_id; }

    [[nodiscard]] auto creator_id() const noexcept { return m_.creator_id; }

    [[nodiscard]] std::string_view name() const noexcept { return m_.name; }

    [[nodiscard]] std::string_view description() const noexcept { return m_.description; }

    [[nodiscard]] auto const &parent_id() const noexcept { return m_.parent_id; }

    [[nodiscard]] auto const &assignee_id() const noexcept { return m_.assignee_id; }

    [[nodiscard]] auto status() const noexcept { return m_.status; }

    [[nodiscard]] auto priority() const noexcept { return m_.priority; }

    [[nodiscard]] auto const &changelog() const noexcept { return changelog_; }

    std::expected<void, Errors> assign_to(unsigned int assignee_id) noexcept {
        auto const &evt = AssignedTo{.new_assignee_id = assignee_id};

        if (auto const &res = apply(evt); !res) {
            return std::unexpected(res.error());
        }

        changelog_.emplace_back(evt);
        return {};
    }

    std::expected<void, Errors> remove_assignee() noexcept {
        auto const &evt = RemovedAssignee{};

        if (auto const &res = apply(evt); !res) {
            return std::unexpected(res.error());
        }

        changelog_.emplace_back(evt);
        return {};
    }

    std::expected<void, Errors> add_to_project(unsigned int project_id) noexcept {
        auto const &evt = AddedToProject{project_id};

        if (auto const &res = apply(evt); !res) {
            return std::unexpected(res.error());
        }

        changelog_.emplace_back(evt);
        return {};
    }

    std::expected<void, Errors> set_priority(Priority const &priority) noexcept {
        auto const &evt = ChangedPriority{.initial_priority = m_.priority, .new_priority = priority};

        if (auto const &res = apply(evt); !res) {
            return std::unexpected(res.error());
        }

        changelog_.emplace_back(evt);
        return {};
    }

    std::expected<void, Errors> set_status(Status const &status) noexcept {
        auto const &evt = ChangedStatus{.initial_status = m_.status, .new_status = status};

        if (const auto &res = apply(evt); !res) {
            return std::unexpected(res.error());
        }

        changelog_.emplace_back(evt);
        return {};
    }

    friend std::ostream &operator<<(std::ostream &out, std::vector<Event> const &changelog) noexcept {
        out << "[Issue changelog]" << "\n\t";
        for (auto idx = 0; auto const &event: changelog) {
            std::visit(
                [&](auto const &evt) { out << idx << ". " << evt << "\n\t"; },
                event);
            idx++;
        }

        return out;
    }

    friend std::ostream &operator<<(std::ostream &out, Issue const &issue) noexcept {
        out << "[Issue]" << "\n\t"
            << "id: " << issue.m_.id << "\n\t"
            << "project_id: " << issue.m_.project_id << "\n\t"
            << "creator_id: " << issue.m_.creator_id << "\n\t"
            << "name: " << issue.m_.name << "\n\t"
            << "description: " << issue.m_.description << "\n\t";

        if (issue.m_.parent_id) {
            out << "parent_id: " << issue.m_.parent_id.value() << "\n\t";
        }

        out << "assignee_id: ";
        if (issue.m_.assignee_id) {
            out << issue.m_.assignee_id.value() << "\n\t";
        } else {
            out << "unassigned" << "\n\t";
        }

        out << "status: " << issue.m_.status << "\n\t"
            << "priority: " << issue.m_.priority;

        return out;
    }
};

#endif //OOP_ISSUE_H
