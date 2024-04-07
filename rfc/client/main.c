//
// Created by Theo Lincke on 11/27/23.
//

#include <netinet/in.h>
#include <common.h>
#include <unistd.h>
#include "client.h"

static int sockfd = -1;

void clean_up_sockets() {
    if (sockfd > 0) {
        if (get_verbosity() > v_none) {
            printf("Closing server socket\n");
        }
        close(sockfd);
    }
}

int main() {
    register_app_exit(clean_up_sockets);

    // Create socket
    sockfd = open_stream_socket_impl();

    // connect to server
    struct sockaddr_in server_addr = create_internet_addr_any(SERVER_PORT);

    client_tls_exchange(&server_addr, sockfd, NULL, 0, NULL, 0);

    app_exit(0);
}
