//
// Created by Theo Lincke on 12/4/23.
//

#ifndef TLS_TLS_EXTENSIONS_H
#define TLS_TLS_EXTENSIONS_H

#include <stdint.h>
#include "tls_common.h"

#define EXTENSION_TYPE_BYTES                    2
#define SIGNATURE_SCHEME_BYTES                  2

typedef struct {
    uint16_t bytes;  // 1..2^16-1
    uint8_t *data;
} distinguished_name;

/////// TLS Specific Data Types
enum extension_type {
    server_name = 0,                             /* RFC 6066 */
    max_fragment_length = 1,                     /* RFC 6066 */
    status_request = 5,                          /* RFC 6066 */
    supported_groups = 10,                       /* RFC 8422, 7919 */
    signature_algorithms = 13,                   /* RFC 8446 */
    use_srtp = 14,                               /* RFC 5764 */
    heartbeat = 15,                              /* RFC 6520 */
    application_layer_protocol_negotiation = 16, /* RFC 7301 */
    signed_certificate_timestamp = 18,           /* RFC 6962 */
    client_certificate_type = 19,                /* RFC 7250 */
    server_certificate_type = 20,                /* RFC 7250 */
    padding = 21,                                /* RFC 7685 */
    pre_shared_key = 41,                         /* RFC 8446 */
    early_data = 42,                             /* RFC 8446 */
    supported_versions = 43,                     /* RFC 8446 */
    cookie = 44,                                 /* RFC 8446 */
    psk_key_exchange_modes = 45,                 /* RFC 8446 */
    certificate_authorities = 47,                /* RFC 8446 */
    oid_filters = 48,                            /* RFC 8446 */
    post_handshake_auth = 49,                    /* RFC 8446 */
    signature_algorithms_cert = 50,              /* RFC 8446 */
    key_share = 51,                              /* RFC 8446 */
};

struct extension {
    enum extension_type extension_type;
    uint8_t extension_data[2];
};

union supported_versions {
    uint8_t versions_bytes;                 // 2..254
    protocol_version *versions;
    protocol_version selected_version;
};

struct cookie {
    uint16_t cookies_bytes;                 // 1..2^16-1
    uint8_t *cookie;
};

enum signature_scheme {
    /* RSASSA-PKCS1-v1_5 algorithms */
    rsa_pkcs1_sha256 = 0x0401,
    rsa_pkcs1_sha384 = 0x0501,
    rsa_pkcs1_sha512 = 0x0601,

    /* ECDSA algorithms */
    ecdsa_secp256r1_sha256 = 0x0403,
    ecdsa_secp384r1_sha384 = 0x0503,
    ecdsa_secp521r1_sha512 = 0x0603,

    /* RSASSA-PSS algorithms with public key OID rsaEncryption */
    rsa_pss_rsae_sha256 = 0x0804,
    rsa_pss_rsae_sha384 = 0x0805,
    rsa_pss_rsae_sha512 = 0x0806,

    /* EdDSA algorithms */
    ed25519 = 0x0807,
    ed448 = 0x0808,

    /* RSASSA-PSS algorithms with public key OID RSASSA-PSS */
    rsa_pss_pss_sha256 = 0x0809,
    rsa_pss_pss_sha384 = 0x080a,
    rsa_pss_pss_sha512 = 0x080b,

    /* Legacy algorithms */
    rsa_pkcs1_sha1 = 0x0201,
    ecdsa_sha1 = 0x0203,

    /* Reserved Code Points */
    //private_use = 0xFE00..0xFFFF, TODO
};

struct signature_scheme_list {
    uint16_t supported_signature_algorithms_bytes;          // 2..2^16-2
    enum signature_scheme *supported_signature_algorithms;
};

struct certificate_authorities_extension {
    uint16_t authorities_bytes;                             // 3..2^16-1
    distinguished_name *authorities;
};

struct oid_filter {
    uint8_t certificate_extension_oid_bytes;                // 1..2^8-1
    uint8_t *certificate_extension_oid;
    uint8_t certificate_extension_values_bytes;             // 0..2^16-1
    uint8_t *certificate_extension_values;
};

#endif //TLS_TLS_EXTENSIONS_H
