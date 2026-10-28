#!/bin/bash

set -e
gcc -O2 -Wall -Isrc $(find src -name '*.c') -o app

# debug parameters
# -g -fsanitize=address -fno-omit-frame-pointer
