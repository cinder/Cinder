#!/bin/sh

if [ ! -f "./CMakeLists.txt" ]; then
	echo "ERROR: Couldn't find CMakeLists.txt in current directory!"
	exit
fi

BUILD_TYPE=Debug
BUILD_DIR=Debug
if [ ! -d "./${BUILD_DIR}" ]; then
	mkdir ./${BUILD_DIR}
	echo "Created ${BUILD_DIR} directory"
fi

cd ${BUILD_DIR}
cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
make VERBOSE=1
