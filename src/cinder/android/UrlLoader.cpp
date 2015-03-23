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

#include "cinder/android/UrlLoader.h"

#include <sstream>
#include <iomanip>

#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "cinder", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "cinder", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"cinder", __VA_ARGS__))

namespace cinder { namespace android {

jclass		UrlLoader::sUrlLoaderClass  = NULL;
jmethodID	UrlLoader::sLoadUrlMethodId = NULL;

UrlLoader::UrlLoader()
{

}

UrlLoader::UrlLoader( const std::string& url )
	: mUrl( url )
{

}

UrlLoader::~UrlLoader()
{

}

void UrlLoader::cacheJni()
{
	if( JniHelper::get()->AttachCurrentThread() ) {
		jclass urlLoaderClass = JniHelper::get()->retrieveClass( "org/libcinder/net/UrlLoader" );	
		UrlLoader::sUrlLoaderClass = (jclass)JniHelper::get()->NewGlobalRef( urlLoaderClass );

		if( NULL != sUrlLoaderClass ) {
			UrlLoader::sLoadUrlMethodId = JniHelper::get()->GetStaticMethodId( UrlLoader::sUrlLoaderClass, "loadUrl", "(Ljava/lang/String;)[B" );
		}
		JniHelper::get()->DeatchCurrentThread();
	}
}

void UrlLoader::destroyJni()
{
	if( JniHelper::get()->AttachCurrentThread() ) {
		JniHelper::get()->DeleteGlobalRef( UrlLoader::sUrlLoaderClass  );

		UrlLoader::sUrlLoaderClass = NULL;
		UrlLoader::sLoadUrlMethodId = NULL;

		JniHelper::get()->DeatchCurrentThread();
	}
}

ci::Buffer UrlLoader::getData()
{
	ci::Buffer result;

	if( JniHelper::get()->AttachCurrentThread() ) {
		jstring jstrUrl = JniHelper::get()->NewStringUTF( mUrl );

		jbyteArray jBytes = (jbyteArray)JniHelper::get()->CallStaticObjectMethod( UrlLoader::sUrlLoaderClass, UrlLoader::sLoadUrlMethodId, jstrUrl );
		if( NULL == jBytes ) {
		    throw std::runtime_error( "loadUrl failed" );
		}

		size_t dataLength = JniHelper::get()->GetArrayLength( jBytes );
		jbyte* dataPtr = (jbyte*)JniHelper::get()->GetByteArrayElements( jBytes, NULL );
		if( NULL == dataPtr ) {
			throw std::runtime_error( "unable to get bytearray" );
		}

		if( dataLength > 0 ) {
			result = ci::Buffer( dataLength );
			memcpy( (void *)result.getData(), (const void *)dataPtr, dataLength );
		}

		JniHelper::get()->ReleaseByteArrayElements( jBytes, dataPtr, 0 );
		JniHelper::get()->DeleteLocalRef( jstrUrl );
		JniHelper::get()->DeatchCurrentThread();
	}

	return result;
}

}} // namespace cinder::android