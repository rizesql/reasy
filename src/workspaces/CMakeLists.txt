add_library(workspaces)

target_sources(workspaces
        PRIVATE
        workspaces_service.cpp
        workspace.cpp

        PUBLIC FILE_SET headers TYPE HEADERS
        FILES
        workspaces_service.h
        workspace.h
)

target_link_libraries(workspaces
        PRIVATE ${PROJECT_NAME}::core
        PUBLIC ${PROJECT_NAME}::libproto
)
