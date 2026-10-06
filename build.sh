#!/bin/bash

set -e
gcc -Wall -Isrc $(find src -name '*.c') -o app
