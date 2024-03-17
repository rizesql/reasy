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

enum class Status {
    backlog, todo, in_progress, done, cancelled
};

std::ostream &operator<<(std::ostream &out, Status status) {
    if (status == Status::backlog) out << "backlog";
    else if (status == Status::todo) out << "todo";
    else if (status == Status::in_progress) out << "in progress";
    else if (status == Status::done) out << "done";
    else if (status == Status::cancelled) out << "cancelled";
    else std::unreachable();

    return out;
}

enum class Priority {
    none, urgent, high, medium, low
};

std::ostream &operator<<(std::ostream &out, Priority priority) {
    if (priority == Priority::none) out << "none";
    else if (priority == Priority::urgent) out << "urgent";
    else if (priority == Priority::high) out << "high";
    else if (priority == Priority::medium) out << "medium";
    else if (priority == Priority::low) out << "low";
    else std::unreachable();

    return out;
}

class Issue {
public:
    enum Errors {
        issue_done,
        events_unordered
    };

    struct CreatedIssue {
        unsigned int id{};
        unsigned int project_id{};
        unsigned int creator_id{};
        std::string name{};
        std::string description{};
        std::optional<unsigned int> assignee_id{std::nullopt};
        std::optional<unsigned int> parent_id{std::nullopt};
        Status status{Status::todo};
        Priority priority{Priority::low};
        sc::time_point created_at{sc::now()};

        friend std::ostream &operator<<(std::ostream &out, const CreatedIssue &evt) {
            out << evt.creator_id << " Created Issue \n";
            return out;
        }
    };

    struct AssignedTo {
        unsigned int new_assignee_id{};
        sc::time_point created_at{sc::now()};

        friend std::ostream &operator<<(std::ostream &out, const AssignedTo &evt) {
            out << "Issue was assigned to: " << evt.new_assignee_id << "\n";
            return out;
        }
    };

    struct RemovedAssignee {
        sc::time_point created_at{sc::now()};

        friend std::ostream &operator<<(std::ostream &out, const RemovedAssignee &) {
            out << "Removed assignee\n";
            return out;
        }
    };

    struct AddedToProject {
        unsigned int project_id{};
        sc::time_point created_at{sc::now()};

        friend std::ostream &operator<<(std::ostream &out, const AddedToProject &evt) {
            out << "Issue was added to project " << evt.project_id << "\n";
            return out;
        }
    };

    struct ChangedPriority {
        Priority initial_priority{};
        Priority new_priority{};
        sc::time_point created_at{sc::now()};

        friend std::ostream &operator<<(std::ostream &out, const ChangedPriority &evt) {
            out << "Changed priority from " << evt.initial_priority << " to " << evt.new_priority << "\n";
            return out;
        }
    };

    struct ChangedStatus {
        Status initial_status{};
        Status new_status{};
        sc::time_point created_at{sc::now()};

        friend std::ostream &operator<<(std::ostream &out, const ChangedStatus &evt) {
            out << "Changed priority from " << evt.initial_status << " to " << evt.new_status << "\n";
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

        [[nodiscard]] static M from(const CreatedIssue &evt) {
            return {
                evt.id,
                evt.project_id,
                evt.creator_id,
                evt.name,
                evt.description,
                evt.parent_id,
                evt.assignee_id,
                evt.status,
                evt.priority,
                evt.created_at
            };
        }
    } m_;

    using Changelog = std::vector<Event>;
    Changelog changelog_;

    Issue() = default;

    Issue(M m, Changelog changelog) : m_(std::move(m)), changelog_(std::move(changelog)) {}

    std::expected<void, Errors> apply(const CreatedIssue &evt) noexcept {
        if (!changelog_.empty()) {
            return std::unexpected{Errors::events_unordered};
        }

        m_ = M::from(evt);
        return {};
    }

    std::expected<void, Errors> apply(const AssignedTo &evt) noexcept {
        if (m_.status == Status::done) {
            return std::unexpected{Errors::issue_done};
        }

        // if issue is assigned to same person, ignore the request
        if (m_.assignee_id == evt.new_assignee_id) return {};

        m_.assignee_id = evt.new_assignee_id;
        return {};
    }

    std::expected<void, Errors> apply([[maybe_unused]] const RemovedAssignee &evt) noexcept {
        if (m_.status == Status::done) {
            return std::unexpected{Errors::issue_done};
        }

        // if issue is already unassigned, ignore the request
        if (!m_.assignee_id) return {};

        m_.assignee_id = std::nullopt;
        return {};
    }

    std::expected<void, Errors> apply(const AddedToProject &evt) noexcept {
        if (m_.status == Status::done) {
            return std::unexpected{Errors::issue_done};
        }

        // if issue is reassigned to the same project, ignore the request
        if (m_.project_id == evt.project_id) return {};

        m_.project_id = evt.project_id;
        return {};
    }

