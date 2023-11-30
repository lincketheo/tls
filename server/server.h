//
// Created by tjl on 11/30/23.
//

#ifndef TLS_SERVER_H
#define TLS_SERVER_H

#include <stddef.h>

#define SEND_CHUNK_SIZE 20

enum server_tls_result {
    tls_server_success,
    tls_server_failure,
};

enum server_tls_result server_tls_exchange(
        int server_socket,
        const char *sr,
        size_t sr_len,
        char *sk,
        size_t sk_len
);

#endif //TLS_SERVER_H
