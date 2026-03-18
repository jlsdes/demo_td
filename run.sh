#!/bin/bash

if [[ ! -e build || "$1" == "-build" ]]; then
    cmake -B build -DCMAKE_BUILD_TYPE=Debug
fi

make -C build -j$(nproc) > /dev/null && ./build/demo_td
