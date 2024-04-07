//
// Created by Theo Lincke on 12/4/23.
//
#include <tls_main.h>

void send_client_hello() {
    // Send struct client_hello
    // Wait for server_hello or hello_retry_request
    // If early data is in use - client may transmit early application data [section 2.3](https://datatracker.ietf.org/doc/html/rfc8446#section-2.3)
}

void send_server_hello() {

}

uint8_t httoi(enum handshake_type type) {
    return type;
}

uint16_t ettoi(enum extension_type type) {
    return type;
}

void guard_server_hello_struct(struct server_hello *s) {
    if (!s) {
        printf("Invalid struct server_hello");
        app_exit(1);
    }
}

static const uint8_t hello_retry_request_random[32] = {0xCF, 0x21, 0xAD, 0x74, 0xE5, 0x9A, 0x61, 0x11, 0xBE, 0x1D, 0x8C,
                                                       0x02,
                                                       0x1E, 0x65, 0xB8, 0x91, 0xC2, 0xA2, 0x11, 0x16, 0x7A, 0xBB, 0x8C,
                                                       0x5E,
                                                       0x07, 0x9E, 0x09, 0xE2, 0xC8, 0xA8, 0x33, 0x9};

void init_hrr(struct server_hello *s) {
    guard_server_hello_struct(s);
    memset(s, 0, sizeof(struct server_hello));
    memcpy(s->random, hello_retry_request_random, 32);
}

bool hrr_rand_vfy(struct server_hello *s) {
    guard_server_hello_struct(s);
    return memcmp(s->random, hello_retry_request_random, 32) == 0;
}

static const uint8_t last_8_byte_server_hello_tls_1_2p[8] = {0x44, 0x4F, 0x57, 0x4E, 0x47, 0x52, 0x44, 0x01};

bool tls12_rand_vfy(struct server_hello *s) {
    guard_server_hello_struct(s);
    return memcmp(s->random[32 - 8], last_8_byte_server_hello_tls_1_2p, 8);
}

static const uint8_t last_8_byte_server_hello_tls_1_1m[8] = {0x44, 0x4F, 0x57, 0x4E, 0x47, 0x52, 0x44, 0x00};

bool tls11m_rand_vfy(struct server_hello *s) {
    guard_server_hello_struct(s);
    return memcmp(s->random[32 - 8], last_8_byte_server_hello_tls_1_1m, 8);
}

bool tls13_r_vfy(struct server_hello *s) {
    return !tls11m_rand_vfy(s) && !tls12_rand_vfy(s);
}
