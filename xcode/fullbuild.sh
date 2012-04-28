#!/bin/sh

xcodebuild -project cinder.xcodeproj -target cinder -configuration Release
xcodebuild -project cinder.xcodeproj -target cinder_iphone -configuration Release
xcodebuild -project cinder.xcodeproj -target cinder_iphone_sim -configuration Release -sdk iphonesimulator
xcodebuild -project cinder.xcodeproj -target cinder -configuration Debug
xcodebuild -project cinder.xcodeproj -target cinder_iphone -configuration Debug
xcodebuild -project cinder.xcodeproj -target cinder_iphone_sim -configuration Debug -sdk iphonesimulator
