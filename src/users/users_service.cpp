#include <google/rpc/code.pb.h>
#include <spdlog/spdlog.h>
#include "users_service.h"
#include "user.h"

struct users_queries {
    constexpr static std::string_view get_all = R"(
        select id, name, username, email from users;
    )";

    constexpr static std::string_view get = R"(
        select id, name, username, email
        from users
        where id = ?;
    )";

    constexpr static std::string_view get_by_email = R"(
        select id
        from users
        where email = ?;
    )";

    constexpr static std::string_view create = R"(
        insert into users (id, name, username, email)
        values (?, ?, ?, ?);
    )";

    constexpr static std::string_view update_name = R"(
        update users set name = ? where id = ?;
    )";

    constexpr static std::string_view update_username = R"(
        update users set username = ? where id = ?;
    )";

    constexpr static std::string_view delete_user = R"(
        delete from users where id = ?;
    )";
};

namespace api::users {
    Rpc::Rpc(sqlite::database db) : rpc_t(), db_{std::move(db)} {}

    template<>
    rpcGetAll::result_type Rpc::call<rpcGetAll>(grpcxx::context &ctx, rpcGetAll::request_type const &req) {
        auto response = reasy::api::v1::UsersGetAllResponse();

        for (auto &&row: db_ << users_queries::get_all) {
            map(row, response.add_users());
        }

        return {grpcxx::status::code_t::ok, response};
    }

    template<>
    rpcGet::result_type Rpc::call<rpcGet>(grpcxx::context &ctx, rpcGet::request_type const &req) {
        if (req.id().empty()) {
            return {grpcxx::status::code_t::invalid_argument};
        }

        auto response = reasy::api::v1::User();
        auto rows_count = 0uz;
        for (row_t row: db_ << users_queries::get << req.id()) {
            map(row, &response);
            ++rows_count;
            break;
        }

        if (rows_count == 0) {
            return {grpcxx::status::code_t::not_found};
        }

        return {grpcxx::status::code_t::ok, response};
    }

    template<>
    rpcCreate::result_type Rpc::call<rpcCreate>(grpcxx::context &ctx, rpcCreate::request_type const &req) {
        if (req.name().empty() || req.username().empty() || req.email().empty()) {
            return {grpcxx::status::code_t::invalid_argument};
        }

        auto id = user::id();
        for (row_t row: db_ << users_queries::get_by_email << req.email()) {
            return {grpcxx::status::code_t::already_exists};
        }

        try {
            db_ << "begin;";
            db_ << users_queries::create << id.val().string() << req.name() << req.username() << req.email();
            db_ << "commit;";
        } catch (sqlite::sqlite_exception const &e) {
            spdlog::error("{}", e.errstr());
            return {grpcxx::status::code_t::already_exists};
        }

        auto response = reasy::api::v1::User();
        response.set_id(id.val().string());
        response.set_name(req.name());
        response.set_username(req.username());
        response.set_email(req.email());

        return {grpcxx::status::code_t::ok, response};
    }

    template<>
    rpcChangeName::result_type Rpc::call<rpcChangeName>(grpcxx::context &ctx, rpcChangeName::request_type const &req) {
        if (req.id().empty() || req.new_name().empty()) {
            return {grpcxx::status::code_t::invalid_argument};
        }

        auto found = false;
        for (row_t row: db_ << users_queries::get << req.id()) {
            found = true;
            break;
        }

        if (!found) {
            return {grpcxx::status::code_t::not_found};
        }

        db_ << "begin;";
        db_ << users_queries::update_name << req.new_name() << req.id();
        db_ << "commit;";

        return {grpcxx::status::code_t::ok};
    }

    template<>
    rpcChangeUsername::result_type Rpc::call<rpcChangeUsername>(
        grpcxx::context &ctx,
        rpcChangeUsername::request_type const &req) {
        if (req.id().empty() || req.new_username().empty()) {
            return {grpcxx::status::code_t::invalid_argument};
        }

        auto found = false;
        for (row_t row: db_ << users_queries::get << req.id()) {
            found = true;
            break;
        }

        if (!found) {
            return {grpcxx::status::code_t::not_found};
        }

        db_ << "begin;";
        db_ << users_queries::update_username << req.new_username() << req.id();
        db_ << "commit;";

        return {grpcxx::status::code_t::ok};
    }

    template<>
    rpcDelete::result_type Rpc::call<rpcDelete>(
        grpcxx::context &ctx,
        rpcDelete::request_type const &req) {
        if (req.id().empty()) {
            return {grpcxx::status::code_t::invalid_argument};
        }

        auto found = false;
        for (row_t row: db_ << users_queries::get << req.id()) {
            found = true;
            break;
        }

        if (!found) {
            return {grpcxx::status::code_t::not_found};
        }

        db_ << "begin;";
        db_ << users_queries::delete_user << req.id();
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

    void Rpc::map(row_t const &from, reasy::api::v1::User *const to) noexcept {
        to->set_id(get<0>(from));
        to->set_name(get<1>(from));
        to->set_username(get<2>(from));
        to->set_email(get<3>(from));
    }
}