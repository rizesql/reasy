include(FetchContent)

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
    find_program(_PROTOBUF_PROTOC protoc)
else()
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()

FetchContent_Declare(googleapis
        URL https://github.com/googleapis/googleapis/archive/0e3b813b0d0da539eacbe86b8716feeed00943c5.tar.gz
        URL_HASH SHA256=44f3b9c73a5df760c4fad3cf0c5cc54732b881f00708308f7635ff75a13dcaa5
)
FetchContent_MakeAvailable(googleapis)

FetchContent_Declare(grpcxx
#        URL https://github.com/uatuko/grpcxx/archive/refs/tags/v0.2.0.tar.gz
        URL https://github.com/uatuko/grpcxx/archive/refs/tags/v0.1.4.tar.gz
#        URL_HASH SHA256=ed0e0c6ccd44aabb9447de9030b9be092bfb97cb654d69c970cbefd7b7bb44da
)
FetchContent_MakeAvailable(grpcxx)

FetchContent_Declare(libxid
        URL https://github.com/uatuko/libxid/archive/refs/tags/v0.1.0.tar.gz
        URL_HASH SHA256=31589bb5274c9d25a8b6c49ee04a6c76151f10082e7feb13314be02a4b2d58c8
)
FetchContent_MakeAvailable(libxid)

#include_directories(${CMAKE_SOURCE_DIR}/dependencies/sqlite)
find_package(SQLite3 REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/dependencies/sqlite_modern_cpp/hdr)

FetchContent_Declare(json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
        URL_HASH SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d
)
FetchContent_MakeAvailable(json)

find_package(spdlog REQUIRED)