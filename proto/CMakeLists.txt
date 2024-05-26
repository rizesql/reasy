cmake_path(SET users_proto ${CMAKE_CURRENT_SOURCE_DIR}/reasy/api/v1/users.proto)
cmake_path(SET users_header ${CMAKE_CURRENT_BINARY_DIR}/reasy/api/v1/users.pb.h)
cmake_path(SET users_source ${CMAKE_CURRENT_BINARY_DIR}/reasy/api/v1/users.pb.cc)

cmake_path(SET issues_proto ${CMAKE_CURRENT_SOURCE_DIR}/reasy/api/v1/issues.proto)
cmake_path(SET issues_header ${CMAKE_CURRENT_BINARY_DIR}/reasy/api/v1/issues.pb.h)
cmake_path(SET issues_source ${CMAKE_CURRENT_BINARY_DIR}/reasy/api/v1/issues.pb.cc)

set(protos
        ${users_proto}
        ${issues_proto}
)

set(headers
        ${users_header}
        ${issues_header}
)

set(sources
        ${users_source}
        ${issues_source}
)

add_custom_command(
        OUTPUT ${headers} ${sources}
        DEPENDS ${protos}
        COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        ARGS
        --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
        --proto_path=${googleapis_SOURCE_DIR}
        --proto_path=${Protobuf_INCLUDE_DIR}
        --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
        --grpcxx_out=${CMAKE_CURRENT_BINARY_DIR}
        --plugin=protoc-gen-grpcxx=$<TARGET_FILE:libgrpcxx::protoc-gen>
        ${protos}
)

cmake_path(SET core_proto ${CMAKE_CURRENT_SOURCE_DIR}/reasy/core.proto)
cmake_path(SET core_header ${CMAKE_CURRENT_BINARY_DIR}/reasy/core.pb.h)
cmake_path(SET core_source ${CMAKE_CURRENT_BINARY_DIR}/reasy/core.pb.cc)

set(core_protos ${core_proto})
set(core_headers ${core_header})
set(core_sources ${core_source})

add_custom_command(
        OUTPUT ${core_headers} ${core_sources}
        DEPENDS ${core_protos}
        COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        ARGS
        --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
        --proto_path=${googleapis_SOURCE_DIR}
        --proto_path=${Protobuf_INCLUDE_DIR}
        --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
        ${core_protos}
)

add_library(proto ${sources} ${core_sources})

target_include_directories(proto
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(proto
        googleapis
        libgrpcxx::grpcxx
        protobuf::libprotobuf
)

add_library(${PROJECT_NAME}::libproto ALIAS proto)
