#include "console.h"

#include <stdio.h>
#include <string.h>

#define EXELERATOR_VERSION "0.1.0"

static void console_print_separator(void)
{
    printf("----------------------------------------\n");
}

void console_print_banner(void)
{
    console_print_separator();
    printf("Exelerator Utility\n");
    printf("Duplicate File Finder\n");
    printf("Version %s\n", EXELERATOR_VERSION);
    console_print_separator();
    printf("\n");
}

int console_read_path(char *buffer, size_t size)
{
    if (buffer == NULL || size < 2)
        return 0;

    printf("Enter directory path: ");

    if (fgets(buffer, (int)size, stdin) == NULL)
        return 0;

    buffer[strcspn(buffer, "\r\n")] = '\0';

    if (buffer[0] == '\0')
        return 0;

    return 1;
}

void console_print_scan_result(const FileList *files)
{
    if (files == NULL)
        return;

    printf("\nScan completed.\n");
    printf("Files found: %zu\n", files->count);
}

void console_print_duplicates(const DuplicateGroupList *groups)
{
    if (groups == NULL)
        return;

    printf("\n");

    if (groups->count == 0) {
        printf("No duplicate files found.\n");
        return;
    }

    console_print_separator();
    printf("Duplicate groups found: %zu\n", groups->count);
    console_print_separator();

    for (size_t i = 0; i < groups->count; ++i) {
        const DuplicateGroup *group = &groups->groups[i];

        printf("\nGroup %zu: %zu files\n", i + 1, group->count);

        for (size_t j = 0; j < group->count; ++j) {
            const FileInfo *file = group->files[j];

            if (file == NULL)
                continue;

            printf("  [%zu] %s", j + 1, file->path);

            if (j == 0)
                printf(" [KEEP]");

            printf("\n");
        }
    }

    printf("\n");
}

int console_confirm_delete(const FileInfo *file)
{
    if (file == NULL || file->path == NULL || file->path[0] == '\0')
        return 0;

    char answer[16];

    printf("Delete file:\n");
    printf("%s\n", file->path);
    printf("Confirm deletion [y/N]: ");

    if (fgets(answer, sizeof(answer), stdin) == NULL)
        return 0;

    answer[strcspn(answer, "\r\n")] = '\0';

    if (answer[0] == 'y' || answer[0] == 'Y')
        return 1;

    return 0;
}
