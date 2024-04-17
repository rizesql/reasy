add_library(users)

target_sources(users
        PRIVATE
        users_service.cpp
        user.cpp

        PUBLIC FILE_SET headers TYPE HEADERS
        FILES
        users_service.h
        user.h
)

target_link_libraries(users
        PRIVATE ${PROJECT_NAME}::core
        PUBLIC ${PROJECT_NAME}::libproto
)
