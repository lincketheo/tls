cmake_minimum_required(VERSION 3.26)
project(tls C)

set(CMAKE_C_STANDARD 11)

add_executable(server server/main.c)
add_executable(client client/main.c)
add_executable(ca ca/main.c)
