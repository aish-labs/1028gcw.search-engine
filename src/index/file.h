#ifndef FILE_H
#define FILE_H

#include <stdlib.h>
#include <stdint.h>

typedef struct {
    const char *path;
    uint64_t hash;
} FileEntry;

// Scan the directory/file and return all the files.
// File count returns via `file_count`.
const FileEntry *scan_dir(const char *root, size_t *file_count);

void free_entries(const FileEntry *array, size_t file_count);

#endif
