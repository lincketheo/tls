//
// Created by tjl on 11/30/23.
//

#include <client.h>
#include <dynamic_string.h>
#include <stdio.h>
#include <app.h>
#include <stdbool.h>
#include <sockets.h>
#include <utils.h>

#define CLIENT_BUFFER_SIZE 20
static char client_buffer[CLIENT_BUFFER_SIZE];

// Just creating one global string to make free's and everything easier (for app exit callbacks)
static struct string client_send_string;
static struct string client_recv_string;

static struct string client_random;
static struct string premaster;
static struct string server_random;
static struct string server_public_key;
static struct string session_key;
static struct string server_session_key;

void print_results() {
    printf("===============\n");
    printf("Client Summary:\n");
    printf("Client Random: %s\n", client_random.head);
    printf("Premaster: %s\n", premaster.head);
    printf("Server Random: %s\n", server_random.head);
    printf("Server Public Key: %s\n", server_public_key.head);
    printf("Session Key: %s\n", session_key.head);
    printf("Server Session Key: %s\n", server_session_key.head);
    printf("===============\n");
}

void free_internal_string() {
    destroy_string(&client_send_string);
    destroy_string(&client_recv_string);

    destroy_string(&client_random);
    destroy_string(&premaster);
    destroy_string(&server_random);
    destroy_string(&server_public_key);
    destroy_string(&session_key);
    destroy_string(&server_session_key);
}

void init_strings() {
    create_string(&client_recv_string, 0);
    create_string(&client_send_string, 0);

    create_string(&client_random, 0);
    create_string(&premaster, 0);
    create_string(&server_random, 0);
    create_string(&server_public_key, 0);
    create_string(&session_key, 0);
    create_string(&server_session_key, 0);

    register_app_exit(free_internal_string);
}

void consume(bool end) {
    if (end) {
        size_t size = find_char_assert(client_buffer, CLIENT_BUFFER_SIZE, '\0');
        append_string(&client_recv_string, client_buffer, size);
    } else {
        append_string(&client_recv_string, client_buffer, CLIENT_BUFFER_SIZE);
    }
}

void client_send(int fd, const char *prefix) {
    send_in_chunks(fd, CLIENT_BUFFER_SIZE, client_send_string.head);
    if (get_verbosity() > v_none) {
        printf("%s\n", prefix);
        printf("Sent: %s\n", client_send_string.head);
    }
}

void client_recv(int fd, const char *prefix) {
    reset_string(&client_recv_string);
    recv_in_chunks(fd, client_buffer, CLIENT_BUFFER_SIZE, consume);
    if (get_verbosity() > v_none) {
        printf("%s\n", prefix);
        printf("Received: %s\n", client_recv_string.head);
    }
}

enum client_tls_result client_tls_exchange(
        int client_server_socket,
        int client_ca_socket
) {
    init_strings();

    /**
     * Send Client Hello
     */
    new_random_string(&client_random, 10);
    copy_string(&client_random, &client_send_string);
    client_send(client_server_socket, "Client Hello");

    /**
     * Receive Server Hello
     */
    client_recv(client_server_socket, "Server Hello");
    copy_string(&client_recv_string, &server_random);

    /**
     * Receive Server Public Key
     */
    client_recv(client_server_socket, "Server Public Key");
    copy_string(&client_recv_string, &server_public_key);

    /**
     * Send Server Public Key to CA
     */
    copy_string(&server_public_key, &client_send_string);
    client_send(client_ca_socket, "Server Public Key");

    /**
     * Receive CA Response
     */
    client_recv(client_ca_socket, "CA Response");

    /**
     * Send premaster secret
     */
    new_random_string(&premaster, 10);
    copy_string(&premaster, &client_send_string);
    client_send(client_server_socket, "Premaster Secret");

    /**
     * Send Client Finished
     */
    new_random_string(&session_key, 10);
    copy_string(&session_key, &client_send_string);
    client_send(client_server_socket, "Client Finished");

    /**
     * Receive Server Finished
     */
    client_recv(client_server_socket, "Server Finished");
    copy_string(&client_recv_string, &server_session_key);

    print_results();

    return tls_client_success;
}

