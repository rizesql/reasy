add_library(core INTERFACE)

target_sources(core
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fixed-string.h>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/id.h>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/log.h>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/json.h>
)

target_link_libraries(core
        INTERFACE libxid::xid
        INTERFACE nlohmann_json::nlohmann_json
)