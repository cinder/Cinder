#!/bin/sh
sh fullbuild-armeabi.sh "$@"
sh fullbuild-armeabi-v7a.sh "$@"
sh fullbuild-x86.sh "$@"
sh fullbuild-mips.sh "$@"
sh fullbuild-arm64-v8a.sh "$@"
sh fullbuild-x86_64.sh "$@"
sh fullbuild-mips64.sh "$@"
