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

#include <algorithm>
#include <sstream>

#include "cinder/android/AndroidDevLog.h"


namespace cinder { namespace android { 

/** \class JniGlobalObject
 *
 */
JniGlobalObject::JniGlobalObject( jobject globalRef )
	: mJavaObject( globalRef )
{
}

JniGlobalObject::~JniGlobalObject()
{
	if( nullptr != mJavaObject ) {
		deleteGlobalRef();
	}
}

void JniGlobalObject::deleteGlobalRef()
{
	if( nullptr != mJavaObject ) {
		JniHelper::Get()->DeleteGlobalRef( mJavaObject );
		mJavaObject = nullptr;
	}
}

/** \class JniThreadVars
 *
 */
class JniThreadVars {
public:

	JniThreadVars() {

	}

	virtual ~JniThreadVars() {
		deleteAllGlobalRefs();
	}

	void trackGlobalRef( const JniGlobalObjectRef& ref ) {
		if( ref ) {
			auto iter = std::find( mGlobalRefs.begin(), mGlobalRefs.end(), ref );
			if( mGlobalRefs.end() == iter ) {
				mGlobalRefs.push_back( ref );
			}
		}
	}

	void removeGlobalRef( const JniGlobalObjectRef& ref ) {
		if( mGlobalRefs.end() != std::find( mGlobalRefs.begin(), mGlobalRefs.end(), ref ) ) {
			mGlobalRefs.erase( 
				std::remove( mGlobalRefs.begin(), mGlobalRefs.end(), ref ), 
				mGlobalRefs.end()
			);
		}
	}

