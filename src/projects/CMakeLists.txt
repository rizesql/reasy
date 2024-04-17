add_library(projects)

target_sources(projects
        PRIVATE project.cpp

        PUBLIC FILE_SET headers TYPE HEADERS
        FILES
        project.h
)

target_link_libraries(projects
        PRIVATE ${PROJECT_NAME}::core
)