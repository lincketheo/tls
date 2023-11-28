#include <sys/socket.h>
#include <printf.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <stdbool.h>

static const char message[] = "Hello from the server, my name is Theo Lincke and I like to eat pasta :)";

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

void bind_impl(int sockfd, struct sockaddr_in *server_addr) {
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
}

void write_data_impl(int sockfd, char *buffer, size_t buffer_size) {
    int client = accept(sockfd, NULL, NULL);
    printf("Sending message: %s\n", buffer);
    send(client, buffer, buffer_size, 0);
}

/**
 * Copies msg_head to buffer (as much as possible) - until it finds
 * a null terminator - in which case it returns NULL;
 */
int write_message_to_buffer(char *buffer, size_t buff_size, const char *msg_head) {
    int i = 0;
    do {
        buffer[i] = msg_head[i];
        i++;
    } while (i < buff_size && msg_head[i] != '\0');

    return i;
}

int main() {
    int sockfd = open_socket_impl();
    struct sockaddr_in server_addr = create_sockaddr_in_impl();
    bind_impl(sockfd, &server_addr);

    char buffer[20];
    char* msg_head = message;
    int n;
    while((n = write_message_to_buffer(buffer, 20, msg_head)) == 20){
        msg_head += n;
    }
    write_data_impl(sockfd, buffer, sizeof(buffer));

    return 0;
}
