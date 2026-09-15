#ifndef EXELERATOR_TYPES_H
#define EXELERATOR_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    char *path;
    char *name;
    uint64_t size;
} FileInfo;

typedef struct {
    FileInfo *items;
    size_t count;
    size_t capacity;
} FileList;

typedef struct {
    FileInfo **files;
    size_t count;
    size_t capacity;
} DuplicateGroup;

typedef struct {
    DuplicateGroup *groups;
    size_t count;
    size_t capacity;
} DuplicateGroupList;

#endif
