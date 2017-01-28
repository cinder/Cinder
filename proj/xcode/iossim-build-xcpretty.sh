#!/bin/sh
set -e

CURRENT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
CINDER_XCODEPROJ="${CURRENT_DIR}/cinder.xcodeproj"

# iOS Sim
set -o pipefail && xcrun xcodebuild -project ${CINDER_XCODEPROJ} -target cinder_iphone_sim -configuration Release -sdk iphonesimulator $@ | xcpretty
set -o pipefail && xcrun xcodebuild -project ${CINDER_XCODEPROJ} -target cinder_iphone_sim -configuration Debug -sdk iphonesimulator $@ | xcpretty