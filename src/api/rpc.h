#pragma once

#include "google/rpc/status.pb.h"

/// Interface for RPC implementations
class rpc_t {
public:
    virtual ~rpc_t() = default;

    virtual google::rpc::Status exception() noexcept = 0;
};
