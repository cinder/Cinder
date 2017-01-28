#!/bin/sh
set -e

CURRENT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
CINDER_XCODEPROJ="${CURRENT_DIR}/cinder.xcodeproj"

# iOS
set -o pipefail && xcrun xcodebuild -project ${CINDER_XCODEPROJ} -target cinder_iphone -configuration Release -sdk iphoneos $@ | xcpretty
set -o pipefail && xcrun xcodebuild -project ${CINDER_XCODEPROJ} -target cinder_iphone -configuration Debug -sdk iphoneos $@ | xcpretty