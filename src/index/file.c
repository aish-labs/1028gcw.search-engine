#include "file.h"
#include "../hash/fnv1a.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
  #define lstat stat

  static char *fullpath(const char *path) {
      return _fullpath(NULL, path, 0);
  }
#else
  static char *fullpath(const char *path) {
      return realpath(path, NULL);
  }
#endif

static int walk(const char *dir, FileEntry **array, size_t *file_count, size_t *capacity) {
    DIR *directory = opendir(dir);
    if (!directory) return -1;

    struct dirent *entry;
    int rc = 0;
    while ((entry = readdir(directory))) {
        // skip current and parent directory
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;

        // declare heap for path
        size_t needed_size = strlen(dir) + 1 + strlen(entry->d_name) + 1;
        char *path = malloc(needed_size);
        if (!path) { rc = -2; break; }
        snprintf(path, needed_size, "%s/%s", dir, entry->d_name);

        struct stat st;
        // ignore accessible files
        if (lstat(path, &st)) { free(path); continue; }

        if (S_ISDIR(st.st_mode)) {
            // is directory
            rc = walk(path, array, file_count, capacity);
            if (rc == -1) rc = 0; // skip if **child** directory walk fail
            if (rc) break;
        } else if (S_ISREG(st.st_mode)) {
            // is file
            uint64_t hash;
            if (fnv1a_file(path, &hash)) { free(path); continue; } // skip if hash calculation failed

            if (*file_count == *capacity) {
                size_t new_cap = *capacity ? *capacity * 2 : 64;
                FileEntry *tmp = realloc(*array, new_cap * sizeof(FileEntry));
                if (!tmp) { free(path); rc = -3; break; }
                *array = tmp;
                *capacity = new_cap;
            }

            char *full_path = fullpath(path);
            if (full_path) {
                (*array)[*file_count].path = full_path;
                (*array)[*file_count].hash = hash;
                (*file_count)++;
            }
        }

        free(path);
    }

    closedir(directory);
    return rc;
}

int scan_dir(const char *root, FileEntry **array, size_t *file_count) {
    *array = NULL;
    *file_count = 0;
    size_t capacity = 0;

    int error = walk(root, array, file_count, &capacity);
    if (error) {
        free_entries(*array, *file_count);
        *array = NULL;
        *file_count = 0;
        return error;
    }

    return 0;
}

void free_entries(FileEntry *array, size_t file_count) {
    for (size_t i = 0; i < file_count; i++) free(array[i].path);
    free(array);
}
