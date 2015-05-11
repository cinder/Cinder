/*
 Copyright (c) 2012, The Cinder Project: http://libcinder.org All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Surface.h"
#include "cinder/Capture.h"
#include <vector>

#import <Foundation/Foundation.h>

namespace cinder {

//! This class implements a stubbed-out Capture implementation in order to allow an iOS app which requires capture support to run in the simulator
class CaptureImplCocoaDummyDevice : public Capture::Device {
  public:
	CaptureImplCocoaDummyDevice( const std::string &name, const Capture::DeviceIdentifier &uniqueId, bool frontFacing )
		: Capture::Device(), mUniqueId( uniqueId ), mFrontFacing( frontFacing )
	{
		mName = name;
	}
	
	bool						checkAvailable() const { return true; }
	bool						isConnected() const { return true; }
	Capture::DeviceIdentifier	getUniqueId() const { return mUniqueId; }
	bool						isFrontFacing() const { return mFrontFacing; }
	void*						getNative() const { return NULL; }
  private:
	Capture::DeviceIdentifier	mUniqueId;
	bool						mFrontFacing;
};

} //namespace

@interface CaptureImplCocoaDummy : NSObject {
	bool							mCapturing;
	cinder::Surface8uRef			mSurface;
	cinder::Capture::DeviceRef		mDevice;
}

+ (const std::vector<cinder::Capture::DeviceRef>&)getDevices:(BOOL)forceRefresh;

- (id)initWithDevice:(const cinder::Capture::DeviceRef)device width:(int)width height:(int)height;
- (void)prepareStartCapture;
- (void)startCapture;
- (void)stopCapture;
- (bool)isCapturing;
- (cinder::Surface8uRef)getCurrentFrame;
- (bool)checkNewFrame;
- (const cinder::Capture::DeviceRef)getDevice;
- (int32_t)getWidth;
- (int32_t)getHeight;

@end