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

#include "cinder/audio/Context.h"

using namespace cinder::android;

#include "cinder/ImageIo.h"

#include <fstream>

namespace cinder { namespace android { namespace app {

jclassID	CinderNativeActivity::Java::ClassName				= "org/libcinder/app/CinderNativeActivity";
jclass  	CinderNativeActivity::Java::ClassObject 			= nullptr;
jmethodID 	CinderNativeActivity::Java::getCacheDirectory		= nullptr;
jmethodID	CinderNativeActivity::Java::getPicturesDirectory 	= nullptr;
jmethodID 	CinderNativeActivity::Java::setWallpaper			= nullptr;
jmethodID 	CinderNativeActivity::Java::getDisplayRotation		= nullptr;
jmethodID 	CinderNativeActivity::Java::setKeepScreenOn			= nullptr;
jmethodID 	CinderNativeActivity::Java::setFullScreen			= nullptr;
jmethodID 	CinderNativeActivity::Java::launchWebBrowser		= nullptr;
jmethodID 	CinderNativeActivity::Java::launchTwitter			= nullptr;

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
				Java::getCacheDirectory 	= JniHelper::Get()->GetMethodId( Java::ClassObject, "getCacheDirectory", "()Ljava/lang/String;" );
				Java::getPicturesDirectory 	= JniHelper::Get()->GetMethodId( Java::ClassObject, "getPicturesDirectory", "()Ljava/lang/String;" );
				Java::setWallpaper			= JniHelper::Get()->GetMethodId( Java::ClassObject, "setWallpaper", "(Ljava/lang/String;)V" );
				Java::getDisplayRotation 	= JniHelper::Get()->GetMethodId( Java::ClassObject, "getDisplayRotation", "()I" );
				Java::setKeepScreenOn		= JniHelper::Get()->GetMethodId( Java::ClassObject, "setKeepScreenOn", "(Z)V" );
				Java::setFullScreen			= JniHelper::Get()->GetMethodId( Java::ClassObject, "setFullScreen", "(Z)V" );
				Java::launchWebBrowser		= JniHelper::Get()->GetMethodId( Java::ClassObject, "launchWebBrowser", "(Ljava/lang/String;)V" );
				Java::launchTwitter			= JniHelper::Get()->GetMethodId( Java::ClassObject, "launchTwitter", "(Ljava/lang/String;Ljava/lang/String;)V" );
				jni_obtained_check( CinderNativeActivity::Java::getCacheDirectory );
				jni_obtained_check( CinderNativeActivity::Java::getPicturesDirectory );
				jni_obtained_check( CinderNativeActivity::Java::setWallpaper );
				jni_obtained_check( CinderNativeActivity::Java::getDisplayRotation );
				jni_obtained_check( CinderNativeActivity::Java::setKeepScreenOn );
				jni_obtained_check( CinderNativeActivity::Java::setFullScreen );
				jni_obtained_check( CinderNativeActivity::Java::launchWebBrowser );
				jni_obtained_check( CinderNativeActivity::Java::launchTwitter );
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
		Java::getCacheDirectory		= nullptr;
		Java::getPicturesDirectory 	= nullptr;
		Java::setWallpaper			= nullptr;
		Java::getDisplayRotation	= nullptr;
		Java::setKeepScreenOn		= nullptr;
		Java::setFullScreen			= nullptr;
		Java::launchWebBrowser		= nullptr;
		Java::launchTwitter			= nullptr;
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
	ci::audio::Context::setMaster( nullptr, nullptr );

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

cinder::fs::path CinderNativeActivity::getCacheDirectory()
{
	cinder::fs::path result;

	auto jniEnv = JniHelper::Get()->AttachCurrentThread();
	if( jniEnv ) {
		jstring jstr = (jstring)jniEnv->CallObjectMethod( getInstance()->getJavaObject(), Java::getCacheDirectory );
		const char *c_str = jniEnv->GetStringUTFChars( jstr, nullptr );
		result = std::string( c_str );
		jniEnv->ReleaseStringUTFChars( jstr, c_str );
	}

	return result;
}

cinder::fs::path CinderNativeActivity::getPicturesDirectory()
{
	cinder::fs::path result;

	auto jniEnv = JniHelper::Get()->AttachCurrentThread();
	if( jniEnv ) {
		jstring jstr = (jstring)jniEnv->CallObjectMethod( getInstance()->getJavaObject(), Java::getPicturesDirectory );
		const char * c_str = jniEnv->GetStringUTFChars( jstr, nullptr );
		result = std::string( c_str );
		jniEnv->ReleaseStringUTFChars( jstr, c_str );
	}

	return result;
}

void CinderNativeActivity::setWallpaper( const Surface8u& surf )
{
	std::string pathStr = "";

	try {
		cinder::fs::path path = getCacheDirectory() / "setWallpaper_tmp_image.png";
		pathStr = path.string();
		cinder::writeImage( path, surf );
	}
	catch( const std::exception& e ) {
		pathStr = "";
		dbg_app_error( std::string( "setWallpaper temp image write failed: " ) + e.what() );
	}

	if( ! pathStr.empty() ) {
		jstring jstrPath = JniHelper::Get()->NewStringUTF( pathStr );
		JniHelper::Get()->CallVoidMethod( getInstance()->getJavaObject(), Java::setWallpaper, jstrPath );
		JniHelper::Get()->DeleteLocalRef( jstrPath );
	}
}

int CinderNativeActivity::getDisplayRotation()
{
	jint result = JniHelper::Get()->CallIntMethod( getInstance()->getJavaObject(), Java::getDisplayRotation );
	return (int)result;
}

void CinderNativeActivity::setKeepScreenOn( bool keepScreenOn )
{
	JniHelper::Get()->CallVoidMethod( getInstance()->getJavaObject(), Java::setKeepScreenOn, (jboolean)keepScreenOn );
}

void CinderNativeActivity::setFullScreen( bool fullScreen )
{
	JniHelper::Get()->CallVoidMethod( getInstance()->getJavaObject(), Java::setFullScreen, (jboolean)fullScreen );
}

void CinderNativeActivity::launchWebBrowser( const Url &url )
{
	if( url.str().empty() ) {
		return;
	}

	jstring jstrUrl = JniHelper::Get()->NewStringUTF( url.str() );
	JniHelper::Get()->CallVoidMethod( getInstance()->getJavaObject(), Java::launchWebBrowser, jstrUrl );
	JniHelper::Get()->DeleteLocalRef( jstrUrl );
}

void CinderNativeActivity::launchTwitter( const std::string& text, const Surface8u* surf )
{
	std::string pathStr = "";
	if( nullptr != surf ) {
		try {
			cinder::fs::path path = getCacheDirectory() / "launchTwitter_tmp_image.png";
			pathStr = path.string();
			cinder::writeImage( path, *surf );
		}
		catch( const std::exception& e ) {
			dbg_app_error( std::string( "launchTwitter temp image write failed: " ) + e.what() );
		}
	}

	jstring jstrText = JniHelper::Get()->NewStringUTF( text );
	jstring jstrPath = JniHelper::Get()->NewStringUTF( pathStr );
	JniHelper::Get()->CallVoidMethod( getInstance()->getJavaObject(), Java::launchTwitter, jstrText, jstrPath );
	JniHelper::Get()->DeleteLocalRef( jstrText );
	JniHelper::Get()->DeleteLocalRef( jstrPath );
}


}}} // namespace cinder::android::app