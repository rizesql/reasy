#pragma once

#include "wrapper.h"
#include "users_service.h"
#include "issues_service.h"
#include "projects_service.h"

namespace api::v1 {
    using issues = Wrapper<api::issues::Rpc>;
    using projects = Wrapper<api::projects::Rpc>;
    using users = Wrapper<api::users::Rpc>;
    using workspaces = Wrapper<api::projects::Rpc>;
}
