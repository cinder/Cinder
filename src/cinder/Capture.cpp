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
	typedef CaptureImplQtKit	CapturePlatformImpl;
#elif defined( CINDER_MSW )
	#include "cinder/CaptureImplDirectShow.h"
	typedef cinder::CaptureImplDirectShow	CapturePlatformImpl;
#endif

#include <set>
using namespace std;

namespace cinder {

#if defined( CINDER_MSW )
	#define PLATFORM_DEFAULT_CHANNEL_ORDER SurfaceChannelOrder::BGR
#else
	#define PLATFORM_DEFAULT_CHANNEL_ORDER SurfaceChannelOrder::RGB
#endif

//bool Capture::sDevicesEnumerated = false;
//vector<Capture::Device> Capture::sDevices;

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

/*bool Capture::Device::checkAvailable() const
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
}*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Capture
/*const vector<Capture::Device>& Capture::getDevices( bool forceRefresh )
{	
#if defined( CINDER_MAC )
	if( sDevicesEnumerated && ( ! forceRefresh ) )
		return sDevices;

	sDevices.clear();	

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

	sDevicesEnumerated = true;
	return sDevices;
#else
	return CapturePlatformImpl::getDevices( forceRefresh );
#endif
	
	
}*/

/*Capture::Device Capture::findDeviceByName( const string &name )
{
	const vector<Device> &devices = getDevices();
	for( vector<Device>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt ) {
		if( deviceIt->getName() == name )
			return *deviceIt;
	}
	
	return Device(); // failed - return "null" device
}*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Capture::Obj
Capture::Obj::Obj( int32_t width, int32_t height, const Device &device )
	/*: mWidth( width ), mHeight( height ), mCurrentFrame( width, height, false, PLATFORM_DEFAULT_CHANNEL_ORDER ), mDevice( device )*/
{
#if defined( CINDER_COCOA )
	mImpl = [[::CapturePlatformImpl alloc] initWithDevice:mDevice width:width height:height];
#else
	mImpl = new CapturePlatformImpl( width, height, device );
#endif	
}

Capture::Obj::Obj( int32_t width, int32_t height )
	/*: mWidth( width ), mHeight( height ), mCurrentFrame( width, height, false, PLATFORM_DEFAULT_CHANNEL_ORDER ), mDevice( device )*/
{
#if defined( CINDER_COCOA )
	mImpl = [[::CapturePlatformImpl alloc] initWithWidth:width height:height];
#else
	mImpl = new CapturePlatformImpl( width, height );
#endif	
}

Capture::Obj::~Obj()
{
#if defined( CINDER_COCOA )
	[((::CapturePlatformImpl*)mImpl) stopCapture];
	[((::CapturePlatformImpl*)mImpl) release];
#else
	delete mImpl;
#endif
}

Capture::Capture( int32_t width, int32_t height, const Device &device ) 
{
	mObj = shared_ptr<Obj>( new Obj( width, height, device ) );
}

Capture::Capture( int32_t width, int32_t height) 
{
	mObj = shared_ptr<Obj>( new Obj( width, height ) );
}

void Capture::start()
{
#if defined( CINDER_COCOA )
	[((::CapturePlatformImpl*)mObj->mImpl) startCapture];
#else
	mObj->mImpl->start();
#endif
}

void Capture::stop()
{
#if defined( CINDER_COCOA )
	[((::CapturePlatformImpl*)mObj->mImpl) stopCapture];
#else
	mObj->mImpl->stop();
#endif
}

bool Capture::isCapturing()
{
#if defined( CINDER_COCOA )
	return [((::CapturePlatformImpl*)mObj->mImpl) isCapturing];
#else
	return mObj->mImpl->isCapturing();
#endif
}

bool Capture::checkNewFrame() const
{
#if defined( CINDER_COCOA )
	return [((::CapturePlatformImpl*)mObj->mImpl) checkNewFrame];
#else
	return mObj->mImpl->checkNewFrame();
#endif	
}

Surface8u Capture::getSurface() const
{
#if defined( CINDER_COCOA )
	return [((::CapturePlatformImpl*)mObj->mImpl) getCurrentFrame];
#else
	return mObj->mImpl->getSurface();
#endif
}

int32_t	Capture::getWidth() const { 
#if defined( CINDER_COCOA )
	
#else 
	return mObj->mImpl->getWidth();
#endif
}

int32_t	Capture::getHeight() const { 
#if defined( CINDER_COCOA )
	
#else
	return mObj->mImpl->getHeight();
#endif
}

const Capture::Device& Capture::getDevice() const {
#if defined( CINDER_COCOA )
	 
#else
	return mObj->mImpl->getDevice();
#endif
}
	
} //namespace cinder
