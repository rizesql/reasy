add_library(workspaces)

target_sources(workspaces
        PRIVATE workspace.cpp

        PUBLIC FILE_SET headers TYPE HEADERS
        FILES
        workspace.h
)

target_link_libraries(workspaces
        PRIVATE ${PROJECT_NAME}::core
)