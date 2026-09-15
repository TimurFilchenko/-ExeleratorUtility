#include "file.h"

#include <stdio.h>
#include <stdlib.h>

int file_compare_content(const FileInfo *first, const FileInfo *second)
{
    if (first == NULL || second == NULL)
        return 0;

    if (first->path == NULL || second->path == NULL)
        return 0;

    if (first->size != second->size)
        return 0;

    FILE *file1 = fopen(first->path, "rb");
    if (file1 == NULL)
        return 0;

    FILE *file2 = fopen(second->path, "rb");
    if (file2 == NULL) {
        fclose(file1);
        return 0;
    }

    unsigned char buffer1[8192];
    unsigned char buffer2[8192];
    uint64_t remaining = first->size;
    int result = 1;

    while (remaining > 0) {
        size_t chunk = remaining > sizeof(buffer1)
            ? sizeof(buffer1)
            : (size_t)remaining;

        size_t read1 = fread(buffer1, 1, chunk, file1);
        size_t read2 = fread(buffer2, 1, chunk, file2);

        if (read1 != chunk || read2 != chunk) {
            result = 0;
            break;
        }

        for (size_t i = 0; i < chunk; ++i) {
            if (buffer1[i] != buffer2[i]) {
                result = 0;
                break;
            }
        }

        if (!result)
            break;

        remaining -= chunk;
    }

    if (ferror(file1) || ferror(file2))
        result = 0;

    fclose(file1);
    fclose(file2);

    return result;
}