	void deleteAllGlobalRefs() {
		if( JniHelper::Get()->AttachCurrentThread() ) {
			for( JniGlobalObjectRef objRef : mGlobalRefs ) {
				if( objRef ) {
					objRef->deleteGlobalRef();
				}

				/*
				jobject jobj = obj->getObject();
				if( nullptr != jobj ) {
					JniHelper::Get()->DeleteGlobalRef( jobj );
				}
				*/
			}
		}
		mGlobalRefs.clear();
	}

private:
	std::vector<JniGlobalObjectRef>	mGlobalRefs;
};

// Currently, the GCC implementation of thread_local does not handle
// non-trivial destructors. This is a work around until a newer version
// of GCC is available that handles non-trivial destructors.
static pthread_key_t 	sThreadExitKey;
static pthread_once_t	sThreadExitOnceInit = PTHREAD_ONCE_INIT;
static JavaVM*			sJavaVm = nullptr;

static void JvmHelper_ThreadExit( void* block )
{
//dbg_app_log("JVMAttach_ThreadExit");

	if( nullptr != block ) {
		JniThreadVars* threadVars = reinterpret_cast<JniThreadVars*>( block );
		delete threadVars;
		dbg_app_log("JVMAttach_ThreadExit -> deleted JNI threadVars (via pthread_key_create destructor)");		

		if( nullptr != sJavaVm ) {
			sJavaVm->DetachCurrentThread();
			dbg_app_log("JVMAttach_ThreadExit -> JavaVM::DetachCurrentThread (via pthread_key_create destructor)");
		}
		pthread_setspecific( sThreadExitKey, nullptr );
	}
}

static void JvmHelper_MakeKey()
{
//dbg_app_log("JVMAttach_MakeKey");

	pthread_key_create( &sThreadExitKey, JvmHelper_ThreadExit );
}

static void JvmHelper_InitOnce( JavaVM* jvm )
{
//dbg_app_log("JVMAttach_InitOnce");

	pthread_once( &sThreadExitOnceInit, JvmHelper_MakeKey );
	sJavaVm = jvm;
}

static JNIEnv* JvmHelper_Attach()
{
	JNIEnv* jniEnv = nullptr;
	if( nullptr != sJavaVm ) {
		if( JNI_OK != sJavaVm->AttachCurrentThread( &jniEnv, nullptr ) ) {
			jniEnv = nullptr;
		}
	}

	void* value = pthread_getspecific( sThreadExitKey );
	if( nullptr == value ) {
		JniThreadVars* threadVars = new JniThreadVars();
		if( 0 == pthread_setspecific( sThreadExitKey, (void*)threadVars ) ) {
			dbg_app_log( "JvmHelper_Attach -> allocated JNI threadVars (via pthread_setspecific)" );			
		}
		else {
			dbg_app_error( "vmHelper_Attach -> pthread_setspecific failed allocating JNI threadVars!" );
			delete threadVars;
		}
	}

	return jniEnv;
}

static void JvmHelper_Deatch()
{
//dbg_app_log("JvmHelper_Deatch");

	void* block = pthread_getspecific( sThreadExitKey );
	if( nullptr != block ) {
		if( nullptr != sJavaVm ) {
			sJavaVm->DetachCurrentThread();
		}
		pthread_setspecific( sThreadExitKey, nullptr );
	}
}

// Friendly name
static JNIEnv* JvmHelper_CurrentJniEnv()
{
	return JvmHelper_Attach();
}

static JniThreadVars* JvmHelper_GetCurrentThreadVars()
{
	JniThreadVars* result = nullptr;
	void* value = pthread_getspecific( sThreadExitKey );
	if( nullptr != value ) {
		result = reinterpret_cast<JniThreadVars*>( value );
	}
	return result;
}

/** \class JniHelper
 *
 */
std::unique_ptr<JniHelper> JniHelper::sInstance;

JniHelper::JniHelper( ANativeActivity* nativeActivity )
	: mActivityObject( nativeActivity->clazz )
{

	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		try{
			// ClassLoader
			{
				// Get android.app.NativeActivity
				jclass activityClass = jniEnv->FindClass( "android/app/NativeActivity" );			
				mActivityClass = (jclass)jniEnv->NewGlobalRef( activityClass );			

				// Get getClassLoader in android.app.NativeActivity
				mGetClassLoaderMethodId	= jniEnv->GetMethodID( mActivityClass, "getClassLoader", "()Ljava/lang/ClassLoader;" );

				// Call android.app.NativeActivity.getClassLoader to get ClassLoader object
				jobject classLoaderObject = jniEnv->CallObjectMethod( mActivityObject, mGetClassLoaderMethodId );
				mClassLoaderObject = jniEnv->NewGlobalRef( classLoaderObject );

				// Get java.lang.ClassLoader
				jclass classLoaderClass = jniEnv->FindClass( "java/lang/ClassLoader" );
				mClassLoaderClass = (jclass)jniEnv->NewGlobalRef( classLoaderClass );

				// Get loadClass in java.lang.ClassLoader
				mLoadClassMethodId = jniEnv->GetMethodID( mClassLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;" );
			}

			/*
			// Throwable
			{
				// Get java.lang.Throwable
				jclass throwableClass = jniEnv->FindClass( "java/lang/Throwable" );
				JniHelper::ThrowAsCppException( jniEnv, "FindClass: java/lang/Throwable" );
				mThrowableClass = (jclass)jniEnv->NewGlobalRef( throwableClass );

				// Get getMessage in java.lang.Throwable
				mThrowableGetMessageMethodId = jniEnv->GetMethodID( mClassLoaderClass, "getMessage", "()Ljava/lang/String;" );
				JniHelper::ThrowAsCppException( jniEnv, "GetMethodID: java.lang.Throwable.getMessage" );

				// Get toString in java.lang.Throwable
				mThrowableToStringMethodId = jniEnv->GetMethodID( mClassLoaderClass, "toString", "()Ljava/lang/String;" );
				JniHelper::ThrowAsCppException( jniEnv, "GetMethodID: java.lang.Throwable.toString" );
			}
			*/
		}
		catch( std::exception& e ) {
			dbg_app_error( e.what() + std::string( " (JniHelper::JniHelper)" ) );
			jniEnv->ExceptionDescribe();
		}
	}
}

JniHelper::~JniHelper()
{
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		jniEnv->DeleteGlobalRef( mActivityClass );
		jniEnv->DeleteGlobalRef( mClassLoaderObject );
		jniEnv->DeleteGlobalRef( mClassLoaderClass );
		mActivityClass = nullptr;
		mClassLoaderObject = nullptr;
		mClassLoaderClass = nullptr;

		/*
		jniEnv->DeleteGlobalRef( mThrowableClass );
		mThrowableGetMessageMethodId = nullptr;
		mThrowableToStringMethodId = nullptr;
		*/
	}
}

void JniHelper::Initialize( ANativeActivity* nativeActivity )
{
dbg_app_fn_enter( __PRETTY_FUNCTION__ );	
	if( JniHelper::sInstance ) {
		return;
	}

	JvmHelper_InitOnce( nativeActivity->vm );
	JniHelper::sInstance = std::unique_ptr<JniHelper>( new JniHelper( nativeActivity ) );
dbg_app_fn_exit( __PRETTY_FUNCTION__ );
}

void JniHelper::Destroy()
{
dbg_app_fn_enter( __PRETTY_FUNCTION__ );	
	if( JniHelper::sInstance ) {
		JniHelper::sInstance.reset();
	}
dbg_app_fn_exit( __PRETTY_FUNCTION__ );	
}

JniHelper* JniHelper::Get()
{
	return JniHelper::sInstance.get();
}

jclass JniHelper::RetrieveClass( const std::string& name, bool clearExc )
{
	jclass result = nullptr;
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		jstring jstrClassName = jniEnv->NewStringUTF( name.c_str() );
		result = reinterpret_cast<jclass>( jniEnv->CallObjectMethod( mClassLoaderObject, mLoadClassMethodId, jstrClassName ) );
		JNI_THROW_CPP_EXC( jniEnv, "RetrieveClass failed on CallObjectMethod for: " + name, clearExc );
		
		jniEnv->DeleteLocalRef( jstrClassName );
	}
	return result;
}

