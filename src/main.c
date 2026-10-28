#include "./index/file.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    // check arguments
    if (argc == 2 && strcmp(argv[1], "--help") == 0) goto exit_print_usage;
    if (argc > 2) goto exit_error_arguments_invalid;

    // check if the given path is a directory
    char *path = (argc == 2) ? argv[1] : ".";
    struct stat st;
    if (stat(path, &st) == -1) goto exit_error_path_access_failed;
    if (!S_ISDIR(st.st_mode)) goto exit_error_path_not_directory;

    // clean the trailing "/" in the `path` if possible
    {
        size_t len = strlen(path);
        // `len > 1` protects "/"
        while (len > 1 && path[len - 1] == '/') {
            path[--len] = '\0';
        }
    }

    size_t file_count;
    FileEntry *array;
    int error = scan_dir(path, &array, &file_count);
    if (error) goto exit_error_scan_dir_failed; // no need to free here

    for (size_t i = 0; i < file_count; i++)
        printf("%016llx  %s\n", (unsigned long long) array[i].hash, array[i].path);

    free_entries(array, file_count);
    printf("total: %zu\n", file_count);

    return 0;

    // === abnormal exits ===
    // Don't forget to free if malloc-ed.
    exit_print_usage:
        printf("Usage: %s [path]\nIf no path is given, the current working directory will be used.\n", argv[0]);
        return 0;

    exit_error_arguments_invalid:
        fprintf(stderr, "Usage: %s [path]\nPlease check your input.\n", argv[0]);
        return 1;

    exit_error_path_access_failed:
        fprintf(stderr, "Failed to access \"%s\". The path may be invalid, or the program may not have permission to access it.\n", path);
        return 1;

    exit_error_path_not_directory:
        fprintf(stderr, "\"%s\" is not a directory.\n", path);
        return 1;

    exit_error_scan_dir_failed:
        fprintf(stderr, "Scan directory failed, error code %d.\n", error);
        return 1;
}
