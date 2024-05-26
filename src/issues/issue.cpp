#include "issue.h"

using namespace issue;
using sc = std::chrono::system_clock;

exceptions::issue_done::issue_done() : std::domain_error("Cannot modify a done issue.") {}

exceptions::changelog_unordered::changelog_unordered() : std::domain_error("Changelog is unordered.") {}

void Issue::ensure_active() const {
    if (m_.status == Status::done) {
        throw exceptions::issue_done();
    }
}

void Issue::apply(const events::CreatedIssue &evt) {
    if (!changelog_.empty()) {
        throw exceptions::changelog_unordered();
    }

    m_ = M{
        .id = evt.id,
        .project_id = evt.project_id,
        .creator_id = evt.creator_id,
        .name = evt.name,
        .description = evt.description,
        .parent_id = evt.parent_id,
        .assignee_id = evt.assignee_id,
        .status = evt.status,
        .priority = evt.priority
    };
}

void Issue::apply(const events::AssignedTo &evt) {
    ensure_active();
    m_.assignee_id = evt.new_assignee_id;
}

void Issue::apply(const events::RemovedAssignee &) {
    ensure_active();
    m_.assignee_id = std::nullopt;
}

void Issue::apply(const events::AddedToProject &evt) {
    ensure_active();
    m_.project_id = evt.project_id;
}

void Issue::apply(const events::ChangedPriority &evt) {
    ensure_active();
    m_.priority = evt.new_priority;
}

void Issue::apply(const events::ChangedStatus &evt) {
    m_.status = evt.new_status;
}

Issue::Issue(Issue::M const &m) : m_(m), changelog_(events::Changelog()) {
    changelog_.emplace_back<events::CreatedIssue>(
        {
            .id = m.id,
            .project_id = m.project_id,
            .creator_id = m.creator_id,
            .name = m.name,
            .description = m.description,
            .parent_id = m.parent_id,
            .assignee_id = m.assignee_id,
            .status = m.status,
            .priority = m.priority
        });
}

Issue Issue::from_changelog(const events::Changelog &changelog) {
    auto issue = Issue();

    for (auto const &evt: changelog) {
        std::visit(
            [&issue](auto const &evt) { return issue.apply(evt); },
            evt);
    }

    for (auto const &evt: changelog) {
        issue.changelog_.emplace_back(evt);
    }

//    std::copy(changelog.begin(), changelog.end(), std::back_inserter(issue.changelog_));

    return issue;
}

issue::id Issue::id() const noexcept { return m_.id; }

project::id Issue::project_id() const noexcept { return m_.project_id; }

user::id Issue::creator_id() const noexcept { return m_.creator_id; }

std::string Issue::name() const noexcept { return m_.name; }

std::string Issue::description() const noexcept { return m_.description; }

std::optional<issue::id> const &Issue::parent_id() const noexcept { return m_.parent_id; }

std::optional<user::id> const &Issue::assignee_id() const noexcept { return m_.assignee_id; }

Status Issue::status() const noexcept { return m_.status; }

Priority Issue::priority() const noexcept { return m_.priority; }

events::Changelog const &Issue::changelog() const noexcept { return changelog_; }

void Issue::assign_to(user::id const &assignee_id) {
    // if issue is assigned to same person, ignore the request
    if (m_.assignee_id == assignee_id) { return; }

    changelog_.emplace_back<events::AssignedTo>({.new_assignee_id = assignee_id});
}

void Issue::remove_assignee() {
    // if issue is already unassigned, ignore the request
    if (!m_.assignee_id) { return; }

    changelog_.emplace_back<events::RemovedAssignee>({});
}

void Issue::add_to_project(project::id const &project_id) {
    // if issue is reassigned to the same project, ignore the request
    if (m_.project_id == project_id) { return; }

    changelog_.emplace_back<events::AddedToProject>({.project_id=project_id});
}

void Issue::set_priority(Priority const &priority) {
    changelog_.emplace_back<events::ChangedPriority>(
        {
            .initial_priority = m_.priority,
            .new_priority = priority
        });
}

void Issue::set_status(Status const &status) {
    changelog_.emplace_back<events::ChangedStatus>(
        {
            .initial_status = m_.status,
            .new_status = status
        });
}
