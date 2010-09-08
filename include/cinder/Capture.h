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

#if defined( CINDER_MAC )
#	if defined( __OBJC__ )
		@class CaptureImplQtKit;
		@class QTCaptureDevice;
#	else
		class CaptureImplQtKit;
		class QTCaptureDevice;
#	endif

#elif defined( CINDER_COCOA_TOUCH )
#	if defined( __OBJC__ )
		@class CaptureImplAvFoundation;
		//@class QTCaptureDevice;
#	else
		class CaptureImplAvFoundation;
		//class QTCaptureDevice;
#	endif
#endif

#include <map>

namespace cinder {

#if defined( CINDER_MSW )
	class CaptureImplDirectShow;
#endif

class Capture {
 public:
	class Device;
	typedef shared_ptr<Device> DeviceRef;
	
	Capture() {}
	Capture( int32_t width, int32_t height, const DeviceRef device = DeviceRef() );
	~Capture() {}

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
	Vec2i		getSize() const { return Vec2i( getWidth(), getHeight() ); }
	//! Returns the aspect ratio of the capture imagee, which is its width / height
	float		getAspectRatio() const { return getWidth() / (float)getHeight(); }
	//! Returns the bounding rectangle of the capture imagee, which is Area( 0, 0, width, height )
	Area		getBounds() const { return Area( 0, 0, getWidth(), getHeight() ); }
	
	//! Returns a Surface representing the current captured frame.
	Surface8u	getSurface() const;
	//! Returns the associated Device for this instace of Capture
	const Capture::DeviceRef getDevice() const;

	//! Returns a vector of all Devices connected to the system. If \a forceRefresh then the system will be polled for connected devices.
	static const std::vector<DeviceRef>&	getDevices( bool forceRefresh = false );
	//! Finds a particular device based on its name
	static DeviceRef findDeviceByName( const std::string &name );
	//! Finds the first device whose name contains the string \a nameFragment
	static DeviceRef findDeviceByNameContains( const std::string &nameFragment );

#if defined( CINDER_COCOA )
	typedef std::string DeviceIdentifier;
#else
	typedef int DeviceIdentifier;
#endif

	//! \cond
	// This is an abstract base class for implementing platform specific devices
	class Device {
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
	 protected:
		Device() {}
		std::string		mName;
	};
		
 protected: 
	struct Obj {
		Obj( int32_t width, int32_t height, const Capture::DeviceRef device );
		virtual ~Obj();

#if defined( CINDER_MAC ) 
		CaptureImplQtKit				*mImpl;
#elif defined( CINDER_COCOA_TOUCH )
		CaptureImplAvFoundation			*mImpl;
#elif defined( CINDER_MSW )
		CaptureImplDirectShow			*mImpl;
#endif
	};
	
	shared_ptr<Obj>						mObj;
	
  public:
 	//@{
	//! Emulates shared_ptr-like behavior
	typedef shared_ptr<Obj> Capture::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &Capture::mObj; }
	void reset() { mObj.reset(); }
	//@}
};

class CaptureExc : public Exception {
};

class CaptureExcInitFail : public CaptureExc {
};

class CaptureExcInvalidChannelOrder : public CaptureExc {
};

} //namespace cinder