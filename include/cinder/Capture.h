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
#include "cinder/Surface.h"
#include "cinder/Exception.h"
#include "cinder/MediaTime.h"

#if defined( CINDER_ANDROID )
  #include "cinder/gl/Texture.h"
#endif 

#if defined( CINDER_MAC ) || defined( CINDER_COCOA_TOUCH_DEVICE )
	#if defined( __OBJC__ )
		@class CaptureImplAvFoundation;
	#else
		class CaptureImplAvFoundation;
	#endif
#elif defined( CINDER_COCOA_TOUCH_SIMULATOR )
	#if defined( __OBJC__ )
		@class CaptureImplCocoaDummy;
	#else
		class CaptureImplCocoaDummy;
	#endif
#elif defined( CINDER_MSW )
	namespace cinder {
		class CaptureImplDirectShow;
	}
#elif defined( CINDER_LINUX )
	namespace cinder {
		class CaptureImplGStreamer;
	}
#elif defined( CINDER_ANDROID )
	namespace cinder {
		class CaptureImplJni;
	}	
#endif

#include <map>

namespace cinder {

typedef std::shared_ptr<class Capture>	CaptureRef;

class CI_API Capture {
  public:
	class Mode;
	class Device;
	typedef std::shared_ptr<Device> DeviceRef;

	//! Creates a new Capture requesting (but not promising) a resolution of \a width x \a height pixels.
	static CaptureRef	create( int32_t width, int32_t height, const DeviceRef device = DeviceRef() ) { return CaptureRef( new Capture( width, height, device ) ); }
	//! Creates a new Capture using a specific Mode supported by the Device. The Mode must be from the Device's getModes() list.
	static CaptureRef	create( const DeviceRef& device, const Mode& mode ) { return CaptureRef( new Capture( device, mode ) ); }

	~Capture();

	//! Begin capturing video
	void		start();
	//! Stop capturing video
	void		stop();
	//! Is the device capturing video
	bool		isCapturing();

	//! Returns whether there is a new video frame available since the last call to checkNewFrame()
	bool		checkNewFrame() const;

	//! Returns the width of the captured image in pixels.
	int32_t		getWidth() const;
	//! Returns the height of the captured image in pixels.
	int32_t		getHeight() const;
	//! Returns the size of the captured image in pixels.
	ivec2		getSize() const { return ivec2( getWidth(), getHeight() ); }
	//! Returns the aspect ratio of the capture imagee, which is its width / height
	float		getAspectRatio() const { return getWidth() / (float)getHeight(); }
	//! Returns the bounding rectangle of the capture imagee, which is Area( 0, 0, width, height )
	Area		getBounds() const { return Area( 0, 0, getWidth(), getHeight() ); }
	
	//! Returns a SurfaceRef representing the current captured frame.
	Surface8uRef	getSurface() const;

#if defined( CINDER_ANDROID )
	gl::Texture2dRef	getTexture() const;
#endif

	//! Returns the associated Device for this instace of Capture
	const Capture::DeviceRef getDevice() const;

	//! Returns a vector of all Devices connected to the system. If \a forceRefresh then the system will be polled for connected devices.
	static const std::vector<DeviceRef>&	getDevices( bool forceRefresh = false );
	//! Finds a particular device based on its name
	static DeviceRef findDeviceByName( const std::string &name );
	//! Finds the first device whose name contains the string \a nameFragment
	static DeviceRef findDeviceByNameContains( const std::string &nameFragment );

#if defined( CINDER_COCOA ) || defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
	typedef std::string DeviceIdentifier;
#else
	typedef int DeviceIdentifier;
#endif

	//! Represents a specific capture mode with resolution, framerate, codec and pixel format
	class CI_API Mode {
	public:
		enum class Codec {
			Uncompressed,   // Raw uncompressed data
			JPEG,          // Motion JPEG
			H264,          // H.264/AVC
			HEVC,          // H.265/HEVC
			Unknown
		};

		enum class PixelFormat {
			// RGB formats
			RGB24,         // 24-bit RGB
			BGR24,         // 24-bit BGR
			ARGB32,        // 32-bit ARGB
			BGRA32,        // 32-bit BGRA

			// YUV formats
			YUV420P,       // Planar 4:2:0 YUV
			NV12,          // Semi-planar 4:2:0 YUV
			YUY2,          // Packed 4:2:2 YUV
			UYVY,          // Packed 4:2:2 YUV (U first)
			I420,          // Same as YUV420P but explicit
			YV12,          // YUV 4:2:0 with swapped U/V planes

