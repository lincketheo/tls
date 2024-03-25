//
// Created by tjl on 3/25/24.
//

#ifndef TLS_SOCKETS_H
#define TLS_SOCKETS_H

#include <stdlib.h>
#include <stdbool.h>

int open_stream_socket_impl();

struct sockaddr_in create_internet_addr_any(int port);

void bind_impl(int sockfd, struct sockaddr_in *to_address);

void listen_impl(int sockfd);

int accept_impl(int sockfd);

void connect_impl(int sockfd, struct sockaddr_in *server_addr);

size_t send_impl(int to_socket, const char *buffer, size_t buffer_size);

void send_impl_flush(int to_socket, const char *buffer, size_t len);

void send_in_chunks(
        int to_socket,
        size_t chunk_size,
        const char *full_message);

size_t recv_impl(int sockfd, char *buffer, size_t buffer_size);

/**
 * Receives data in chunks - When buffer is full or finished - calls consume
 * @param consume_buffer - A callback executed whenever the buffer is filled
 * or done executing
 */
void recv_in_chunks(
        int sockfd,
        char *buffer,
        size_t buffer_size,
        void (*consume_buffer)(bool end));

#endif //TLS_SOCKETS_H
