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
#include <string.h>
#include <getopt.h>

#define MAX_CALLBACKS 10

static int exit_callbacks_size = 0;

static void (*app_exit_callbacks[MAX_CALLBACKS])() = {NULL};

void app_exit(int status) {
    for (int i = 0; i < exit_callbacks_size; ++i) {
        if (app_exit_callbacks[i] != NULL) {
            if (get_verbosity() > v_medium) {
                printf("Executing callback %d\n", i);
            }
            (*app_exit_callbacks[i])();
        }
    }
    if (get_verbosity() > v_medium) {
        printf("Exiting\n");
    }
    exit(status);
}

void register_app_exit(void(*on_exit)()) {
    if (exit_callbacks_size >= MAX_CALLBACKS) {
        printf("Can't register any more callbacks");
    }
    app_exit_callbacks[exit_callbacks_size] = on_exit;
    exit_callbacks_size++;
}

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

ssize_t find_char(const char *buffer, size_t buffer_size, char c) {
    for (size_t i = 0; i < buffer_size; ++i) {
        if (buffer[i] == c) {
            return (ssize_t) i;
        }
    }
    return -1;
}

size_t find_char_assert(const char *buffer, size_t buffer_size, char c) {
    for (size_t i = 0; i < buffer_size; ++i) {
        if (buffer[i] == c) {
            return (ssize_t) i;
        }
    }
    printf("Couldn't find char: %c\n", c);
    app_exit(1);
    return -1; // Unreachable
}

enum verbosity get_verbosity() {
    return v_high;
}

void array_exists_guard(struct string *array) {
    if (!array) {
        printf("Passed string is uninitialized\n");
        app_exit(1);
    }
}

void create_string(struct string *string, size_t initial_capacity) {
    array_exists_guard(string);

    string->head = malloc(initial_capacity + 1); // Alloc one more for null terminator
    if (!string->head) {
        printf("Couldn't create string\n");
        app_exit(1);
    }
    string->size = 0;
    string->capacity = initial_capacity;
}

void destroy_string(struct string *string) {
    if (string) {
        if (string->head) {
            if (get_verbosity() > v_medium) {
                printf("Destroying string\n");
            }
            free(string->head);
            string->head = NULL;
        }
    }
}

void reset_string(struct string *string) {
    delete_string(string, string->size);
}

void increase_array_size(struct string *array) {
    size_t new_capacity;
    if (array->capacity == 0) {
        new_capacity = 1;
    } else {
        new_capacity = array->capacity * 2;
    }

    char *new_head = realloc(array->head, new_capacity + 1);
    if (new_head == NULL) {
        printf("Failed to increase string size\n");
        app_exit(1);
    }
    array->head = new_head;
    array->capacity = new_capacity;
}

void append_string(struct string *string, const char *elements, size_t elements_size) {
    array_exists_guard(string);

    size_t new_size = string->size + elements_size;
    while (new_size > string->capacity) {
        increase_array_size(string);
    }
    memcpy(string->head + string->size, elements, elements_size);

    string->size = new_size;
    string->head[string->size] = '\0';
}

void delete_string(struct string *string, size_t num_elements) {
    array_exists_guard(string);

    if (string->size - num_elements < 0) {
        printf("Trying to remove more elements than string has\n");
        app_exit(1);
    }

    string->size -= num_elements;
    string->head[string->size] = '\0';
}

void new_random_string(struct string *string, size_t num_elements) {
    reset_string(string);
    char *elem = malloc(num_elements * sizeof(char));
    for (int i = 0; i < num_elements; ++i) {
        elem[i] = (char) ((long) 'a' + (rand() % 26));
    }
    append_string(string, elem, num_elements);
    free(elem);
}

void copy_string(const struct string *from, struct string *to) {
    reset_string(to);
    append_string(to, from->head, from->size);
}

enum simple_json_state {
    START, // Haven't encountered { yet
    START_KEY, // In between START and KEY
    KEY, // From " to "
    KEY_DATA, // In between Key and Data
    DATA, // From : " to " [, | }|
    DATA_KEY, // In between Data and Key
    END
};

void init_simple_json(struct simple_json *json) {
    json->key_size = 0;
    json->data_size = 0;
    json->key = NULL;
    json->data = NULL;
}

