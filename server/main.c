#include <netinet/in.h>
#include <common.h>
#include <unistd.h>
#include "server.h"

static int sockfd = -1;
static int clientfd = -1;
static struct string pubkey;
static struct string privkey;
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
            printf("Closing client socket from server\n");
        }
        close(clientfd);
    }
    destroy_string(&pubkey);
    destroy_string(&privkey);
    if (config) {
        free_simple_json(config);
    }
}

void parse_server_json(char *config_name) {
    config = parse_simple_json(config_name);

    create_string(&pubkey, 10);
    create_string(&privkey, 10);

    // Required public
    size_t pk_l;
    char pk[100];
    if (!(pk_l = get_json_value(config, pk, 100, "public", 6))) {
        printf("Invalid config, must contain public key\n");
        app_exit(1);
    }
    append_string(&pubkey, pk, pk_l);

    // Required Private
    if (!(pk_l = get_json_value(config, pk, 100, "private", 7))) {
        printf("Invalid config, must contain private key\n");
        app_exit(1);
    }
    append_string(&privkey, pk, pk_l);

    srand_from_json(config, 3);

    if (get_verbosity() > v_none) {
        printf("---------- Server Config Parameters:\n");
        printf("Public Key: %s\n", pubkey.head);
        printf("Private Key: %s\n", privkey.head);
        printf("----------\n");
    }
}

int main(int argc, char **argv) {
    register_app_exit(clean_up);

    parse_server_json(parse_only_config_name(argc, argv));

    // Create socket and bind / listen
    sockfd = open_stream_socket_impl();

    // Create server socket
    struct sockaddr_in server_addr = create_internet_addr_any(SERVER_PORT);
    bind_impl(sockfd, &server_addr);
    listen_impl(sockfd);
    clientfd = accept_impl(sockfd);

    server_tls_exchange(clientfd, pubkey.head, pubkey.size, privkey.head, privkey.size);

    app_exit(0);
}
