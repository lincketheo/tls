//
// Created by tjl on 3/25/24.
//

#ifndef TLS_UTILS_H
#define TLS_UTILS_H

#include <stdlib.h>

ssize_t find_char(const char *buffer, size_t buffer_size, char c);

size_t find_char_assert(const char *buffer, size_t buffer_size, char c);

#endif //TLS_UTILS_H
