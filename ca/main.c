//
// Created by Theo Lincke on 11/27/23.
//

#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include "common.h"
#include "ca.h"

static int sockfd = -1;
static int clientfd = -1;
static struct string seed;
static struct string pubkey;
static struct simple_json *config;

void clean_up() {
    if (sockfd > 0) {
        if (get_verbosity() > v_medium) {
            printf("Closing server socket\n");
        }
        close(sockfd);
    }
    if (clientfd > 0) {
        if (get_verbosity() > v_medium) {
            printf("Closing server socket\n");
        }
        close(sockfd);
    }
    destroy_string(&seed);
    destroy_string(&pubkey);
    if (config) {
        free_simple_json(config);
    }
}

void parse_ca_json(char *config_name) {
    config = parse_simple_json(config_name);

    create_string(&pubkey, 10);
    create_string(&seed, 10);

    // Required public key
    size_t pk_l;
    char pk[100];
    if (!(pk_l = get_json_value(config, pk, 100, "public", 6))) {
        printf("Invalid config, must contain public key\n");
        app_exit(1);
    }
    append_string(&pubkey, pk, pk_l);

    srand_from_json(config, 3);

    if (get_verbosity() > v_none) {
        printf("---------- CA Config Parameters:\n");
        printf("Public Key: %s\n", pubkey.head);
        printf("----------\n");
    }
}


int main(int argc, char **argv) {
    register_app_exit(clean_up);

    parse_ca_json(parse_only_config_name(argc, argv));

    sockfd = open_stream_socket_impl();

    struct sockaddr_in ca_addr = create_internet_addr_any(CA_PORT);
    bind_impl(sockfd, &ca_addr);
    listen_impl(sockfd);
    clientfd = accept_impl(sockfd);

    ca_tls_exchange(clientfd, pubkey.head, pubkey.size);

    app_exit(0);

    return 0;
}