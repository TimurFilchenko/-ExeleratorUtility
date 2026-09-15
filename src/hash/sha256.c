#include "sha256.h"

#include <stdint.h>

static const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static uint32_t rotr(uint32_t value, uint32_t bits)
{
    return (value >> bits) | (value << (32 - bits));
}

static uint32_t ch(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (~x & z);
}

static uint32_t maj(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

static uint32_t sigma0(uint32_t x)
{
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

static uint32_t sigma1(uint32_t x)
{
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

static uint32_t gamma0(uint32_t x)
{
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

static uint32_t gamma1(uint32_t x)
{
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

static uint32_t load_be32(const uint8_t *data)
{
    return ((uint32_t)data[0] << 24) |
           ((uint32_t)data[1] << 16) |
           ((uint32_t)data[2] << 8) |
           (uint32_t)data[3];
}

static void store_be32(uint8_t *data, uint32_t value)
{
    data[0] = (uint8_t)(value >> 24);
    data[1] = (uint8_t)(value >> 16);
    data[2] = (uint8_t)(value >> 8);
    data[3] = (uint8_t)value;
}

static void transform(SHA256_CTX *ctx, const uint8_t data[64])
{
    uint32_t words[64];

    uint32_t a = ctx->state[0];
    uint32_t b = ctx->state[1];
    uint32_t c = ctx->state[2];
    uint32_t d = ctx->state[3];
    uint32_t e = ctx->state[4];
    uint32_t f = ctx->state[5];
    uint32_t g = ctx->state[6];
    uint32_t h = ctx->state[7];

    for (size_t i = 0; i < 16; ++i)
        words[i] = load_be32(data + i * 4);

    for (size_t i = 16; i < 64; ++i) {
        words[i] = gamma1(words[i - 2]) +
                   words[i - 7] +
                   gamma0(words[i - 15]) +
                   words[i - 16];
    }

    for (size_t i = 0; i < 64; ++i) {
        uint32_t t1 = h +
                       sigma1(e) +
                       ch(e, f, g) +
                       k[i] +
                       words[i];

        uint32_t t2 = sigma0(a) + maj(a, b, c);

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

void sha256_init(SHA256_CTX *ctx)
{
    if (ctx == NULL)
        return;

    ctx->data_length = 0;
    ctx->bit_count = 0;

    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
}

void sha256_update(SHA256_CTX *ctx, const uint8_t *data, size_t length)
{
    if (ctx == NULL || (data == NULL && length != 0))
        return;

    while (length > 0) {
        size_t space = sizeof(ctx->data) - ctx->data_length;
        size_t chunk = length < space ? length : space;

        for (size_t i = 0; i < chunk; ++i)
            ctx->data[ctx->data_length + i] = data[i];

        ctx->data_length += chunk;
        data += chunk;
        length -= chunk;

        if (ctx->data_length == sizeof(ctx->data)) {
            transform(ctx, ctx->data);
            ctx->bit_count += 512;
            ctx->data_length = 0;
        }
    }
}

void sha256_final(SHA256_CTX *ctx, uint8_t hash[32])
{
    if (ctx == NULL || hash == NULL)
        return;

    size_t length = ctx->data_length;

    ctx->data[length++] = 0x80;

    if (length > 56) {
        while (length < 64)
            ctx->data[length++] = 0;

        transform(ctx, ctx->data);
        length = 0;
    }

    while (length < 56)
        ctx->data[length++] = 0;

    ctx->bit_count += (uint64_t)ctx->data_length * 8;

    for (size_t i = 0; i < 8; ++i)
        ctx->data[63 - i] =
            (uint8_t)(ctx->bit_count >> (i * 8));

    transform(ctx, ctx->data);

    for (size_t i = 0; i < 8; ++i)
        store_be32(hash + i * 4, ctx->state[i]);
}
