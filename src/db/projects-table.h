#pragma once

#include "table.h"
#include "sqlite_modern_cpp.h"
#include "spdlog/spdlog.h"
#include "core/log.h"

namespace tables {
    class projects : table {
    private:
        sqlite::database db_;

    public:
        explicit projects(sqlite::database db) : table(), db_{std::move(db)} {}

        ~projects() override = default;

        void migrate() override {
            spdlog::info("[{}] Applying migrations", log::label("Projects table"));

            db_ << "BEGIN;";
            db_ << "create table if not exists projects("
                   "    id text primary key not null,"
                   "    workspace_id text not null references workspaces(id),"
                   "    name text not null"
                   ");";
            db_ << "COMMIT;";
        }
    };
}