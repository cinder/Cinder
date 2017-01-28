#!/bin/bash
set -e

# meant to be run from the root directory of Cinder

# build cinder and ios using xcode
./proj/xcode/osx-build-xcpretty.sh

# build the unit tests 
xcrun xcodebuild -project ./test/unit/xcode/UnitTests.xcodeproj
# run tests
./test/unit/xcode/build/Release/CinderUnitTests
