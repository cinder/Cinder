#!/bin/sh
ARCH=arm64-v8a
sh cbuilder.sh "$@" -a ${ARCH} -es2
