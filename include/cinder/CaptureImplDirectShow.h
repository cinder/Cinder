/*
 Copyright (c) 2025, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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
#include <memory>
#include <atomic>
#include <Windows.h>

namespace cinder {

class CaptureImplDirectShow {
  public:
	class Device;
	friend class SampleGrabberCallback;

	CaptureImplDirectShow( int32_t width, int32_t height, const Capture::DeviceRef device );
	CaptureImplDirectShow( const Capture::DeviceRef& device, const Capture::Mode& mode );
	~CaptureImplDirectShow();

	void start();
	void stop();

	bool isCapturing();
	bool checkNewFrame() const;

	int32_t getWidth() const { return mWidth; }
	int32_t getHeight() const { return mHeight; }

	Surface8uRef getSurface() const;

	const Capture::DeviceRef getDevice() const { return mDevice; }

	static const std::vector<Capture::DeviceRef>& getDevices( bool forceRefresh = false );

	// Public method to update dimensions with stride awareness (used by DirectShow setup)
	void updateDimensions( int width, int height, int rowStride, int imageSizeBytes );

	class Device : public Capture::Device {
	  public:
		bool					   checkAvailable() const;
		bool					   isConnected() const;
		Capture::DeviceIdentifier  getUniqueId() const { return mUniqueId; }
		std::vector<Capture::Mode> getModes() const override;

		Device( const std::string& name, int uniqueId )
			: Capture::Device()
			, mUniqueId( uniqueId )
		{
			mName = name;
		}

	  protected:
		int mUniqueId;
	};

  protected:
	int									mDeviceID;
	bool								mIsCapturing;
	std::unique_ptr<class SurfaceCache> mSurfaceCache;

	int32_t				 mWidth, mHeight;
	int32_t				 mRowStride;   // Actual bytes per row (may include padding)
	int32_t				 mImageSize;   // Total buffer size in bytes
	mutable Surface8uRef mCurrentFrame;
	Capture::DeviceRef	 mDevice;

	// New integrated DirectShow members (using void* to avoid forward declaration issues)
	void*							 mComInit;
	void*							 mDeviceContext;
	void*							 mCallback;
	std::unique_ptr<unsigned char[]> mPixelBuffer;

	mutable std::atomic<bool> mNewFrameAvailable;
	mutable CRITICAL_SECTION  mCriticalSection;

	// Device connection monitoring
	void checkDeviceEvents();

	// Direct DirectShow setup methods
	bool setupDeviceDirect( int deviceId, int width, int height );
};

} // namespace cinder
