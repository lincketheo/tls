//
// Created by tjl on 11/30/23.
//

#ifndef TLS_CLIENT_H
#define TLS_CLIENT_H

#include <stddef.h>

enum client_tls_result {
    tls_client_success,
};

/**
 * Initiates a tls exchange with a server specified by address
 * Also stores the session key on success
 * @param server_addr - The address of the server to connect to
 * @param client_socket - The fully initialized socket to initiate the tls exchange
 */
enum client_tls_result client_tls_exchange(
        int client_server_socket,
        int client_ca_socket
);

#endif //TLS_CLIENT_H