std::string JniHelper::StdStringFromJString( JNIEnv* jniEnv, jstring jstr )
{
	std::string result;
	if( ( nullptr != jniEnv ) && ( nullptr != jstr ) ) {		
		const char *utf = jniEnv->GetStringUTFChars( jstr, nullptr );
		jsize n = jniEnv->GetStringUTFLength( jstr );
		result = std::string( utf, n );
		jniEnv->ReleaseStringUTFChars( jstr, utf );
	}
	return result;
}

void JniHelper::ThrowAsCppException( JNIEnv* jniEnv, const std::string& msg, bool clearExc )
{
	jthrowable jexc = jniEnv->ExceptionOccurred();
	if( nullptr != jexc ) {
		if( clearExc ) {
			jniEnv->ExceptionClear();
		}
		throw std::runtime_error( "[Java Exc] " + msg );
	}
}

void JniHelper::ThrowAsCppExceptionFull( JNIEnv* jniEnv, const std::string& file, const int line, const std::string& fn, const std::string& msg, bool clearExc )
{
	jthrowable jexc = jniEnv->ExceptionOccurred();
	if( nullptr != jexc ) {
		if( clearExc ) {
			jniEnv->ExceptionClear();
		}

		// std::stringstream ss;
		// ss << "[Java Exc]" << " " << msg << " " << "(" << fn << " @ " << file << ":" << line << ")";
		// throw std::runtime_error( ss.str() );
	}
}

JNIEnv* JniHelper::AttachCurrentThread()
{
	JNIEnv* jniEnv = JvmHelper_Attach();
	return jniEnv;
}

void JniHelper::DeatchCurrentThread()
{
	JvmHelper_Deatch();
}

jclass JniHelper::FindClass( const std::string& name )
{
	jclass result = nullptr;

	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		result = jniEnv->FindClass( name.c_str() );
	}

	return result;
}

