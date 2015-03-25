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

#include "cinder/android/JniHelper.h"

#include "cinder/app/App.h"

#include <thread>
#include <sstream>
#include <iomanip>

#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "cinder", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "cinder", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"cinder", __VA_ARGS__))

namespace cinder { namespace android { 

// Thread specific JNIEnv
static thread_local JNIEnv*  threadJniEnv = nullptr;
static thread_local uint32_t threadAttachCount = 0;

// Static instance of JniHelper
std::unique_ptr<JniHelper> JniHelper::sInstance;

JniHelper::JniHelper( ANativeActivity* nativeActivity )
	: mJavaVm( nativeActivity->vm ),
	  mActivityObject( nativeActivity->clazz )
{
	if( AttachCurrentThread() ) {
		// Get android.app.NativeActivity
		jclass activityClass = threadJniEnv->FindClass( "android/app/NativeActivity" );
		mActivityClass = (jclass)threadJniEnv->NewGlobalRef( activityClass );			

		// Get getClassLoader in android.app.NativeActivity
		mGetClassLoaderMethodId	= threadJniEnv->GetMethodID( mActivityClass, "getClassLoader", "()Ljava/lang/ClassLoader;" );

		// Call android.app.NativeActivity.getClassLoader to get ClassLoader object
		jobject classLoaderObject = threadJniEnv->CallObjectMethod( mActivityObject, mGetClassLoaderMethodId );
		mClassLoaderObject = threadJniEnv->NewGlobalRef( classLoaderObject );

		// Get java.lang.ClassLoader
		jclass classLoaderClass = threadJniEnv->FindClass( "java/lang/ClassLoader" );
		mClassLoaderClass = (jclass)threadJniEnv->NewGlobalRef( classLoaderClass );

		// Get loadClass in java.lang.ClassLoader
		mLoadClassMethodId = threadJniEnv->GetMethodID( mClassLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;" );

		DeatchCurrentThread();
	}
}

JniHelper::~JniHelper()
{
	if( AttachCurrentThread() ) {
		threadJniEnv->DeleteGlobalRef( mActivityClass );
		threadJniEnv->DeleteGlobalRef( mClassLoaderObject );
		threadJniEnv->DeleteGlobalRef( mClassLoaderClass );
		mActivityClass = NULL;
		mClassLoaderObject = NULL;
		mClassLoaderClass = NULL;

		DeatchCurrentThread();
	}
}

void JniHelper::initialize( ANativeActivity* nativeActivity )
{
	if( JniHelper::sInstance ) {
		return;
	}

	JniHelper::sInstance = std::unique_ptr<JniHelper>( new JniHelper( nativeActivity ) );
}

void JniHelper::destroy()
{

}

JniHelper* JniHelper::get()
{
	return JniHelper::sInstance.get();
}

jclass JniHelper::retrieveClass( const std::string& name )
{
	jclass result = NULL;

	if( AttachCurrentThread() ) {
		jstring jstrClassName = NewStringUTF( name.c_str() );

		result = (jclass)CallObjectMethod( mClassLoaderObject, mLoadClassMethodId, jstrClassName );
		// if( NULL != result ) {
		// 	std::stringstream ss;
		// 	ss << "retrieveClass - class found: " << name;
		// 	LOGI( ss.str().c_str() );
		// }

		DeleteLocalRef( jstrClassName );
		DeatchCurrentThread();
	}

	return result;
}

bool JniHelper::AttachCurrentThread()
{
	if( nullptr == threadJniEnv ) {
		if( JNI_OK != mJavaVm->AttachCurrentThread( &threadJniEnv, NULL ) ) {
			threadJniEnv = nullptr;
		}
		// else {
		// 	std::stringstream ss;
		// 	ss << "CINDER-JNI::AttachCurrentThread - threadJniEnv: 0x" << std::hex << (uint32_t)threadJniEnv;
		// 	LOGI( ss.str().c_str() );
		// }
	}

	if( nullptr != threadJniEnv ) {
		++threadAttachCount;
	}

	return (nullptr != threadJniEnv);
}

void JniHelper::DeatchCurrentThread()
{
	if( threadAttachCount > 0 ) {
		--threadAttachCount;
	}

	if((0 == threadAttachCount) && (nullptr != threadJniEnv)) {
		threadJniEnv = nullptr;
		mJavaVm->DetachCurrentThread();
		//LOGI( "CINDER-JNI::DeatchCurrentThread" );
	}
}

jclass JniHelper::FindClass( const std::string& name )
{
	jclass result = NULL;

	if( AttachCurrentThread() ) {
		result = threadJniEnv->FindClass( name.c_str() );
		DeatchCurrentThread();
	}

	return result;
}

jmethodID JniHelper::GetStaticMethodId( jclass clazz, const std::string& name, const std::string& sig )
{
	jmethodID result = NULL;

	if( AttachCurrentThread() ) {
		result = threadJniEnv->GetStaticMethodID( clazz, name.c_str(), sig.c_str() );
		DeatchCurrentThread();
	}

	return result;
}

jmethodID JniHelper::GetMethodId( jclass clazz, const std::string& name, const std::string& sig )
{
	jmethodID result = NULL;

	if( AttachCurrentThread()) {
		result = threadJniEnv->GetMethodID( clazz, name.c_str(), sig.c_str() );
		DeatchCurrentThread();
	}

	return result;
}

#define CI_CALL_TYPE_METHOD_IMPL( _jtype, _jname, _jdefval )					\
_jtype JniHelper::Call##_jname##Method( jobject obj, jmethodID methodId, ... )	\
{																				\
	_jtype result = _jdefval;													\
	if( AttachCurrentThread() ) {												\
		va_list args;															\
		va_start( args, methodId );												\
		result = threadJniEnv->Call##_jname##MethodV( obj, methodId, args );	\
		va_end( args );															\
		DeatchCurrentThread();													\
	}																			\
	return result;																\
}

CI_CALL_TYPE_METHOD_IMPL( jobject, Object, NULL )
CI_CALL_TYPE_METHOD_IMPL( jboolean, Boolean, 0 )
CI_CALL_TYPE_METHOD_IMPL( jbyte, Byte, 0 )
CI_CALL_TYPE_METHOD_IMPL( jchar, Char, 0 )
CI_CALL_TYPE_METHOD_IMPL( jshort, Short, 0 )
CI_CALL_TYPE_METHOD_IMPL( jint, Int, 0 )
CI_CALL_TYPE_METHOD_IMPL( jlong, Long, 0 )
CI_CALL_TYPE_METHOD_IMPL( jfloat, Float, 0.0f )
CI_CALL_TYPE_METHOD_IMPL( jdouble, Double, 0.0 )
#undef CI_CALL_TYPE_METHOD_IMPL	

void JniHelper::CallVoidMethod( jobject obj, jmethodID methodId, ... ) 
{
	if( AttachCurrentThread() ) {
		va_list args;
		va_start( args, methodId );
		threadJniEnv->CallVoidMethodV( obj, methodId, args );
		va_end( args );
		DeatchCurrentThread();
	}
}

#define CI_CALL_STATIC_TYPE_METHOD_IMPL( _jtype, _jname, _jdefval )						\
_jtype JniHelper::CallStatic##_jname##Method( jclass clazz, jmethodID methodId, ... )	\
{																						\
	_jtype result = _jdefval;															\
	if( AttachCurrentThread() ) {														\
		va_list args;																	\
		va_start( args, methodId );														\
		result = threadJniEnv->CallStatic##_jname##MethodV( clazz, methodId, args );	\
		va_end( args );																	\
		DeatchCurrentThread();															\
	}																					\
	return result;																		\
}

CI_CALL_STATIC_TYPE_METHOD_IMPL( jobject, Object, NULL )
CI_CALL_STATIC_TYPE_METHOD_IMPL( jboolean, Boolean, 0 )
CI_CALL_STATIC_TYPE_METHOD_IMPL( jbyte, Byte, 0 )
CI_CALL_STATIC_TYPE_METHOD_IMPL( jchar, Char, 0 )
CI_CALL_STATIC_TYPE_METHOD_IMPL( jshort, Short, 0 )
CI_CALL_STATIC_TYPE_METHOD_IMPL( jint, Int, 0 )
CI_CALL_STATIC_TYPE_METHOD_IMPL( jlong, Long, 0 )
CI_CALL_STATIC_TYPE_METHOD_IMPL( jfloat, Float, 0.0f )
CI_CALL_STATIC_TYPE_METHOD_IMPL( jdouble, Double, 0.0 )
#undef CI_CALL_STATIC_TYPE_METHOD_IMPL

void JniHelper::CallStaticVoidMethod( jclass clazz, jmethodID methodId, ... ) 
{
	if( AttachCurrentThread() ) {
		va_list args;
		va_start( args, methodId );
		threadJniEnv->CallStaticVoidMethodV( clazz, methodId, args );
		va_end( args );
		DeatchCurrentThread();
	}
}

jobject JniHelper::NewGlobalRef( jobject obj )
{
	jobject result = NULL;
	
	if( AttachCurrentThread() ) {
		result = threadJniEnv->NewGlobalRef( obj );
		DeatchCurrentThread();
	}

	return result;
}

void JniHelper::DeleteGlobalRef( jobject globalRef )
{
	if( AttachCurrentThread() ) {
		threadJniEnv->DeleteGlobalRef( globalRef );
		DeatchCurrentThread();
	}
}

jstring JniHelper::NewStringUTF( const std::string& str )
{
	jstring result = NULL;

	if( AttachCurrentThread() ) {
		result = threadJniEnv->NewStringUTF( str.c_str() );
		DeatchCurrentThread();
	}

	return result;
}

void JniHelper::DeleteLocalRef( jobject localRef )
{
	if( AttachCurrentThread() ) {
		threadJniEnv->DeleteLocalRef( localRef );
		DeatchCurrentThread();
	}
}

jsize JniHelper::GetArrayLength( jarray array )
{
	jsize result = 0;
	if( AttachCurrentThread() ) {
		result = threadJniEnv->GetArrayLength( array );
		DeatchCurrentThread();
	}
	return result;
}

jbyte* JniHelper::GetByteArrayElements( jbyteArray array, jboolean* isCopy )
{
	jbyte* result = NULL;
	if( AttachCurrentThread() ) {
		result = threadJniEnv->GetByteArrayElements( array, isCopy );
		DeatchCurrentThread();
	}
	return result;
}

void JniHelper::ReleaseByteArrayElements( jbyteArray array, jbyte* elems, jint mode )
{
	if( AttachCurrentThread() ) {
		threadJniEnv->ReleaseByteArrayElements( array, elems, mode );
		DeatchCurrentThread();
	}
}

}} // namespace cinder::android