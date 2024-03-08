//
// Created by tjl on 3/8/24.
//

#include <ca.h>
#include <common.h>

static char client_buffer[CA_BUFFER_SIZE];
static struct string ca_send_string;
static struct string ca_recv_string;

static struct string server_public_key;

void free_internal_string() {
    destroy_string(&ca_send_string);
    destroy_string(&ca_recv_string);

    destroy_string(&server_public_key);
}

void init_strings() {
    create_string(&ca_recv_string, 10);
    create_string(&ca_send_string, 10);

    create_string(&server_public_key, 10);

    register_app_exit(free_internal_string);
}

void print_results() {
    printf("===============\n");
    printf("CA Summary:\n");
    printf("Server Public Key: %s\n", server_public_key.head);
    printf("===============\n");
}

void consume(bool end) {
    if (end) {
        size_t size = find_char_assert(client_buffer, CA_BUFFER_SIZE, '\0');
        append_string(&ca_recv_string, client_buffer, size);
    } else {
        append_string(&ca_recv_string, client_buffer, CA_BUFFER_SIZE);
    }
}

void ca_send(int fd, const char *prefix) {
    send_in_chunks(fd, CA_BUFFER_SIZE, ca_send_string.head);
    if (get_verbosity() > v_none) {
        printf("%s\n", prefix);
        printf("Sent: %s\n", ca_send_string.head);
    }
}

void ca_recv(int fd, const char *prefix) {
    reset_string(&ca_recv_string);
    recv_in_chunks(fd, client_buffer, CA_BUFFER_SIZE, consume);
    if (get_verbosity() > v_none) {
        printf("%s\n", prefix);
        printf("Received: %s\n", ca_recv_string.head);
    }
}

void ca_tls_exchange(int clientfd) {
    init_strings();

    /**
     * Receive Server Public Key from the Client
     */
    ca_recv(clientfd, "Server Public Key:");
    copy_string(&ca_recv_string, &server_public_key);

    /**
     * Send success
     */
    new_random_string(&ca_send_string, 10);
    ca_send(clientfd, "CA Response");

    print_results();
}
