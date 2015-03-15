#!/bin/sh
ARCH=x86_64
sh cbuilder.sh "$@" -a ${ARCH} -es2
