add_library(issues)

target_sources(issues
        PRIVATE
        issues_service.cpp
        issue.cpp

        PUBLIC FILE_SET headers TYPE HEADERS
        FILES
        issues_service.h
        status.h
        priority.h
        issue.h
        id.h
        events.h

        PRIVATE FILE_SET private_headers TYPE HEADERS
        FILES
)

target_link_libraries(issues
        PRIVATE ${PROJECT_NAME}::core
        PRIVATE ${PROJECT_NAME}::libproto
)
