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
#include "cinder/Capture.h"
#if defined( CINDER_MAC )
	#import <QTKit/QTKit.h>
	#import "cinder/CaptureImplQtKit.h"
	#include "cinder/cocoa/CinderCocoa.h"
#else
	#include <boost/noncopyable.hpp>
#endif

#include <set>
using namespace std;

namespace cinder {

#if defined( CINDER_MSW )
	#define PLATFORM_DEFAULT_CHANNEL_ORDER SurfaceChannelOrder::BGR
#else
	#define PLATFORM_DEFAULT_CHANNEL_ORDER SurfaceChannelOrder::RGB
#endif

bool Capture::sDevicesEnumerated = false;
vector<Capture::Device> Capture::sDevices;

#if defined( CINDER_MSW )
class CaptureMgr : private boost::noncopyable
{
 public:
	CaptureMgr();
	~CaptureMgr();

	static shared_ptr<CaptureMgr>	instance();
	static videoInput*	instanceVI() { return instance()->mVideoInput; }

	static shared_ptr<CaptureMgr>	sInstance;
	static int						sTotalDevices;
	
 private:	
	videoInput			*mVideoInput;
};
shared_ptr<CaptureMgr>	CaptureMgr::sInstance;
int						CaptureMgr::sTotalDevices = 0;

CaptureMgr::CaptureMgr()
{
	mVideoInput = new videoInput;
	mVideoInput->setUseCallback( true );
}

CaptureMgr::~CaptureMgr()
{
	delete mVideoInput;
}

shared_ptr<CaptureMgr> CaptureMgr::instance()
{
	if( ! sInstance ) {
		sInstance = shared_ptr<CaptureMgr>( new CaptureMgr );
	}
	return sInstance;
}

class SurfaceCache {
 public:
	SurfaceCache( int32_t width, int32_t height, SurfaceChannelOrder sco, int numSurfaces )
		: mWidth( width ), mHeight( height ), mSCO( sco )
	{
		for( int i = 0; i < numSurfaces; ++i ) {
			mSurfaceData.push_back( shared_ptr<uint8_t>( new uint8_t[width*height*sco.getPixelInc()], checked_array_deleter<uint8_t>() ) );
			mDeallocatorRefcon.push_back( make_pair( this, i ) );
			mSurfaceUsed.push_back( false );
		}
	}
	
	Surface8u getNewSurface()
	{
		// try to find an available block of pixel data to wrap a surface around	
		for( size_t i = 0; i < mSurfaceData.size(); ++i ) {
			if( ! mSurfaceUsed[i] ) {
				mSurfaceUsed[i] = true;
				Surface8u result( mSurfaceData[i].get(), mWidth, mHeight, mWidth * mSCO.getPixelInc(), mSCO );
				result.setDeallocator( surfaceDeallocator, &mDeallocatorRefcon[i] );
				return result;
			}
		}

		// we couldn't find an available surface, so we'll need to allocate one
		return Surface8u( mWidth, mHeight, mSCO.hasAlpha(), mSCO );
	}
	
	static void surfaceDeallocator( void *refcon )
	{
		pair<SurfaceCache*,int> *info = reinterpret_cast<pair<SurfaceCache*,int>*>( refcon );
		info->first->mSurfaceUsed[info->second] = false;
	}

