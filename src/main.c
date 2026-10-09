#include "./index/file.h"

int main(void) {
    size_t n;
    const FileEntry *list = scan_dir(".", &n);
    for (size_t i = 0; i < n; i++)
        printf("%016llx  %s\n", (unsigned long long) list[i].hash, list[i].path);
    free_entries(list, n);
}
