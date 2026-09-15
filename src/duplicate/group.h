#ifndef EXELERATOR_GROUP_H
#define EXELERATOR_GROUP_H

#include "types.h"

int group_add(DuplicateGroupList *groups, const FileInfo *file);
int group_add_file(DuplicateGroup *group, const FileInfo *file);
void group_list_free(DuplicateGroupList *groups);

#endif
