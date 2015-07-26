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

#include "cinder/CaptureImplJni.h"
#include "cinder/android/hardware/Camera.h"
#include "cinder/android/AndroidDevLog.h"
using namespace ci::android;

#include "cinder/app/App.h"

namespace cinder {

/** \class SurfaceCache
 *
 */
class SurfaceCache {
 public:
	SurfaceCache( int32_t width, int32_t height, SurfaceChannelOrder sco, int numSurfaces )
		: mWidth( width ), mHeight( height ), mSCO( sco )
	{
		for( int i = 0; i < numSurfaces; ++i ) {
			mSurfaceData.push_back( std::shared_ptr<uint8_t>( new uint8_t[width*height*sco.getPixelInc()], std::default_delete<uint8_t[]>() ) );
			mSurfaceUsed.push_back( false );
		}
	}
	
	Surface8uRef getNewSurface()
	{
		// try to find an available block of pixel data to wrap a surface around	
		for( size_t i = 0; i < mSurfaceData.size(); ++i ) {
			if( ! mSurfaceUsed[i] ) {
				mSurfaceUsed[i] = true;
				auto newSurface = new Surface( mSurfaceData[i].get(), mWidth, mHeight, mWidth * mSCO.getPixelInc(), mSCO );
				Surface8uRef result = std::shared_ptr<Surface8u>( newSurface, [=] ( Surface8u* s ) { mSurfaceUsed[i] = false; } );
				return result;
			}
		}

		// we couldn't find an available surface, so we'll need to allocate one
		return Surface8u::create( mWidth, mHeight, mSCO.hasAlpha(), mSCO );
	}

