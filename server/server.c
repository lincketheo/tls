//
// Created by tjl on 11/30/23.
//

#include <server.h>
#include <common.h>

static char server_buffer[SERVER_BUFFER_SIZE];
static struct string server_send_string;
static struct string server_recv_string;

static struct string client_random;
static struct string premaster;
static struct string server_random;
static struct string server_public_key;
static struct string session_key;
static struct string client_session_key;

void print_results() {
    printf("===============\n");
    printf("Server Summary:\n");
    printf("Client Random: %s\n", client_random.head);
    printf("Premaster: %s\n", premaster.head);
    printf("Server Random: %s\n", server_random.head);
    printf("Server Public Key: %s\n", server_public_key.head);
    printf("Session Key: %s\n", session_key.head);
    printf("Client Session Key: %s\n", client_session_key.head);
    printf("===============\n");
}

void free_internal_string() {
    destroy_string(&server_recv_string);
    destroy_string(&server_send_string);

    destroy_string(&client_random);
    destroy_string(&premaster);
    destroy_string(&server_random);
    destroy_string(&server_public_key);
    destroy_string(&session_key);
    destroy_string(&client_session_key);
}

void init_strings() {
    create_string(&server_recv_string, 10);
    create_string(&server_send_string, 10);

    create_string(&client_random, 10);
    create_string(&premaster, 10);
    create_string(&server_random, 10);
    create_string(&server_public_key, 10);
    create_string(&session_key, 10);
    create_string(&client_session_key, 10);

    register_app_exit(free_internal_string);
}

void consume(bool end) {
    if (end) {
        size_t size = find_char_assert(server_buffer, SERVER_BUFFER_SIZE, '\0');
        append_string(&server_recv_string, server_buffer, size);
    } else {
        append_string(&server_recv_string, server_buffer, SERVER_BUFFER_SIZE);
    }
}

void server_send(int fd, const char *prefix) {
    send_in_chunks(fd, SERVER_BUFFER_SIZE, server_send_string.head);
    if (get_verbosity() > v_none) {
        printf("%s\n", prefix);
        printf("Sent: %s\n", server_send_string.head);
    }
}

void server_recv(int fd, const char *prefix) {
    reset_string(&server_recv_string);
    recv_in_chunks(fd, server_buffer, SERVER_BUFFER_SIZE, consume);
    if (get_verbosity() > v_none) {
        printf("%s\n", prefix);
        printf("Received: %s\n", server_recv_string.head);
    }
}

enum server_tls_result server_tls_exchange(int clientfd) {
    init_strings();

    /**
     * Receive Client Hello
     */
    server_recv(clientfd, "Client Hello");
    copy_string(&server_recv_string, &client_random);

    /**
     * Send Server Hello
     */
    new_random_string(&server_random, 10);
    copy_string(&server_random, &server_send_string);
    server_send(clientfd, "Server Hello");

    /**
     * Send Server Public Key
     */
    new_random_string(&server_public_key, 10);
    copy_string(&server_public_key, &server_send_string);
    server_send(clientfd, "Server Public Key");

    /**
     * Receive Premaster Secret
     */
    server_recv(clientfd, "Premaster Secret");
    copy_string(&server_recv_string, &premaster);

    /**
     * Receive Client Finished
     */
    server_recv(clientfd, "Client Finished");
    copy_string(&server_recv_string, &client_session_key);

    /**
     * Send Server Finished
     */
    new_random_string(&session_key, 10);
    copy_string(&session_key, &server_send_string);
    server_send(clientfd, "Server Finished");

    print_results();

    return tls_server_success;
}
