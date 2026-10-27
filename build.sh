#!/bin/bash

set -e
gcc -O2 -Wall -Isrc $(find src -name '*.c') -o app
