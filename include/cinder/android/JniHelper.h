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

using jclassID = const std::string;

namespace cinder { namespace android {

class JniGlobalObject;
typedef std::shared_ptr<JniGlobalObject> JniGlobalObjectRef;

/** \class JniGlobalObject
 *
 */
class JniGlobalObject : public std::enable_shared_from_this<JniGlobalObject> {
public:
	JniGlobalObject( jobject globalRef );
	~JniGlobalObject();
	void 			deleteGlobalRef();
	jobject 		getObject() { return mJavaObject; }
	const jobject 	getObject() const { return mJavaObject; }
private:
	jobject 		mJavaObject = nullptr;
};

#define JNI_THROW_CPP_EXC( _jniEnv, _msg, _clearEx ) \
  JniHelper::ThrowAsCppExceptionFull( _jniEnv, __FILE__, __LINE__, __FUNCTION__, _msg, _clearEx )

/** \class JniHelper
 *
 */
class JniHelper {
public:

	virtual ~JniHelper();

	static void 		Initialize( ANativeActivity* nativeActivity );
	static void 		Destroy();

	static JniHelper* 	Get();

	jobject 			GetActivityObject() { return mActivityObject; }

	jclass				RetrieveClass( const std::string& name, bool clearExc = true );	

	static std::string 	StdStringFromJString( JNIEnv* jniEnv, jstring jstr );
	static void 		ThrowAsCppException( JNIEnv* jniEnv, const std::string& msg, bool clearExc = true );
	static void 		ThrowAsCppExceptionFull( JNIEnv* jniEnv, const std::string& file, const int line, const std::string& fn, const std::string& msg, bool clearExc = true );

	JNIEnv*				AttachCurrentThread();
	void 				DeatchCurrentThread();

	jclass				FindClass( const std::string& name );

	// ---------------------------------------------------------------------------------------------
	// Java Static Methods
	// ---------------------------------------------------------------------------------------------
	jmethodID GetStaticMethodId( jclass clazz, const std::string& name, const std::string& sig );

	#define CI_CALL_STATIC_TYPE_METHOD_DECL( _jtype, _jname ) \
		_jtype CallStatic##_jname##Method( jclass clazz, jmethodID methodId, ... );

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


	// ---------------------------------------------------------------------------------------------
	// Java Methods
	// ---------------------------------------------------------------------------------------------
	jmethodID GetMethodId( jclass clazz, const std::string& name, const std::string& sig );

	#define CI_CALL_TYPE_METHOD_DECL( _jtype, _jname ) \
		_jtype Call##_jname##Method( jobject obj, jmethodID methodId, ... );

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

	void  CallVoidMethod( jobject obj, jmethodID methodId, ... );

	// ---------------------------------------------------------------------------------------------
	// Java Static Fields
	// ---------------------------------------------------------------------------------------------
	jfieldID GetStaticFieldId( jclass clazz, const std::string& name, const std::string& sig );
	jfieldID GetStaticObjectFieldId( jclass clazz, const std::string& name, const std::string& sig );

	#define CI_GET_STATIC_TYPE_FIELDID_DECL( _jname ) \
		jfieldID GetStatic##_jname##FieldId( jclass clazz, const std::string& name );	

		//
		// GetStaticBooleanFieldID
		// GetStaticByteFieldID
		// GetStaticCharFieldID
		// GetStaticShortFieldID
		// GetStaticIntFieldID
		// GetStaticLongFieldID
		// GetStaticFloatFieldID
		// GetStaticDoubleFieldID
		// GetStaticStringFieldID
		//	
		CI_GET_STATIC_TYPE_FIELDID_DECL( Boolean )
		CI_GET_STATIC_TYPE_FIELDID_DECL( Byte )
		CI_GET_STATIC_TYPE_FIELDID_DECL( Char )
		CI_GET_STATIC_TYPE_FIELDID_DECL( Short )
		CI_GET_STATIC_TYPE_FIELDID_DECL( Int )
		CI_GET_STATIC_TYPE_FIELDID_DECL( Long )
		CI_GET_STATIC_TYPE_FIELDID_DECL( Float )
		CI_GET_STATIC_TYPE_FIELDID_DECL( Double )
		CI_GET_STATIC_TYPE_FIELDID_DECL( String )
	#undef CI_GET_STATIC_TYPE_FIELDID_DECL	

