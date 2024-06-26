//
// Created by tjl on 3/25/24.
//
#include <sockets.h>
#include <app.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <strings.h>
#include <utils.h>

int open_stream_socket_impl() {
    if (get_verbosity() > v_medium) {
        printf("Opening a stream socket\n");
    }
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Error opening socket\n");
        app_exit(1);
    }
    if (get_verbosity() > v_medium) {
        printf("Stream socket successfully opened\n");
    }
    return sockfd;
}

struct sockaddr_in create_internet_addr_any(int port) {
    if (get_verbosity() > v_medium) {
        printf("Creating socket address in internet namespace on port %d\n", port);
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    return server_addr;
}

void bind_impl(int sockfd, struct sockaddr_in *to_address) {
    if (get_verbosity() > v_medium) {
        printf("Binding to socket address\n");
    }
    int status = bind(sockfd, (struct sockaddr *) to_address, sizeof(struct sockaddr_in));
    if (status == -1) {
        printf("Error binding\n");
        app_exit(1);
    }
    if (get_verbosity() > v_medium) {
        printf("Bind Success\n");
    }
}

void listen_impl(int sockfd) {
    if (get_verbosity() > v_medium) {
        printf("Listening on socket %d\n", sockfd);
    }
    int status = listen(sockfd, 1);
    if (status == -1) {
        printf("Error listening\n");
        app_exit(1);
    }
    if (get_verbosity() > v_medium) {
        printf("Listen Success\n");
    }
}

int accept_impl(int sockfd) {
    if (get_verbosity() > v_medium) {
        printf("Accepting Client connection on socket %d\n", sockfd);
    }
    int client = accept(sockfd, NULL, NULL);
    if (client == -1) {
        printf("Error accepting\n");
        app_exit(1);
    }
    if (get_verbosity() > v_medium) {
        printf("Accept success\n");
    }
    return client;
}

void connect_impl(int sockfd, struct sockaddr_in *server_addr) {
    if (get_verbosity() > v_medium) {
        printf("Connecting to server address\n");
    }
    int status = connect(sockfd, (struct sockaddr *) server_addr, sizeof(struct sockaddr_in));
    if (status == -1) {
        printf("Error connecting\n");
        app_exit(1);
    }
    if (get_verbosity() > v_medium) {
        printf("Connection success\n");
    }
}

size_t send_impl(int to_socket, const char *buffer, size_t buffer_size) {
    if (get_verbosity() > v_medium) {
        printf("Sending %zu bytes. Message: [%.*s]\n", buffer_size, (int) buffer_size, buffer);
    }
    ssize_t sent = send(to_socket, buffer, buffer_size, 0);
    if (sent == -1) {
        printf("Error sending\n");
        app_exit(1);
    }
    if (get_verbosity() > v_medium) {
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

void send_in_chunks(
        int to_socket,
        size_t chunk_size,
        const char *full_message) {
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
    if (get_verbosity() > v_medium) {
        printf("Requesting %zu bytes\n", buffer_size);
    }

    ssize_t recvd = recv(sockfd, buffer, buffer_size, 0);
    if (recvd == -1) {
        printf("Error receiving message\n");
        app_exit(1);
    }

    if (get_verbosity() > v_medium) {
        printf("Received %zu bytes. Message: [%.*s]\n", recvd, (int) recvd, buffer);
    }

    return (size_t) recvd;
}

void recv_in_chunks(
        int sockfd,
        char *buffer,
        size_t buffer_size,
        void (*consume_buffer)(bool end)) {

    char *buf_head = buffer;
    size_t requested_size = buffer_size;
    bzero(buffer, buffer_size);

    while (true) {
        size_t recvd = recv_impl(sockfd, buf_head, requested_size);


        // Finished
        if (find_char(buf_head, recvd, '\0') != -1) {
            // Consume data one last time
            (*consume_buffer)(true);
            return;
        }

        // Check if we need to receive more to fill our buffer
        if (requested_size == recvd) {
            buf_head = buffer;
            requested_size = buffer_size;

            // Consume data when buffer is full
            (*consume_buffer)(false);

            bzero(buffer, buffer_size);
        } else {
            // Assumes recvd < requested_size - if not - recv_impl will throw soon
            requested_size = (requested_size - recvd);
            buf_head = buf_head + recvd;
        }
    }
}

