//
// Created by tjl on 11/30/23.
//

#include <server.h>
#include "common.h"

const char *message = "Hello World, My name is Theo Lincke";

enum server_tls_result server_tls_exchange(
        int server_socket,
        const char *sr,
        size_t sr_len,
        char *sk,
        size_t sk_len
) {
    int clientfd = accept_impl(server_socket);
    send_in_chunks(clientfd, SEND_CHUNK_SIZE, message);
    return tls_server_success;
}
