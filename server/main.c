#include <sys/socket.h>
#include <printf.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <unistd.h>

static char message[] = "Hello from the server, my name is Theo Lincke and I like to eat pasta :)";

int open_socket_impl() {
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Error opening socket\n");
        exit(1);
    }
    return sockfd;
}

struct sockaddr_in create_sockaddr_in_impl() {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8000);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    return server_addr;
}

int bind_impl(int sockfd, struct sockaddr_in *server_addr) {
    int status = bind(sockfd, (struct sockaddr *) server_addr, sizeof(struct sockaddr_in));
    if (status == -1) {
        printf("Error binding\n");
        exit(1);
    }
    status = listen(sockfd, 1);
    if (status == -1) {
        printf("Error listening\n");
        exit(1);
    }
    int client = accept(sockfd, NULL, NULL);
    if (client == -1) {
        printf("Error accepting\n");
        exit(1);
    }
    return client;
}

void write_data_impl(int sockfd, int client, char *buffer, size_t buffer_size) {
    printf("Sending message: %s\n", buffer);
    send(client, buffer, buffer_size, 0);
}

/**
 * Copies msg_head to buffer (as much as possible) - until it finds
 * a null terminator - in which case it returns NULL;
 */
void write_message_in_chunks(int sockfd, int client, char *buffer, size_t buff_size, char *full_message) {
    int i;
    char *msg_head = full_message;
    while (true) {
        for (i = 0; i < buff_size && *(msg_head - 1) != '\0'; ++i, msg_head++) {
            buffer[i] = *msg_head;
        }
        write_data_impl(sockfd, client, buffer, i);
        if (i != buff_size) {
            return;
        }
    }
}

int main() {
    int sockfd = open_socket_impl();
    struct sockaddr_in server_addr = create_sockaddr_in_impl();
    int client = bind_impl(sockfd, &server_addr);

    char buffer[20];
    write_message_in_chunks(sockfd, client, buffer, sizeof(buffer), message);

    return 0;
}
