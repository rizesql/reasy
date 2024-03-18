#include <iostream>
#include <random>

#include "issue.h"
#include "project.h"
#include "user.h"
#include "workspace.h"

int main() {
    auto id = std::random_device();

    auto const admin = User(id(), "Admin", "@admin", "admin@mail.com");
    std::cout << admin << "\n\t"
              << admin.id() << " " << admin.name() << " " << admin.username() << " " << admin.email()
              << "\n";

    auto const rand_user = User(id(), "Rand user", "@rand.user343343", "rand.user@mail.com");

    auto const workspace = Workspace(id(), admin.id(), "Workspace");
    std::cout << workspace << "\n\t"
              << workspace.id() << " " << workspace.admin_id() << " " << workspace.name()
              << "\n";

    auto const project = Project(id(), workspace.id(), "Project 1");
    std::cout << project << "\n\t"
              << project.id() << " " << project.name() << " " << project.workspace_id()
              << "\n";

    auto issue = Issue(
        {
            id(),
            project.id(),
            admin.id(),
            "Issue #1",
            "Lorem ipsum dolor sit amet",
            std::nullopt,
            std::nullopt,
            Status::in_progress,
            Priority::low
        });

    std::cout << "(initial) " << issue << "\n\t"
              << issue.id() << " " << issue.project_id() << " " << issue.creator_id() << " "
              << issue.name() << " " << issue.description() << " " << issue.parent_id().value_or(0) << " "
              << issue.assignee_id().value_or(0) << " " << issue.status() << " " << issue.priority()
              << "\n\n";

    if (auto const &res = issue.assign_to(rand_user.id()); !res) {
        std::cout << res.error() << "\n";
        return 0;
    }

    if (auto const &res = issue.remove_assignee(); !res) {
        std::cout << res.error() << "\n";
        return 0;
    }

    if (auto const &res = issue.assign_to(admin.id()); !res) {
        std::cout << res.error() << "\n";
        return 0;
    }

    if (auto const &res = issue.add_to_project(project.id()); !res) {
        std::cout << res.error() << "\n";
        return 0;
    }

    if (auto const &res = issue.set_priority(Priority::urgent); !res) {
        std::cout << res.error() << "\n";
        return 0;
    }

    if (auto const &res = issue.set_status(Status::done); !res) {
        std::cout << res.error() << "\n";
        return 0;
    }

    std::cout << issue << "\n" << issue.changelog() << "\n\n";

    auto const activity = std::vector<Issue::Event>{
        Issue::CreatedIssue{
            issue.id(),
            project.id(),
            admin.id(),
            "Issue #1",
            "Lorem ipsum dolor sit amet",
            std::nullopt,
            std::nullopt,
            Status::in_progress,
            Priority::low
        },
        Issue::AssignedTo{rand_user.id()},
        Issue::RemovedAssignee{},
        Issue::AssignedTo{admin.id()},
        Issue::AddedToProject{project.id()},
        Issue::ChangedPriority{Priority::low, Priority::urgent},
        Issue::ChangedStatus{Status::in_progress, Status::done}
    };

    if (auto const &issue_r = Issue::from_activity(activity); !issue_r) {
        std::cout << issue_r.error() << "\n";
    } else {
        auto const &new_issue = issue_r.value();
        std::cout << new_issue << "\n" << new_issue.changelog() << "\n";
    }

    return 0;
}
