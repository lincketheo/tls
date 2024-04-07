cmake_minimum_required(VERSION 3.20)
project(tls C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

include_directories(client common server ca)

add_library(libcommon
        common/app.c
        common/sockets.c
        common/utils.c
)

add_library(libca ca/ca.c)
add_library(libclient client/client.c)
add_library(libserver server/server.c)

target_link_libraries(libca PUBLIC libcommon)
target_link_libraries(libclient PUBLIC libcommon)
target_link_libraries(libserver PUBLIC libcommon)

add_executable(server server/main.c)
target_link_libraries(server libserver)

add_executable(client client/main.c)
target_link_libraries(client libclient)

add_executable(ca ca/main.c)
target_link_libraries(ca libca)

add_executable(test test.c)
target_link_libraries(test libcommon)
