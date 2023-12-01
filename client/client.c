//
// Created by tjl on 11/30/23.
//

#include <client.h>
#include <netinet/in.h>
#include "common.h"

static char client_buffer[CLIENT_BUFFER_SIZE];
static struct string internal_string;

const char *client_hello = "Hello from the client";
const char *premaster_secret = "Premaster secret";
const char *client_finished = "Client finished";

void free_internal_string() {
    destroy_string(&internal_string);
}

void consume(bool end) {
    if (end) {
        size_t size = find_char_assert(client_buffer, CLIENT_BUFFER_SIZE, '\0');
        append_string(&internal_string, client_buffer, size);
    } else {
        append_string(&internal_string, client_buffer, CLIENT_BUFFER_SIZE);
    }
}

enum client_tls_result client_tls_exchange(
        struct sockaddr_in *server_addr,
        int client_socket,
        const char *cr,
        size_t cr_len,
        char *sk,
        size_t sk_len
) {
    connect_impl(client_socket, server_addr);

    create_string(&internal_string, 0);
    register_app_exit(free_internal_string);

    /**
     * Send Client Hello
     */
    send_in_chunks(client_socket, CLIENT_BUFFER_SIZE, client_hello);

    /**
     * Receive Server Hello
     */
    recv_in_chunks(client_socket, client_buffer, CLIENT_BUFFER_SIZE, consume);
    if (get_verbosity() > v_none) {
        printf("Full Server Hello: %s\n", internal_string.head);
    }
    reset_string(&internal_string);

    /**
     * Send premaster secret
     */
    send_in_chunks(client_socket, CLIENT_BUFFER_SIZE, premaster_secret);

    /**
     * Receive Server Finished
     */
    recv_in_chunks(client_socket, client_buffer, CLIENT_BUFFER_SIZE, consume);
    if (get_verbosity() > v_none) {
        printf("Full Server Finished: %s\n", internal_string.head);
    }
    reset_string(&internal_string);

    /**
     * Send Client Finished
     */
    send_in_chunks(client_socket, CLIENT_BUFFER_SIZE, client_finished);

    return tls_client_success;
}
