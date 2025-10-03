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
#include <iomanip>
#if defined( CINDER_MAC ) || defined( CINDER_COCOA_TOUCH_DEVICE )
	#import "cinder/CaptureImplAvFoundation.h"
	typedef CaptureImplAvFoundation	CapturePlatformImpl;
#elif defined( CINDER_COCOA_TOUCH_SIMULATOR )
	#include "cinder/CaptureImplCocoaDummy.h"
	typedef CaptureImplCocoaDummy	CapturePlatformImpl;
#elif defined( CINDER_MSW )
	#include "cinder/CaptureImplDirectShow.h"
	typedef cinder::CaptureImplDirectShow	CapturePlatformImpl;
#elif defined( CINDER_LINUX )
	#include "cinder/CaptureImplGStreamer.h"
	typedef cinder::CaptureImplGStreamer	CapturePlatformImpl;
#elif defined( CINDER_ANDROID )
	#include "cinder/CaptureImplJni.h"
	typedef cinder::CaptureImplJni CapturePlatformImpl;	
#endif

#include <set>
using namespace std;


namespace cinder {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Capture::Mode

Capture::Mode::Mode()
	: mSize( 0, 0 ), mFrameRate( MediaTime() ), mCodec( Codec::Unknown ), mPixelFormat( PixelFormat::Unknown )
{
}

Capture::Mode::Mode( int32_t width, int32_t height, const MediaTime& frameRate, Codec codec, PixelFormat pixelFormat, const std::string& description )
	: mSize( width, height ), mFrameRate( frameRate ), mCodec( codec ), mPixelFormat( pixelFormat ), mDescription( description )
{
}

bool Capture::Mode::isRGBFormat() const
{
	switch( mPixelFormat ) {
		case PixelFormat::RGB24:
		case PixelFormat::BGR24:
		case PixelFormat::ARGB32:
		case PixelFormat::BGRA32:
			return true;
		default:
			return false;
	}
}

bool Capture::Mode::isYUVFormat() const
{
	switch( mPixelFormat ) {
		case PixelFormat::YUV420P:
		case PixelFormat::NV12:
		case PixelFormat::YUY2:
		case PixelFormat::UYVY:
		case PixelFormat::I420:
		case PixelFormat::YV12:
			return true;
		default:
			return false;
	}
}

std::string Capture::Mode::getCodecString() const
{
	switch( mCodec ) {
		case Codec::Uncompressed: return "Uncompressed";
		case Codec::JPEG: return "JPEG";
		case Codec::H264: return "H264";
		case Codec::HEVC: return "HEVC";
		case Codec::Unknown: return "Unknown";
		default: return "Unknown";
	}
}

std::string Capture::Mode::getPixelFormatString() const
{
	switch( mPixelFormat ) {
		case PixelFormat::RGB24: return "RGB24";
		case PixelFormat::BGR24: return "BGR24";
		case PixelFormat::ARGB32: return "ARGB32";
		case PixelFormat::BGRA32: return "BGRA32";
		case PixelFormat::YUV420P: return "YUV420P";
		case PixelFormat::NV12: return "NV12";
		case PixelFormat::YUY2: return "YUY2";
		case PixelFormat::UYVY: return "UYVY";
		case PixelFormat::I420: return "I420";
		case PixelFormat::YV12: return "YV12";
		case PixelFormat::Unknown: return "Unknown";
		default: return "Unknown";
	}
}

bool Capture::Mode::operator==( const Mode& other ) const
{
	return mSize == other.mSize &&
		   mFrameRate == other.mFrameRate &&
		   mCodec == other.mCodec &&
		   mPixelFormat == other.mPixelFormat;
}

bool Capture::Mode::operator<( const Mode& other ) const
{
	// Sort by resolution (pixel count), then frame rate, then codec, then pixel format
	int32_t thisPixels = getPixelCount();
	int32_t otherPixels = other.getPixelCount();

	if( thisPixels != otherPixels )
		return thisPixels < otherPixels;

	if( mFrameRate != other.mFrameRate )
		return mFrameRate < other.mFrameRate;

	if( mCodec != other.mCodec )
		return static_cast<int>( mCodec ) < static_cast<int>( other.mCodec );

	return static_cast<int>( mPixelFormat ) < static_cast<int>( other.mPixelFormat );
}

// Stream operators
std::ostream& operator<<( std::ostream& os, const Capture::Mode& mode )
{
	// Format FPS to 2 decimal places
	os << mode.getWidth() << "x" << mode.getHeight()
	   << " @";

	// Use fixed format with 2 decimal precision for FPS
	std::ios_base::fmtflags oldFlags = os.flags();
	std::streamsize oldPrecision = os.precision();
	os << std::fixed << std::setprecision(2) << mode.getFrameRateFloat();
	os.flags(oldFlags);
	os.precision(oldPrecision);

	os << "fps"
	   << " " << mode.getCodecString()
	   << " " << mode.getPixelFormatString();
	return os;
}

std::ostream& operator<<( std::ostream& os, const Capture::Mode::Codec& codec )
{
	switch( codec ) {
		case Capture::Mode::Codec::Uncompressed: return os << "Uncompressed";
		case Capture::Mode::Codec::JPEG: return os << "JPEG";
		case Capture::Mode::Codec::H264: return os << "H264";
		case Capture::Mode::Codec::HEVC: return os << "HEVC";
		case Capture::Mode::Codec::Unknown: return os << "Unknown";
		default: return os << "Unknown";
	}
}

std::ostream& operator<<( std::ostream& os, const Capture::Mode::PixelFormat& format )
{
	switch( format ) {
		case Capture::Mode::PixelFormat::RGB24: return os << "RGB24";
		case Capture::Mode::PixelFormat::BGR24: return os << "BGR24";
		case Capture::Mode::PixelFormat::ARGB32: return os << "ARGB32";
		case Capture::Mode::PixelFormat::BGRA32: return os << "BGRA32";
		case Capture::Mode::PixelFormat::YUV420P: return os << "YUV420P";
		case Capture::Mode::PixelFormat::NV12: return os << "NV12";
		case Capture::Mode::PixelFormat::YUY2: return os << "YUY2";
		case Capture::Mode::PixelFormat::UYVY: return os << "UYVY";
		case Capture::Mode::PixelFormat::I420: return os << "I420";
		case Capture::Mode::PixelFormat::YV12: return os << "YV12";
		case Capture::Mode::PixelFormat::Unknown: return os << "Unknown";
		default: return os << "Unknown";
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Capture

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

Capture::Capture( const DeviceRef& device, const Mode& mode )
{
#if defined( CINDER_COCOA )
	mImpl = [[::CapturePlatformImpl alloc] initWithDevice:device mode:mode];
#else
	mImpl = new CapturePlatformImpl( device, mode );
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
