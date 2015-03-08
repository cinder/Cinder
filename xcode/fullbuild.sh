#!/bin/sh

CURRENT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
CINDER_XCODEPROJ="${CURRENT_DIR}/cinder.xcodeproj"

xcrun xcodebuild -project ${CINDER_XCODEPROJ} -alltargets -configuration Release $@
xcrun xcodebuild -project ${CINDER_XCODEPROJ} -alltargets -configuration Debug $@
