#include "duplicate.h"
#include "group.h"
#include "hash.h"
#include "file.h"

#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 32

static int same_hash(const uint8_t first[HASH_SIZE],
                     const uint8_t second[HASH_SIZE])
{
    return memcmp(first, second, HASH_SIZE) == 0;
}

static int has_same_size(const FileList *files, size_t index)
{
    for (size_t i = index + 1; i < files->count; ++i) {
        if (files->items[index].size == files->items[i].size)
            return 1;
    }

    return 0;
}

static int duplicate_find_group(
    const FileList *files,
    size_t index,
    unsigned char *used,
    DuplicateGroupList *groups)
{
    uint8_t base_hash[HASH_SIZE];

    if (!hash_file(files->items[index].path, base_hash))
        return 1;

    DuplicateGroup group;
    group.files = NULL;
    group.count = 0;
    group.capacity = 0;

    if (!group_add_file(&group, &files->items[index])) {
        free(group.files);
        return 0;
    }

    for (size_t i = index + 1; i < files->count; ++i) {
        if (used[i])
            continue;

        if (files->items[index].size != files->items[i].size)
            continue;

        uint8_t current_hash[HASH_SIZE];

        if (!hash_file(files->items[i].path, current_hash))
            continue;

        if (!same_hash(base_hash, current_hash))
            continue;

        if (!file_compare_content(
                &files->items[index],
                &files->items[i])) {
            continue;
        }

        if (!group_add_file(&group, &files->items[i])) {
            free(group.files);
            return 0;
        }

        used[i] = 1;
    }

    if (group.count < 2) {
        free(group.files);
        return 1;
    }

    if (groups->count >= groups->capacity) {
        size_t new_capacity =
            groups->capacity == 0 ? 16 : groups->capacity * 2;

        if (new_capacity < groups->capacity) {
            free(group.files);
            return 0;
        }

        DuplicateGroup *new_groups =
            realloc(
                groups->groups,
                new_capacity * sizeof(DuplicateGroup)
            );

        if (new_groups == NULL) {
            free(group.files);
            return 0;
        }

        groups->groups = new_groups;
        groups->capacity = new_capacity;
    }

    groups->groups[groups->count] = group;
    groups->count++;

    used[index] = 1;

    return 1;
}

int duplicate_find(const FileList *files, DuplicateGroupList *groups)
{
    if (files == NULL || groups == NULL)
        return 0;

    groups->groups = NULL;
    groups->count = 0;
    groups->capacity = 0;

    if (files->count < 2)
        return 1;

    unsigned char *used = calloc(files->count, sizeof(unsigned char));

    if (used == NULL)
        return 0;

    for (size_t i = 0; i < files->count; ++i) {
        if (used[i])
            continue;

        if (!has_same_size(files, i))
            continue;

        if (!duplicate_find_group(files, i, used, groups)) {
            free(used);
            duplicate_free(groups);
            return 0;
        }
    }

    free(used);

    return 1;
}

void duplicate_free(DuplicateGroupList *groups)
{
    if (groups == NULL)
        return;

    group_list_free(groups);
}