// -------------------------------------------------------------------------------------------------
// Java Static Methods
// -------------------------------------------------------------------------------------------------
jmethodID JniHelper::GetStaticMethodId( jclass clazz, const std::string& name, const std::string& sig )
{
	if( name.empty() ) {
		throw std::runtime_error( "(GetStaticMethodId) parameter 'name' is an empty string" );
	}

	jmethodID result = nullptr;

	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		result = jniEnv->GetStaticMethodID( clazz, name.c_str(), sig.c_str() );
		if( jniEnv->ExceptionCheck() ) {
			throw std::runtime_error( "(GetStaticMethodID) couldn't find matching name=" + name + ", sig=" + sig );
		}

	}

	return result;
}

#define CI_CALL_STATIC_TYPE_METHOD_IMPL( _jtype, _jname, _jdefval )						\
_jtype JniHelper::CallStatic##_jname##Method( jclass clazz, jmethodID methodId, ... )	\
{																						\
	_jtype result = _jdefval;															\
	auto jniEnv = JvmHelper_CurrentJniEnv();											\
	if( jniEnv ) {																		\
		va_list args;																	\
		va_start( args, methodId );														\
		result = jniEnv->CallStatic##_jname##MethodV( clazz, methodId, args );			\
		va_end( args );																	\
	}																					\
	return result;																		\
}

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
CI_CALL_STATIC_TYPE_METHOD_IMPL( jobject, Object, nullptr )
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
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		va_list args;
		va_start( args, methodId );
		jniEnv->CallStaticVoidMethodV( clazz, methodId, args );
		va_end( args );
	}
}

// -------------------------------------------------------------------------------------------------
// Java Methods
// -------------------------------------------------------------------------------------------------
jmethodID JniHelper::GetMethodId( jclass clazz, const std::string& name, const std::string& sig )
{
	if( name.empty() ) {
		throw std::runtime_error( "(GetMethodId) parameter 'name' is an empty string" );
	}

	jmethodID result = nullptr;

	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		result = jniEnv->GetMethodID( clazz, name.c_str(), sig.c_str() );
		if( jniEnv->ExceptionCheck() ) {
			//throw std::runtime_error( "GetMethodID error: name=" + name + ", sig=" + sig );
			throw std::runtime_error( "(GetMethodId) couldn't find matching name=" + name + ", sig=" + sig );
		}
	}

	return result;
}

#define CI_CALL_TYPE_METHOD_IMPL( _jtype, _jname, _jdefval )					\
_jtype JniHelper::Call##_jname##Method( jobject obj, jmethodID methodId, ... )	\
{																				\
	_jtype result = _jdefval;													\
	auto jniEnv = JvmHelper_CurrentJniEnv();									\
	if( jniEnv ) {																\
		va_list args;															\
		va_start( args, methodId );												\
		result = jniEnv->Call##_jname##MethodV( obj, methodId, args );			\
		va_end( args );															\
	}																			\
	return result;																\
}

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
CI_CALL_TYPE_METHOD_IMPL( jobject, Object, nullptr )
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
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		va_list args;
		va_start( args, methodId );
		jniEnv->CallVoidMethodV( obj, methodId, args );
		va_end( args );
	}
}

// -------------------------------------------------------------------------------------------------
// Java Static Fields
// -------------------------------------------------------------------------------------------------
jfieldID JniHelper::GetStaticFieldId( jclass clazz, const std::string& name, const std::string& sig )
{
	jfieldID result = nullptr;

	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		result = jniEnv->GetStaticFieldID( clazz, name.c_str(), sig.c_str() );
	}	

	return result;
}

jfieldID JniHelper::GetStaticObjectFieldId( jclass clazz, const std::string& name, const std::string& sig )
{
	return GetStaticFieldId( clazz, name, sig );
}

#define CI_GET_STATIC_TYPE_FIELDID_IMPL( _jname, _jsig )								\
jfieldID JniHelper::GetStatic##_jname##FieldId( jclass clazz, const std::string& name )	\
{																						\
	return GetStaticFieldId( clazz, name, _jsig );										\
}

