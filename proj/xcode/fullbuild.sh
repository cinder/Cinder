#!/bin/sh

CURRENT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
CINDER_XCODEPROJ="${CURRENT_DIR}/cinder.xcodeproj"

# OS X
xcrun xcodebuild -project ${CINDER_XCODEPROJ} -target cinder -configuration Release $@
xcrun xcodebuild -project ${CINDER_XCODEPROJ} -target cinder -configuration Debug $@

# iOS
xcrun xcodebuild -project ${CINDER_XCODEPROJ} -target cinder_iphone -configuration Release -sdk iphoneos $@
xcrun xcodebuild -project ${CINDER_XCODEPROJ} -target cinder_iphone -configuration Debug -sdk iphoneos $@
xcrun xcodebuild -project ${CINDER_XCODEPROJ} -target cinder_iphone_sim -configuration Release -sdk iphonesimulator $@
xcrun xcodebuild -project ${CINDER_XCODEPROJ} -target cinder_iphone_sim -configuration Debug -sdk iphonesimulator $@
