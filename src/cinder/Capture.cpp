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
#if defined( CINDER_MAC ) || defined( CINDER_COCOA_TOUCH_DEVICE )
	#import "cinder/CaptureImplAvFoundation.h"
	typedef CaptureImplAvFoundation	CapturePlatformImpl;
#elif defined( CINDER_COCOA_TOUCH_SIMULATOR )
	#include "cinder/CaptureImplCocoaDummy.h"
	typedef CaptureImplCocoaDummy	CapturePlatformImpl;
#elif defined( CINDER_MSW )
	#include "cinder/CaptureImplDirectShow.h"
	typedef cinder::CaptureImplDirectShow	CapturePlatformImpl;
#elif defined( CINDER_ANDROID )
	#include "cinder/CaptureImplJni.h"
	typedef cinder::CaptureImplJni CapturePlatformImpl;	
#endif

#include <set>
using namespace std;


namespace cinder {

const vector<Capture::DeviceRef>& Capture::getDevices( bool forceRefresh )
{	
#if defined( CINDER_COCOA )
	return [CapturePlatformImpl getDevices:forceRefresh];
#else
	return CapturePlatformImpl::getDevices( forceRefresh );
#endif
}

Capture::DeviceRef Capture::findDeviceByName( const string &name )
{
	for( const auto &dev : getDevices() ) {
		if( dev->getName() == name )
			return dev;
	}
	
	return DeviceRef(); // failed - return "null" device
}

Capture::DeviceRef Capture::findDeviceByNameContains( const string &nameFragment )
{	
	for( const auto &dev : getDevices() ) {
		if( dev->getName().find( nameFragment ) != std::string::npos )
			return dev;
	}

	return DeviceRef();
}

Capture::Capture( int32_t width, int32_t height, const DeviceRef device ) 
{
#if defined( CINDER_COCOA )
	mImpl = [[::CapturePlatformImpl alloc] initWithDevice:device width:width height:height];
#else
	mImpl = new CapturePlatformImpl( width, height, device );
#endif
}

Capture::~Capture()
{
#if defined( CINDER_COCOA )
	[((::CapturePlatformImpl*)mImpl) release];
#else
	delete mImpl;
#endif
}

void Capture::start()
{
#if defined( CINDER_COCOA )
	[mImpl startCapture];
#else
	mImpl->start();
#endif
}

void Capture::stop()
{
#if defined( CINDER_COCOA )
	[mImpl stopCapture];
#else
	mImpl->stop();
#endif
}

bool Capture::isCapturing()
{
#if defined( CINDER_COCOA )
	return [mImpl isCapturing];
#else
	return mImpl->isCapturing();
#endif
}

bool Capture::checkNewFrame() const
{
#if defined( CINDER_COCOA )
	return [mImpl checkNewFrame];
#else
	return mImpl->checkNewFrame();
#endif	
}

Surface8uRef Capture::getSurface() const
{
#if defined( CINDER_COCOA )
	return [mImpl getCurrentFrame];
#else
	return mImpl->getSurface();
#endif
}

#if defined( CINDER_ANDROID )
gl::Texture2dRef Capture::getTexture() const
{
	return mImpl->getTexture();
}
#endif

int32_t	Capture::getWidth() const { 
#if defined( CINDER_COCOA )
	return [mImpl getWidth];
#else 
	return mImpl->getWidth();
#endif
}

int32_t	Capture::getHeight() const { 
#if defined( CINDER_COCOA )
	return [mImpl getHeight];
#else
	return mImpl->getHeight();
#endif
}

const Capture::DeviceRef Capture::getDevice() const {
#if defined( CINDER_COCOA )
	return [mImpl getDevice];
#else
	return mImpl->getDevice();
#endif
}

} //namespace cinder
