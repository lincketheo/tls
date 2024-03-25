//
// Created by tjl on 3/25/24.
//
#include <json.h>
#include <app.h>
#include <getopt.h>
#include <stdio.h>
#include <dynamic_string.h>
#include <string.h>

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
