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

#include "cinder/CaptureImplDirectShow.h"

using namespace std;

namespace cinder {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceCache

class SurfaceCache {
  public:
	SurfaceCache( int32_t width, int32_t height, SurfaceChannelOrder sco, int numSurfaces )
		: mSurfaces( numSurfaces, nullptr ), mWidth( width ), mHeight( height ), mSCO( sco )
	{
		for( auto& surf : mSurfaces ) {
			surf = Surface8u::create( mWidth, mHeight, mSCO.hasAlpha(), mSCO );
		}
	}

	Surface8uRef getNewSurface()
	{
		// try to find a surface that isn't used by anyone else
		auto it = std::find_if( mSurfaces.begin(), mSurfaces.end(), [](const Surface8uRef& s) { return s.unique(); } );

		// if no free surface is found, create a new one and add it to the cache set
		if( it == mSurfaces.end() ) {
			mSurfaces.push_back( Surface8u::create( mWidth, mHeight, mSCO.hasAlpha(), mSCO ) );
			it = mSurfaces.end() - 1;
		}

		return *it;
	}

  private:
	std::vector<Surface8uRef>			mSurfaces;
	int32_t								mWidth, mHeight;
	SurfaceChannelOrder					mSCO;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// videoInput

namespace impl {
	static videoInput& setupVideoInput()
	{
		static videoInput inst;
		inst.setUseCallback( true );
		return inst;
	}
}

static videoInput& getVideoInput()
{
	static videoInput& instance = impl::setupVideoInput();
	return instance;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CaptureImplDirectShow

bool CaptureImplDirectShow::Device::checkAvailable() const
{
	return ( mUniqueId >=0 ) && ( mUniqueId < (int)CaptureImplDirectShow::getDevices().size() ) && ( ! getVideoInput().isDeviceSetup( mUniqueId ) );
}

bool CaptureImplDirectShow::Device::isConnected() const
{
	return getVideoInput().isDeviceConnected( mUniqueId );
}

const vector<Capture::DeviceRef>& CaptureImplDirectShow::getDevices( bool forceRefresh )
{
	static bool firstCall = true;
	static std::vector<Capture::DeviceRef>	devices;

	if( firstCall || forceRefresh ) {
		const int devCnt = getVideoInput().listDevices( true );
		devices.resize( devCnt );
		for ( int i = 0; i < devCnt; ++i ) {
			devices[i] = std::make_shared<CaptureImplDirectShow::Device>( videoInput::getDeviceName( i ), i );
		}

		firstCall = false;
	}
	return devices;
}

CaptureImplDirectShow::CaptureImplDirectShow( int32_t width, int32_t height, const Capture::DeviceRef device )
	: mWidth( width ), mHeight( height ), mCurrentFrame( Surface8u::create( width, height, false, SurfaceChannelOrder::BGR ) ), mDeviceID( 0 )
{
	mDevice = device;
	if( mDevice ) {
		mDeviceID = device->getUniqueId();
	}
	if( ! getVideoInput().setupDevice( mDeviceID, mWidth, mHeight ) )
		throw CaptureExcInitFail();
	mWidth = getVideoInput().getWidth( mDeviceID );
	mHeight = getVideoInput().getHeight( mDeviceID );
	mIsCapturing = true;
	mSurfaceCache.reset( new SurfaceCache( mWidth, mHeight, SurfaceChannelOrder::BGR, 4 ) );
}

CaptureImplDirectShow::~CaptureImplDirectShow()
{
	getVideoInput().stopDevice( mDeviceID );
}

void CaptureImplDirectShow::start()
{
	if( mIsCapturing ) return;

	if( ! getVideoInput().setupDevice( mDeviceID, mWidth, mHeight ) )
		throw CaptureExcInitFail();
	if( ! getVideoInput().isDeviceSetup( mDeviceID ) )
		throw CaptureExcInitFail();
	mWidth = getVideoInput().getWidth( mDeviceID );
	mHeight = getVideoInput().getHeight( mDeviceID );
	mIsCapturing = true;
}

void CaptureImplDirectShow::stop()
{
	if( ! mIsCapturing ) return;

	getVideoInput().stopDevice( mDeviceID );
	mIsCapturing = false;
}

bool CaptureImplDirectShow::isCapturing()
{
	return mIsCapturing;
}

bool CaptureImplDirectShow::checkNewFrame() const
{
	return getVideoInput().isFrameNew( mDeviceID );
}

Surface8uRef CaptureImplDirectShow::getSurface() const
{
	if( getVideoInput().isFrameNew( mDeviceID ) ) {
		mCurrentFrame = mSurfaceCache->getNewSurface();
		getVideoInput().getPixels( mDeviceID, mCurrentFrame->getData(), false, true );
	}

	return mCurrentFrame;
}

} //namespace