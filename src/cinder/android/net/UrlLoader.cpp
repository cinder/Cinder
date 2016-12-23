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

#include "cinder/android/net/UrlLoader.h"
#include "cinder/android/AndroidDevLog.h" 
#include <sstream>

namespace cinder { namespace android { namespace net {

jclassID	UrlLoader::sJavaClassName			= "org/libcinder/net/UrlLoader";
jclass 		UrlLoader::sJavaClass 				= nullptr;
jmethodID	UrlLoader::sJavaStaticMethodCreate	= nullptr;
jmethodID	UrlLoader::sJavaMethodLoadUrl 		= nullptr;
jfieldID 	UrlLoader::sJavaFieldResponseCode	= nullptr;
jfieldID 	UrlLoader::sJavaFieldResponseMsg	= nullptr;
jfieldID 	UrlLoader::sJavaFieldExceptionMsg	= nullptr;

UrlLoader::UrlLoader()
{
	initialize();
}

UrlLoader::~UrlLoader()
{
	if( JniHelper::Get()->AttachCurrentThread() ) {
		JniHelper::Get()->DeleteGlobalRef( mJavaObject );
	}
}

void UrlLoader::cacheJni()
{
	if( JniHelper::Get()->AttachCurrentThread() ) {

		try {
			jclass javaClass = JniHelper::Get()->RetrieveClass( UrlLoader::sJavaClassName );
			if( nullptr != javaClass ) {
				UrlLoader::sJavaClass = reinterpret_cast<jclass>( JniHelper::Get()->NewGlobalRef( javaClass ) );

				jni_obtained_check( UrlLoader::sJavaClass );
			}

			if( nullptr != UrlLoader::sJavaClass ) {
				UrlLoader::sJavaStaticMethodCreate	  = JniHelper::Get()->GetStaticMethodId( UrlLoader::sJavaClass, "create", "()Lorg/libcinder/net/UrlLoader;" );
				jni_obtained_check( UrlLoader::sJavaStaticMethodCreate );

				UrlLoader::sJavaMethodLoadUrl 		  = JniHelper::Get()->GetMethodId( UrlLoader::sJavaClass, "loadUrl", "(Ljava/lang/String;)[B" );
				jni_obtained_check( UrlLoader::sJavaMethodLoadUrl );

				UrlLoader::sJavaFieldResponseCode = JniHelper::Get()->GetIntFieldId( UrlLoader::sJavaClass, "mResponseCode" );
				UrlLoader::sJavaFieldResponseMsg  = JniHelper::Get()->GetStringFieldId( UrlLoader::sJavaClass, "mResponseMsg" );
				UrlLoader::sJavaFieldExceptionMsg = JniHelper::Get()->GetStringFieldId( UrlLoader::sJavaClass, "mExceptionMsg" );
				jni_obtained_check( UrlLoader::sJavaFieldResponseCode );
				jni_obtained_check( UrlLoader::sJavaFieldResponseMsg );
				jni_obtained_check( UrlLoader::sJavaFieldExceptionMsg );
			}
		}
		catch( const std::exception& e ) {
			dbg_app_error( e.what() );
		}		
	}
}

void UrlLoader::destroyJni()
{
	if( JniHelper::Get()->AttachCurrentThread() ) {	
		JniHelper::Get()->DeleteGlobalRef( UrlLoader::sJavaClass  );
		UrlLoader::sJavaClass 				= nullptr;
		UrlLoader::sJavaStaticMethodCreate	= nullptr;
		UrlLoader::sJavaMethodLoadUrl 		= nullptr;
		UrlLoader::sJavaFieldResponseCode	= nullptr;
		UrlLoader::sJavaFieldResponseMsg	= nullptr;
		UrlLoader::sJavaFieldExceptionMsg	= nullptr;
	}
}

void UrlLoader::initialize()
{
	if( JniHelper::Get()->AttachCurrentThread() ) {
		jobject javaObject = JniHelper::Get()->CallStaticObjectMethod( UrlLoader::sJavaClass, UrlLoader::sJavaStaticMethodCreate );
		if( nullptr != javaObject ) {
			mJavaObject = reinterpret_cast<jobject>( JniHelper::Get()->NewGlobalRef( javaObject ) );
			jni_obtained_check( UrlLoader::mJavaObject );
		}		
	}
}

ci::BufferRef UrlLoader::loadUrl( const std::string& url )
{
	ci::BufferRef result;

	mUrl = url;

	if( JniHelper::Get()->AttachCurrentThread() ) {
		jstring jstrUrl = JniHelper::Get()->NewStringUTF( mUrl );

		jbyteArray jBytes = (jbyteArray)JniHelper::Get()->CallObjectMethod( mJavaObject, UrlLoader::sJavaMethodLoadUrl, jstrUrl );
		if( nullptr == jBytes ) {
			int resCode = getResponseCode();
			std::string resMsg = getResponseMsg();
			std::string excMsg = getExceptionMsg();
			std::stringstream ss;
			ss << "UrlLoader.loadUrl returned null (Response Code: " << resCode << ", Response Msg: " << resMsg << ", Exception Msg: " << excMsg << ")";
		    throw std::runtime_error( ss.str() );
		}

		size_t dataLength = JniHelper::Get()->GetArrayLength( jBytes );
		jbyte* dataPtr = (jbyte*)JniHelper::Get()->GetByteArrayElements( jBytes, NULL );
		if( nullptr == dataPtr ) {
			int resCode = getResponseCode();
			std::string resMsg = getResponseMsg();
			std::string excMsg = getExceptionMsg();
			std::stringstream ss;
			ss << "failed to get byte array (Response Code: " << resCode << ", Response Msg: " << resMsg << ", Exception Msg: " << excMsg << ")";
		    throw std::runtime_error( ss.str() );
		}

		if( dataLength > 0 ) {
			result = ci::Buffer::create( dataLength );
			memcpy( (void *)result->getData(), (const void *)dataPtr, dataLength );
		}

		JniHelper::Get()->ReleaseByteArrayElements( jBytes, dataPtr, 0 );
		JniHelper::Get()->DeleteLocalRef( jstrUrl );
	}

	return result;
}

int UrlLoader::getResponseCode() const
{
	return JniHelper::Get()->GetIntField( mJavaObject, UrlLoader::sJavaFieldResponseCode );
}

std::string UrlLoader::getResponseMsg() const
{
	return JniHelper::Get()->GetStringField( mJavaObject, UrlLoader::sJavaFieldResponseMsg );
}

std::string UrlLoader::getExceptionMsg() const
{
	return JniHelper::Get()->GetStringField( mJavaObject, UrlLoader::sJavaFieldExceptionMsg );
}

}}} // namespace cinder::android::net