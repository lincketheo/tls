cmake_minimum_required(VERSION 3.20)
project(tls C)

set(CMAKE_C_STANDARD 11)

include_directories(client common server ca)

add_library(libcommon common/common.c)
add_library(libclient client/client.c)
add_library(libserver server/server.c)

add_executable(server server/main.c)
target_link_libraries(server libcommon libserver)

add_executable(client client/main.c)
target_link_libraries(client libcommon libclient)
