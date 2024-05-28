add_library(projects)

target_sources(projects
        PRIVATE
        projects_service.cpp
        project.cpp

        PUBLIC FILE_SET headers TYPE HEADERS
        FILES
        projects_service.h
        project.h
)

target_link_libraries(projects
        PRIVATE ${PROJECT_NAME}::core
        PUBLIC ${PROJECT_NAME}::libproto
)
