#include <netinet/in.h>
#include <common.h>
#include <unistd.h>
#include <stdlib.h>
#include "server.h"

static int sockfd = -1;
static int clientfd = -1;

void clean_up_sockets() {
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
}

int main() {
    srand(2);

    register_app_exit(clean_up_sockets);

    // Create socket and bind / listen
    sockfd = open_stream_socket_impl();

    // Create server socket
    struct sockaddr_in server_addr = create_internet_addr_any(SERVER_PORT);
    bind_impl(sockfd, &server_addr);
    listen_impl(sockfd);
    clientfd = accept_impl(sockfd);

    server_tls_exchange(clientfd);

    app_exit(0);
    return 0;
}
