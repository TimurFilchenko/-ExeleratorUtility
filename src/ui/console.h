#ifndef EXELERATOR_CONSOLE_H
#define EXELERATOR_CONSOLE_H

#include "types.h"

void console_print_banner(void);
int console_read_path(char *buffer, size_t size);
void console_print_scan_result(const FileList *files);
void console_print_duplicates(const DuplicateGroupList *groups);
int console_confirm_delete(const FileInfo *file);

#endif