//
// GetStaticObjectFieldID
// GetStaticBooleanFieldID
// GetStaticByteFieldID
// GetStaticCharFieldID
// GetStaticShortFieldID
// GetStaticIntFieldID
// GetStaticLongFieldID
// GetStaticFloatFieldID
// GetStaticDoubleFieldID
// GetStaticStringleFieldID
//	
CI_GET_STATIC_TYPE_FIELDID_IMPL( Boolean, "Z" )
CI_GET_STATIC_TYPE_FIELDID_IMPL( Byte, "B" )
CI_GET_STATIC_TYPE_FIELDID_IMPL( Char, "C" )
CI_GET_STATIC_TYPE_FIELDID_IMPL( Short, "S" )
CI_GET_STATIC_TYPE_FIELDID_IMPL( Int, "I" )
CI_GET_STATIC_TYPE_FIELDID_IMPL( Long, "J" )
CI_GET_STATIC_TYPE_FIELDID_IMPL( Float, "F" )
CI_GET_STATIC_TYPE_FIELDID_IMPL( Double, "D" )
CI_GET_STATIC_TYPE_FIELDID_IMPL( String, "Ljava/lang/String;" )
#undef CI_GET_STATIC_TYPE_FIELDID_IMPL

#define CI_GET_STATIC_TYPE_FIELD_IMPL( _jtype, _jname, _jdefval )				\
_jtype JniHelper::GetStatic##_jname##Field( jclass clazz, jfieldID fieldId )	\
{																				\
	_jtype result = _jdefval;													\
	auto jniEnv = JvmHelper_CurrentJniEnv();									\
	if( jniEnv && ( nullptr != fieldId ) ) {									\
		result = jniEnv->GetStatic##_jname##Field( clazz, fieldId );			\
	}																			\
	return result;																\
}

//
// GetStaticObjectFieldID
// GetStaticBooleanFieldID
// GetStaticByteFieldID
// GetStaticCharFieldID
// GetStaticShortFieldID
// GetStaticIntFieldID
// GetStaticLongFieldID
// GetStaticFloatFieldID
// GetStaticDoubleFieldID
//	
CI_GET_STATIC_TYPE_FIELD_IMPL( jobject, Object, nullptr )
CI_GET_STATIC_TYPE_FIELD_IMPL( jboolean, Boolean, 0 )
CI_GET_STATIC_TYPE_FIELD_IMPL( jbyte, Byte, 0 )
CI_GET_STATIC_TYPE_FIELD_IMPL( jchar, Char, 0 )
CI_GET_STATIC_TYPE_FIELD_IMPL( jshort, Short, 0 )
CI_GET_STATIC_TYPE_FIELD_IMPL( jint, Int, 0 )
CI_GET_STATIC_TYPE_FIELD_IMPL( jlong, Long, 0 )
CI_GET_STATIC_TYPE_FIELD_IMPL( jfloat, Float, 0.0f )
CI_GET_STATIC_TYPE_FIELD_IMPL( jdouble, Double, 0.0 )
#undef CI_GET_STATIC_TYPE_FIELD_IMPL

std::string JniHelper::GetStaticStringField( jclass clazz, jfieldID fieldId )
{
	jstring strObj = (jstring)GetStaticObjectField( clazz, fieldId );
	return JniHelper::StdStringFromJString( JvmHelper_CurrentJniEnv(), strObj );
}

#define CI_SET_STATIC_TYPE_FIELD_IMPL( _jtype, _jname )										\
void JniHelper::SetStatic##_jname##Field( jclass clazz, jfieldID fieldId, _jtype value )	\
{																							\
	auto jniEnv = JvmHelper_CurrentJniEnv();												\
	if( jniEnv && ( nullptr != fieldId ) ) {												\
		jniEnv->SetStatic##_jname##Field( clazz, fieldId, value );							\
	}																						\
}

