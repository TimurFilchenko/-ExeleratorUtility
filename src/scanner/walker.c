#include "walker.h"
#include "config.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int add_file(FileList *list, const char *path, const char *name, uint64_t size)
{
    if (list == NULL || path == NULL || name == NULL)
        return 0;

    if (list->count >= list->capacity) {
        size_t new_capacity =
            list->capacity == 0 ? 64 : list->capacity * 2;

        if (new_capacity < list->capacity)
            return 0;

        FileInfo *new_items =
            realloc(list->items, new_capacity * sizeof(FileInfo));

        if (new_items == NULL)
            return 0;

        list->items = new_items;
        list->capacity = new_capacity;
    }

    size_t path_len = strlen(path);
    size_t name_len = strlen(name);

    char *file_path = malloc(path_len + 1);
    char *file_name = malloc(name_len + 1);

    if (file_path == NULL || file_name == NULL) {
        free(file_path);
        free(file_name);
        return 0;
    }

    memcpy(file_path, path, path_len + 1);
    memcpy(file_name, name, name_len + 1);

    list->items[list->count].path = file_path;
    list->items[list->count].name = file_name;
    list->items[list->count].size = size;

    list->count++;

    return 1;
}

static int build_path(
    char *buffer,
    size_t buffer_size,
    const char *directory,
    const char *name)
{
    if (buffer == NULL ||
        buffer_size == 0 ||
        directory == NULL ||
        name == NULL)
        return 0;

    size_t directory_len = strlen(directory);

    if (directory_len == 0)
        return 0;

    const char *separator = "";

    if (directory[directory_len - 1] != '\\' &&
        directory[directory_len - 1] != '/')
        separator = "\\";

    int result = snprintf(
        buffer,
        buffer_size,
        "%s%s%s",
        directory,
        separator,
        name
    );

    if (result < 0)
        return 0;

    if ((size_t)result >= buffer_size)
        return 0;

    return 1;
}

static int walk_directory(const char *directory, FileList *list)
{
    if (directory == NULL || list == NULL)
        return 0;

    char pattern[EXELERATOR_MAX_PATH];

    if (!build_path(pattern, sizeof(pattern), directory, "*"))
        return 0;

    WIN32_FIND_DATAA data;
    HANDLE handle = FindFirstFileA(pattern, &data);

    if (handle == INVALID_HANDLE_VALUE)
        return 0;

    int success = 1;

    do {
        if (strcmp(data.cFileName, ".") == 0 ||
            strcmp(data.cFileName, "..") == 0)
            continue;

        char path[EXELERATOR_MAX_PATH];

        if (!build_path(
                path,
                sizeof(path),
                directory,
                data.cFileName)) {
            success = 0;
            break;
        }

        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
                continue;

            if (!walk_directory(path, list)) {
                success = 0;
                break;
            }

            continue;
        }

        ULARGE_INTEGER size;
        size.HighPart = data.nFileSizeHigh;
        size.LowPart = data.nFileSizeLow;

        if (!add_file(
                list,
                path,
                data.cFileName,
                size.QuadPart)) {
            success = 0;
            break;
        }

    } while (FindNextFileA(handle, &data));

    if (GetLastError() != ERROR_NO_MORE_FILES)
        success = 0;

    FindClose(handle);

    return success;
}

int walker_scan(const char *root, FileList *list)
{
    if (root == NULL || list == NULL)
        return 0;

    if (root[0] == '\0')
        return 0;

    DWORD attributes = GetFileAttributesA(root);

    if (attributes == INVALID_FILE_ATTRIBUTES)
        return 0;

    if ((attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        return 0;

    return walk_directory(root, list);
}
