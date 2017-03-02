#!/bin/bash
set -e

# meant to be run from the root directory of Cinder
CMAKE_ARGS="-DCINDER_BUILD_TESTS=1 -DCINDER_BUILD_ALL_SAMPLES=0"

# build debug
mkdir build-debug
cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug ${CMAKE_ARGS}
make -j4
cd ..
# build release
mkdir build-release
cd build-release
cmake .. -DCMAKE_BUILD_TYPE=Release ${CMAKE_ARGS}
make -j4
# test release
make check
# return to root dir
cd ..