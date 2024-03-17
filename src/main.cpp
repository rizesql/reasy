#include <iostream>
#include <random>

#include "issue.h"
#include "project.h"
#include "user.h"
#include "workspace.h"

int main() {
    auto id = std::random_device();

    auto admin = User(id(), "Admin", "@admin", "admin@mail.com");
    auto rand_user = User(id(), "Rand user", "@rand.user343343", "rand.user@mail.com");

    auto workspace = Workspace(id(), admin.id(), "Workspace");
    auto project = Project(id(), workspace.id(), "Project 1");

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

    if (const auto &res = issue.assign_to(rand_user.id()); !res) {
        std::cout << res.error() << std::endl;
        return 0;
    }

    if (const auto &res = issue.remove_assignee(); !res) {
        std::cout << res.error() << std::endl;
        return 0;
    }

    if (const auto &res = issue.assign_to(admin.id()); !res) {
        std::cout << res.error() << std::endl;
        return 0;
    }

    if (const auto &res = issue.set_priority(Priority::urgent); !res) {
        std::cout << res.error() << std::endl;
        return 0;
    }

    if (const auto &res = issue.set_status(Status::done); !res) {
        std::cout << res.error() << std::endl;
        return 0;
    };

    std::cout << issue.activity() << std::endl;
    std::cout << issue << std::endl << std::endl;

    const auto activity = std::vector<Issue::Event>{
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
        Issue::ChangedPriority{Priority::low, Priority::urgent},
        Issue::ChangedStatus{Status::in_progress, Status::done}
    };
    if (const auto &new_issue = Issue::from_activity(activity); !new_issue) {
        std::cout << new_issue.error() << std::endl;
    } else {
        std::cout << "this" << std::endl;
        std::cout << new_issue.value().activity() << std::endl;
        std::cout << new_issue.value() << std::endl;
    }

    return 0;
}
