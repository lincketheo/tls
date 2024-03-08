//
// Created by tjl on 11/30/23.
//

#ifndef TLS_SERVER_H
#define TLS_SERVER_H

#include <stddef.h>

#define SERVER_BUFFER_SIZE 20

enum server_tls_result {
    tls_server_success,
    tls_server_failure,
};

/**
 * Initiate Server tls exchange
 * @param server_socket The server socket set up and listening
 * @param sr - Server random
 * @param sr_len - Server random length
 * @param sk - Session key
 * @param sk_len - Session key length
 * @return TLS Status
 */
enum server_tls_result server_tls_exchange(
        int clientfd
);

#endif //TLS_SERVER_H
