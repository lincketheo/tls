//
// Created by tjl on 3/8/24.
//

#ifndef TLS_CA_H
#define TLS_CA_H

#include <stdlib.h>

#define CA_BUFFER_SIZE 20

void ca_tls_exchange(
        int clientfd,
        char *pbk,
        size_t pbk_l
);

#endif //TLS_CA_H
