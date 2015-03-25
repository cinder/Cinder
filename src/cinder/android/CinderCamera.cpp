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

#include "cinder/android/CinderCamera.h"

#include <sstream>
#include <iomanip>

#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "cinder", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "cinder", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"cinder", __VA_ARGS__))

#define FN_LOG( __FNREF__ ) 							\
 	if( NULL != __FNREF__ ) {							\
 		LOGI( #__FNREF__ " successfully obtained" );	\
 	}

namespace cinder { namespace android {

jclass		CinderCamera::sCinderCameraClass		= NULL;
jmethodID	CinderCamera::sInitializeMethodId 		= NULL;
jmethodID 	CinderCamera::sHasFrontCameraMethodId	= NULL;
jmethodID 	CinderCamera::sHasBackCameraMethodId 	= NULL;
jmethodID 	CinderCamera::sStartCaptureMethodId 	= NULL;
jmethodID 	CinderCamera::sStopCaptureMethodId 		= NULL;
jmethodID 	CinderCamera::sLockPixelsMethodId 		= NULL;
jmethodID 	CinderCamera::sUnlockPixelsMethodId 	= NULL;
jmethodID 	CinderCamera::sGetWidthMethodId			= NULL;
jmethodID 	CinderCamera::sGetHeightMethodId		= NULL;

CinderCamera::CinderCamera()
{
}

CinderCamera::~CinderCamera()
{
	if( mCapturing ) {
		stopCapture();
	}
}

void CinderCamera::cacheJni()
{
	if( JniHelper::get()->AttachCurrentThread() ) {
		jclass cinderCameraClass = JniHelper::get()->retrieveClass( "org/libcinder/hardware/CinderCamera" );	
		CinderCamera::sCinderCameraClass = (jclass)JniHelper::get()->NewGlobalRef( cinderCameraClass );

		if( NULL != CinderCamera::sCinderCameraClass ) {
			CinderCamera::sInitializeMethodId 		= JniHelper::get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "initialize", "()Z" );
			CinderCamera::sHasFrontCameraMethodId	= JniHelper::get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "hasFrontCamera", "()Z" );
			CinderCamera::sHasBackCameraMethodId 	= JniHelper::get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "hasBackCamera", "()Z" );
			CinderCamera::sStartCaptureMethodId 	= JniHelper::get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "startCapture", "()V" );
			CinderCamera::sStopCaptureMethodId 		= JniHelper::get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "stopCapture", "()V" );
			CinderCamera::sLockPixelsMethodId 		= JniHelper::get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "lockPixels", "()[B" );
			CinderCamera::sUnlockPixelsMethodId 	= JniHelper::get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "unlockPixels", "()V" );
			CinderCamera::sGetWidthMethodId 		= JniHelper::get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "getWidth", "()I" );
			CinderCamera::sGetHeightMethodId 		= JniHelper::get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "getHeight", "()I" );

			FN_LOG( CinderCamera::sInitializeMethodId );	
			FN_LOG( CinderCamera::sHasFrontCameraMethodId );	
			FN_LOG( CinderCamera::sHasBackCameraMethodId );	
			FN_LOG( CinderCamera::sStartCaptureMethodId );	
			FN_LOG( CinderCamera::sStopCaptureMethodId );	
			FN_LOG( CinderCamera::sLockPixelsMethodId );	
			FN_LOG( CinderCamera::sUnlockPixelsMethodId );	
		}
		JniHelper::get()->DeatchCurrentThread();
	}
}

void CinderCamera::destroyJni()
{
	if( JniHelper::get()->AttachCurrentThread() ) {
		
		JniHelper::get()->DeleteGlobalRef( CinderCamera::sCinderCameraClass );

		CinderCamera::sCinderCameraClass		= NULL;
		CinderCamera::sInitializeMethodId 		= NULL;
		CinderCamera::sHasFrontCameraMethodId	= NULL;
		CinderCamera::sHasBackCameraMethodId 	= NULL;
		CinderCamera::sStartCaptureMethodId 	= NULL;
		CinderCamera::sStopCaptureMethodId 		= NULL;
		CinderCamera::sLockPixelsMethodId 		= NULL;
		CinderCamera::sUnlockPixelsMethodId 	= NULL;	

		JniHelper::get()->DeatchCurrentThread();
	}
}

