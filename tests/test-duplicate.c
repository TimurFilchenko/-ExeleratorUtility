#include "duplicate.h"
#include "scanner.h"
#include "walker.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>

static int create_test_file(const char *path, const char *content)
{
    FILE *file = fopen(path, "wb");

    if (file == NULL)
        return 0;

    size_t length = strlen(content);
    size_t written = fwrite(content, 1, length, file);

    fclose(file);

    return written == length;
}

static void cleanup_test_directory(
    const char *directory,
    const char *file1,
    const char *file2,
    const char *file3,
    const char *file4,
    const char *file5,
    const char *file6)
{
    remove(file1);
    remove(file2);
    remove(file3);
    remove(file4);
    remove(file5);
    remove(file6);
    RemoveDirectoryA(directory);
}

static int group_contains(
    const DuplicateGroup *group,
    const char *path)
{
    if (group == NULL || path == NULL)
        return 0;

    for (size_t i = 0; i < group->count; ++i) {
        if (group->files[i] == NULL ||
            group->files[i]->path == NULL)
            continue;

        if (strcmp(group->files[i]->path, path) == 0)
            return 1;
    }

    return 0;
}

static int verify_pair_group(
    const DuplicateGroup *group,
    const char *file1,
    const char *file2)
{
    if (group == NULL || group->count != 2)
        return 0;

    return group_contains(group, file1) &&
           group_contains(group, file2);
}

static int verify_triple_group(
    const DuplicateGroup *group,
    const char *file1,
    const char *file2,
    const char *file3)
{
    if (group == NULL || group->count != 3)
        return 0;

    return group_contains(group, file1) &&
           group_contains(group, file2) &&
           group_contains(group, file3);
}

int main(void)
{
    const char *directory = "test_duplicate_data";

    const char *file1 = "test_duplicate_data\\file1.txt";
    const char *file2 = "test_duplicate_data\\file2.txt";
    const char *file3 = "test_duplicate_data\\file3.txt";
    const char *file4 = "test_duplicate_data\\file4.txt";
    const char *file5 = "test_duplicate_data\\file5.txt";
    const char *file6 = "test_duplicate_data\\file6.txt";

    FileList files;
    DuplicateGroupList groups;

    if (!CreateDirectoryA(directory, NULL)) {
        DWORD error = GetLastError();

        if (error != ERROR_ALREADY_EXISTS) {
            printf("Failed to create test directory\n");
            return 1;
        }
    }

    if (!scanner_init(&files)) {
        printf("scanner_init failed\n");
        RemoveDirectoryA(directory);
        return 1;
    }

    if (!create_test_file(file1, "duplicate")) {
        printf("Failed to create file1\n");
        scanner_free(&files);
        RemoveDirectoryA(directory);
        return 1;
    }

    if (!create_test_file(file2, "duplicate")) {
        printf("Failed to create file2\n");
        cleanup_test_directory(
            directory, file1, file2, file3,
            file4, file5, file6);
        scanner_free(&files);
        return 1;
    }

    if (!create_test_file(file3, "different")) {
        printf("Failed to create file3\n");
        cleanup_test_directory(
            directory, file1, file2, file3,
            file4, file5, file6);
        scanner_free(&files);
        return 1;
    }

    if (!create_test_file(file4, "triple")) {
        printf("Failed to create file4\n");
        cleanup_test_directory(
            directory, file1, file2, file3,
            file4, file5, file6);
        scanner_free(&files);
        return 1;
    }

    if (!create_test_file(file5, "triple")) {
        printf("Failed to create file5\n");
        cleanup_test_directory(
            directory, file1, file2, file3,
            file4, file5, file6);
        scanner_free(&files);
        return 1;
    }

    if (!create_test_file(file6, "triple")) {
        printf("Failed to create file6\n");
        cleanup_test_directory(
            directory, file1, file2, file3,
            file4, file5, file6);
        scanner_free(&files);
        return 1;
    }

    if (!walker_scan(directory, &files)) {
        printf("walker_scan failed\n");
        cleanup_test_directory(
            directory, file1, file2, file3,
            file4, file5, file6);
        scanner_free(&files);
        return 1;
    }

    if (files.count != 6) {
        printf("Unexpected file count: %zu\n", files.count);
        cleanup_test_directory(
            directory, file1, file2, file3,
            file4, file5, file6);
        scanner_free(&files);
        return 1;
    }

    if (!duplicate_find(&files, &groups)) {
        printf("duplicate_find failed\n");
        cleanup_test_directory(
            directory, file1, file2, file3,
            file4, file5, file6);
        scanner_free(&files);
        return 1;
    }

    if (groups.count != 2) {
        printf("Unexpected duplicate group count: %zu\n",
               groups.count);

        duplicate_free(&groups);
        cleanup_test_directory(
            directory, file1, file2, file3,
            file4, file5, file6);
        scanner_free(&files);
        return 1;
    }

    int found_pair = 0;
    int found_triple = 0;

    for (size_t i = 0; i < groups.count; ++i) {
        const DuplicateGroup *group = &groups.groups[i];

        if (verify_pair_group(group, file1, file2))
            found_pair = 1;

        if (verify_triple_group(
                group,
                file4,
                file5,
                file6))
            found_triple = 1;
    }

    if (!found_pair) {
        printf("Pair duplicate group not found\n");
        duplicate_free(&groups);
        cleanup_test_directory(
            directory, file1, file2, file3,
            file4, file5, file6);
        scanner_free(&files);
        return 1;
    }

    if (!found_triple) {
        printf("Triple duplicate group not found\n");
        duplicate_free(&groups);
        cleanup_test_directory(
            directory, file1, file2, file3,
            file4, file5, file6);
        scanner_free(&files);
        return 1;
    }

    for (size_t i = 0; i < groups.count; ++i) {
        const DuplicateGroup *group = &groups.groups[i];

        if (group_contains(group, file3)) {
            printf("Non-duplicate file was included\n");
            duplicate_free(&groups);
            cleanup_test_directory(
                directory, file1, file2, file3,
                file4, file5, file6);
            scanner_free(&files);
            return 1;
        }
    }

    printf("Duplicate test passed\n");

    duplicate_free(&groups);

    cleanup_test_directory(
        directory,
        file1,
        file2,
        file3,
        file4,
        file5,
        file6);

    scanner_free(&files);

    return 0;
}
