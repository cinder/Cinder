#!/bin/sh
ARCH=armeabi-v7a
sh cbuilder.sh "$@" -a ${ARCH} -es2