bool CinderCamera::initialize()
{
	mInitialized = false;

	if( JniHelper::get()->AttachCurrentThread() ) {
		mInitialized = (bool)JniHelper::get()->CallStaticBooleanMethod( CinderCamera::sCinderCameraClass, CinderCamera::sInitializeMethodId );

		if( mInitialized ) {
			mHasFrontCamera = (bool)JniHelper::get()->CallStaticBooleanMethod( CinderCamera::sCinderCameraClass, CinderCamera::sHasFrontCameraMethodId );
			mHasBackCamera = (bool)JniHelper::get()->CallStaticBooleanMethod( CinderCamera::sCinderCameraClass, CinderCamera::sHasBackCameraMethodId );
		}

		JniHelper::get()->DeatchCurrentThread();
	}

	return mInitialized;	
}

void CinderCamera::startCapture()
{
	if( ! mInitialized ) {
		return;
	}

	if( JniHelper::get()->AttachCurrentThread() ) {
		JniHelper::get()->CallStaticVoidMethod( CinderCamera::sCinderCameraClass, CinderCamera::sStartCaptureMethodId );
		
		mWidth = JniHelper::get()->CallStaticIntMethod( CinderCamera::sCinderCameraClass, CinderCamera::sGetWidthMethodId );
		mHeight = JniHelper::get()->CallStaticIntMethod( CinderCamera::sCinderCameraClass, CinderCamera::sGetHeightMethodId );

		mCapturing = true;

		JniHelper::get()->DeatchCurrentThread();
	}	
}

void CinderCamera::stopCapture()
{
	if( ! mInitialized ) {
		return;
	}

	if( JniHelper::get()->AttachCurrentThread() ) {
		JniHelper::get()->CallStaticVoidMethod( CinderCamera::sCinderCameraClass, CinderCamera::sStopCaptureMethodId );

		mCapturing = false;

		JniHelper::get()->DeatchCurrentThread();
	}	
}

const uint8_t * CinderCamera::lockPixels()
{
	const uint8_t * result = nullptr;

	if( mCapturing ) {
		if( JniHelper::get()->AttachCurrentThread() ) {
			jbyteArray jBytes = (jbyteArray)JniHelper::get()->CallStaticObjectMethod( CinderCamera::sCinderCameraClass, CinderCamera::sLockPixelsMethodId );
			if( NULL != jBytes ) {
				result = (const uint8_t *)jBytes;
				mLockedPixels = true;    
			}
			JniHelper::get()->DeatchCurrentThread();
		}	
	}

	return result;
}

void CinderCamera::unlockPixels()
{
	if( mLockedPixels ) {
		if( JniHelper::get()->AttachCurrentThread() ) {
			JniHelper::get()->CallStaticVoidMethod( CinderCamera::sCinderCameraClass, CinderCamera::sUnlockPixelsMethodId );
			JniHelper::get()->DeatchCurrentThread();
		}
		mLockedPixels = false;
	}
}

ci::Surface	CinderCamera::getSurface()
{
	ci::Surface result = ci::Surface( mWidth, mHeight, false );

	const uint8_t * pixels = lockPixels();
	if( nullptr != pixels ) {

		const uint8_t * yuyv_image = pixels;
		uint8_t * rgb_image = result.getData();

		int y;
		int cr;
		int cb;

		double r;
		double g;
		double b;

		for( int i = 0, j = 0; i < mWidth*mHeight*3; i += 6, j += 4 ) {
		    //first pixel
		    y  = yuyv_image[j];
		    cb = yuyv_image[j+1];
		    cr = yuyv_image[j+3];

		    r = y + (1.4065 * (cr - 128));
		    g = y - (0.3455 * (cb - 128)) - (0.7169 * (cr - 128));
		    b = y + (1.7790 * (cb - 128));

		    //This prevents colour distortions in your rgb image
		    if (r < 0) r = 0;
		    else if (r > 255) r = 255;
		    if (g < 0) g = 0;
		    else if (g > 255) g = 255;
		    if (b < 0) b = 0;
		    else if (b > 255) b = 255;

		    rgb_image[i]   = (unsigned char)r;
		    rgb_image[i+1] = (unsigned char)g;
		    rgb_image[i+2] = (unsigned char)b;

		    //second pixel
		    y = yuyv_image[j+2];
		    cb = yuyv_image[j+1];
		    cr = yuyv_image[j+3];

		    r = y + (1.4065 * (cr - 128));
		    g = y - (0.3455 * (cb - 128)) - (0.7169 * (cr - 128));
		    b = y + (1.7790 * (cb - 128));

		    if (r < 0) r = 0;
		    else if (r > 255) r = 255;
		    if (g < 0) g = 0;
		    else if (g > 255) g = 255;
		    if (b < 0) b = 0;
		    else if (b > 255) b = 255;

		    rgb_image[i+3] = (unsigned char)r;
		    rgb_image[i+4] = (unsigned char)g;
		    rgb_image[i+5] = (unsigned char)b;
		}


		unlockPixels();
	}

	return result;
}

}} // namespace cinder::android