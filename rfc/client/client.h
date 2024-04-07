//
// Created by tjl on 11/30/23.
//

#ifndef TLS_CLIENT_H
#define TLS_CLIENT_H

#include <stddef.h>
#include <netinet/in.h>

#define CLIENT_BUFFER_SIZE 20

enum client_tls_result {
    tls_client_success,
    tls_client_failure,
};

/**
 * Initiates a tls exchange with a server specified by address
 * Also stores the session key on success
 * @param server_addr - The address of the server to connect to
 * @param client_socket - The fully initialized socket to initate the tls exchange
 * @param cr - Client random
 * @param cr_len - Client random length
 * @param sk - Session key output
 * @param sk_len - Output of the session key buffer
 * @return TLS Status
 */
enum client_tls_result client_tls_exchange(
        struct sockaddr_in *server_addr,
        int client_socket,
        const char *cr,
        size_t cr_len,
        char *sk,
        size_t sk_len
);

#endif //TLS_CLIENT_H
