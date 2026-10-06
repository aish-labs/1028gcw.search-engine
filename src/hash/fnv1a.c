#include "fnv1a.h"

// The hash algorithm used to detect file changes.
// We assume the file exists, but if not it returns 0.
uint64_t fnv1a_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) return 0;

    uint64_t hash = 14695981039346656037ULL;
    unsigned char buf[4096];
    size_t n;

    while ((n = fread(buf, 1, sizeof buf, file)) > 0) {
        for (size_t i = 0; i < n; i++) {
            hash ^= buf[i];
            hash *= 1099511628211ULL;
        }
    }

    fclose(file);
    return hash;
}
