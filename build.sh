#!/usr/bin/env bash

if [[ $1 == "clean" ]]; then
    rm -r build/ bin/
    echo "Removed ./build/ and ./bin/ directories."
    exit 0
fi

if [[ $1 == "server" ]]; then
    cmake -S . -B build/ -DSERVER_ONLY=ON && make --no-print-directory -j4 -C build/
    exit 0
fi

cmake -S . -B build/ && make --no-print-directory -j4 -C build/
