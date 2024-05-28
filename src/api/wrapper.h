#pragma once

#include "spdlog/spdlog.h"
#include "rpc.h"
#include "core/log.h"

namespace api {
    /// Internal wrapper around RPCs, applies:
    ///  - request logging
    ///  - request timing
    ///  - response mapping
    ///  - error handling
    ///  - forwards constructor dependencies
    template<class Impl>
    class Wrapper {
    private:
        Impl rpc_;
        typename Impl::service_t service_;

    public:
        template<class... Args,
            std::enable_if_t<std::is_constructible<Impl, Args &&...>::value, int> = 0>
        explicit Wrapper(Args &&... args) :
            rpc_(std::forward<Args>(args)...),
            service_(*this) {}

        template<typename T>
        typename T::result_type call(grpcxx::context &ctx, typename T::request_type const &req) {
            try {
                spdlog::info("[RPC {}] - START", log::label(service_.name()));
                auto begin = std::chrono::steady_clock::now();

                auto res = rpc_.template call<T>(ctx, req);

                auto end = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

                spdlog::info("[RPC {}] - END status={} (elapsed_milliseconds={})",
                             log::label(service_.name()), format(res.status.code()), elapsed.count());

                return res;
            } catch (...) {
                auto s = rpc_.exception();

                std::string data;
                s.SerializeToString(&data);
                auto code = static_cast<grpcxx::status::code_t>(s.code());

                spdlog::error("ERROR - {} status={}", data, format(code));

                return {code, std::nullopt};
            }
        }

        constexpr auto &service() noexcept { return service_; }

    private:
        std::string_view format(grpcxx::status::code_t const &code) {
            using sc = grpcxx::status::code_t;

            if (code == sc::ok) return "OK";
            if (code == sc::cancelled) return "CANCELLED";
            if (code == sc::unknown) return "UNKNOWN";
            if (code == sc::invalid_argument) return "INVALID_ARGUMENT";
            if (code == sc::deadline_exceeded) return "DEADLINE_EXCEEDED";
            if (code == sc::not_found) return "NOT_FOUND";
            if (code == sc::already_exists) return "ALREADY_EXISTS";
            if (code == sc::permission_denied) return "PERMISSION_DENIED";
            if (code == sc::resource_exhausted) return "RESOURCE_EXHAUSTED";
            if (code == sc::failed_precondition) return "FAILED_PRECONDITION";
            if (code == sc::aborted) return "ABORTED";
            if (code == sc::out_of_range) return "OUT_OF_RANGE";
            if (code == sc::unimplemented) return "UNIMPLEMENTED";
            if (code == sc::internal) return "INTERNAL";
            if (code == sc::unavailable) return "UNAVAILABLE";
            if (code == sc::data_loss) return "DATA_LOSS";
            if (code == sc::unauthenticated) return "UNAUTHENTICATED";
            else std::unreachable();
        }
    };
}
