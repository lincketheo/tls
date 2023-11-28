//
// Created by Theo Lincke on 11/27/23.
//

#include <netinet/in.h>
#include <printf.h>
#include <stdlib.h>
#include <stdbool.h>

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

void connect_impl(int sockfd, struct sockaddr_in *server_addr) {
    int status = connect(sockfd, (struct sockaddr *) server_addr, sizeof(struct sockaddr_in));
    if (status == -1) {
        printf("Error connecting\n");
        exit(1);
    }
}

void read_data_impl(int sockfd, char *buffer, size_t buffer_size) {
    recv(sockfd, buffer, buffer_size, 0);
    printf("Message: %s\n", buffer);
}

bool is_msg_done(char *buffer, size_t buffer_size) {
    char *c = buffer;
    int i;
    for (i = 0; *c != '\0'; i++, c++);
    return i < buffer_size;
}

void read_data_in_chunks(int sockfd, char *buffer, size_t buffer_size) {
    do {
        read_data_impl(sockfd, buffer, buffer_size);
    } while(!is_msg_done(buffer, buffer_size));
}

int main() {
    int sockfd = open_socket_impl();
    struct sockaddr_in server_addr = create_sockaddr_in_impl();
    connect_impl(sockfd, &server_addr);

    char buffer[20];
    read_data_in_chunks(sockfd, buffer, 20);

    return 0;
}