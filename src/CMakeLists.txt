add_subdirectory(api)
add_subdirectory(core)
add_subdirectory(issues)
add_subdirectory(projects)
add_subdirectory(users)
add_subdirectory(workspaces)

# api
add_library(libapi INTERFACE)
target_link_libraries(libapi
        INTERFACE api
)

target_include_directories(libapi
        INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_library(${PROJECT_NAME}::api ALIAS libapi)

# core
add_library(libcore INTERFACE)
target_link_libraries(libcore
        INTERFACE core
)

target_include_directories(libcore
        INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_library(${PROJECT_NAME}::core ALIAS libcore)

# db
add_library(libdb INTERFACE)
target_link_libraries(libdb
        INTERFACE db
)

target_include_directories(libdb
        INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_library(${PROJECT_NAME}::db ALIAS libdb)

# issues
add_library(libissues INTERFACE)
target_link_libraries(libissues
        INTERFACE issues
)

target_include_directories(libissues
        INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_library(${PROJECT_NAME}::issues ALIAS libissues)

# projects
add_library(libprojects INTERFACE)
target_link_libraries(libprojects
        INTERFACE projects
)

target_include_directories(libprojects
        INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_library(${PROJECT_NAME}::projects ALIAS libprojects)

# users
add_library(libusers INTERFACE)
target_link_libraries(libusers
        INTERFACE users
)

target_include_directories(libusers
        INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_library(${PROJECT_NAME}::users ALIAS libusers)

# workspaces
add_library(libworkspaces INTERFACE)
target_link_libraries(libworkspaces
        INTERFACE workspaces
)

target_include_directories(libworkspaces
        INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_library(${PROJECT_NAME}::workspaces ALIAS libworkspaces)


add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ${PROJECT_NAME}::core
        ${PROJECT_NAME}::issues
        ${PROJECT_NAME}::projects
        ${PROJECT_NAME}::users
        ${PROJECT_NAME}::workspaces
        spdlog::spdlog_header_only
        SQLite3
        nlohmann_json::nlohmann_json
        PUBLIC libxid::xid
)