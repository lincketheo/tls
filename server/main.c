#include <netinet/in.h>
#include <common.h>
#include <unistd.h>
#include "server.h"

#define SEND_CHUNK_SIZE 40

static char message[] = "Hello from the server, my name is Theo Lincke and I like to eat pasta :)";
static int sockfd = -1;

void close_impl() {
    if (sockfd > 0) {
        if (get_verbosity() > v_none) {
            printf("Closing server socket\n");
        }
        close(sockfd);
    }
}

int main() {
    register_app_app_exit(close_impl);

    // Create socket and bind / listen
    sockfd = open_stream_socket_impl();
    struct sockaddr_in server_addr = create_internet_addr_any(SERVER_PORT);
    bind_impl(sockfd, &server_addr);
    listen_impl(sockfd);

    server_tls_exchange(sockfd, NULL, 0, NULL, 0);

    close_impl();

    return 0;
}
