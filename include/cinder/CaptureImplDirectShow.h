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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Capture.h"
#include "cinder/Surface.h"
#include "msw/videoInput/videoInput.h"

namespace cinder {

class CaptureImplDirectShow {
 public:
	class Device;

	CaptureImplDirectShow( int32_t width, int32_t height, const Capture::DeviceRef device );
	~CaptureImplDirectShow();

	void start();
	void stop();

	bool		isCapturing();
	bool		checkNewFrame() const;

	int32_t		getWidth() const { return mWidth; }
	int32_t		getHeight() const { return mHeight; }

	Surface8uRef	getSurface() const;

	const Capture::DeviceRef getDevice() const { return mDevice; }

	static const std::vector<Capture::DeviceRef>&	getDevices( bool forceRefresh = false );

	class Device : public Capture::Device {
 	  public:
		bool						checkAvailable() const;
		bool						isConnected() const;
		Capture::DeviceIdentifier	getUniqueId() const { return mUniqueId; }

		Device( const std::string &name, int uniqueId ) : Capture::Device(), mUniqueId( uniqueId ) { mName = name; }
	 protected:
		int				mUniqueId;
	};

 protected:
	int									mDeviceID;
	bool								mIsCapturing;
	std::unique_ptr<class SurfaceCache>	mSurfaceCache;

	int32_t					mWidth, mHeight;
	mutable Surface8uRef	mCurrentFrame;
	Capture::DeviceRef		mDevice;
};

} //namespace

