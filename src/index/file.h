#ifndef FILE_H
#define FILE_H

#include <stdlib.h>
#include <stdint.h>

typedef struct {
    const char *path;
    uint64_t hash;
} FileEntry;

// Scan the directory/file and return all the files.
// File count returns via `count`.
const FileEntry *scan_dir(const char *root, size_t *count);

void free_entries(const FileEntry *list, size_t count);

#endif
