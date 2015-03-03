#!/bin/sh
SAVED_DIR=`pwd`

rm -rf ./build/arm/android-21/Debug
mkdir -p ./build/arm/android-21/Debug
cd ./build/arm/android-21/Debug
cmake ../../../.. -DCMAKE_BUILD_TYPE=Debug -DNDK_ARCH=arm
make -j 4

cd $SAVED_DIR
rm -rf ./build/arm/android-21/Release
mkdir -p ./build/arm/android-21/Release
cd ./build/arm/android-21/Release
cmake ../../../.. -DCMAKE_BUILD_TYPE=Release -DNDK_ARCH=arm
make -j 4

