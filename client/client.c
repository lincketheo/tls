//
// Created by tjl on 11/30/23.
//

#include <client.h>
#include <netinet/in.h>
#include "common.h"

static char client_buffer[RECV_BUFFER_SIZE];

enum client_tls_result client_tls_exchange(
        struct sockaddr_in *server_addr,
        int client_socket,
        const char *cr,
        size_t cr_len,
        char *sk,
        size_t sk_len
) {
    connect_impl(client_socket, server_addr);
    recv_in_chunks(client_socket, client_buffer, RECV_BUFFER_SIZE);
    return tls_client_success;
}
