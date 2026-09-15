#include "scanner.h"

#include <stdlib.h>

int scanner_init(FileList *list)
{
    if (list == NULL)
        return 0;

    list->items = NULL;
    list->count = 0;
    list->capacity = 0;

    return 1;
}

void scanner_free(FileList *list)
{
    if (list == NULL)
        return;

    for (size_t i = 0; i < list->count; ++i) {
        free(list->items[i].path);
        free(list->items[i].name);
    }

    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}
