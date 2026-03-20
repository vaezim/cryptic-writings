#!/usr/bin/env bash

if [[ $1 == "clean" ]]; then
    rm -r build/ bin/
    echo "Removed ./build/ and ./bin/ directories."
    exit 0
fi

cmake -S . -B build/ && make --no-print-directory -j4 -C build/
