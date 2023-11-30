//
// Created by tjl on 11/30/23.
//

#ifndef TLS_CLIENT_H
#define TLS_CLIENT_H

#include <stddef.h>
#include <netinet/in.h>

#define RECV_BUFFER_SIZE 20

enum client_tls_result {
    tls_client_success,
    tls_client_failure,
};

enum client_tls_result client_tls_exchange(
        struct sockaddr_in *server_addr,
        int client_socket,
        const char *cr,
        size_t cr_len,
        char *sk,
        size_t sk_len
);

#endif //TLS_CLIENT_H
