//
// Created by tjl on 11/30/23.
//

#include <server.h>
#include "common.h"

static char server_buffer[SERVER_BUFFER_SIZE];

const char *server_hello = "Hello from the server";
const char *server_finished = "Server finished";

enum server_tls_result server_tls_exchange(
        int server_socket,
        const char *sr,
        size_t sr_len,
        char *sk,
        size_t sk_len
) {
    int clientfd = accept_impl(server_socket);

    /**
     * Receive Client Hello
     */
    recv_in_chunks(clientfd, server_buffer, SERVER_BUFFER_SIZE);

    /**
     * Send Server Hello
     */
    send_in_chunks(clientfd, SERVER_BUFFER_SIZE, server_hello);

    /**
     * Receive Premaster Secret
     */
    recv_in_chunks(clientfd, server_buffer, SERVER_BUFFER_SIZE);

    /**
     * Send Server Finished
     */
    send_in_chunks(clientfd, SERVER_BUFFER_SIZE, server_finished);

    /**
     * Receive Client Finished
     */
    recv_in_chunks(clientfd, server_buffer, SERVER_BUFFER_SIZE);

    return tls_server_success;
}