 private:
	vector<shared_ptr<uint8_t> >	mSurfaceData;
	vector<bool>					mSurfaceUsed;
	vector<pair<SurfaceCache*,int> >	mDeallocatorRefcon;
	int32_t				mWidth, mHeight;
	SurfaceChannelOrder	mSCO;
};

#endif


#if defined( CINDER_MAC )
Capture::Device::Device( QTCaptureDevice *device ) 
{
	mUniqueId = cocoa::convertNsString( [device uniqueID] );
	mName = cocoa::convertNsString( [device localizedDisplayName] );

	// Apparently this stuff is basically useless
/*	NSArray *formats = [device formatDescriptions];
	for( int f = 0; f < [formats count]; ++f ) {
		QTFormatDescription *format = [formats objectAtIndex:f];
		if( [[format mediaType] isEqualToString:QTMediaTypeVideo] ) {
			NSLog( @"%d %@", [formats count], [format formatDescriptionAttributes] );
		}
	}*/
//	NSLog( @"%@", [device deviceAttributes] );
}
#endif

bool Capture::Device::checkAvailable() const
{
#if defined( CINDER_MAC )
	QTCaptureDevice *device = [QTCaptureDevice deviceWithUniqueID:[NSString stringWithUTF8String:mUniqueId.c_str()]];
	return [device isConnected] && (! [device isInUseByAnotherApplication]);
#elif defined( CINDER_MSW )
	return ( mUniqueId < CaptureMgr::sTotalDevices ) && ( ! CaptureMgr::instanceVI()->isDeviceSetup( mUniqueId ) );
#endif
}

bool Capture::Device::isConnected() const
{
#if defined( CINDER_MAC )
	QTCaptureDevice *device = [QTCaptureDevice deviceWithUniqueID:[NSString stringWithUTF8String:mUniqueId.c_str()]];
	return [device isConnected];
#elif defined( CINDER_MSW )
	return CaptureMgr::instanceVI()->isDeviceConnected( mUniqueId );
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Capture
const vector<Capture::Device>& Capture::getDevices( bool forceRefresh )
{
	if( sDevicesEnumerated && ( ! forceRefresh ) )
		return sDevices;

	sDevices.clear();
	
#if defined( CINDER_MAC )
	NSArray *devices = [QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo];
	for( int i = 0; i < [devices count]; i++ ) {
		QTCaptureDevice *device = [devices objectAtIndex:i];
		sDevices.push_back( Capture::Device( device ) );
	}
	
	devices = [QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeMuxed];
	for( int i = 0; i < [devices count]; i++) {
		QTCaptureDevice *device = [devices objectAtIndex:i];
		sDevices.push_back( Capture::Device( device ) );
	}
#else
	CaptureMgr::instance()->sTotalDevices = CaptureMgr::instanceVI()->listDevices( true );
	for( int i = 0; i < CaptureMgr::instance()->sTotalDevices; ++i ) {
		sDevices.push_back( Device( videoInput::getDeviceName( i ), i ) );
	}
#endif
	
	sDevicesEnumerated = true;
	return sDevices;
}

Capture::Device Capture::findDeviceByName( const string &name )
{
	const vector<Device> &devices = getDevices();
	for( vector<Device>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt ) {
		if( deviceIt->getName() == name )
			return *deviceIt;
	}
	
	return Device(); // failed - return "null" device
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Capture::Obj
Capture::Obj::Obj( int32_t width, int32_t height, const Device &device )
	: mWidth( width ), mHeight( height ), mCurrentFrame( width, height, false, PLATFORM_DEFAULT_CHANNEL_ORDER ), mDevice( device )
{
#if defined( CINDER_MAC )
	mImpl = [[::CaptureImplQtKit alloc] initWithDevice:mDevice width:width height:height];
#else
	mDeviceID = device.getUniqueId();
	if( ! CaptureMgr::instanceVI()->setupDevice( mDeviceID, mWidth, mHeight ) )
		throw CaptureExcInitFail();
	mWidth = CaptureMgr::instanceVI()->getWidth( mDeviceID );
	mHeight = CaptureMgr::instanceVI()->getHeight( mDeviceID );
	mIsCapturing = true;
	mSurfaceCache = shared_ptr<SurfaceCache>( new SurfaceCache( mWidth, mHeight, PLATFORM_DEFAULT_CHANNEL_ORDER, 4 ) );
#endif	
}

Capture::Obj::~Obj()
{
#if defined( CINDER_MAC )
	[((::CaptureImplQtKit*)mImpl) stopCapture];
	[((::CaptureImplQtKit*)mImpl) release];
#else
	CaptureMgr::instanceVI()->stopDevice( mDeviceID );
#endif
}

Capture::Capture( int32_t width, int32_t height, const Device &device ) 
{
	mObj = shared_ptr<Obj>( new Obj( width, height, device ) );
#if defined( CINDER_MSW )
	mObj->mMgrPtr = CaptureMgr::instance();
#endif
}

void Capture::start()
{
#if defined( CINDER_MAC )
	[((::CaptureImplQtKit*)mObj->mImpl) startCapture];
#else
	if( ! mObj->mIsCapturing ) {
		if( ! CaptureMgr::instanceVI()->setupDevice( mObj->mDeviceID, mObj->mWidth, mObj->mHeight ) )
			throw CaptureExcInitFail();
		if( ! CaptureMgr::instanceVI()->isDeviceSetup( mObj->mDeviceID ) )
			throw CaptureExcInitFail();
		mObj->mWidth = CaptureMgr::instanceVI()->getWidth( mObj->mDeviceID );
		mObj->mHeight = CaptureMgr::instanceVI()->getHeight( mObj->mDeviceID );
		mObj->mIsCapturing = true;
	}
#endif
}

void Capture::stop()
{
#if defined( CINDER_MAC )
	[((::CaptureImplQtKit*)mObj->mImpl) stopCapture];
#else
	if( mObj->mIsCapturing ) {
		CaptureMgr::instanceVI()->stopDevice( mObj->mDeviceID );
		mObj->mIsCapturing = false;
	}
#endif
}

bool Capture::isCapturing()
{
#if defined( CINDER_MAC )
	return [((::CaptureImplQtKit*)mObj->mImpl) isCapturing];
#else
	return mObj->mIsCapturing;
#endif
}

bool Capture::checkNewFrame() const
{
#if defined( CINDER_MAC )
	return [((::CaptureImplQtKit*)mObj->mImpl) checkNewFrame];
#else
	return CaptureMgr::instanceVI()->isFrameNew( mObj->mDeviceID );
#endif	
}

Surface8u Capture::getSurface() const
{
#if defined( CINDER_MAC )
	return [((::CaptureImplQtKit*)mObj->mImpl) getCurrentFrame];
#else
	if( CaptureMgr::instanceVI()->isFrameNew( mObj->mDeviceID ) ) {
		mObj->mCurrentFrame = mObj->mSurfaceCache->getNewSurface();
		CaptureMgr::instanceVI()->getPixels( mObj->mDeviceID, mObj->mCurrentFrame.getData(), false, true );
	}
	
	return mObj->mCurrentFrame;
#endif
}
	
} //namespace cinder
