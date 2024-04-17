#pragma once

#include "spdlog/spdlog.h"
#include "sqlite_modern_cpp.h"
#include "core/log.h"
#include "changelog-table.h"
#include "projects-table.h"
#include "users-table.h"
#include "workspaces-table.h"

class DB {
public:
    static sqlite::database init(std::basic_string<char> const &db_name) {
        auto db = sqlite::database(db_name);

        spdlog::info("[{}] Applying migrations to database...", log::label("Database"));
        try {
            tables::changelog(db.connection()).migrate();
            tables::projects(db.connection()).migrate();
            tables::users(db.connection()).migrate();
            tables::workspaces(db.connection()).migrate();

            spdlog::info("[{}] Applied migrations successfully.", log::label("Database"));
        } catch (sqlite::sqlite_exception const &e) {
            spdlog::error("{}: {} during \n\t \"{}\"", e.get_code(), e.what(), e.get_sql());
            db << "ROLLBACK;";
        }

        return db;
    }
};