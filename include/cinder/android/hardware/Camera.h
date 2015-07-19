/*
 Copyright (c) 2015, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#include "cinder/android/CinderAndroid.h"
#include "cinder/android/JniHelper.h"
#include "cinder/Surface.h"

namespace cinder { namespace android { namespace app {

class CinderNativeActivity;

}}} // namespace cinder::android::app

namespace cinder { namespace android { namespace hardware {

/** \class Camera
 *
 */
class Camera {
public:

	enum ImageFormat {
		UNKNOWN 	= 0,
		RGB_565		= 4,
		YV12		= 0x32315659,
		Y8 			= 0x20203859,
		Y16 		= 0x20363159,
		NV16 		= 0x10,
		NV21 		= 0x11,
		YUY2 		= 0x14,
		JPEG 		= 0x100,
		YUV_420_888	= 0x23,
		RAW_SENSOR 	= 0x20,
		RAW10 		= 0x25,
	};

	class DeviceInfo;
	using DeviceInfoRef = std::shared_ptr<DeviceInfo>;

	/** \class DeviceInfo
	 *
	 */
	class DeviceInfo {
	public:
		std::string 		id;
		bool 				frontFacing = false;
		std::vector<ivec2>	resolutions;

		DeviceInfo( jobject obj );
		virtual ~DeviceInfo();

		void sync();

	private:
		struct Java {
			static jclassID		ClassName;
			static jclass 		ClassObject;
			static jfieldID		id;
			static jfieldID		frontFacing;
			static jfieldID		resolutions;
		};

		static void 		cacheJni();
		static void 		destroyJni();

		JniGlobalObjectRef	mJavaObject;

		friend class Camera;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////
	// Camera

	Camera();
	virtual ~Camera();

	static Camera*				getInstance();
	static void 				destroyInstance();

	void 						initialize();
	std::vector<DeviceInfoRef>	enumerateDevices() const;

	void 						startCapture( const std::string& deviceId, int width, int height );
	void 						stopCapture();

	bool 						isNewFrameAvailable() const;
	void 						clearNewFrameAvailable();

	void 						initPreviewTexture(int textureId);
	void 						updateTexImage();

	void						getPixels( Surface8u* outSurface ) const;

private:
	struct Java {
		static jmethodID		hardware_camera_enumerateDevices;
		static jmethodID 		hardware_camera_initialize;
		static jmethodID		hardware_camera_startCapture;
		static jmethodID		hardware_camera_stopCapture;
		static jmethodID 		hardware_camera_lockPixels;
		static jmethodID 		hardware_camera_unlockPixels;
		static jmethodID 		hardware_camera_isNewFrameAvailable;
		static jmethodID 		hardware_camera_clearNewFrameAvailable;
		static jmethodID 		hardware_camera_initPreviewTexture;
		static jmethodID 		hardware_camera_updateTexImage;
	};

	static void 				cacheJni();
	static void 				destroyJni();

	static std::unique_ptr<Camera>	sInstance;

	int 							mWidth = 0;
	int 							mHeight = 0;

	bool 							mCapturing = false;

private:
	friend class ci::android::app::CinderNativeActivity;
};

}}} // namespace cinder::android::hardware