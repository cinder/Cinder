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

#include "cinder/CaptureImplCocoaDummy.h"
#include "cinder/ip/Fill.h"

using namespace std;

@implementation CaptureImplCocoaDummy

+ (const std::vector<cinder::Capture::DeviceRef>&)getDevices:(BOOL)forceRefresh
{
	static bool sInitialized = false;
	static vector<cinder::Capture::DeviceRef> sDevices;
	if( ! sInitialized ) {
		sDevices.push_back( cinder::Capture::DeviceRef( new cinder::CaptureImplCocoaDummyDevice( "Front", "front", true ) ) );
		sDevices.push_back( cinder::Capture::DeviceRef( new cinder::CaptureImplCocoaDummyDevice( "Rear", "rear", false ) ) );
		
		sInitialized = true;
	}
	
	return sDevices;
}

- (id)initWithDevice:(const cinder::Capture::DeviceRef)device width:(int)width height:(int)height
{
	self = [super init];
	
	mDevice = device;
	mSurface = cinder::Surface8u::create( width, height, false );
	cinder::ip::fill( mSurface.get(), cinder::Color8u( 0, 0, 0 ) );
	
	return self;
}

- (void)prepareStartCapture
{}

- (void)startCapture
{
	mCapturing = true;
}

- (void)stopCapture
{
	mCapturing = false;
}

- (bool)isCapturing
{
	return mCapturing;
}

- (cinder::Surface8uRef)getCurrentFrame
{
	return mSurface;
}

- (bool)checkNewFrame
{
	return true;
}

- (const cinder::Capture::DeviceRef)getDevice
{
	return mDevice;
}

- (int32_t)getWidth
{
	return mSurface->getWidth();
}

- (int32_t)getHeight
{
	return mSurface->getHeight();
}

@end
