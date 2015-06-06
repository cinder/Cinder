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

#include "cinder/android/app/CinderNativeActivity.h"
#include "cinder/android/hardware/Camera.h"
#include "cinder/android/net/UrlLoader.h"
#include "cinder/android/video/VideoPlayer.h"
#include "cinder/android/AndroidDevLog.h"
using namespace cinder::android;

namespace cinder { namespace android { namespace app {

jclassID	CinderNativeActivity::Java::ClassName			= "org/libcinder/app/CinderNativeActivity";
jclass  	CinderNativeActivity::Java::ClassObject 		= nullptr;
jmethodID 	CinderNativeActivity::Java::getDisplayRotation	= nullptr;

std::unique_ptr<CinderNativeActivity> CinderNativeActivity::sInstance;

CinderNativeActivity::CinderNativeActivity( jobject obj )
{
	// No global ref needed for this
	mJavaObject = obj;
}

CinderNativeActivity::~CinderNativeActivity()
{

}

void CinderNativeActivity::cacheJni()
{
dbg_app_fn_enter( __PRETTY_FUNCTION__ );

	if( JniHelper::Get()->AttachCurrentThread() ) {
		try {
			jclass javaClass = JniHelper::Get()->RetrieveClass( Java::ClassName );
			if( nullptr != javaClass ) {
				Java::ClassObject = reinterpret_cast<jclass>( JniHelper::Get()->NewGlobalRef( javaClass ) );
				jni_obtained_check( CinderNativeActivity::Java::ClassObject );
			}

			jobject activityObject = JniHelper::Get()->GetActivityObject();
			CinderNativeActivity::sInstance = std::unique_ptr<CinderNativeActivity>( new CinderNativeActivity( activityObject ) );				

			if( nullptr != Java::ClassObject ) {
				Java::getDisplayRotation = JniHelper::Get()->GetMethodId( Java::ClassObject, "getDisplayRotation", "()I" ); 
				jni_obtained_check( CinderNativeActivity::Java::getDisplayRotation );				
			}

		}
		catch( const std::exception& e ) {
			dbg_app_error( e.what() );
		}
	}	

dbg_app_fn_exit( __PRETTY_FUNCTION__ );
}

void CinderNativeActivity::destroyJni()
{
dbg_app_fn_enter( __PRETTY_FUNCTION__ );
	if( JniHelper::Get()->AttachCurrentThread() ) {	
		JniHelper::Get()->DeleteGlobalRef( Java::ClassObject  );
		Java::ClassObject			= nullptr;
		Java::getDisplayRotation	= nullptr;
	}
dbg_app_fn_exit( __PRETTY_FUNCTION__ );
}

void CinderNativeActivity::registerComponents()
{
dbg_app_fn_enter( __PRETTY_FUNCTION__ );

	CinderNativeActivity::cacheJni();
	ci::android::hardware::Camera::cacheJni();
	ci::android::net::UrlLoader::cacheJni();
	ci::android::video::VideoPlayer::cacheJni();

dbg_app_fn_exit( __PRETTY_FUNCTION__ );
}

void CinderNativeActivity::unregisterComponents()
{
dbg_app_fn_enter( __PRETTY_FUNCTION__ );

	CinderNativeActivity::destroyJni();
	ci::android::hardware::Camera::destroyJni();
	ci::android::net::UrlLoader::destroyJni();
	ci::android::video::VideoPlayer::destroyJni();
	
dbg_app_fn_exit( __PRETTY_FUNCTION__ );
}

CinderNativeActivity* CinderNativeActivity::getInstance()
{
	return CinderNativeActivity::sInstance.get();
}

jclass CinderNativeActivity::getJavaClass()
{
	return Java::ClassObject;
}

jobject CinderNativeActivity::getJavaObject()
{
	return CinderNativeActivity::getInstance()->mJavaObject;
}

int CinderNativeActivity::getDisplayRotation()
{
	jint result = JniHelper::Get()->CallIntMethod( getInstance()->getJavaObject(), Java::getDisplayRotation );
	return (int)result;
}

}}} // namespace cinder::android::app