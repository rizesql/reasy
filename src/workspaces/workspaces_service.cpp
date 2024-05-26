#include "workspaces_service.h"
#include "workspace.h"

struct workspaces_queries {
    constexpr static std::string_view get_all = R"(
        select id, admin_id, name from workspaces;
    )";

    constexpr static std::string_view get = R"(
        select id, admin_id, name
        from workspaces
        where id = ?;
    )";

    constexpr static std::string_view exists = R"(
        select count(*)
        from workspaces
        where id = ?;
    )";

    constexpr static std::string_view check_unique = R"(
        select id
        from workspaces
        where name = ?;
    )";

    constexpr static std::string_view create = R"(
        insert into workspaces(id, admin_id, name)
        values(?, ?, ?);
    )";

    constexpr static std::string_view change_admin_id = R"(
        update workspaces
        set admin_id = ?
        where id = ?;
    )";

    constexpr static std::string_view change_name = R"(
        update workspaces
        set name = ?
        where id = ?;
    )";

    constexpr static std::string_view delete_workspace = R"(
        delete from workspaces
        where id = ?;
    )";
};

namespace api::workspaces {
    Rpc::Rpc(sqlite::database db) : rpc_t(), db_{std::move(db)} {}

    template<>
    rpcGetAll::result_type Rpc::call<rpcGetAll>(grpcxx::context &, rpcGetAll::request_type const &) {
        auto response = reasy::api::v1::WorkspacesGetAllResponse();

        for (auto &&row: db_ << workspaces_queries::get_all) {
            map(row, response.add_workspaces());
        }

        return {grpcxx::status::code_t::ok, response};
    }

    template<>
    rpcGet::result_type Rpc::call<rpcGet>(grpcxx::context &, rpcGet::request_type const &req) {
        if (req.id().empty()) {
            return {grpcxx::status::code_t::invalid_argument};
        }

        auto response = reasy::api::v1::Workspace();
        auto found = false;
        for (auto row: db_ << workspaces_queries::get << req.id()) {
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
        if (req.name().empty() || req.admin_id().empty()) {
            return {grpcxx::status::code_t::invalid_argument};
        }

        auto id = workspace::id();
        for (auto &&row: db_ << workspaces_queries::check_unique << req.name()) {
            return {grpcxx::status::code_t::already_exists};
        }

        db_ << "begin;";
        db_ << workspaces_queries::create << id.val().string() << req.admin_id() << req.name();
        db_ << "commit;";

        auto response = reasy::api::v1::Workspace();
        response.set_id(id.val().string());
        response.set_admin_id(req.admin_id());
        response.set_name(req.name());

        return {grpcxx::status::code_t::ok, response};
    }

    template<>
    rpcChangeName::result_type Rpc::call<rpcChangeName>(grpcxx::context &, rpcChangeName::request_type const &req) {
        if (req.id().empty() || req.new_name().empty()) {
            return {grpcxx::status::code_t::invalid_argument};
        }

        auto rows_count = 0;
        db_ << workspaces_queries::exists << req.id() >> rows_count;

        if (rows_count == 0) {
            return {grpcxx::status::code_t::not_found};
        }

        db_ << "begin;";
        db_ << workspaces_queries::change_name << req.new_name() << req.id();
        db_ << "commit;";

        return {grpcxx::status::code_t::ok};
    }

    template<>
    rpcChangeAdmin::result_type Rpc::call<rpcChangeAdmin>(grpcxx::context &, rpcChangeAdmin::request_type const &req) {
        if (req.id().empty() || req.new_admin_id().empty()) {
            return {grpcxx::status::code_t::invalid_argument};
        }

        auto rows_count = 0;
        db_ << workspaces_queries::exists << req.id() >> rows_count;

        if (rows_count == 0) {
            return {grpcxx::status::code_t::not_found};
        }

        db_ << "begin;";
        db_ << workspaces_queries::change_admin_id << req.new_admin_id() << req.id();
        db_ << "commit;";

        return {grpcxx::status::code_t::ok};
    }

    template<>
    rpcDelete::result_type Rpc::call<rpcDelete>(grpcxx::context &, rpcDelete::request_type const &req) {
        if (req.id().empty()) {
            return {grpcxx::status::code_t::invalid_argument};
        }

        auto rows_count = 0;
        db_ << workspaces_queries::exists << req.id() >> rows_count;

        if (rows_count == 0) {
            return {grpcxx::status::code_t::not_found};
        }

        db_ << "begin;";
        db_ << workspaces_queries::delete_workspace << req.id();
        db_ << "commit;";

        return {grpcxx::status::code_t::ok};
    }

    void Rpc::map(const api::workspaces::row_t &from, reasy::api::v1::Workspace *to) noexcept {
        to->set_id(get<0>(from));
        to->set_admin_id(get<1>(from));
        to->set_name(get<2>(from));
    }
}