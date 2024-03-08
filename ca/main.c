//
// Created by Theo Lincke on 11/27/23.
//

#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "ca.h"

static int sockfd = -1;
static int clientfd = -1;

void clean_up_sockets() {
    if (sockfd > 0) {
        if (get_verbosity() > v_medium) {
            printf("Closing server socket\n");
        }
        close(sockfd);
    }
    if(clientfd > 0){
        if (get_verbosity() > v_medium) {
            printf("Closing server socket\n");
        }
        close(sockfd);
    }
}

int main() {
    srand(3);

    register_app_exit(clean_up_sockets);

    sockfd = open_stream_socket_impl();

    struct sockaddr_in ca_addr = create_internet_addr_any(CA_PORT);
    bind_impl(sockfd, &ca_addr);
    listen_impl(sockfd);
    clientfd = accept_impl(sockfd);

    ca_tls_exchange(clientfd);

    app_exit(0);

    return 0;
}