struct simple_json *sj_transition_func(
        enum simple_json_state *state,
        char next,
        struct string *current,
        struct simple_json *head
) {
    switch (*state) {
        case START:
            if (next == '{')
                *state = START_KEY;
            break;
        case START_KEY:
            if (next == '"')
                *state = KEY;
            break;
        case KEY:
            if (next == '"') {
                head->key = malloc(current->size);
                memcpy(head->key, current->head, current->size);
                head->key_size = current->size;
                reset_string(current);
                *state = KEY_DATA;
            } else
                append_string(current, &next, 1);
            break;
        case KEY_DATA:
            if (next == '"')
                *state = DATA;
            break;
        case DATA:
            if (next == '"') {
                head->data = malloc(current->size);
                memcpy(head->data, current->head, current->size);
                head->data_size = current->size;
                reset_string(current);
                *state = DATA_KEY;
            } else
                append_string(current, &next, 1);
            break;
        case DATA_KEY:
            if (next == '"') {
                head->next = malloc(sizeof(struct simple_json));
                init_simple_json(head->next);
                *state = KEY;
                return head->next;
            } else if (next == '}') {
                *state = END;
                head->next = NULL;
                return head->next;
            }
            break;
        case END:
            *state = END;
    }
    return head;
}

void print_simple_json(struct simple_json *json) {
    struct simple_json *head = json;
    while (head != NULL) {
        printf("Key: %.*s, Value: %.*s\n", (int) head->key_size, head->key, (int) head->data_size, head->data);
        head = head->next;
    }
}

struct simple_json *parse_simple_json(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Invalid JSON file: %s\n", filename);
        app_exit(1);
    }

    struct simple_json *ret = malloc(sizeof(struct simple_json));
    init_simple_json(ret);
    struct simple_json *head = ret;

    // The string to hold either the key or value
    struct string value;
    create_string(&value, 10);

    // Current FSA state
    enum simple_json_state state = START;

    // Bytes read
    size_t bytes;

    // Buffer to read data into
    char fbuffer[256];

    while ((bytes = fread(fbuffer, 1, 256, fp)) > 0) {
        for (int i = 0; i < bytes; ++i) {
            head = sj_transition_func(&state, fbuffer[i], &value, head);
        }
    }

    if (get_verbosity() > v_medium) {
        printf("--------\n");
        printf("Parsed JSON %s: \n", filename);
        print_simple_json(ret);
        printf("--------\n");
    }

    destroy_string(&value);

    return ret;
}

void free_simple_json(struct simple_json *json) {
    struct simple_json *head = json;

    while (head != NULL) {
        if (head->data) {
            free(head->data);
            head->data = NULL;
            head->data_size = 0;
        }
        if (head->key) {
            free(head->key);
            head->key = NULL;
            head->key_size = 0;
        }
        struct simple_json *temp = head;
        head = head->next;
        free(temp);
    }
}

size_t get_json_value(struct simple_json *json, char *dest, size_t destl, char *key, size_t keyl) {
    struct simple_json *head = json;
    while (head != NULL) {
        if (head->key_size == keyl && !strncmp(head->key, key, keyl)) {
            size_t s = head->data_size;
            if (destl < head->data_size) {
                printf("Warning: in get_json_value destination is shorter than data size, output buffer will be truncated\n");
                s = destl;
            }
            memcpy(dest, head->data, s);
            return s;
        } else {
            head = head->next;
        }
    }
    return 0;
}

char *parse_only_config_name(int argc, char **argv) {
    struct option long_options[] = {
            {"config", required_argument, 0, 'a'},
            {0, 0,                        0, 0}
    };

    int opt;
    int option_ind = 0;
    char *config_name = NULL;

    while ((opt = getopt_long(argc, argv, "a:", long_options, &option_ind)) != -1) {
        if (opt == 'a') {
            config_name = optarg;
        } else {
            printf("Unknown option: %c\n", optopt);
        }
    }

    if (config_name == NULL) {
        printf("Please provide a file name\n");
        app_exit(1);
    }

    return config_name;
}

void srand_from_json(struct simple_json *json, long dflt) {
    long seedl = dflt;
    char seed_buff[100];
    size_t sl = get_json_value(json, seed_buff, 100, "seed", 4);
    if (sl) {
        char *endptr;
        seedl = strtol(seed_buff, &endptr, 10);
        if (endptr == seed_buff) {
            printf("No digits were found in seed, using default\n");
            seedl = 2;
        }
    }
    if (get_verbosity() > v_none) {
        printf("Seeding: %ld\n", seedl);
    }
    srand(seedl);
}
