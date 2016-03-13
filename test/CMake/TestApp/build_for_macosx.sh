#!/bin/bash

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCINDER_TARGET=macosx
make VERBOSE=1
