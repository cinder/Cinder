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

#import <QTKit/QTKit.h>
#include "cinder/Surface.h"

/*class CaptureImplQtKitDevice {
	CaptureImplQtKitDevice() {}
	CaptureImplQtKitDevice( QTCaptureDevice* device );
	~CaptureImplQtKitDevice();
	
	const std::string&		getName() const { return mName; }
	bool					checkAvailable() const;
	bool					isConnected() const;
	const std::string&		getUniqueId() const { return mUniqueId; }
};*/

@interface CaptureImplQtKit : NSObject {
	BOOL								isCapturing;
	QTCaptureSession					*mCaptureSession;
	QTCaptureDecompressedVideoOutput	*mCaptureDecompressedOutput;
	QTCaptureDeviceInput				*mCaptureDeviceInput;
	
	CVPixelBufferRef				mWorkingPixelBuffer;
	cinder::Surface8u				mCurrentFrame;
	int								mWidth, mHeight;
	int								mSurfaceChannelOrderCode;
	NSString						*mDeviceUniqueId;
	int								mExposedFrameBytesPerRow;
	int								mExposedFrameHeight;
	int								mExposedFrameWidth;
	BOOL							mHasNewFrame;
}

- (id)initWithDevice:(const cinder::Capture::Device&)device width:(int)width height:(int)height;
- (void)prepareStartCapture;
- (void)startCapture;
- (void)stopCapture;
- (BOOL)isCapturing;
- (cinder::Surface8u)getCurrentFrame;
- (BOOL)checkNewFrame;
- (size_t)getCurrentFrameBytesPerRow;
- (size_t)getCurrentFrameWidth;
- (size_t)getCurrentFrameHeight;

@end