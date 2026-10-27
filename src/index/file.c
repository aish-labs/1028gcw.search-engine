#include "file.h"
#include "../hash/fnv1a.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static void walk(const char *dir, FileEntry **array, size_t *file_count, size_t *capacity) {
    DIR *d = opendir(dir); // potential fail here

    struct dirent *entry;
    while ((entry = readdir(d))) {
        // skip current and parent directory
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;

        // declare heap for path
        size_t needed_size = strlen(dir) + 1 + strlen(entry->d_name) + 1;
        char *path = malloc(needed_size); // potential fail here
        snprintf(path, needed_size, "%s/%s", dir, entry->d_name);

        struct stat file_stat;
        lstat(path, &file_stat); // potential fail here, **must check**, like permission denied

        if (S_ISDIR(file_stat.st_mode)) {
            // is directory
            walk(path, array, file_count, capacity);
        } else if (S_ISREG(file_stat.st_mode)) {
            // is file
            if (*file_count == *capacity) {
                size_t new_cap = *capacity ? *capacity * 2 : 64;
                *array = realloc(*array, new_cap * sizeof(FileEntry)); // potential fail here
                *capacity = new_cap;
            }
            (*array)[*file_count].path = strdup(path); // potential fail here
            (*array)[*file_count].hash = fnv1a_file(path);
            (*file_count)++;
        }

        free(path);
    }

    closedir(d);
}

const FileEntry *scan_dir(const char *root, size_t *file_count) {
    FileEntry *arr = NULL;
    *file_count = 0;
    size_t capacity = 0;
    walk(root, &arr, file_count, &capacity);
    return arr;
}

void free_entries(const FileEntry *array, size_t file_count) {
    for (size_t i = 0; i < file_count; i++) free((void *)array[i].path);
    free((void *)array);
}
