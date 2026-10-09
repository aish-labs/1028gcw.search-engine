#include "file.h"

static void walk(const char *dir, FileEntry **arr, size_t *n) {
    DIR *d = opendir(dir);
    if (!d) return;

    struct dirent *e;
    while ((e = readdir(d))) {
        if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..")) continue;

        char path[4096];
        snprintf(path, sizeof path, "%s/%s", dir, e->d_name);

        if (e->d_type == DT_DIR) {
            walk(path, arr, n);
        } else if (e->d_type == DT_REG) {
            *arr = realloc(*arr, (*n + 1) * sizeof **arr);
            (*arr)[*n].path = strdup(path);
            (*arr)[*n].hash = fnv1a_file(path);
            (*n)++;
        }
    }

    closedir(d);
}

const FileEntry *scan_dir(const char *root, size_t *count) {
    FileEntry *arr = NULL;
    *count = 0;
    walk(root, &arr, count);
    return arr;
}

void free_entries(const FileEntry *list, size_t count) {
    for (size_t i = 0; i < count; i++) free((void *)list[i].path);
    free((void *)list);
}
