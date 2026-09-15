#include "hash.h"
#include "sha256.h"

#include <stdio.h>

int hash_file(const char *path, uint8_t hash[32])
{
    FILE *file = fopen(path, "rb");

    if (file == NULL)
        return 0;

    SHA256_CTX ctx;
    unsigned char buffer[8192];
    size_t bytes_read;

    sha256_init(&ctx);

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0)
        sha256_update(&ctx, buffer, bytes_read);

    if (ferror(file)) {
        fclose(file);
        return 0;
    }

    sha256_final(&ctx, hash);
    fclose(file);

    return 1;
}
