//
// Created by tjl on 11/30/23.
//

#include <server.h>
#include <unistd.h>
#include "common.h"

static char server_buffer[SERVER_BUFFER_SIZE];
static int clientfd;
static struct string internal_string;

const char *server_hello = "Hello from the server";
const char *server_finished = "Server finished";

void free_internal_string() {
    destroy_string(&internal_string);
}

void close_clientfd() {
    if (get_verbosity() > v_none) {
        printf("Closing client socket from server\n");
    }
    close(clientfd);
}

void consume(bool end) {
    if (end) {
        size_t size = find_char_assert(server_buffer, SERVER_BUFFER_SIZE, '\0');
        append_string(&internal_string, server_buffer, size);
    } else {
        append_string(&internal_string, server_buffer, SERVER_BUFFER_SIZE);
    }
}

enum server_tls_result server_tls_exchange(
        int server_socket,
        const char *sr,
        size_t sr_len,
        char *sk,
        size_t sk_len
) {
    clientfd = accept_impl(server_socket);
    register_app_exit(close_clientfd);

    create_string(&internal_string, 0);
    register_app_exit(free_internal_string);

    /**
     * Receive Client Hello
     */
    recv_in_chunks(clientfd, server_buffer, SERVER_BUFFER_SIZE, consume);
    if (get_verbosity() > v_none) {
        printf("Full Client Hello: %s\n", internal_string.head);
    }
    reset_string(&internal_string);

    /**
     * Send Server Hello
     */
    send_in_chunks(clientfd, SERVER_BUFFER_SIZE, server_hello);

    /**
     * Receive Premaster Secret
     */
    recv_in_chunks(clientfd, server_buffer, SERVER_BUFFER_SIZE, consume);
    if (get_verbosity() > v_none) {
        printf("Full Premaster Secret: %s\n", internal_string.head);
    }
    reset_string(&internal_string);

    /**
     * Send Server Finished
     */
    send_in_chunks(clientfd, SERVER_BUFFER_SIZE, server_finished);

    /**
     * Receive Client Finished
     */
    recv_in_chunks(clientfd, server_buffer, SERVER_BUFFER_SIZE, consume);
    if (get_verbosity() > v_none) {
        printf("Full Client Finished: %s\n", internal_string.head);
    }
    reset_string(&internal_string);

    return tls_server_success;
}