	#define CI_GET_STATIC_TYPE_FIELD_DECL( _jtype, _jname ) \
		_jtype GetStatic##_jname##Field( jclass clazz, jfieldID fieldId );

		//
		// GetStaticBooleanField
		// GetStaticByteField
		// GetStaticCharField
		// GetStaticShortField
		// GetStaticIntField
		// GetStaticLongField
		// GetStaticFloatField
		// GetStaticDoubleField
		//	
		CI_GET_STATIC_TYPE_FIELD_DECL( jobject, Object )
		CI_GET_STATIC_TYPE_FIELD_DECL( jboolean, Boolean )
		CI_GET_STATIC_TYPE_FIELD_DECL( jbyte, Byte )
		CI_GET_STATIC_TYPE_FIELD_DECL( jchar, Char )
		CI_GET_STATIC_TYPE_FIELD_DECL( jshort, Short )
		CI_GET_STATIC_TYPE_FIELD_DECL( jint, Int )
		CI_GET_STATIC_TYPE_FIELD_DECL( jlong, Long )
		CI_GET_STATIC_TYPE_FIELD_DECL( jfloat, Float )
		CI_GET_STATIC_TYPE_FIELD_DECL( jdouble, Double )
	#undef CI_GET_STATIC_TYPE_FIELD_DECL

	std::string GetStaticStringField( jclass clazz, jfieldID fieldId );

	#define CI_SET_STATIC_TYPE_FIELD_DECL( _jtype, _jname ) \
		void SetStatic##_jname##Field( jclass clazz, jfieldID fieldId, _jtype value );

		//
		// SetStaticBooleanField
		// SetStaticByteField
		// SetStaticCharField
		// SetStaticShortField
		// SetStaticIntField
		// SetStaticLongField
		// SetStaticFloatField
		// SetStaticDoubleField
		//	
		CI_SET_STATIC_TYPE_FIELD_DECL( jobject, Object )
		CI_SET_STATIC_TYPE_FIELD_DECL( jboolean, Boolean )
		CI_SET_STATIC_TYPE_FIELD_DECL( jbyte, Byte )
		CI_SET_STATIC_TYPE_FIELD_DECL( jchar, Char )
		CI_SET_STATIC_TYPE_FIELD_DECL( jshort, Short )
		CI_SET_STATIC_TYPE_FIELD_DECL( jint, Int )
		CI_SET_STATIC_TYPE_FIELD_DECL( jlong, Long )
		CI_SET_STATIC_TYPE_FIELD_DECL( jfloat, Float )
		CI_SET_STATIC_TYPE_FIELD_DECL( jdouble, Double )
	#undef CI_SET_STATIC_TYPE_FIELD_DECL

	void SetStaticStringField( jclass clazz, jfieldID fieldId, const std::string& value );	

	// ---------------------------------------------------------------------------------------------
	// Java Fields
	// ---------------------------------------------------------------------------------------------
	jfieldID GetFieldId( jclass clazz, const std::string& name, const std::string& sig );
	jfieldID GetObjectFieldId( jclass clazz, const std::string& name, const std::string& sig );

	#define CI_GET_TYPE_FIELDID_DECL( _jname ) \
		jfieldID Get##_jname##FieldId( jclass clazz, const std::string& name );	

		//
		// GetBooleanFieldID
		// GetByteFieldID
		// GetCharFieldID
		// GetShortFieldID
		// GetIntFieldID
		// GetLongFieldID
		// GetFloatFieldID
		// GetDoubleFieldID
		// GetStringFieldID
		//	
		CI_GET_TYPE_FIELDID_DECL( Boolean )
		CI_GET_TYPE_FIELDID_DECL( Byte )
		CI_GET_TYPE_FIELDID_DECL( Char )
		CI_GET_TYPE_FIELDID_DECL( Short )
		CI_GET_TYPE_FIELDID_DECL( Int )
		CI_GET_TYPE_FIELDID_DECL( Long )
		CI_GET_TYPE_FIELDID_DECL( Float )
		CI_GET_TYPE_FIELDID_DECL( Double )
		CI_GET_TYPE_FIELDID_DECL( String )
	#undef CI_GET_TYPE_FIELDID_DECL	

	#define CI_GET_TYPE_FIELD_DECL( _jtype, _jname ) \
		_jtype Get##_jname##Field( jobject obj, jfieldID fieldId );