 private:
	std::vector<std::shared_ptr<uint8_t>>	mSurfaceData;
	std::vector<bool>						mSurfaceUsed;
	int32_t									mWidth, mHeight;
	SurfaceChannelOrder						mSCO;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// CaptureImplJni::Device

CaptureImplJni::Device::Device( const Capture::DeviceIdentifier& uniqueId, bool frontFacing, const std::vector<ivec2>& supportedResolutions, ci::android::hardware::Camera* nativeDevice )
	: mUniqueId( uniqueId ), mFrontFacing( frontFacing ), mSupportedResolutions( supportedResolutions ), mNativeDevice( nativeDevice )
{
	/*
	ci::app::console() << "CaptureImplJni::Device id=" << mUniqueId << std::endl;
	for( const auto& res : mSupportedResolutions ) {
		ci::app::console() << res.x << "x" << res.y << std::endl;
	}
	*/
}

CaptureImplJni::Device::~Device()
{

}

bool CaptureImplJni::Device::checkAvailable() const
{
	return false;
}

bool CaptureImplJni::Device::isConnected() const
{
	return false;
}

void CaptureImplJni::Device::start(int width, int height)
{
	if( ! getNative() ) {
		return;
	}

	getNative()->startCapture( getUniqueId(), width, height );
}

void CaptureImplJni::Device::stop()
{
	if( ! getNative() ) {
		return;
	}

	getNative()->stopCapture();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// CaptureImplJni

bool 							CaptureImplJni::sDevicesEnumerated = false;
std::vector<Capture::DeviceRef>	CaptureImplJni::sDevices;

CaptureImplJni::CaptureImplJni( int width, int height, const Capture::DeviceRef device )
	: mWidth( width ), mHeight( height ), mDevice( device )
{

	if( ! mDevice ) {
		auto devices = CaptureImplJni::getDevices();
		for( auto& device : devices ) {
			if( ! device->isFrontFacing() ) {
				mDevice = device;
			}
		}

		if( ( ! mDevice ) && ( ! devices.empty() ) ) {
			mDevice = devices.front();
		}
	}

	if( mDevice ) {
		int targetArea = mWidth*mHeight;
		int leastDiff = std::numeric_limits<int>::max();
		int preferredWidth = mWidth;
		int preferredHeight = mHeight;
		auto fullDevice = std::dynamic_pointer_cast<CaptureImplJni::Device>( mDevice );	
		if( fullDevice ) {
			for( const auto& res : fullDevice->getSupportedResolutions() ) {
				int area = res.x*res.y;
				int diff = abs(area - targetArea);
				if( diff < leastDiff ) {
					leastDiff = diff;
					preferredWidth = res.x;
					preferredHeight = res.y;
				}
			}
		}

		mWidth = preferredWidth;
		mHeight = preferredHeight;

		ci::app::console() << "mWidth=" << mWidth << ", mHeight=" << mHeight << std::endl;

		mSurfaceCache = std::shared_ptr<SurfaceCache>( new SurfaceCache( mWidth, mHeight, SurfaceChannelOrder::RGB, 4 ) );	
	}
}

CaptureImplJni::~CaptureImplJni()
{
	// NOTE: This is not redundant. Clear/reset so if/when activity restarts
	//       the values are in their default state.
	//
	CaptureImplJni::sDevicesEnumerated = false;
	CaptureImplJni::sDevices.clear();

	ci::android::hardware::Camera::destroyInstance();
}

void CaptureImplJni::start()
{
	//ci::android::hardware::Camera::getInstance()->startCapture( "0" );

	if( ! mDevice ) {
		return;
	}

	auto fullDevice = std::dynamic_pointer_cast<CaptureImplJni::Device>( mDevice );	
	if( fullDevice ) {
		fullDevice->start(mWidth, mHeight);
		mCapturing = true;
	}
}

void CaptureImplJni::stop()
{
	mCapturing = false;

	if( mDevice ) {
		auto fullDevice = std::dynamic_pointer_cast<CaptureImplJni::Device>( mDevice );	
		fullDevice->stop();
		mDevice.reset();
	}
}

bool CaptureImplJni::isCapturing() const
{
	return mCapturing;
}

bool CaptureImplJni::checkNewFrame() const
{
	bool result = false;
	if( isCapturing() && mDevice ) {
		auto fullDevice = std::dynamic_pointer_cast<CaptureImplJni::Device>( mDevice );	
		result = fullDevice && fullDevice->getNative() && fullDevice->getNative()->isNewFrameAvailable();
	}
	return result;
}

Surface8uRef CaptureImplJni::getSurface() const
{
	if( checkNewFrame() ) {
		mCurrentFrame = mSurfaceCache->getNewSurface();

		auto fullDevice = std::dynamic_pointer_cast<CaptureImplJni::Device>( mDevice );	
		fullDevice->getNative()->getPixels( &(*mCurrentFrame) );
		fullDevice->getNative()->clearNewFrameAvailable();		
	}

	return mCurrentFrame;
}

gl::Texture2dRef CaptureImplJni::getTexture() const
{
	if( ! mCurrentTexture ) {
		gl::Texture2d::Format texFmt;
		texFmt.target( GL_TEXTURE_EXTERNAL_OES );
		texFmt.minFilter( GL_LINEAR );
		texFmt.magFilter( GL_LINEAR );
		texFmt.wrap( GL_CLAMP_TO_EDGE );
		mCurrentTexture = gl::Texture2d::create( mWidth, mHeight, texFmt );

		auto fullDevice = std::dynamic_pointer_cast<CaptureImplJni::Device>( mDevice );	
		fullDevice->getNative()->initPreviewTexture( mCurrentTexture->getId() );		
	}

	if( checkNewFrame() ) {
		auto fullDevice = std::dynamic_pointer_cast<CaptureImplJni::Device>( mDevice );	
		fullDevice->getNative()->updateTexImage();				
	}

	return mCurrentTexture;
}

const std::vector<Capture::DeviceRef>& CaptureImplJni::getDevices( bool forceRefresh )
{
	if( ( ! CaptureImplJni::sDevicesEnumerated ) || forceRefresh ) {
		try{
			auto nativeDevices = ci::android::hardware::Camera::getInstance()->enumerateDevices();
			for( auto& nd : nativeDevices ) {				
				auto device = std::make_shared<CaptureImplJni::Device>( nd->id, nd->frontFacing, nd->resolutions, ci::android::hardware::Camera::getInstance() );
				sDevices.push_back( device );
			}
			nativeDevices.clear();

			CaptureImplJni::sDevicesEnumerated = true;
		}
		catch( const std::exception& e ) {
			ci::android::dbg_app_error( std::string( "failed enumering devices: ") + e.what() );
		}
	}

	return CaptureImplJni::sDevices;
}

} // namespace cinder