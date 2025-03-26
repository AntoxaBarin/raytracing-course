#!/usr/bin/env bash

files=$(find ./source -maxdepth 1 -type f \( -name "*.cpp" -o -name "*.hpp" \))

for file in $files; do
    clang-format -i "$file"
done
