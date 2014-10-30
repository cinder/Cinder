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
#include <set>

namespace cinder {

	class CaptureMgr : private boost::noncopyable
	{
	public:
		CaptureMgr();
		~CaptureMgr();

		static std::shared_ptr<CaptureMgr>	instance();
		static videoInput*	instanceVI() { return instance()->mVideoInput; }

		static std::shared_ptr<CaptureMgr>	sInstance;
		static int						sTotalDevices;

	private:
		videoInput			*mVideoInput;
	};

	class SurfaceCache {
	public:
		SurfaceCache(int32_t width, int32_t height, SurfaceChannelOrder sco, int numSurfaces)
			: mWidth(width), mHeight(height), mSCO(sco)
		{
			for (int i = 0; i < numSurfaces; ++i) {
				mSurfaceData.push_back(std::shared_ptr<uint8_t>(new uint8_t[width*height*sco.getPixelInc()], checked_array_deleter<uint8_t>()));
				mDeallocatorRefcon.push_back(std::make_pair(this, i));
				mSurfaceUsed.push_back(false);
			}
		}

		Surface8u getNewSurface()
		{
			// try to find an available block of pixel data to wrap a surface around	
			for (size_t i = 0; i < mSurfaceData.size(); ++i) {
				if (!mSurfaceUsed[i]) {
					mSurfaceUsed[i] = true;
					Surface8u result(mSurfaceData[i].get(), mWidth, mHeight, mWidth * mSCO.getPixelInc(), mSCO);
					result.setDeallocator(surfaceDeallocator, &mDeallocatorRefcon[i]);
					return result;
				}
			}

			// we couldn't find an available surface, so we'll need to allocate one
			return Surface8u(mWidth, mHeight, mSCO.hasAlpha(), mSCO);
		}

		static void surfaceDeallocator(void *refcon)
		{
			std::pair<SurfaceCache*, int> *info = reinterpret_cast<std::pair<SurfaceCache*, int>*>(refcon);
			info->first->mSurfaceUsed[info->second] = false;
		}

	private:
		std::vector<std::shared_ptr<uint8_t> >	mSurfaceData;
		std::vector<bool>					mSurfaceUsed;
		std::vector<std::pair<SurfaceCache*, int> >	mDeallocatorRefcon;
		int32_t				mWidth, mHeight;
		SurfaceChannelOrder	mSCO;
	};

class CaptureImplDirectShow {
 public:
	class Device;
	CaptureImplDirectShow() { };
	CaptureImplDirectShow( int32_t width, int32_t height, const Capture::DeviceRef device );
	CaptureImplDirectShow( int32_t width, int32_t height );
	~CaptureImplDirectShow();
	
	void start();
	void stop();
	
	bool		isCapturing();
	bool		checkNewFrame() const;

	int32_t		getWidth() const { return mWidth; }
	int32_t		getHeight() const { return mHeight; }
	
	Surface8u	getSurface() const;
	
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
	void	init( int32_t width, int32_t height, const Capture::Device &device );

	int								mDeviceID;
	// this maintains a reference to the mgr so that we don't destroy it before
	// the last Capture is destroyed
	std::shared_ptr<class CaptureMgr>	mMgrPtr;
	bool								mIsCapturing;
	std::shared_ptr<class SurfaceCache>	mSurfaceCache;

	int32_t				mWidth, mHeight;
	mutable Surface8u	mCurrentFrame;
	Capture::DeviceRef	mDevice;

	static bool							sDevicesEnumerated;
	static std::vector<Capture::DeviceRef>	sDevices;
};

} //namespace

