//
// Created by tjl on 3/25/24.
//
#include <app.h>
#include <stdio.h>
#include <utils.h>

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
    return 0; // Unreachable
}
