#pragma once

#include "reasy/api/v1/issues.grpcxx.pb.h"
#include "sqlite_modern_cpp.h"
#include "api/rpc.h"
#include "issue.h"

namespace api::issues {
    using namespace reasy::api::v1::issues;

    class Rpc : public rpc_t {
    private:
        sqlite::database db_;

    public:
        explicit Rpc(sqlite::database db);

        ~Rpc() override = default;

        using service_t = Service;

        /// Base `call` method, it is called when no rpc is matched
        template<typename T>
        T::result_type call(grpcxx::context &, T::request_type const &) {
            return {grpcxx::status::code_t::unimplemented, std::nullopt};
        }

        /// Implementation of `reasy.api.v1.users/Get`
        template<>
        rpcGet::result_type call<rpcGet>(grpcxx::context &, rpcGet::request_type const &);

        /// Implementation of `reasy.api.v1.users/Create`
        template<>
        rpcCreate::result_type call<rpcCreate>(grpcxx::context &, rpcCreate::request_type const &);

        /// Implementation of `reasy.api.v1.users/Assign`
        template<>
        rpcAssign::result_type call<rpcAssign>(grpcxx::context &, rpcAssign::request_type const &);

        /// Implementation of `reasy.api.v1.users/RemoveAssignee`
        template<>
        rpcRemoveAssignee::result_type call<rpcRemoveAssignee>(
            grpcxx::context &,
            rpcRemoveAssignee::request_type const &);

        /// Implementation of `reasy.api.v1.users/AddToProject`
        template<>
        rpcAddToProject::result_type call<rpcAddToProject>(grpcxx::context &, rpcAddToProject::request_type const &);

        /// Implementation of `reasy.api.v1.users/ChangePriority`
        template<>
        rpcChangePriority::result_type call<rpcChangePriority>(
            grpcxx::context &,
            rpcChangePriority::request_type const &);

        /// Implementation of `reasy.api.v1.users/ChangeStatus`
        template<>
        rpcChangeStatus::result_type call<rpcChangeStatus>(grpcxx::context &, rpcChangeStatus::request_type const &);

        google::rpc::Status exception() noexcept override;
    };
}