			Unknown
		};

		// Construction
		Mode( int32_t width, int32_t height, const MediaTime& frameRate, Codec codec, PixelFormat pixelFormat, const std::string& description = "" );
		Mode();  // Default constructor

		// Core properties
		const ivec2&        getSize() const { return mSize; }
		int32_t             getWidth() const { return mSize.x; }
		int32_t             getHeight() const { return mSize.y; }
		const MediaTime&    getFrameRate() const { return mFrameRate; }
		float               getFrameRateFloat() const { return static_cast<float>( 1.0 / mFrameRate.getSeconds() ); }
		Codec               getCodec() const { return mCodec; }
		PixelFormat         getPixelFormat() const { return mPixelFormat; }
		const std::string&  getDescription() const { return mDescription; }

		// Utility
		float               getAspectRatio() const { return mSize.x / static_cast<float>( mSize.y ); }
		int32_t             getPixelCount() const { return mSize.x * mSize.y; }
		bool                isRGBFormat() const;
		bool                isYUVFormat() const;
		bool                isCompressed() const { return mCodec != Codec::Uncompressed; }

		// Human-readable representation
		std::string         getCodecString() const;
		std::string         getPixelFormatString() const;

		// Comparison
		bool operator==( const Mode& other ) const;
		bool operator!=( const Mode& other ) const { return !( *this == other ); }
		bool operator<( const Mode& other ) const;

		// Platform-specific data access
		const std::string& getPlatformData() const { return mPlatformData; }
		void setPlatformData( const std::string& data ) { mPlatformData = data; }

	private:
		ivec2       mSize;
		MediaTime   mFrameRate;
		Codec       mCodec;
		PixelFormat mPixelFormat;
		std::string mDescription;
		std::string mPlatformData;
	};

	// This is an abstract base class for implementing platform specific devices
	class CI_API Device {
	 public:
		virtual ~Device() {}
		//! Returns the human-readable name of the device.
		const std::string&					getName() const { return mName; }
		//! Returns whether the device is available for use.
		virtual bool						checkAvailable() const = 0;
		//! Returns whether the device is currently connected.
		virtual bool						isConnected() const = 0;
		//! Returns the OS-specific unique identifier
		virtual Capture::DeviceIdentifier	getUniqueId() const = 0;
		//! Returns an OS-specific pointer. AVCaptureDevice* on OS X and iOS. Not implemented on MSW.
#if defined( CINDER_COCOA )
		virtual void*		getNative() const = 0;
#endif
#if defined( CINDER_COCOA_TOUCH ) || defined( CINDER_ANDROID )
		//! Returns whether device is front-facing. False implies rear-facing.
		virtual bool		isFrontFacing() const = 0;
#endif
		//! Returns supported capture modes for this device
		virtual std::vector<Mode> getModes() const = 0;

	 protected:
		Device() {}
		std::string		mName;
	};
		
 protected:
 	Capture( int32_t width, int32_t height, const DeviceRef device );
 	Capture( const DeviceRef& device, const Mode& mode );

#if defined( CINDER_MAC ) || defined( CINDER_COCOA_TOUCH_DEVICE )
	CaptureImplAvFoundation			*mImpl;
#elif defined( CINDER_COCOA_TOUCH_SIMULATOR )
	CaptureImplCocoaDummy			*mImpl;
#elif defined( CINDER_MSW )
	CaptureImplDirectShow			*mImpl;
#elif defined( CINDER_LINUX )
	CaptureImplGStreamer			*mImpl;
#elif defined( CINDER_ANDROID )
	CaptureImplJni					*mImpl;		
#endif
};

class CI_API CaptureExc : public Exception {
  public:
	CaptureExc() {}
	CaptureExc( const std::string &description ) : Exception( description ) {}
};

class CI_API CaptureExcInitFail : public CaptureExc {
  public:
	CaptureExcInitFail() {}
	CaptureExcInitFail( const std::string &description ) : CaptureExc( description ) {}
};

class CI_API CaptureExcInvalidChannelOrder : public CaptureExc {
};

// Stream operators for Mode - enables Cinder's toString() to work
CI_API std::ostream& operator<<( std::ostream& os, const Capture::Mode& mode );
CI_API std::ostream& operator<<( std::ostream& os, const Capture::Mode::Codec& codec );
CI_API std::ostream& operator<<( std::ostream& os, const Capture::Mode::PixelFormat& format );

} //namespace cinder