    std::expected<void, Errors> apply(const ChangedPriority &evt) noexcept {
        if (m_.status == Status::done) {
            return std::unexpected{Errors::issue_done};
        }

        m_.priority = evt.new_priority;
        return {};
    }

    std::expected<void, Errors> apply(const ChangedStatus &evt) noexcept {
        m_.status = evt.new_status;

        return {};
    }

public:
    explicit Issue(const M &m) : Issue(m, std::vector<Event>()) {
        changelog_.emplace_back<CreatedIssue>(
            {
                m.id,
                m.project_id,
                m.creator_id,
                m.name,
                m.description,
                m.assignee_id,
                m.parent_id,
                m.status,
                m.priority
            });
    }

    Issue(const Issue &src) : m_(src.m_), changelog_(src.changelog_) {}

    Issue &operator=(const Issue &src) {
        m_ = src.m_;
        changelog_ = src.changelog_;

        return *this;
    }

    ~Issue() = default;

    static std::expected<Issue, Errors> from_activity(const Changelog &changelog) {
        auto issue = Issue();

        for (auto &&evt: changelog) {
            const auto res = std::visit(
                [&issue](const auto &evt) { return issue.apply(evt); },
                evt);

            if (!res) {
                return std::unexpected{res.error()};
            }
        }

        return issue;
    }

    [[nodiscard]] auto id() const noexcept { return m_.id; }

    [[nodiscard]] auto project_id() const noexcept { return m_.project_id; }

    [[nodiscard]] auto creator_id() const noexcept { return m_.creator_id; }

    [[nodiscard]] std::string_view name() const noexcept { return m_.name; }

    [[nodiscard]] std::string_view description() const noexcept { return m_.description; }

    [[nodiscard]] const auto &parent_id() const noexcept { return m_.parent_id; }

    [[nodiscard]] const auto &assignee_id() const noexcept { return m_.assignee_id; }

    [[nodiscard]] auto status() const noexcept { return m_.status; }

    [[nodiscard]] auto priority() const noexcept { return m_.priority; }

    [[nodiscard]] const auto &activity() const noexcept { return changelog_; }

    std::expected<void, Errors> assign_to(unsigned int assignee_id) noexcept {
        const auto &evt = AssignedTo{.new_assignee_id = assignee_id};

        if (const auto &res = apply(evt); !res) {
            return std::unexpected(res.error());
        }

        changelog_.emplace_back(evt);
        return {};
    }

    std::expected<void, Errors> remove_assignee() noexcept {
        const auto &evt = RemovedAssignee{};

        if (const auto &res = apply(evt); !res) {
            return std::unexpected(res.error());
        }

        changelog_.emplace_back(evt);
        return {};
    }

    std::expected<void, Errors> add_to_project(unsigned int project_id) noexcept {
        const auto &evt = AddedToProject{project_id};

        if (const auto &res = apply(evt); !res) {
            return std::unexpected(res.error());
        }

        changelog_.emplace_back(evt);
        return {};
    }

    std::expected<void, Errors> set_priority(const Priority priority) noexcept {
        const auto &evt = ChangedPriority{.initial_priority = m_.priority, .new_priority = priority};

        if (const auto &res = apply(evt); !res) {
            return std::unexpected(res.error());
        }

        changelog_.emplace_back(evt);
        return {};
    }

    std::expected<void, Errors> set_status(Status status) noexcept {
        const auto &evt = ChangedStatus{.initial_status = m_.status, .new_status = status};

        if (const auto &res = apply(evt); !res) {
            return std::unexpected(res.error());
        }
        changelog_.emplace_back(evt);
        return {};
    }

    friend std::ostream &operator<<(std::ostream &out, const std::vector<Event> &activity) {
        for (auto idx = 0; const auto &event: activity) {
            std::visit(
                [&](const auto &e) { out << idx << ". " << e; },
                event);
            idx++;
        }

        return out;
    }

    friend std::ostream &operator<<(std::ostream &out, const Issue &issue) {
        out << "Issue"
            << "\n\tid: " << issue.m_.id
            << "\n\tproject_id: " << issue.m_.project_id
            << "\n\tcreator_id: " << issue.m_.creator_id
            << "\n\tname: " << issue.m_.name
            << "\n\tdescription: " << issue.m_.description;

        if (issue.m_.parent_id) {
            out << "\n\tparent_id: " << *issue.m_.parent_id;
        }

        out << "\n\tassignee_id: ";
        if (issue.m_.assignee_id) {
            out << *issue.m_.assignee_id;
        } else {
            out << "unassigned";
        }

        out << "\n\tstatus: " << issue.m_.status
            << "\n\tpriority: " << issue.m_.priority;

        return out;
    }
};

#endif //OOP_ISSUE_H
