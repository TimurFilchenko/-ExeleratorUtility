#include "file.h"

#include <windows.h>

int file_delete(const FileInfo *file)
{
    if (file == NULL || file->path == NULL || file->path[0] == '\0')
        return 0;

    DWORD attributes = GetFileAttributesA(file->path);

    if (attributes == INVALID_FILE_ATTRIBUTES)
        return 0;

    if (attributes & FILE_ATTRIBUTE_DIRECTORY)
        return 0;

    if (!DeleteFileA(file->path))
        return 0;

    return 1;
}
