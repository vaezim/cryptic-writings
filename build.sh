#!/usr/bin/env bash

if [[ $1 == "clean" ]]; then
    rm -r build/ bin/
    echo "Removed ./build/ and ./bin/ directories."
    exit 0
fi

# Build server only
SERVER_ONLY="OFF"
if [[ $1 == "server" ]]; then
    SERVER_ONLY="ON"
fi

# If a local installation of Qt exists in /home, link against that.
QT_HOME=~/Qt/6.10.2/gcc_64
if [ -d $QT_HOME ]; then
    echo "[+] Found local Qt installation at $QT_HOME"
    cmake -S . -B build/ -DCMAKE_PREFIX_PATH=$QT_HOME -DSERVER_ONLY=$SERVER_ONLY && make --no-print-directory -j4 -C build/
    exit 0
fi

echo "[-] Failed to find local Qt installation at $QT_HOME"
echo "    System's Qt libraries will be used to link executables."
echo "    If your installation is located elsewhere or has a different"
echo "    version, edit \`QT_HOME\` variable in ./build.sh script."
cmake -S . -B build/ -DSERVER_ONLY=$SERVER_ONLY && make --no-print-directory -j4 -C build/
