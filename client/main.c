//
// Created by Theo Lincke on 11/27/23.
//

#include <netinet/in.h>
#include <common.h>
#include <unistd.h>
#include <stdlib.h>
#include "client.h"

static int server_sockfd = -1;
static int ca_sockfd = -1;

void clean_up_sockets() {
    if (server_sockfd > 0) {
        if (get_verbosity() > v_medium) {
            printf("Closing server socket\n");
        }
        close(server_sockfd);
    }
    if (ca_sockfd > 0) {
        if (get_verbosity() > v_medium) {
            printf("Closing ca socket\n");
        }
        close(ca_sockfd);
    }
}

int main() {
    srand(1);

    // Register this callback on app exit
    register_app_exit(clean_up_sockets);

    // Create socket
    server_sockfd = open_stream_socket_impl();
    ca_sockfd = open_stream_socket_impl();

    // connect to server
    struct sockaddr_in server_addr = create_internet_addr_any(SERVER_PORT);
    connect_impl(server_sockfd, &server_addr);

    // connect to certificate authority
    struct sockaddr_in ca_addr = create_internet_addr_any(CA_PORT);
    connect_impl(ca_sockfd, &ca_addr);

    client_tls_exchange(server_sockfd, ca_sockfd);

    app_exit(0);
}
