#pragma once

#include "wrapper.h"
#include "users_service.h"
#include "issues_service.h"

namespace api::v1 {
    using issues = Wrapper<api::issues::Rpc>;
    using users = Wrapper<api::users::Rpc>;
}