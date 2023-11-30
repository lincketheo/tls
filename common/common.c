//
// Created by tjl on 11/30/23.
//
#include <common.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <strings.h>

static void (*app_exit_callback)() = NULL;

void app_exit() {
    if (app_exit_callback != NULL) {
        printf("Cleaning up resources\n");
        (*app_exit_callback)();
    }
    printf("Exiting\n");
    exit(1);
}

void register_app_app_exit(void(*on_exit)()) {
    app_exit_callback = on_exit;
}

int open_stream_socket_impl() {
    if (get_verbosity() > v_none) {
        printf("Opening a stream socket\n");
    }
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Error opening socket\n");
        app_exit();
    }
    if (get_verbosity() > v_none) {
        printf("Stream socket successfully opened\n");
    }
    return sockfd;
}

struct sockaddr_in create_internet_addr_any(int port) {
    if (get_verbosity() > v_none) {
        printf("Creating socket address in internet namespace on port %d\n", port);
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    return server_addr;
}

void bind_impl(int sockfd, struct sockaddr_in *to_address) {
    if (get_verbosity() > v_none) {
        printf("Binding to socket address\n");
    }
    int status = bind(sockfd, (struct sockaddr *) to_address, sizeof(struct sockaddr_in));
    if (status == -1) {
        printf("Error binding\n");
        app_exit();
    }
    if (get_verbosity() > v_none) {
        printf("Bind Success\n");
    }
}

void listen_impl(int sockfd) {
    if (get_verbosity() > v_none) {
        printf("Listening on socket %d\n", sockfd);
    }
    int status = listen(sockfd, 1);
    if (status == -1) {
        printf("Error listening\n");
        app_exit();
    }
    if (get_verbosity() > v_none) {
        printf("Listen Success\n");
    }
}

int accept_impl(int sockfd) {
    if (get_verbosity() > v_none) {
        printf("Accepting Client connection on socket %d\n", sockfd);
    }
    int client = accept(sockfd, NULL, NULL);
    if (client == -1) {
        printf("Error accepting\n");
        app_exit();
    }
    if (get_verbosity() > v_none) {
        printf("Accept success\n");
    }
    return client;
}

void connect_impl(int sockfd, struct sockaddr_in *server_addr) {
    if(get_verbosity() > v_none) {
        printf("Connecting to server address\n");
    }
    int status = connect(sockfd, (struct sockaddr *) server_addr, sizeof(struct sockaddr_in));
    if (status == -1) {
        printf("Error connecting\n");
        app_exit();
    }
    if(get_verbosity() > v_none) {
        printf("Connection success\n");
    }
}

size_t send_impl(int to_socket, const char *buffer, size_t buffer_size) {
    if (get_verbosity() > v_none) {
        printf("Sending %zu bytes. Message: [%.*s]\n", buffer_size, (int) buffer_size, buffer);
    }
    ssize_t sent = send(to_socket, buffer, buffer_size, 0);
    if (sent == -1) {
        printf("Error sending\n");
        app_exit();
    }
    if (get_verbosity() > v_none) {
        printf("Sent %zu bytes.\n", sent);
    }
    return (size_t) sent;
}

void send_impl_flush(int to_socket, const char *buffer, size_t len) {
    const char *msg_head = buffer;
    while (len > 0) {
        size_t sent = send_impl(to_socket, msg_head, len);
        msg_head += sent;
        len -= sent;
    }
}

void send_in_chunks(int to_socket, size_t chunk_size, const char *full_message) {
    const char *msg_head = full_message;
    while (true) {

        ssize_t ind_end = find_char(msg_head, chunk_size, '\0');

        if (ind_end == -1) {
            size_t sent = send_impl(to_socket, msg_head, chunk_size);
            msg_head += sent;
        } else {
            size_t len = ind_end + 1;
            send_impl_flush(to_socket, msg_head, len);
            return;
        }
    }
}

size_t recv_impl(int sockfd, char *buffer, size_t buffer_size) {
    if (get_verbosity() > v_none) {
        printf("Requesting %zu bytes\n", buffer_size);
    }

    ssize_t recvd = recv(sockfd, buffer, buffer_size, 0);
    if (recvd == -1) {
        printf("Error receiving message\n");
        app_exit();
    }

    if (get_verbosity() > v_none) {
        printf("Received %zu bytes. Message: [%.*s]\n", recvd, (int) recvd, buffer);
    }

    return (size_t) recvd;
}

void recv_in_chunks(int sockfd, char *buffer, size_t buffer_size) {
    char *buf_head = buffer;
    size_t requested_size = buffer_size;
    bzero(buffer, buffer_size);

    while (true) {
        size_t recvd = recv_impl(sockfd, buf_head, requested_size);

        // Finished
        if (find_char(buf_head, recvd, '\0') != -1) {
            return;
        }

        // Check if we need to receive more to fill our buffer
        if (requested_size == recvd) {
            buf_head = buffer;
            requested_size = buffer_size;
            bzero(buf_head, requested_size);
        } else {
            // Assumes recvd < requested_size - if not - recv_impl will throw soon
            requested_size = (requested_size - recvd);
            buf_head = buf_head + recvd;
        }
    }
}

ssize_t find_char(const char *buffer, size_t buffer_size, char c) {
    for (size_t i = 0; i < buffer_size; ++i) {
        if (buffer[i] == c) {
            return (ssize_t) i;
        }
    }
    return -1;
}

enum verbosity get_verbosity() {
    return v_high;
}
