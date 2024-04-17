#include <google/rpc/code.pb.h>
#include "users_service.h"
#include "user.h"

struct users_queries {
    constexpr static std::string_view get_all = R"(
        select id, name, username, email from users;
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
        auto user = User(user::id(), "rizesql", "@rizesql_", "mail@mail.com");

        rpcGet::response_type res;
        res.set_id(user.id().val());

        return {grpcxx::status::code_t::ok, res};
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