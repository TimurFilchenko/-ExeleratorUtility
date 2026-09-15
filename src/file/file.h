#ifndef EXELERATOR_FILE_H
#define EXELERATOR_FILE_H

#include "types.h"

int file_compare_content(const FileInfo *first, const FileInfo *second);
int file_delete(const FileInfo *file);

#endif