//
// SetStaticObjectField
// SetStaticBooleanField
// SetStaticByteField
// SetStaticCharField
// SetStaticShortField
// SetStaticIntField
// SetStaticLongField
// SetStaticFloatField
// SetStaticDoubleField
//	
CI_SET_STATIC_TYPE_FIELD_IMPL( jobject, Object )
CI_SET_STATIC_TYPE_FIELD_IMPL( jboolean, Boolean )
CI_SET_STATIC_TYPE_FIELD_IMPL( jbyte, Byte )
CI_SET_STATIC_TYPE_FIELD_IMPL( jchar, Char )
CI_SET_STATIC_TYPE_FIELD_IMPL( jshort, Short )
CI_SET_STATIC_TYPE_FIELD_IMPL( jint, Int )
CI_SET_STATIC_TYPE_FIELD_IMPL( jlong, Long )
CI_SET_STATIC_TYPE_FIELD_IMPL( jfloat, Float )
CI_SET_STATIC_TYPE_FIELD_IMPL( jdouble, Double )
#undef CI_SET_STATIC_TYPE_FIELD_IMPL

void JniHelper::SetStaticStringField( jclass clazz, jfieldID fieldId, const std::string& value )
{
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv && ( nullptr != fieldId ) ) {
		jstring jstr = jniEnv->NewStringUTF( value.c_str() );
		jniEnv->SetStaticObjectField( clazz, fieldId, (jobject)jstr );
		jniEnv->DeleteLocalRef( (jobject)jstr );
	}	
}

// -------------------------------------------------------------------------------------------------
// Java Fields
// -------------------------------------------------------------------------------------------------
jfieldID JniHelper::GetFieldId( jclass clazz, const std::string& name, const std::string& sig )
{
	jfieldID result = nullptr;

	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		result = jniEnv->GetFieldID( clazz, name.c_str(), sig.c_str() );
	}	

	return result;
}

jfieldID JniHelper::GetObjectFieldId( jclass clazz, const std::string& name, const std::string& sig )
{
	return GetFieldId( clazz, name, sig );
}

#define CI_GET_TYPE_FIELDID_IMPL( _jname, _jsig )									\
jfieldID JniHelper::Get##_jname##FieldId( jclass clazz, const std::string& name )	\
{																					\
	return GetFieldId( clazz, name, _jsig );										\
}

//
// GetBooleanFieldID
// GetByteFieldID
// GetCharFieldID
// GetShortFieldID
// GetIntFieldID
// GetLongFieldID
// GetFloatFieldID
// GetDoubleFieldID
// GetStringleFieldID
//	
CI_GET_TYPE_FIELDID_IMPL( Boolean, "Z" )
CI_GET_TYPE_FIELDID_IMPL( Byte, "B" )
CI_GET_TYPE_FIELDID_IMPL( Char, "C" )
CI_GET_TYPE_FIELDID_IMPL( Short, "S" )
CI_GET_TYPE_FIELDID_IMPL( Int, "I" )
CI_GET_TYPE_FIELDID_IMPL( Long, "J" )
CI_GET_TYPE_FIELDID_IMPL( Float, "F" )
CI_GET_TYPE_FIELDID_IMPL( Double, "D" )
CI_GET_TYPE_FIELDID_IMPL( String, "Ljava/lang/String;" )
#undef CI_GET_TYPE_FIELDID_IMPL

#define CI_GET_TYPE_FIELD_IMPL( _jtype, _jname, _jdefval )				\
_jtype JniHelper::Get##_jname##Field( jobject obj, jfieldID fieldId )	\
{																		\
	_jtype result = _jdefval;											\
	auto jniEnv = JvmHelper_CurrentJniEnv();							\
	if( jniEnv && ( nullptr != fieldId ) ) {							\
		result = jniEnv->Get##_jname##Field( obj, fieldId );			\
	}																	\
	return result;														\
}

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
CI_GET_TYPE_FIELD_IMPL( jobject, Object, nullptr )
CI_GET_TYPE_FIELD_IMPL( jboolean, Boolean, 0 )
CI_GET_TYPE_FIELD_IMPL( jbyte, Byte, 0 )
CI_GET_TYPE_FIELD_IMPL( jchar, Char, 0 )
CI_GET_TYPE_FIELD_IMPL( jshort, Short, 0 )
CI_GET_TYPE_FIELD_IMPL( jint, Int, 0 )
CI_GET_TYPE_FIELD_IMPL( jlong, Long, 0 )
CI_GET_TYPE_FIELD_IMPL( jfloat, Float, 0.0f )
CI_GET_TYPE_FIELD_IMPL( jdouble, Double, 0.0 )
#undef CI_GET_TYPE_FIELD_IMPL

