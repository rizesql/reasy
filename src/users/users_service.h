#pragma once

#include "reasy/api/v1/users.grpcxx.pb.h"
//#include "user.h"
#include "api/rpc.h"
#include "sqlite_modern_cpp.h"

namespace api::users {
    using namespace reasy::api::v1::users;
    using row_t = std::tuple<std::string, std::string, std::string, std::string>;

    class Rpc : public rpc_t {
    private:
        sqlite::database db_;

        static void map(row_t const &from, reasy::api::v1::User *to) noexcept;

    public:
        explicit Rpc(sqlite::database db);

        ~Rpc() override = default;

        using service_t = Service;

        template<typename T>
        T::result_type call(grpcxx::context &, T::request_type const &) {
            return {grpcxx::status::code_t::unimplemented, std::nullopt};
        }

        template<>
        rpcGetAll::result_type call<rpcGetAll>(grpcxx::context &, rpcGetAll::request_type const &);

        template<>
        rpcGet::result_type call<rpcGet>(grpcxx::context &ctx, rpcGet::request_type const &req);

        google::rpc::Status exception() noexcept override;
    };
}

