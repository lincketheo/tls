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

enum server_tls_result server_tls_exchange(
        int clientfd,
        char *pbk,
        size_t pbk_l,
        char *pvk,
        size_t pvk_l
);

#endif //TLS_SERVER_H