std::string JniHelper::GetStringField( jobject obj, jfieldID fieldId )
{
	jstring strObj = (jstring)GetObjectField( obj, fieldId );
	std::string result = JniHelper::StdStringFromJString( JvmHelper_CurrentJniEnv(), strObj );
	JniHelper::DeleteLocalRef( strObj );
	return result;
}

#define CI_SET_TYPE_FIELD_IMPL( _jtype, _jname )									\
void JniHelper::Set##_jname##Field( jobject obj, jfieldID fieldId, _jtype value )	\
{																					\
	auto jniEnv = JvmHelper_CurrentJniEnv();										\
	if( jniEnv && ( nullptr != fieldId ) ) {										\
		jniEnv->Set##_jname##Field( obj, fieldId, value );							\
	}																				\
}

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
CI_SET_TYPE_FIELD_IMPL( jobject, Object )
CI_SET_TYPE_FIELD_IMPL( jboolean, Boolean )
CI_SET_TYPE_FIELD_IMPL( jbyte, Byte )
CI_SET_TYPE_FIELD_IMPL( jchar, Char )
CI_SET_TYPE_FIELD_IMPL( jshort, Short )
CI_SET_TYPE_FIELD_IMPL( jint, Int )
CI_SET_TYPE_FIELD_IMPL( jlong, Long )
CI_SET_TYPE_FIELD_IMPL( jfloat, Float )
CI_SET_TYPE_FIELD_IMPL( jdouble, Double )
#undef CI_SET_TYPE_FIELD_IMPL

void JniHelper::SetStringField( jobject obj, jfieldID fieldId, const std::string& value )
{
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv && ( nullptr != fieldId ) ) {
		jstring jstr = jniEnv->NewStringUTF( value.c_str() );
		jniEnv->SetObjectField( obj, fieldId, (jobject)jstr );
		jniEnv->DeleteLocalRef( (jobject)jstr );
	}	
}

jobject JniHelper::NewGlobalRef( jobject obj )
{
	jobject result = nullptr;	
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv && ( nullptr != obj ) ) {
		result = jniEnv->NewGlobalRef( obj );
	}
	return result;
}

void JniHelper::DeleteGlobalRef( jobject globalRef )
{
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv && ( nullptr != globalRef ) ) {
		try {
			jniEnv->DeleteGlobalRef( globalRef );
			JNI_THROW_CPP_EXC( jniEnv, "DeleteGlobalRef error" , true );		
		}
		catch( const std::exception& e ) {
			dbg_app_error( e.what() );
		}
	}
}

JniGlobalObjectRef JniHelper::TrackedNewGlobalRef( jobject obj )
{
	JniGlobalObjectRef result;
	jobject globalRef = NewGlobalRef( obj );
	if( nullptr != globalRef ) {
		result = std::make_shared<JniGlobalObject>( globalRef );
		JniThreadVars* currentThreadVars = JvmHelper_GetCurrentThreadVars();
		if( nullptr != currentThreadVars ) {
			currentThreadVars->trackGlobalRef( result );
		}
	}
	return result;
}

void JniHelper::TrackedDeleteGlobalRef( const JniGlobalObjectRef& trackedGlobalRef )
{
	if( trackedGlobalRef ) {
		if( nullptr != trackedGlobalRef->getObject() ) {
			trackedGlobalRef->deleteGlobalRef();
		}

		JniThreadVars* currentThreadVars = JvmHelper_GetCurrentThreadVars();
		if( nullptr != currentThreadVars ) {
			currentThreadVars->removeGlobalRef( trackedGlobalRef );
		}
	}

/*	
dbg_app_log( "TrackedDeleteGlobalRef Mark 0:" );
	if( trackedGlobalRef ) {
		jobject obj = trackedGlobalRef->getObject();
		if( nullptr != obj ) {
			dbg_app_log( "TrackedDeleteGlobalRef Mark 1:" );
			DeleteGlobalRef( obj );
			trackedGlobalRef
		}

		JniThreadVars* currentThreadVars = JvmHelper_GetCurrentThreadVars();
		if( nullptr != currentThreadVars ) {
			currentThreadVars->removeGlobalRef( trackedGlobalRef );
		}
	}
*/	
}

