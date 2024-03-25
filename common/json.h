//
// Created by tjl on 3/25/24.
//

#ifndef TLS_JSON_H
#define TLS_JSON_H

#include <stdlib.h>

struct simple_json {
    char *key;
    char *data;
    size_t key_size;
    size_t data_size;
    struct simple_json *next;
};

char *parse_only_config_name(int argc, char **argv);

void srand_from_json(struct simple_json *json, long dflt);

size_t get_json_value(struct simple_json *json, char *dest, size_t destl, char *key, size_t keyl);

struct simple_json *parse_simple_json(char *filename);

void free_simple_json(struct simple_json *json);

#endif //TLS_JSON_H
