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

#include "cinder/Capture.h"
#include "cinder/android/hardware/Camera.h"
#include "cinder/gl/Texture.h"

namespace cinder {

/** \class CaptureImplJni
 *
 */
class CaptureImplJni {
public:
	class Device;

	CaptureImplJni( int width, int height, const Capture::DeviceRef device );
	virtual ~CaptureImplJni();

	void 				start();
	void 				stop();

	bool				isCapturing() const;
	bool				checkNewFrame() const;

	int32_t				getWidth() const { return mWidth; }
	int32_t				getHeight() const { return mHeight; }

	Surface8uRef		getSurface() const;

	gl::Texture2dRef	getTexture() const;

	const Capture::DeviceRef getDevice() const { return mDevice; }

	static const std::vector<Capture::DeviceRef>&	getDevices( bool forceRefresh = false );

	/** \class Device
	 *
	 */
	class Device : public Capture::Device {
	public:

		Device( const Capture::DeviceIdentifier& uniqueId, bool frontFacing, const std::vector<ivec2>& supportedResolutions, ci::android::hardware::Camera* nativeDevice );
		virtual ~Device();

		bool								checkAvailable() const;
		bool								isConnected() const;
		Capture::DeviceIdentifier			getUniqueId() const { return mUniqueId; }
		bool								isFrontFacing() const { return mFrontFacing; }
		ci::android::hardware::Camera*		getNative() const { return mNativeDevice; }		

		const std::vector<ivec2>&			getSupportedResolutions() const { return mSupportedResolutions; }

		void 								start(int width, int height);
		void 								stop();

	private:
		Capture::DeviceIdentifier 			mUniqueId;
		bool 								mFrontFacing = false;
		std::vector<ivec2>					mSupportedResolutions;

		ci::android::hardware::Camera*		mNativeDevice;
	};

private:
	Capture::DeviceRef	mDevice;
	int 				mWidth = 0;
	int 				mHeight = 0;
	bool 				mCapturing = false;

	std::shared_ptr<class SurfaceCache>	mSurfaceCache;
	mutable Surface8uRef				mCurrentFrame;

	mutable gl::Texture2dRef 			mCurrentTexture;

	static bool 							sDevicesEnumerated;
	static std::vector<Capture::DeviceRef>	sDevices;	
};

} // namespace cinder