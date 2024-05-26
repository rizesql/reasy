#pragma once

#include <sqlite_modern_cpp.h>
#include "reasy/api/v1/workspaces.grpcxx.pb.h"
#include "api/rpc.h"

namespace api::workspaces {
    using namespace reasy::api::v1::workspaces;
    using row_t = std::tuple<std::string, std::string, std::string>;

    class Rpc : public rpc_t {
    private:
        sqlite::database db_;

        static void map(row_t const &from, reasy::api::v1::Workspace *to) noexcept;

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
        rpcGet::result_type call<rpcGet>(grpcxx::context &, rpcGet::request_type const &);

        template<>
        rpcCreate::result_type call<rpcCreate>(grpcxx::context &, rpcCreate::request_type const &);

        template<>
        rpcChangeName::result_type call<rpcChangeName>(grpcxx::context &, rpcChangeName::request_type const &);

        template<>
        rpcChangeAdmin::result_type call<rpcChangeAdmin>(grpcxx::context &, rpcChangeAdmin::request_type const &);

        template<>
        rpcDelete::result_type call<rpcDelete>(grpcxx::context &, rpcDelete::request_type const &);
    };
}