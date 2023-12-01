#include <netinet/in.h>
#include <common.h>
#include <unistd.h>
#include "server.h"

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

    // Create socket and bind / listen
    sockfd = open_stream_socket_impl();
    struct sockaddr_in server_addr = create_internet_addr_any(SERVER_PORT);
    bind_impl(sockfd, &server_addr);
    listen_impl(sockfd);

    server_tls_exchange(sockfd, NULL, 0, NULL, 0);

    app_exit(0);
    return 0;
}
