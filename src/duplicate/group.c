#include "group.h"

#include <stdlib.h>
#include <stdint.h>

int group_add(DuplicateGroupList *groups, const FileInfo *file)
{
    if (groups == NULL || file == NULL)
        return 0;

    if (groups->count >= groups->capacity) {
        size_t new_capacity =
            groups->capacity == 0 ? 16 : groups->capacity * 2;

        if (new_capacity < groups->capacity)
            return 0;

        DuplicateGroup *new_groups =
            realloc(groups->groups,
                    new_capacity * sizeof(DuplicateGroup));

        if (new_groups == NULL)
            return 0;

        groups->groups = new_groups;
        groups->capacity = new_capacity;
    }

    DuplicateGroup *group = &groups->groups[groups->count];

    group->files = NULL;
    group->count = 0;
    group->capacity = 0;

    if (!group_add_file(group, file)) {
        free(group->files);
        group->files = NULL;
        group->count = 0;
        group->capacity = 0;
        return 0;
    }

    groups->count++;

    return 1;
}

int group_add_file(DuplicateGroup *group, const FileInfo *file)
{
    if (group == NULL || file == NULL)
        return 0;

    if (group->count >= group->capacity) {
        size_t new_capacity =
            group->capacity == 0 ? 4 : group->capacity * 2;

        if (new_capacity < group->capacity)
            return 0;

        FileInfo **new_files =
            realloc(group->files,
                    new_capacity * sizeof(FileInfo *));

        if (new_files == NULL)
            return 0;

        group->files = new_files;
        group->capacity = new_capacity;
    }

    group->files[group->count] = (FileInfo *)file;
    group->count++;

    return 1;
}

void group_list_free(DuplicateGroupList *groups)
{
    if (groups == NULL)
        return;

    for (size_t i = 0; i < groups->count; ++i) {
        free(groups->groups[i].files);

        groups->groups[i].files = NULL;
        groups->groups[i].count = 0;
        groups->groups[i].capacity = 0;
    }

    free(groups->groups);

    groups->groups = NULL;
    groups->count = 0;
    groups->capacity = 0;
}
