#pragma once

#include "table.h"
#include "sqlite_modern_cpp.h"
#include "spdlog/spdlog.h"
#include "core/log.h"

namespace tables {
    class workspaces : table {
    private:
        sqlite::database db_;

    public:
        explicit workspaces(sqlite::database db) : table(), db_{std::move(db)} {}

        ~workspaces() override = default;

        void migrate() override {
            spdlog::info("[{}] Applying migrations", log::label("Workspaces table"));

            db_ << "BEGIN;";
            db_ << "create table if not exists workspaces("
                   "    id text primary key not null,"
                   "    user_id text not null references users(id),"
                   "    name text not null"
                   ");";
            db_ << "COMMIT;";
        }
    };
}