add_library(api)

target_sources(api
        PUBLIC FILE_SET headers TYPE HEADERS FILES v1.h rpc.h

        PRIVATE FILE_SET private_headers TYPE HEADERS FILES wrapper.h
)

target_link_libraries(api
        PUBLIC ${PROJECT_NAME}::libproto
)