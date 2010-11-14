/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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

#include "cinder/Cinder.h"
#include "cinder/Surface.h"
#include "cinder/Capture.h"
#import <QTKit/QTKit.h>
#include <vector>


namespace cinder {

class CaptureImplQtKitDevice : public Capture::Device {
 public:
	CaptureImplQtKitDevice( QTCaptureDevice* device );
	~CaptureImplQtKitDevice() {}
	
	bool						checkAvailable() const;
	bool						isConnected() const;
	Capture::DeviceIdentifier	getUniqueId() const { return mUniqueId; }
 private:
	Capture::DeviceIdentifier	mUniqueId;
};

} //namespace

@interface CaptureImplQtKit : NSObject {
	bool								mIsCapturing;
	QTCaptureSession					*mCaptureSession;
	QTCaptureDecompressedVideoOutput	*mCaptureDecompressedOutput;
	QTCaptureDeviceInput				*mCaptureDeviceInput;
	
	CVPixelBufferRef				mWorkingPixelBuffer;
	cinder::Surface8u				mCurrentFrame;
	int32_t							mWidth, mHeight;
	cinder::SurfaceChannelOrder		mSurfaceChannelOrderCode;
	NSString						* mDeviceUniqueId;
	int32_t							mExposedFrameBytesPerRow;
	int32_t							mExposedFrameHeight;
	int32_t							mExposedFrameWidth;
	bool							mHasNewFrame;
	cinder::Capture::DeviceRef		mDevice;
}

+ (const std::vector<cinder::Capture::DeviceRef>&)getDevices:(BOOL)forceRefresh;

- (id)initWithDevice:(const cinder::Capture::DeviceRef)device width:(int)width height:(int)height;
- (void)prepareStartCapture;
- (void)startCapture;
- (void)stopCapture;
- (bool)isCapturing;
- (cinder::Surface8u)getCurrentFrame;
- (bool)checkNewFrame;
- (const cinder::Capture::DeviceRef)getDevice;
- (int32_t)getWidth;
- (int32_t)getHeight;
- (int32_t)getCurrentFrameBytesPerRow;
- (int32_t)getCurrentFrameWidth;
- (int32_t)getCurrentFrameHeight;

@end