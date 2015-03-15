#!/bin/sh
ARCH=armeabi
sh cbuilder.sh "$@" -a ${ARCH} -es2
