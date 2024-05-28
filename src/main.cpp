#include <grpcxx/server.h>

#include "api/v1.h"
#include "spdlog/spdlog.h"
#include "db/init.h"
#include "core/log.h"

int main() {
    log::init();
    auto db = DB::init("reasy.db");

    auto server = grpcxx::server();

    auto issues = api::v1::issues(db.connection());
    server.add(issues.service());

    auto projects = api::v1::projects(db.connection());
    server.add(projects.service());
    
    auto users = api::v1::users(db.connection());
    server.add(users.service());

    auto workspaces = api::v1::workspaces(db.connection());
    server.add(workspaces.service());

    try {
        spdlog::info("Server started at {}", log::label("127.0.0.1:8080"));
        server.run("127.0.0.1", 8080);
    } catch (...) {
        spdlog::error("uncaught exception");
    }

    return 0;
}
