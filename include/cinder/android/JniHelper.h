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

#include <android/native_activity.h>
#include <jni.h>
#include <memory>
#include <string>

namespace cinder { namespace android {

class JniHelper {
public:

	virtual ~JniHelper();

	static void 		Initialize( ANativeActivity* nativeActivity );
	static void 		Destroy();

	static JniHelper* 	Get();

	jclass				RetrieveClass( const std::string& name );	

	JNIEnv*				AttachCurrentThread();
	void 				DeatchCurrentThread();

	jclass				FindClass( const std::string& name );
	jmethodID			GetStaticMethodId( jclass clazz, const std::string& name, const std::string& sig );
	jmethodID			GetMethodId( jclass clazz, const std::string& name, const std::string& sig );


//
// CallObjectMethod
// CallBooleanMethod
// CallByteMethod
// CallCharMethod
// CallShortMethod
// CallIntMethod
// CallLongMethod
// CallFloatMethod
// CallDoubleMethod
//	
#define CI_CALL_TYPE_METHOD_DECL( _jtype, _jname ) \
	_jtype Call##_jname##Method( jobject obj, jmethodID methodId, ... );

	CI_CALL_TYPE_METHOD_DECL( jobject, Object )
	CI_CALL_TYPE_METHOD_DECL( jboolean, Boolean )
	CI_CALL_TYPE_METHOD_DECL( jbyte, Byte )
	CI_CALL_TYPE_METHOD_DECL( jchar, Char )
	CI_CALL_TYPE_METHOD_DECL( jshort, Short )
	CI_CALL_TYPE_METHOD_DECL( jint, Int )
	CI_CALL_TYPE_METHOD_DECL( jlong, Long )
	CI_CALL_TYPE_METHOD_DECL( jfloat, Float )
	CI_CALL_TYPE_METHOD_DECL( jdouble, Double )
#undef CI_CALL_TYPE_METHOD	

	void CallVoidMethod( jobject obj, jmethodID methodId, ... );

//
// CallStaticObjectMethod
// CallStaticBooleanMethod
// CallStaticByteMethod
// CallStaticCharMethod
// CallStaticShortMethod
// CallStaticIntMethod
// CallStaticLongMethod
// CallStaticFloatMethod
// CallStaticDoubleMethod
//	
#define CI_CALL_STATIC_TYPE_METHOD_DECL( _jtype, _jname ) \
	_jtype CallStatic##_jname##Method( jclass clazz, jmethodID methodId, ... );

	CI_CALL_STATIC_TYPE_METHOD_DECL( jobject, Object )
	CI_CALL_STATIC_TYPE_METHOD_DECL( jboolean, Boolean )
	CI_CALL_STATIC_TYPE_METHOD_DECL( jbyte, Byte )
	CI_CALL_STATIC_TYPE_METHOD_DECL( jchar, Char )
	CI_CALL_STATIC_TYPE_METHOD_DECL( jshort, Short )
	CI_CALL_STATIC_TYPE_METHOD_DECL( jint, Int )
	CI_CALL_STATIC_TYPE_METHOD_DECL( jlong, Long )
	CI_CALL_STATIC_TYPE_METHOD_DECL( jfloat, Float )
	CI_CALL_STATIC_TYPE_METHOD_DECL( jdouble, Double )
#undef CI_CALL_STATIC_TYPE_METHOD_DECL	

	void CallStaticVoidMethod( jclass clazz, jmethodID methodId, ... );

	jobject 	NewGlobalRef( jobject obj );
	void 		DeleteGlobalRef( jobject globalRef );

	jstring 	NewStringUTF( const std::string& str );	
	void 		DeleteLocalRef( jobject localRef );

	jsize 		GetArrayLength( jarray array );
	jbyte* 		GetByteArrayElements( jbyteArray array, jboolean* isCopy );

	void 		ReleaseByteArrayElements( jbyteArray array, jbyte* elems, jint mode );

private:	
	static std::unique_ptr<JniHelper>	sInstance;

	JniHelper( ANativeActivity* nativeActivity );

	//JavaVM*		mJavaVm;
	//JNIEnv*		mJniEnv;
	jobject			mActivityObject 		= NULL;
	jclass 			mActivityClass 			= NULL;

	jmethodID		mGetClassLoaderMethodId	= NULL;
	jobject 		mClassLoaderObject		= NULL;
	jclass 			mClassLoaderClass		= NULL;
	jmethodID		mLoadClassMethodId		= NULL;

};	

}} // namespace cinder::android