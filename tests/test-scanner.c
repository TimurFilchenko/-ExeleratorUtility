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
    const char *file2)
{
    remove(file1);
    remove(file2);
    RemoveDirectoryA(directory);
}

int main(void)
{
    const char *directory = "test_scanner_data";
    const char *file1 = "test_scanner_data\\file1.txt";
    const char *file2 = "test_scanner_data\\file2.txt";

    FileList list;

    if (!CreateDirectoryA(directory, NULL)) {
        DWORD error = GetLastError();

        if (error != ERROR_ALREADY_EXISTS) {
            printf("Failed to create test directory\n");
            return 1;
        }
    }

    if (!scanner_init(&list)) {
        printf("scanner_init failed\n");
        RemoveDirectoryA(directory);
        return 1;
    }

    if (!create_test_file(file1, "Exelerator")) {
        printf("Failed to create test file 1\n");
        scanner_free(&list);
        RemoveDirectoryA(directory);
        return 1;
    }

    if (!create_test_file(file2, "Test")) {
        printf("Failed to create test file 2\n");
        remove(file1);
        scanner_free(&list);
        RemoveDirectoryA(directory);
        return 1;
    }

    if (!walker_scan(directory, &list)) {
        printf("walker_scan failed\n");
        cleanup_test_directory(directory, file1, file2);
        scanner_free(&list);
        return 1;
    }

    if (list.count != 2) {
        printf("Unexpected file count: %zu\n", list.count);
        cleanup_test_directory(directory, file1, file2);
        scanner_free(&list);
        return 1;
    }

    int found_file1 = 0;
    int found_file2 = 0;

    for (size_t i = 0; i < list.count; ++i) {
        if (strcmp(list.items[i].path, file1) == 0)
            found_file1 = 1;

        if (strcmp(list.items[i].path, file2) == 0)
            found_file2 = 1;
    }

    if (!found_file1 || !found_file2) {
        printf("Scanner returned unexpected files\n");
        cleanup_test_directory(directory, file1, file2);
        scanner_free(&list);
        return 1;
    }

    if (list.items[0].size == 0 || list.items[1].size == 0) {
        printf("Scanner returned invalid file size\n");
        cleanup_test_directory(directory, file1, file2);
        scanner_free(&list);
        return 1;
    }

    printf("Scanner test passed\n");

    cleanup_test_directory(directory, file1, file2);
    scanner_free(&list);

    return 0;
}
