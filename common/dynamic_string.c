//
// Created by tjl on 3/25/24.
//
#include <dynamic_string.h>
#include <stdio.h>
#include <app.h>
#include <string.h>
#include <stdlib.h>

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
