#include "projects_service.h"
#include "project.h"
#include "google/rpc/code.pb.h"

struct projects_queries {
    constexpr static std::string_view get_all = R"(
        select id, workspace_id, name
        from projects;
    )";

    constexpr static std::string_view get = R"(
        select id, workspace_id, name
        from projects
        where id = ?;
    )";

    constexpr static std::string_view exists = R"(
        select count(*)
        from projects
        where id = ?;
    )";

    constexpr static std::string_view check_unique = R"(
        select id
        from projects
        where name = ? and workspace_id = ?;
    )";

    constexpr static std::string_view create = R"(
        insert into projects(id, workspace_id, name)
        values (?, ?, ?);
    )";

    constexpr static std::string_view change_name = R"(
        update projects set name = ?
        where id = ?;
    )";

    constexpr static std::string_view delete_project = R"(
        delete from projects
        where id = ?;
    )";
};

namespace api::projects {
    Rpc::Rpc(sqlite::database db) : rpc_t(), db_{std::move(db)} {}

    template<>
    rpcGetAll::result_type Rpc::call<rpcGetAll>(grpcxx::context &, rpcGetAll::request_type const &req) {
        auto response = reasy::api::v1::ProjectsGetAllResponse();

        for (auto &&row: db_ << projects_queries::get_all) {
            map(row, response.add_projects());
        }

        return {grpcxx::status::code_t::ok, response};
    }

    template<>
    rpcGet::result_type Rpc::call<rpcGet>(grpcxx::context &, rpcGet::request_type const &req) {
        if (req.id().empty()) {
            return {grpcxx::status::code_t::invalid_argument};
        }

        auto response = reasy::api::v1::Project();
        auto found = false;
        for (auto row: db_ << projects_queries::get << req.id()) {
            map(row, &response);
            found = true;
            break;
        }

        if (!found) {
            return {grpcxx::status::code_t::not_found};
        }

        return {grpcxx::status::code_t::ok, response};
    }

    template<>
    rpcCreate::result_type Rpc::call<rpcCreate>(grpcxx::context &, rpcCreate::request_type const &req) {
        if (req.name().empty() || req.workspace_id().empty()) {
            return {grpcxx::status::code_t::invalid_argument};
        }

        auto id = project::id();
        for (auto &&row: db_ << projects_queries::check_unique << req.name() << req.workspace_id()) {
            return {grpcxx::status::code_t::already_exists};
        }

        db_ << "begin;";
        db_ << projects_queries::create << id.val().string() << req.workspace_id() << req.name();
        db_ << "commit;";

        auto response = reasy::api::v1::Project();
        response.set_id(id.val().string());
        response.set_workspace_id(req.workspace_id());
        response.set_name(req.name());

        return {grpcxx::status::code_t::ok, response};
    }

    template<>
    rpcChangeName::result_type Rpc::call<rpcChangeName>(grpcxx::context &, rpcChangeName::request_type const &req) {
        if (req.id().empty() || req.name().empty()) {
            return {grpcxx::status::code_t::invalid_argument};
        }

        auto rows_count = 0;
        db_ << projects_queries::exists << req.id() >> rows_count;

        if (rows_count == 0) {
            return {grpcxx::status::code_t::not_found};
        }

        db_ << "begin;";
        db_ << projects_queries::change_name << req.name() << req.id();
        db_ << "commit;";

        return {grpcxx::status::code_t::ok};
    }

    template<>
    rpcDelete::result_type Rpc::call<rpcDelete>(grpcxx::context &, rpcDelete::request_type const &req) {
        if (req.id().empty()) {
            return {grpcxx::status::code_t::invalid_argument};
        }

        auto rows_count = 0;
        db_ << projects_queries::exists << req.id() >> rows_count;

        if (rows_count == 0) {
            return {grpcxx::status::code_t::not_found};
        }

        db_ << "begin;";
        db_ << projects_queries::delete_project << req.id();
        db_ << "commit;";

        return {grpcxx::status::code_t::ok};
    }

    google::rpc::Status Rpc::exception() noexcept {
        google::rpc::Status status;
        status.set_code(google::rpc::UNKNOWN);

        try {
            std::rethrow_exception(std::current_exception());
        } catch (std::exception const &e) {
            status.set_code(google::rpc::INTERNAL);
            status.set_message(e.what());
        }

        return status;
    }

    void Rpc::map(row_t const &from, reasy::api::v1::Project *const to) noexcept {
        to->set_id(get<0>(from));
        to->set_workspace_id(get<1>(from));
        to->set_name(get<2>(from));
    }
}