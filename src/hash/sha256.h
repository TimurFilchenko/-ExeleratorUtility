#ifndef EXELERATOR_SHA256_H
#define EXELERATOR_SHA256_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t data[64];
    uint32_t state[8];
    uint64_t bit_count;
    size_t data_length;
} SHA256_CTX;

void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, const uint8_t *data, size_t length);
void sha256_final(SHA256_CTX *ctx, uint8_t hash[32]);

#endif
