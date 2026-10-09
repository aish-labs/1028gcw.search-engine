#ifndef FNV1A_H
#define FNV1A_H

#include <stdio.h>
#include <stdint.h>

// The hash algorithm used to detect file changes.
// We assume the file exists, but if not it returns 0.
uint64_t fnv1a_file(const char *path);

#endif
