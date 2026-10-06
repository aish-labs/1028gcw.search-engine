#include <stdint.h>
#include <stdio.h>
#include "hash/fnv1a.h"

int main(void) {
    unsigned long long hash = fnv1a_file("./.gitignore");
    printf("%llu\n", hash);
}
