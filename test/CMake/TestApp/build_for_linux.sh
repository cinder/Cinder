#!/bin/bash

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCINDER_TARGET=linux -DCINDER_TARGET_GL=ogl
make 