		//
		// GetObjectField
		// GetBooleanField
		// GetByteField
		// GetCharField
		// GetShortField
		// GetIntField
		// GetLongField
		// GetFloatField
		// GetDoubleField
		//	
		CI_GET_TYPE_FIELD_DECL( jobject, Object )
		CI_GET_TYPE_FIELD_DECL( jboolean, Boolean )
		CI_GET_TYPE_FIELD_DECL( jbyte, Byte )
		CI_GET_TYPE_FIELD_DECL( jchar, Char )
		CI_GET_TYPE_FIELD_DECL( jshort, Short )
		CI_GET_TYPE_FIELD_DECL( jint, Int )
		CI_GET_TYPE_FIELD_DECL( jlong, Long )
		CI_GET_TYPE_FIELD_DECL( jfloat, Float )
		CI_GET_TYPE_FIELD_DECL( jdouble, Double )
	#undef CI_GET_TYPE_FIELD_DECL

	std::string GetStringField( jobject obj, jfieldID fieldId );

	#define CI_SET_TYPE_FIELD_DECL( _jtype, _jname ) \
		void Set##_jname##Field( jobject obj, jfieldID fieldId, _jtype value );

		//
		// SetObjectField
		// SetBooleanField
		// SetByteField
		// SetCharField
		// SetShortField
		// SetIntField
		// SetLongField
		// SetFloatField
		// SetDoubleField
		//	
		CI_SET_TYPE_FIELD_DECL( jobject, Object )
		CI_SET_TYPE_FIELD_DECL( jboolean, Boolean )
		CI_SET_TYPE_FIELD_DECL( jbyte, Byte )
		CI_SET_TYPE_FIELD_DECL( jchar, Char )
		CI_SET_TYPE_FIELD_DECL( jshort, Short )
		CI_SET_TYPE_FIELD_DECL( jint, Int )
		CI_SET_TYPE_FIELD_DECL( jlong, Long )
		CI_SET_TYPE_FIELD_DECL( jfloat, Float )
		CI_SET_TYPE_FIELD_DECL( jdouble, Double )
	#undef CI_SET_TYPE_FIELD_DECL

	void SetStringField( jobject obj, jfieldID fieldId, const std::string& value );	


	jobject				NewGlobalRef( jobject obj );
	void 				DeleteGlobalRef( jobject globalRef );

	JniGlobalObjectRef	TrackedNewGlobalRef( jobject obj );
	void 				TrackedDeleteGlobalRef( const JniGlobalObjectRef& globalRef );

	jstring 			NewStringUTF( const std::string& str );	
	void 				DeleteLocalRef( jobject localRef );

	jsize 				GetArrayLength( jarray array );

	jobject				GetObjectArrayElements( jobjectArray array, jsize index );

	jbyte* 				GetByteArrayElements( jbyteArray array, jboolean* isCopy );
	jint* 				GetIntArrayElements( jintArray array, jboolean* isCopy );

	void 				ReleaseByteArrayElements( jbyteArray array, jbyte* elems, jint mode );
	void 				ReleaseIntArrayElements( jintArray array, jint* elems, jint mode );

	jthrowable 			ExceptionOccurred();
	void 				ExceptionDescribe();
	void 				ExceptionClear();
	jboolean 			ExceptionCheck();

	/*
	std::string 		ThrowableGetMessage( jthrowable jexc, bool clearExc = true );
	std::string 		ThrowableToString( jthrowable jexc, bool clearExc = true );
	std::string			GetPendingExceptionMessage( bool clearExc = true );
	std::string			GetPendingExceptionString( bool clearExc = true );
	*/

private:	
	static std::unique_ptr<JniHelper>	sInstance;

	JniHelper( ANativeActivity* nativeActivity );

	jobject			mActivityObject 		= nullptr;
	jclass 			mActivityClass 			= nullptr;

	jmethodID		mGetClassLoaderMethodId	= nullptr;
	jobject 		mClassLoaderObject		= nullptr;
	jclass 			mClassLoaderClass		= nullptr;
	jmethodID		mLoadClassMethodId		= nullptr;

	/*
	jclass 			mThrowableClass 				= nullptr;
	jmethodID 		mThrowableGetMessageMethodId	= nullptr;
	jmethodID		mThrowableToStringMethodId 		= nullptr;
	*/
};	

}} // namespace cinder::android