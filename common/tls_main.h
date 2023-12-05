//
// Created by Theo Lincke on 12/4/23.
//

#ifndef TLS_TLS_MAIN_H
#define TLS_TLS_MAIN_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "tls_common.h"

#define DEFAULT_PROTOCOL_VERSION                0x0303
#define DEFAULT_LEGACY_COMPRESSION_METHOD       0
#define HANDSHAKE_TYPE_BYTES                    1
#define CERTIFICATE_TYPE_BYTES                  1

struct client_hello {
    protocol_version legacy_version;
    random random;                              // [Appendix C.1](https://datatracker.ietf.org/doc/html/rfc8446#appendix-C.1)
    uint8_t legacy_session_id_bytes;            // 0..32
    uint8_t *legacy_session_id;
    uint16_t cipher_suites_bytes;               // 2..2^16-2
    cipher_suite *cipher_suites;
    uint8_t legacy_compression_methods_bytes;   // 1..2^8-1
    uint8_t *legacy_compression_methods;
    uint16_t extensions_bytes;                  // 8..2^16-1
    struct extension *extensions;
};

struct server_hello {
    protocol_version legacy_version;
    random random;
    uint8_t legacy_session_id_echo_bytes;   // 0..32
    uint8_t *legacy_session_id_echo;
    cipher_suite cipher_suite;
    uint8_t legacy_compression_method;
    uint16_t extensions_bytes;              // 6..2^16-1
    struct extension *extensions;
};

enum certificate_type {
    x509 = 0,
    open_pgp_reserved = 1,
    raw_public_key = 2,
};

struct public_key_data {
    struct uint24 asb_1_subject_public_key_info_bytes;      // 1..2^24-1
    uint8_t asb_1_subject_public_key_info;      // 1..2^24-1
};

struct x509_data {
    struct uint24 cert_data_bytes;              // 1..2^24-1
    uint8_t *cert_data;
};

union certificate_data {
    struct public_key_data public_key_data;
    struct x509_data x509_data;
};

struct certificate_entry {
    enum certificate_type type;
    union certificate_data data;
};

struct certificate {
    uint8_t certificate_request_context_bytes;      // 0..2^8-1
    uint8_t *certificate_request_context;
    struct uint24 certificate_list_bytes;           // 0..2^24-1
    struct certificate_entry certificate_list;
};

struct end_of_early_data {
};
struct encrypted_extensions {
};
struct certificate_request {
};
struct certificate_verify {

};
struct finished {
};
struct new_session_ticket {
};
struct key_update {
};

union msg_data {
    struct client_hello ch;
    struct server_hello sh;
    struct end_of_early_data eoed;
    struct encrypted_extensions ee;
    struct certificate_request cr;
    struct certificate c;
    struct certificate_verify cv;
    struct finished f;
    struct new_session_ticket nst;
    struct key_update ku;
};

enum handshake_type {
    client_hello = 1,
    server_hello = 2,
    new_session_ticket = 4,
    end_of_early_data = 5,
    encrypted_extensions = 8,
    certificate = 11,
    certificate_request = 13,
    certificate_verify = 15,
    finished = 20,
    key_update = 24,
    message_hash = 254,
};

struct handshake {
    enum handshake_type msg_type;
    struct uint24 length;
    union msg_data data;
};

// Sends client hello
void send_client_hello();

// Initializes a hello retry request TODO - section 4.1.4
void init_hrr(struct server_hello *s);

// Checks if server random is a valid hello retry request random
bool hrr_rand_vfy(struct server_hello *s);

// Checks if server random is valid for TLS version 1.2
bool tls12_rand_vfy(struct server_hello *s);

// Checks if server random is valid for TLS versions 1.1-
bool tls11m_rand_vfy(struct server_hello *s);

// Checks if server random is valid for TLS version 1.3
bool tls13_rand_vfy(struct server_hello *s);

void send_server_hello();


#endif //TLS_TLS_MAIN_H
