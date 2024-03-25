//
// Created by tjl on 3/8/24.
//

#include <ca.h>
#include <common.h>
#include <string.h>

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

void verify_pub_key(
        char *pbk_recv,
        size_t pbk_recv_l,
        char *pbk_exp,
        size_t pbk_exp_l
) {
    if (pbk_recv_l != pbk_exp_l || !strncmp(pbk_recv, pbk_exp, pbk_exp_l)) {
        printf("Invalid public key. Got: %.*s. Expected: %.*s\n", (int) pbk_recv_l, pbk_recv, (int) pbk_exp_l, pbk_exp);
    } else {
        printf("Valid public key. Got: %.*s. Expected: %.*s\n", (int) pbk_recv_l, pbk_recv, (int) pbk_exp_l, pbk_exp);
    }
}

void ca_tls_exchange(
        int clientfd,
        char *pbk,
        size_t pbk_l
) {
    init_strings();

    /**
     * Receive Server Public Key from the Client
     */
    ca_recv(clientfd, "Server Public Key:");
    copy_string(&ca_recv_string, &server_public_key);

    verify_pub_key(server_public_key.head, server_public_key.size, pbk, pbk_l);

    /**
     * Send success
     */
    new_random_string(&ca_send_string, 10);
    ca_send(clientfd, "CA Response");

    print_results();
}
