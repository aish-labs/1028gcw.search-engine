#ifndef FNV1A_H
#define FNV1A_H

#include <stdint.h>

// The hash algorithm used to detect file changes.
//
// File hash returns via `hash`.
//
// Errors:
//   -1 => file read failed.
//
int fnv1a_file(const char *path, uint64_t *hash);

#endif
