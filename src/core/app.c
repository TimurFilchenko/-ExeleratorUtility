#include "app.h"
#include "config.h"
#include "duplicate.h"
#include "scanner.h"
#include "walker.h"
#include "file.h"
#include "console.h"

#include <stdio.h>

int exelerator_run(void)
{
    char path[EXELERATOR_MAX_PATH];
    FileList files;
    DuplicateGroupList groups;

    console_print_banner();

    if (!console_read_path(path, sizeof(path))) {
        printf("Invalid directory path.\n");
        return 1;
    }

    if (!scanner_init(&files)) {
        printf("Failed to initialize scanner.\n");
        return 1;
    }

    if (!walker_scan(path, &files)) {
        printf("Failed to scan directory.\n");
        scanner_free(&files);
        return 1;
    }

    console_print_scan_result(&files);

    if (!duplicate_find(&files, &groups)) {
        printf("Failed to find duplicate files.\n");
        scanner_free(&files);
        return 1;
    }

    console_print_duplicates(&groups);

    if (groups.count > 0) {
        printf("Duplicate files can be removed individually.\n");
        printf("The first file in each group will be preserved.\n\n");

        for (size_t i = 0; i < groups.count; ++i) {
            DuplicateGroup *group = &groups.groups[i];

            for (size_t j = 1; j < group->count; ++j) {
                FileInfo *file = group->files[j];

                if (file == NULL)
                    continue;

                if (console_confirm_delete(file)) {
                    if (file_delete(file))
                        printf("Deleted successfully.\n\n");
                    else
                        printf("Failed to delete file.\n\n");
                } else {
                    printf("File was kept.\n\n");
                }
            }
        }
    }

    duplicate_free(&groups);
    scanner_free(&files);

    return 0;
}