jstring JniHelper::NewStringUTF( const std::string& str )
{
	jstring result = nullptr;
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		result = jniEnv->NewStringUTF( str.c_str() );
	}
	return result;
}

void JniHelper::DeleteLocalRef( jobject localRef )
{
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		jniEnv->DeleteLocalRef( localRef );
	}
}

jsize JniHelper::GetArrayLength( jarray array )
{
	jsize result = 0;
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		result = jniEnv->GetArrayLength( array );
	}
	return result;
}

jobject	JniHelper::GetObjectArrayElements( jobjectArray array, jsize index )
{
	jobject result = nullptr;
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		result = jniEnv->GetObjectArrayElement( array, index );
	}
	return result;	
}

jbyte* JniHelper::GetByteArrayElements( jbyteArray array, jboolean* isCopy )
{
	jbyte* result = nullptr;
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		result = jniEnv->GetByteArrayElements( array, isCopy );
	}
	return result;
}

jint* JniHelper::GetIntArrayElements( jintArray array, jboolean* isCopy )
{
	jint* result = nullptr;
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		result = jniEnv->GetIntArrayElements( array, isCopy );
	}
	return result;	
}

void JniHelper::ReleaseByteArrayElements( jbyteArray array, jbyte* elems, jint mode )
{
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		jniEnv->ReleaseByteArrayElements( array, elems, mode );
	}
}

void JniHelper::ReleaseIntArrayElements( jintArray array, jint* elems, jint mode )
{
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		jniEnv->ReleaseIntArrayElements( array, elems, mode );
	}	
}

jthrowable JniHelper::ExceptionOccurred()
{
	jthrowable result = nullptr;
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		result = jniEnv->ExceptionOccurred();
	}
	return result;
}

void JniHelper::ExceptionDescribe()
{
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		jniEnv->ExceptionDescribe();
	}
}

void JniHelper::ExceptionClear()
{
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		jniEnv->ExceptionClear();
	}
}

jboolean JniHelper::ExceptionCheck()
{
	jboolean result = false;
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		result = jniEnv->ExceptionCheck();
	}
	return result;
}

/*
std::string JniHelper::ThrowableGetMessage( jthrowable jexc, bool clearExc )
{
	std::string result;

	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( ( nullptr != jexc ) && jniEnv ) {
		jstring strObj = (jstring)jniEnv->CallObjectMethod( jexc, mThrowableGetMessageMethodId );
		result = JniHelper::StdStringFromJString( jniEnv, strObj );
	}

	if( clearExc && jniEnv ) {
		jniEnv->ExceptionClear();
	}

	return result;
}

std::string JniHelper::ThrowableToString( jthrowable jexc, bool clearExc )
{
	std::string result;
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( ( nullptr != jexc ) && jniEnv ) {
		jstring strObj = (jstring)jniEnv->CallObjectMethod( jexc, mThrowableToStringMethodId );
		result = JniHelper::StdStringFromJString( jniEnv, strObj );
	}
	//
	if( clearExc && jniEnv ) {
		jniEnv->ExceptionClear();
	}
	return result;
}

std::string JniHelper::GetPendingExceptionMessage( bool clearExc )
{
	std::string result;
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		jthrowable jexc = jniEnv->ExceptionOccurred();
		result = ThrowableGetMessage( jexc, clearExc );
	}
	return result;
}

std::string	JniHelper::GetPendingExceptionString( bool clearExc )
{
	std::string result;
	auto jniEnv = JvmHelper_CurrentJniEnv();
	if( jniEnv ) {
		jthrowable jexc = jniEnv->ExceptionOccurred();
		result = ThrowableToString( jexc, clearExc );
	}
	return result;
}
*/

}} // namespace cinder::android