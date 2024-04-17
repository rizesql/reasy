#pragma once

#include "table.h"
#include "sqlite_modern_cpp.h"
#include "spdlog/spdlog.h"
#include "core/log.h"

namespace tables {
    class changelog : table {
    private:
        sqlite::database db_;

    public:
        explicit changelog(sqlite::database db) : table(), db_{std::move(db)} {}

        ~changelog() override = default;

        void migrate() override {
            spdlog::info("[{}] Applying migrations", log::label("Changelog table"));

            db_ << "BEGIN;";
            db_ << "create table if not exists changelog("
                   "    seq_id integer primary key autoincrement not null,"
                   "    id text not null,"
                   "    issue_id text not null,"
                   "    version int not null,"
                   "    type text not null,"
                   "    data text not null,"
                   "    created_by text not null,"
                   "    created_at int not null"
                   ");";
            db_ << "create index if not exists idx_changelog_issue_id on changelog(issue_id);";
            db_ << "COMMIT;";
        }
    };
}