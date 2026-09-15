#ifndef EXELERATOR_DUPLICATE_H
#define EXELERATOR_DUPLICATE_H

#include "types.h"

int duplicate_find(const FileList *files, DuplicateGroupList *groups);
void duplicate_free(DuplicateGroupList *groups);

#endif
