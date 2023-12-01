//
// Created by tjl on 11/30/23.
//

#include <client.h>
#include <netinet/in.h>
#include "common.h"

static char client_buffer[CLIENT_BUFFER_SIZE];

const char* client_hello = "Hello from the client";
const char *premaster_secret = "Premaster secret";
const char *client_finished = "Client finished";

enum client_tls_result client_tls_exchange(
        struct sockaddr_in *server_addr,
        int client_socket,
        const char *cr,
        size_t cr_len,
        char *sk,
        size_t sk_len
) {
    connect_impl(client_socket, server_addr);

    /**
     * Send Client Hello
     */
    send_in_chunks(client_socket, CLIENT_BUFFER_SIZE, client_hello);

    /**
     * Receive Server Hello
     */
    recv_in_chunks(client_socket, client_buffer, CLIENT_BUFFER_SIZE);

    /**
     * Send premaster secret
     */
    send_in_chunks(client_socket, CLIENT_BUFFER_SIZE, premaster_secret);

    /**
     * Receive Server Finished
     */
    recv_in_chunks(client_socket, client_buffer, CLIENT_BUFFER_SIZE);

    /**
     * Send Client Finished
     */
    send_in_chunks(client_socket, CLIENT_BUFFER_SIZE, client_finished);

    return tls_client_success;
}
