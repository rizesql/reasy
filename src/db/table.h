#pragma once

// Interface for a table definition
struct table {
    virtual ~table() = default;

    virtual void migrate() = 0;
};
