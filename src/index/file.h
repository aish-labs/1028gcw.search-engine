#ifndef FILE_H
#define FILE_H

#include <stdlib.h>
#include <stdint.h>

typedef struct {
    char *path;
    uint64_t hash;
} FileEntry;

// Scan the directory and return all the files.
// Unreadable files are skipped silently.
//
// File count returns via `file_count`.
// FileEntry array returns via `array`.
//
// Errors: (If error, `file_count` will be set to 0, `array` will be freed and set to NULL.)
//   -1 => opendir failed on `root`;
//   -2 => path malloc failed;
//   -3 => array realloc failed.
//
int scan_dir(const char *root, FileEntry **array, size_t *file_count);

// Free a `FileEntry` instance.
void free_entries(FileEntry *array, size_t file_count);

#endif
