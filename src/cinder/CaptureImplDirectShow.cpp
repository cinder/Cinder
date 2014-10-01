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
#include <boost/noncopyable.hpp>

#include <set>
using namespace std;


namespace cinder {

bool CaptureImplDirectShow::sDevicesEnumerated = false;
vector<Capture::DeviceRef> CaptureImplDirectShow::sDevices;


std::shared_ptr<CaptureMgr>	CaptureMgr::sInstance;
int							CaptureMgr::sTotalDevices = 0;

CaptureMgr::CaptureMgr()
{
	mVideoInput = new videoInput;
	mVideoInput->setUseCallback( true );
}

CaptureMgr::~CaptureMgr()
{
	delete mVideoInput;
}

std::shared_ptr<CaptureMgr> CaptureMgr::instance()
{
	if( ! sInstance ) {
		sInstance = std::shared_ptr<CaptureMgr>( new CaptureMgr );
	}
	return sInstance;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CaptureImplDirectShow

bool CaptureImplDirectShow::Device::checkAvailable() const
{
	return ( mUniqueId < CaptureMgr::sTotalDevices ) && ( ! CaptureMgr::instanceVI()->isDeviceSetup( mUniqueId ) );
}

bool CaptureImplDirectShow::Device::isConnected() const
{
	return CaptureMgr::instanceVI()->isDeviceConnected( mUniqueId );
}

const vector<Capture::DeviceRef>& CaptureImplDirectShow::getDevices( bool forceRefresh )
{
	if( sDevicesEnumerated && ( ! forceRefresh ) )
		return sDevices;

	sDevices.clear();

	CaptureMgr::instance()->sTotalDevices = CaptureMgr::instanceVI()->listDevices( true );
	for( int i = 0; i < CaptureMgr::instance()->sTotalDevices; ++i ) {
		sDevices.push_back( Capture::DeviceRef( new CaptureImplDirectShow::Device( videoInput::getDeviceName( i ), i ) ) );
	}

	sDevicesEnumerated = true;
	return sDevices;
}

CaptureImplDirectShow::CaptureImplDirectShow( int32_t width, int32_t height, const Capture::DeviceRef device )
	: mWidth( width ), mHeight( height ), mCurrentFrame( width, height, false, SurfaceChannelOrder::BGR ), mDeviceID( 0 )
{
	mDevice = device;
	if( mDevice ) {
		mDeviceID = device->getUniqueId();
	}
	if( ! CaptureMgr::instanceVI()->setupDevice( mDeviceID, mWidth, mHeight ) )
		throw CaptureExcInitFail();
	mWidth = CaptureMgr::instanceVI()->getWidth( mDeviceID );
	mHeight = CaptureMgr::instanceVI()->getHeight( mDeviceID );
	mIsCapturing = true;
	mSurfaceCache = std::shared_ptr<SurfaceCache>( new SurfaceCache( mWidth, mHeight, SurfaceChannelOrder::BGR, 4 ) );

	mMgrPtr = CaptureMgr::instance();
}

CaptureImplDirectShow::~CaptureImplDirectShow()
{
	CaptureMgr::instanceVI()->stopDevice( mDeviceID );
}

void CaptureImplDirectShow::start()
{
	if( mIsCapturing ) return;
	
	if( ! CaptureMgr::instanceVI()->setupDevice( mDeviceID, mWidth, mHeight ) )
		throw CaptureExcInitFail();
	if( ! CaptureMgr::instanceVI()->isDeviceSetup( mDeviceID ) )
		throw CaptureExcInitFail();
	mWidth = CaptureMgr::instanceVI()->getWidth( mDeviceID );
	mHeight = CaptureMgr::instanceVI()->getHeight( mDeviceID );
	mIsCapturing = true;
}

void CaptureImplDirectShow::stop()
{
	if( ! mIsCapturing ) return;

	CaptureMgr::instanceVI()->stopDevice( mDeviceID );
	mIsCapturing = false;
}

bool CaptureImplDirectShow::isCapturing()
{
	return mIsCapturing;
}

bool CaptureImplDirectShow::checkNewFrame() const
{
	return CaptureMgr::instanceVI()->isFrameNew( mDeviceID );
}

Surface8u CaptureImplDirectShow::getSurface() const
{
	if( CaptureMgr::instanceVI()->isFrameNew( mDeviceID ) ) {
		mCurrentFrame = mSurfaceCache->getNewSurface();
		CaptureMgr::instanceVI()->getPixels( mDeviceID, mCurrentFrame.getData(), false, true );
	}
	
	return mCurrentFrame;
}

} //namespace