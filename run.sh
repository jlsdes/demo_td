#!/bin/bash

if [[ ! -e build ]]; then
    cmake -B build -DCMAKE_BUILD_TYPE=Debug
fi

make -C build > /dev/null && ./build/demo_td
