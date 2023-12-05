//
// Created by Theo Lincke on 12/4/23.
//

#ifndef TLS_TLS_COMMON_H
#define TLS_TLS_COMMON_H

#include <stdint.h>

/////// Typedefs
typedef uint8_t random[32];
typedef uint8_t cipher_suite[2];
typedef uint16_t protocol_version;
struct uint24 {
    uint32_t int24 : 24;
};

#endif //TLS_TLS_COMMON_H
