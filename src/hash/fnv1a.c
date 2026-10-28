#include "fnv1a.h"
#include <stdio.h>

int fnv1a_file(const char *path, uint64_t *hash) {
    FILE *file = fopen(path, "rb");
    if (!file) return -1;

    uint64_t h = 14695981039346656037ULL;
    unsigned char buf[65536];
    size_t n;

    while ((n = fread(buf, 1, sizeof buf, file)) > 0) {
        for (size_t i = 0; i < n; i++) {
            h ^= buf[i];
            h *= 1099511628211ULL;
        }
    }

    int err = ferror(file);
    fclose(file);
    if (err) return -1;

    *hash = h;
    return 0;
}
