//
// Created by tjl on 11/30/23.
//

#ifndef TLS_COMMON_H
#define TLS_COMMON_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#define SERVER_PORT 8000

enum verbosity {
    v_none = 0,
    v_medium = 1,
    v_high = 2
};
void app_exit();

void register_app_app_exit(void(*on_exit)());

int open_stream_socket_impl();

struct sockaddr_in create_internet_addr_any(int port);

void bind_impl(int sockfd, struct sockaddr_in *to_address);

void listen_impl(int sockfd);

int accept_impl(int sockfd);

void connect_impl(int sockfd, struct sockaddr_in *server_addr);

size_t send_impl(int to_socket, const char *buffer, size_t buffer_size);

void send_impl_flush(int to_socket, const char *buffer, size_t len);

void send_in_chunks(int to_socket, size_t chunk_size, const char *full_message);

size_t recv_impl(int sockfd, char *buffer, size_t buffer_size);

void recv_in_chunks(int sockfd, char *buffer, size_t buffer_size);

ssize_t find_char(const char *buffer, size_t buffer_size, char c);

enum verbosity get_verbosity();

#endif //TLS_COMMON_H
