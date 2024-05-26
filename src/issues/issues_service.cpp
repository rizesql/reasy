#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "core/json.h"
#include "google/rpc/code.pb.h"
#include "issues_service.h"

struct issues_queries {
    static constexpr std::string_view get_changelog = R"(
        select id, issue_id, version, type, data
        from changelog
        where issue_id = ?
        order by version;
    )";

    static constexpr std::string_view insert_event = R"(
        insert into changelog(id, issue_id, version, type, data, created_by, created_at)
        values (?, ?, ?, ?, ?, ?, ?);
    )";
};

namespace api::issues {
    Rpc::Rpc(sqlite::database db) : rpc_t(), db_{std::move(db)} {}

    using row_t = std::tuple<std::string, std::string, int, std::string, std::string>;

    void map(Issue const &from, reasy::api::v1::Issue *const to) {
        to->set_id(from.id().val().string());
        to->set_project_id(from.project_id().val().string());
        to->set_creator_id(from.creator_id().val().string());
        to->set_name(from.name());
        to->set_description(from.description());
        if (from.parent_id()) {
            to->set_parent_id(from.parent_id().value().val().string());
        }
        if (from.assignee_id()) {
            to->set_assignee_id(from.assignee_id().value().val().string());
        }

        to->set_status(reasy::api::v1::Status(from.status().to_int()));
        to->set_priority(reasy::api::v1::Priority(from.priority().to_int()));
    }

    template<>
    rpcGet::result_type Rpc::call<rpcGet>(grpcxx::context &, const typename rpcGet::request_type &req) {
        auto changelog = issue::events::Changelog();

        for (row_t row: db_ << issues_queries::get_changelog << req.issue_id()) {
            nlohmann::json j = get<4>(row);
            spdlog::info("{}", j.dump(4));
            changelog.emplace_back(j);
        }

        auto const issue = Issue::from_changelog(changelog);
        auto response = reasy::api::v1::IssuesGetResponse();
        map(issue, response.mutable_issue());

        return {grpcxx::status::code_t::ok, response};
    }

    template<>
    rpcCreate::result_type Rpc::call<rpcCreate>(grpcxx::context &ctx, rpcCreate::request_type const &req) {
        auto evt = issue::events::CreatedIssue{
            .id = issue::id(),
            .project_id = project::id(req.project_id()),
            .creator_id = user::id(req.creator_id()),
            .name = req.name(),
            .description = req.description(),
            .parent_id = req.has_parent_id() ? std::optional(issue::id(req.parent_id())) : std::nullopt,
            .assignee_id = req.has_assignee_id() ? std::optional(user::id(req.assignee_id())) : std::nullopt,
            .status = Status::from(req.has_status() ? req.status() : 0).value_or(Status::todo),
            .priority = Priority::from(req.has_priority() ? req.priority() : 0).value_or(Priority::none),
            .metadata = {.created_at = sc::now()}
        };

        nlohmann::json j = evt;

        db_ << "begin;";
        db_ << issues_queries::insert_event << xid::next().string() << evt.id.val().string() << 0
            << "issue::CreatedIssue" << j.dump()
            << evt.creator_id.val().string()
            << sc::now().time_since_epoch().count();
        db_ << "commit;";

        return {grpcxx::status::code_t::ok};
    }

    template<>
    rpcAssign::result_type Rpc::call<rpcAssign>(grpcxx::context &ctx, rpcAssign::request_type const &res) {
        return {grpcxx::status::code_t::unimplemented, std::nullopt};
    }

    template<>
    rpcRemoveAssignee::result_type Rpc::call<rpcRemoveAssignee>(
        grpcxx::context &,
        rpcRemoveAssignee::request_type const &) {
        return {grpcxx::status::code_t::unimplemented, std::nullopt};
    }

    template<>
    rpcAddToProject::result_type Rpc::call<rpcAddToProject>(grpcxx::context &, rpcAddToProject::request_type const &) {
        return {grpcxx::status::code_t::unimplemented, std::nullopt};
    }

    template<>
    rpcChangePriority::result_type Rpc::call<rpcChangePriority>(
        grpcxx::context &,
        rpcChangePriority::request_type const &req) {
        auto evt = issue::events::ChangedPriority{
            .new_priority = Priority::from(req.new_priority()).value_or(Priority::none),
            .metadata = {.created_at=sc::now()}
        };
        nlohmann::json j = evt;

        db_ << "begin;";
        db_ << issues_queries::insert_event << xid::next().string() << req.issue_id() << 1 << "issue::ChangedPriority"
            << j.dump() << xid::next().string() << sc::now().time_since_epoch().count();
        db_ << "commit;";

        return {grpcxx::status::code_t::ok, std::nullopt};
    }

    template<>
    rpcChangeStatus::result_type Rpc::call<rpcChangeStatus>(grpcxx::context &, rpcChangeStatus::request_type const &) {
        return {grpcxx::status::code_t::unimplemented, std::nullopt};
    }

    google::rpc::Status Rpc::exception() noexcept {
        google::rpc::Status status;
        status.set_code(google::rpc::UNKNOWN);

        try {
            std::rethrow_exception(std::current_exception());
        } catch (std::exception const &e) {
            spdlog::error("{}", e.what());
            status.set_code(google::rpc::INTERNAL);
            status.set_message(e.what());
        }

        return status;
    }
}