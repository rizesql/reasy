#pragma once

#include "table.h"
#include "sqlite_modern_cpp.h"
#include "spdlog/spdlog.h"
#include "core/log.h"

namespace tables {
    class users : table {
    private:
        sqlite::database db_;

    public:
        explicit users(sqlite::database db) : table(), db_{std::move(db)} {}

        ~users() override = default;

        void migrate() override {
            spdlog::info("[{}] Applying migrations", log::label("Users table"));

            db_ << "BEGIN;";
            db_ << "CREATE TABLE IF NOT EXISTS users("
                   "    id text not null primary key,"
                   "    name text not null,"
                   "    username text not null,"
                   "    email text not null"
                   ");";
            db_ << "COMMIT;";
        }
    };
}