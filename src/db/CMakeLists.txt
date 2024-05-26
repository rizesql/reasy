add_library(db)

target_sources(db
        PUBLIC FILE_SET headers TYPE HEADERS FILES init.h

        PRIVATE FILE_SET private_headers TYPE HEADERS
        FILES
        table.h
        changelog-table.h
        projects-table.h
        users-table.h
        workspaces-table.h
)