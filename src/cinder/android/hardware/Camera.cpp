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

#include "cinder/android/hardware/Camera.h"
#include "cinder/android/app/CinderNativeActivity.h"
#include "cinder/android/AndroidDevLog.h"
using namespace ci::android::app;

#include "cinder/app/App.h"

namespace cinder { namespace android { namespace hardware {

////////////////////////////////////////////////////////////////////////////////////////////////////
// Camera::DeviceInfo

jclassID	Camera::DeviceInfo::Java::ClassName 		= "org/libcinder/hardware/Camera$DeviceInfo";
jclass 		Camera::DeviceInfo::Java::ClassObject		= nullptr;
jfieldID	Camera::DeviceInfo::Java::id 				= nullptr;
jfieldID	Camera::DeviceInfo::Java::frontFacing 		= nullptr;
jfieldID	Camera::DeviceInfo::Java::resolutions		= nullptr;

Camera::DeviceInfo::DeviceInfo( jobject obj )
{
	mJavaObject = JniHelper::Get()->TrackedNewGlobalRef( obj );
	if( ! mJavaObject ) {
		throw std::runtime_error( "Error initializing mJavaObject (" + std::string( __PRETTY_FUNCTION__ ) + ")" );
	}

	sync();
}

Camera::DeviceInfo::~DeviceInfo()
{
	JniHelper::Get()->TrackedDeleteGlobalRef( mJavaObject );
}

void Camera::DeviceInfo::cacheJni()
{
	if( JniHelper::Get()->AttachCurrentThread() ) {
		try {
			jclass javaClass = JniHelper::Get()->RetrieveClass( Java::ClassName );
			if( nullptr != javaClass ) {
				Java::ClassObject = reinterpret_cast<jclass>( JniHelper::Get()->NewGlobalRef( javaClass ) );
				jni_obtained_check( Camera::DeviceInfo::Java::ClassObject );
			}

			if( nullptr != Java::ClassObject ) {
				Java::id 				= JniHelper::Get()->GetStringFieldId( Java::ClassObject, "id" );
				Java::frontFacing 		= JniHelper::Get()->GetBooleanFieldId( Java::ClassObject, "frontFacing" );
				Java::resolutions 		= JniHelper::Get()->GetObjectFieldId( Java::ClassObject, "resolutions", "[I" );
				jni_obtained_check( Camera::DeviceInfo::Java::id );
				jni_obtained_check( Camera::DeviceInfo::Java::frontFacing );
				jni_obtained_check( Camera::DeviceInfo::Java::resolutions );
			}
		}
		catch( const std::exception& e ) {
			dbg_app_error( e.what() );
		}
	}
}

void Camera::DeviceInfo::destroyJni()
{
	if( JniHelper::Get()->AttachCurrentThread() ) {	
		JniHelper::Get()->DeleteGlobalRef( Java::ClassObject  );
		Java::ClassObject	= nullptr;
		Java::id 			= nullptr;
		Java::frontFacing 	= nullptr;
		Java::resolutions	= nullptr;
	}
}

void Camera::DeviceInfo::sync() 
{
	if( ! mJavaObject ) {
		return;
	}

	id = JniHelper::Get()->GetStringField( mJavaObject->getObject(), Java::id );
	frontFacing = JniHelper::Get()->GetBooleanField( mJavaObject->getObject(), Java::frontFacing );

	resolutions.clear();

	jintArray arrayObj = (jintArray)JniHelper::Get()->GetObjectField( mJavaObject->getObject(), Java::resolutions );
	if( nullptr == arrayObj ) {
		throw std::runtime_error( "Error reading resolutions field from Java" );
	}
	
	jsize n = JniHelper::Get()->GetArrayLength( arrayObj );
	if( n > 0 ) {
		jint* elems = JniHelper::Get()->GetIntArrayElements( arrayObj, nullptr );
		for( int i = 0; i < n/2; ++i ) {
			int w = elems[2*i + 0];
			int h = elems[2*i + 1];
			resolutions.push_back( ivec2( w, h ) );
		}
		JniHelper::Get()->ReleaseIntArrayElements( arrayObj, elems, JNI_ABORT );
	}

	JniHelper::Get()->DeleteLocalRef( arrayObj );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Camera

/*
jclassID	Camera::Java::ClassName 		= "org/libcinder/hardware/Camera";
jclass 		Camera::Java::ClassObject		= nullptr;
jmethodID 	Camera::Java::enumerateDevices 	= nullptr;
jmethodID 	Camera::Java::startCapture		= nullptr;
jmethodID 	Camera::Java::stopCapture		= nullptr;
jmethodID 	Camera::Java::lockPixels		= nullptr;
jmethodID 	Camera::Java::unlockPixels		= nullptr;
jmethodID 	Camera::Java::isNewFrameAvailable		= nullptr;
jmethodID 	Camera::Java::clearNewFrameAvailable	= nullptr;
*/

jmethodID Camera::Java::hardware_camera_initialize 				= nullptr;
jmethodID Camera::Java::hardware_camera_enumerateDevices		= nullptr;
jmethodID Camera::Java::hardware_camera_startCapture 			= nullptr;
jmethodID Camera::Java::hardware_camera_stopCapture 			= nullptr;
jmethodID Camera::Java::hardware_camera_lockPixels 				= nullptr;
jmethodID Camera::Java::hardware_camera_unlockPixels 			= nullptr;
jmethodID Camera::Java::hardware_camera_isNewFrameAvailable 	= nullptr;
jmethodID Camera::Java::hardware_camera_clearNewFrameAvailable 	= nullptr;
jmethodID Camera::Java::hardware_camera_initPreviewTexture 		= nullptr;
jmethodID Camera::Java::hardware_camera_updateTexImage 			= nullptr;

std::unique_ptr<Camera>	Camera::sInstance;

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::cacheJni()
{
dbg_app_fn_enter( __PRETTY_FUNCTION__ );

	Camera::DeviceInfo::cacheJni();

	if( JniHelper::Get()->AttachCurrentThread() ) {
		try {
			jclass javaClass = ci::android::app::CinderNativeActivity::getInstance()->getJavaClass();
			if( nullptr != javaClass ) {
				Java::hardware_camera_initialize 				= JniHelper::Get()->GetMethodId( javaClass, "hardware_camera_initialize", "(I)V" );
				Java::hardware_camera_enumerateDevices 			= JniHelper::Get()->GetMethodId( javaClass, "hardware_camera_enumerateDevices", "()[Lorg/libcinder/hardware/Camera$DeviceInfo;" );
				Java::hardware_camera_startCapture 				= JniHelper::Get()->GetMethodId( javaClass, "hardware_camera_startCapture", "(Ljava/lang/String;II)V" );
				Java::hardware_camera_stopCapture 				= JniHelper::Get()->GetMethodId( javaClass, "hardware_camera_stopCapture", "()V" );
				Java::hardware_camera_lockPixels 				= JniHelper::Get()->GetMethodId( javaClass, "hardware_camera_lockPixels", "()[B" );
				Java::hardware_camera_unlockPixels 				= JniHelper::Get()->GetMethodId( javaClass, "hardware_camera_unlockPixels", "()V" );
				Java::hardware_camera_isNewFrameAvailable 		= JniHelper::Get()->GetMethodId( javaClass, "hardware_camera_isNewFrameAvailable", "()Z" );
				Java::hardware_camera_clearNewFrameAvailable	= JniHelper::Get()->GetMethodId( javaClass, "hardware_camera_clearNewFrameAvailable", "()V" );
				Java::hardware_camera_initPreviewTexture		= JniHelper::Get()->GetMethodId( javaClass, "hardware_camera_initPreviewTexture", "(I)V" );
				Java::hardware_camera_updateTexImage			= JniHelper::Get()->GetMethodId( javaClass, "hardware_camera_updateTexImage", "()V" );
				jni_obtained_check( Camera::Java::hardware_camera_enumerateDevices );
				jni_obtained_check( Camera::Java::hardware_camera_initialize );
				jni_obtained_check( Camera::Java::hardware_camera_startCapture );
				jni_obtained_check( Camera::Java::hardware_camera_stopCapture );
				jni_obtained_check( Camera::Java::hardware_camera_lockPixels );
				jni_obtained_check( Camera::Java::hardware_camera_unlockPixels );
				jni_obtained_check( Camera::Java::hardware_camera_isNewFrameAvailable );
				jni_obtained_check( Camera::Java::hardware_camera_clearNewFrameAvailable );
				jni_obtained_check( Camera::Java::hardware_camera_initPreviewTexture );
				jni_obtained_check( Camera::Java::hardware_camera_updateTexImage );
			}

			/*
			jclass javaClass = JniHelper::Get()->RetrieveClass( Java::ClassName );
			if( nullptr != javaClass ) {
				Java::ClassObject = reinterpret_cast<jclass>( JniHelper::Get()->NewGlobalRef( javaClass ) );
				jni_obtained_check( Camera::Java::ClassObject );
			}

			if( nullptr != Java::ClassObject ) {
				Java::enumerateDevices 	= JniHelper::Get()->GetMethodId( Java::ClassObject, "enumerateDevices", "()[Lorg/libcinder/hardware/Camera$DeviceInfo;" );
				Java::startCapture 		= JniHelper::Get()->GetMethodId( Java::ClassObject, "startCapture", "(Ljava/lang/String;)V" );
				Java::stopCapture 		= JniHelper::Get()->GetMethodId( Java::ClassObject, "stopCapture", "()V" );
				Java::lockPixels 		= JniHelper::Get()->GetMethodId( Java::ClassObject, "lockPixels", "()[B" );
				Java::unlockPixels 		= JniHelper::Get()->GetMethodId( Java::ClassObject, "unlockPixels", "()V" );
				Java::isNewFrameAvailable 		= JniHelper::Get()->GetMethodId( Java::ClassObject, "isNewFrameAvailable", "()Z" );
				Java::clearNewFrameAvailable	= JniHelper::Get()->GetMethodId( Java::ClassObject, "clearNewFrameAvailable", "()V" );
				jni_obtained_check( Camera::Java::enumerateDevices );
				jni_obtained_check( Camera::Java::startCapture );
				jni_obtained_check( Camera::Java::stopCapture );
				jni_obtained_check( Camera::Java::lockPixels );
				jni_obtained_check( Camera::Java::unlockPixels );
				jni_obtained_check( Camera::Java::isNewFrameAvailable );
				jni_obtained_check( Camera::Java::clearNewFrameAvailable );
			}
			*/
		}
		catch( const std::exception& e ) {
			dbg_app_error( e.what() );
		}
	}

dbg_app_fn_exit( __PRETTY_FUNCTION__ );
}

void Camera::destroyJni()
{	
dbg_app_fn_enter( __PRETTY_FUNCTION__ );

	Camera::DeviceInfo::destroyJni();

	if( JniHelper::Get()->AttachCurrentThread() ) {
		Java::hardware_camera_initialize 				= nullptr;
		Java::hardware_camera_enumerateDevices			= nullptr;
		Java::hardware_camera_startCapture 				= nullptr;
		Java::hardware_camera_stopCapture 				= nullptr;
		Java::hardware_camera_lockPixels 				= nullptr;
		Java::hardware_camera_unlockPixels 				= nullptr;
		Java::hardware_camera_isNewFrameAvailable 		= nullptr;
		Java::hardware_camera_clearNewFrameAvailable 	= nullptr;
		Java::hardware_camera_initPreviewTexture 		= nullptr;
		Java::hardware_camera_updateTexImage 			= nullptr;

		/*
		JniHelper::Get()->DeleteGlobalRef( Java::ClassObject  );
		Java::ClassObject	= nullptr;
		Java::startCapture 	= nullptr;
		Java::stopCapture 	= nullptr;
		*/
	}

dbg_app_fn_exit( __PRETTY_FUNCTION__ );
}

Camera* Camera::getInstance()
{
	if( ! Camera::sInstance ) {
		Camera::sInstance = std::unique_ptr<Camera>( new Camera() );
		if( Camera::sInstance ) {
			try {
				Camera::sInstance->initialize();

				auto jniEnv = JniHelper::Get()->AttachCurrentThread();
				JNI_THROW_CPP_EXC( jniEnv, "failed to initalize camera", true );
			}
			catch( const std::exception& e ) {
				Camera::sInstance.reset();
				dbg_app_error( e.what() );
			}
		}
	}

	return Camera::sInstance.get();
}

void Camera::destroyInstance()
{
dbg_app_fn_enter( __PRETTY_FUNCTION__ );

	if( Camera::sInstance ) {
		if( Camera::sInstance->mCapturing ) {
			Camera::sInstance->stopCapture();
		}

		Camera::sInstance.reset();
		dbg_app_log( "Camera::destroyInstance(): destroyed sInstance!" );
	}
		
dbg_app_fn_exit( __PRETTY_FUNCTION__ );
}

void Camera::initialize()
{
	jobject javaObject = ci::android::app::CinderNativeActivity::getJavaObject();
	jint apiLevel = __ANDROID_API__;
	JniHelper::Get()->CallVoidMethod( javaObject, Java::hardware_camera_initialize, apiLevel );
}

std::vector<Camera::DeviceInfoRef> Camera::enumerateDevices() const
{
dbg_app_fn_enter( __PRETTY_FUNCTION__ );

	std::vector<DeviceInfoRef> result;

	auto jniEnv = JniHelper::Get()->AttachCurrentThread();
	if( jniEnv ) {
		try {
			jobject javaObject = ci::android::app::CinderNativeActivity::getJavaObject();
			jobjectArray array = (jobjectArray)jniEnv->CallObjectMethod( javaObject, Java::hardware_camera_enumerateDevices );
			JNI_THROW_CPP_EXC( jniEnv, "enumerateDevices failed on CallObjectMethod", true );

			if( nullptr != array ) {
				jsize n = jniEnv->GetArrayLength( array );
				for( jsize i = 0; i < n; ++i ) {
					jobject obj = jniEnv->GetObjectArrayElement( array, i );
					if( nullptr == obj ) {
						continue;
					}

					ci::app::console() << "Retrieving information for camera: " << i << std::endl;

					DeviceInfoRef deviceInfo = std::make_shared<DeviceInfo>( obj );
					result.push_back( deviceInfo );

					jniEnv->DeleteLocalRef( obj );
				}
			}

			jniEnv->DeleteLocalRef( array );
		}
		catch( const std::exception& e ) {
			dbg_app_error( e.what() );
		}
	}

dbg_app_fn_exit( __PRETTY_FUNCTION__ );

	return result;	
}

void Camera::startCapture( const std::string& deviceId, int width, int height )
{
	mWidth = width;
	mHeight = height;

	/*
	jobject javaObject = ci::android::app::CinderNativeActivity::getJavaObject();
	JniHelper::Get()->CallVoidMethod( javaObject, Java::hardware_camera_startCapture, (jint)mWidth, (jint)mHeight );	
	*/
	
	jobject javaObject = ci::android::app::CinderNativeActivity::getJavaObject();
	jstring jstrDeviceId = JniHelper::Get()->NewStringUTF( deviceId );	
	JniHelper::Get()->CallVoidMethod( javaObject, Java::hardware_camera_startCapture, jstrDeviceId, (jint)mWidth, (jint)mHeight );
	JniHelper::Get()->DeleteLocalRef( jstrDeviceId );

	mCapturing = true;
}

void Camera::stopCapture()
{
	jobject javaObject = ci::android::app::CinderNativeActivity::getJavaObject();
	JniHelper::Get()->CallVoidMethod( javaObject, Java::hardware_camera_stopCapture );

	mCapturing = false;
}

bool Camera::isNewFrameAvailable() const
{
	bool result = false;
	if( mCapturing ) {
		jobject javaObject = ci::android::app::CinderNativeActivity::getJavaObject();
		result = JniHelper::Get()->CallBooleanMethod( javaObject, Java::hardware_camera_isNewFrameAvailable );
	}
	return result;
}

void Camera::clearNewFrameAvailable()
{
	if( ! mCapturing ) {
		return;
	}

	jobject javaObject = ci::android::app::CinderNativeActivity::getJavaObject();
	JniHelper::Get()->CallVoidMethod( javaObject, Java::hardware_camera_clearNewFrameAvailable );
}

void Camera::initPreviewTexture(int textureId)
{
	if( ! mCapturing ) {
		return;
	}

	jobject javaObject = ci::android::app::CinderNativeActivity::getJavaObject();
	JniHelper::Get()->CallVoidMethod( javaObject, Java::hardware_camera_initPreviewTexture, (jint)textureId );	
}

void Camera::updateTexImage()
{
	if( ! mCapturing ) {
		return;
	}

	jobject javaObject = ci::android::app::CinderNativeActivity::getJavaObject();
	JniHelper::Get()->CallVoidMethod( javaObject, Java::hardware_camera_updateTexImage );
}


// Conversion from yuv nv21 to rgb24 adapted from
// videonet conversion from YUV420 to RGB24
// http://www.codeguru.com/cpp/g-m/multimedia/video/article.php/c7621
static long int crv_tab[256];
static long int cbu_tab[256];
static long int cgu_tab[256];
static long int cgv_tab[256];
static long int tab_76309[256];
static unsigned char clp[1024]; //for clip in CCIR601

static bool gTableInited = false;

//
//Initialize conversion table for YUV420 to RGB
//
static void InitConvertTable()
{
	long int crv,cbu,cgu,cgv;
	int i,ind;

	crv = 104597; cbu = 132201;  /* fra matrise i global.h */
	cgu = 25675;  cgv = 53279;

	for (i = 0; i < 256; i++) {
		crv_tab[i] = (i-128) * crv;
		cbu_tab[i] = (i-128) * cbu;
		cgu_tab[i] = (i-128) * cgu;
		cgv_tab[i] = (i-128) * cgv;
		tab_76309[i] = 76309*(i-16);
	}

	for (i=0; i<384; i++) {
		clp[i] =0;
	}

	ind=384;

	for (i=0;i<256; i++) {
		clp[ind++]=i;
	}

	ind=640;
	
	for (i=0;i<384;i++) {
		clp[ind++]=255;
	}

	gTableInited = true;
}

static void ConvertYUV2RGB(const unsigned char *src0,const unsigned char *src1,unsigned char *dst_ori, int width,int height)
{
	if( ! gTableInited ) {
		InitConvertTable();
	}

	int y1,y2,u,v;
	const unsigned char *py1,*py2;
	int i,j, c1, c2, c3, c4;
	unsigned char *d1, *d2;

	int width3 = 3*width;
	py1=src0;
	py2=py1+width;
	d1=dst_ori;
	d2=d1+width3;
	for (j = 0; j < height; j += 2) {
		for (i = 0; i < width; i += 2) {

			v = *src1++;
			u = *src1++;

			c1 = crv_tab[v];
			c2 = cgu_tab[u];
			c3 = cgv_tab[v];
			c4 = cbu_tab[u];

			//up-left
			y1 = tab_76309[*py1++];
			*d1++ = clp[384+((y1 + c1)>>16)];
			*d1++ = clp[384+((y1 - c2 - c3)>>16)];
			*d1++ = clp[384+((y1 + c4)>>16)];

			//down-left
			y2 = tab_76309[*py2++];
			*d2++ = clp[384+((y2 + c1)>>16)];
			*d2++ = clp[384+((y2 - c2 - c3)>>16)];
			*d2++ = clp[384+((y2 + c4)>>16)];

			//up-right
			y1 = tab_76309[*py1++];
			*d1++ = clp[384+((y1 + c1)>>16)];
			*d1++ = clp[384+((y1 - c2 - c3)>>16)];
			*d1++ = clp[384+((y1 + c4)>>16)];

			//down-right
			y2 = tab_76309[*py2++];
			*d2++ = clp[384+((y2 + c1)>>16)];
			*d2++ = clp[384+((y2 - c2 - c3)>>16)];
			*d2++ = clp[384+((y2 + c4)>>16)];
		}

		d1 += width3;
		d2 += width3;
		py1+=   width;
		py2+=   width;
	}
}

void Camera::getPixels( Surface8u* outSurface ) const
{
	auto jniEnv = JniHelper::Get()->AttachCurrentThread();
	if( jniEnv ) {
		jobject javaObject = ci::android::app::CinderNativeActivity::getJavaObject();

		jbyteArray byteArray = (jbyteArray)jniEnv->CallObjectMethod( javaObject, Java::hardware_camera_lockPixels );
		if( nullptr != byteArray ) {
			//dbg_app_log( "grabbing pixels" );

			size_t dataLength = JniHelper::Get()->GetArrayLength( byteArray );
			jbyte* dataPtr = (jbyte*)JniHelper::Get()->GetByteArrayElements( byteArray, NULL );
			if( ( nullptr != dataPtr ) && ( dataLength > 0 ) ) {
				const uint8_t * src = (const uint8_t *)dataPtr;
				uint8_t * dst = (uint8_t *)(outSurface->getData());
				ConvertYUV2RGB( src, src + (mWidth*mHeight), dst, mWidth, mHeight );

				/*
				const uint8_t * src = (const uint8_t *)dataPtr;
				uint8_t * dst = &(mData[0]); //(uint8_t *)(outSurface->getData());
				ConvertYUV2RGB( src, src + (mWidth*mHeight), dst, mWidth, mHeight );

				src = &(mData[0]);
				dst = (uint8_t *)(outSurface->getData());

				for(int y = 0; y < mWidth; ++y) {
					for( int x = 0; x < mHeight; ++x ) {
						const uint8_t * s = (src + 3*(x*mWidth + y));
						uint8_t * d = (dst + 3*(y*mHeight + (mHeight - x - 1)));

						*(d + 0) = *(s + 0);
						*(d + 1) = *(s + 1);
						*(d + 2) = *(s + 2);
					}
				}
				*/
			}			

			if( nullptr != dataPtr ) {
				jniEnv->ReleaseByteArrayElements( byteArray, dataPtr, JNI_ABORT );
			}

			jniEnv->DeleteLocalRef( byteArray );
		}
		jniEnv->CallVoidMethod( javaObject, Java::hardware_camera_unlockPixels );
	}
}

}}} // namespace cinder::android::hardware

// #include <sstream>
// #include <iomanip>

// #include <android/log.h>
// #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "cinder", __VA_ARGS__))
// #define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "cinder", __VA_ARGS__))
// #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"cinder", __VA_ARGS__))

// #define FN_LOG( __FNREF__ ) 							\
//  	if( NULL != __FNREF__ ) {							\
//  		LOGI( #__FNREF__ " successfully obtained" );	\
//  	}

// namespace cinder { namespace android {

// jclass		CinderCamera::sCinderCameraClass		= nullptr;
// jmethodID	CinderCamera::sInitializeMethodId 		= nullptr;
// jmethodID 	CinderCamera::sHasFrontCameraMethodId	= nullptr;
// jmethodID 	CinderCamera::sHasBackCameraMethodId 	= nullptr;
// jmethodID 	CinderCamera::sStartCaptureMethodId 	= nullptr;
// jmethodID 	CinderCamera::sStopCaptureMethodId 		= nullptr;
// jmethodID 	CinderCamera::sLockPixelsMethodId 		= nullptr;
// jmethodID 	CinderCamera::sUnlockPixelsMethodId 	= nullptr;
// jmethodID 	CinderCamera::sGetWidthMethodId			= nullptr;
// jmethodID 	CinderCamera::sGetHeightMethodId		= nullptr;

// CinderCamera::CinderCamera()
// {
// }

// CinderCamera::~CinderCamera()
// {
// 	if( mCapturing ) {
// 		stopCapture();
// 	}
// }

// void CinderCamera::cacheJni()
// {
// 	const std::string className = "org/libcinder/hardware/CinderCamera";

// 	if( JniHelper::Get()->AttachCurrentThread() ) {
// 		jclass cinderCameraClass = JniHelper::Get()->RetrieveClass( className );
// 		if( nullptr != cinderCameraClass ) {
// 			CinderCamera::sCinderCameraClass = reinterpret_cast<jclass>( JniHelper::Get()->NewGlobalRef( cinderCameraClass ) );
// 		}

// 		if( nullptr != CinderCamera::sCinderCameraClass ) {
// 			CinderCamera::sInitializeMethodId 		= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "initialize", "()Z" );
// 			CinderCamera::sHasFrontCameraMethodId	= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "hasFrontCamera", "()Z" );
// 			CinderCamera::sHasBackCameraMethodId 	= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "hasBackCamera", "()Z" );
// 			CinderCamera::sStartCaptureMethodId 	= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "startCapture", "()V" );
// 			CinderCamera::sStopCaptureMethodId 		= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "stopCapture", "()V" );
// 			CinderCamera::sLockPixelsMethodId 		= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "lockPixels", "()[B" );
// 			CinderCamera::sUnlockPixelsMethodId 	= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "unlockPixels", "()V" );
// 			CinderCamera::sGetWidthMethodId 		= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "getWidth", "()I" );
// 			CinderCamera::sGetHeightMethodId 		= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "getHeight", "()I" );

// 			FN_LOG( CinderCamera::sInitializeMethodId );	
// 			FN_LOG( CinderCamera::sHasFrontCameraMethodId );	
// 			FN_LOG( CinderCamera::sHasBackCameraMethodId );	
// 			FN_LOG( CinderCamera::sStartCaptureMethodId );	
// 			FN_LOG( CinderCamera::sStopCaptureMethodId );	
// 			FN_LOG( CinderCamera::sLockPixelsMethodId );	
// 			FN_LOG( CinderCamera::sUnlockPixelsMethodId );	
// 			FN_LOG( CinderCamera::sGetWidthMethodId );	
// 			FN_LOG( CinderCamera::sGetHeightMethodId );				
// 		}
// 	}
// }

// void CinderCamera::destroyJni()
// {
// 	if( JniHelper::Get()->AttachCurrentThread() ) {
		
// 		JniHelper::Get()->DeleteGlobalRef( CinderCamera::sCinderCameraClass );

// 		CinderCamera::sCinderCameraClass		= nullptr;
// 		CinderCamera::sInitializeMethodId 		= nullptr;
// 		CinderCamera::sHasFrontCameraMethodId	= nullptr;
// 		CinderCamera::sHasBackCameraMethodId 	= nullptr;
// 		CinderCamera::sStartCaptureMethodId 	= nullptr;
// 		CinderCamera::sStopCaptureMethodId 		= nullptr;
// 		CinderCamera::sLockPixelsMethodId 		= nullptr;
// 		CinderCamera::sUnlockPixelsMethodId 	= nullptr;	
// 		CinderCamera::sGetWidthMethodId 		= nullptr;
// 		CinderCamera::sGetHeightMethodId 		= nullptr;	
// 	}
// }

// bool CinderCamera::initialize()
// {
// 	mInitialized = false;

// 	if( JniHelper::Get()->AttachCurrentThread() ) {
// 		mInitialized = (bool)JniHelper::Get()->CallStaticBooleanMethod( CinderCamera::sCinderCameraClass, CinderCamera::sInitializeMethodId );

// 		if( mInitialized ) {
// 			mHasFrontCamera = (bool)JniHelper::Get()->CallStaticBooleanMethod( CinderCamera::sCinderCameraClass, CinderCamera::sHasFrontCameraMethodId );
// 			mHasBackCamera = (bool)JniHelper::Get()->CallStaticBooleanMethod( CinderCamera::sCinderCameraClass, CinderCamera::sHasBackCameraMethodId );
// 		}
// 	}

// 	return mInitialized;	
// }

// void CinderCamera::startCapture()
// {
// 	if( ! mInitialized ) {
// 		return;
// 	}

// 	if( JniHelper::Get()->AttachCurrentThread() ) {
// 		JniHelper::Get()->CallStaticVoidMethod( CinderCamera::sCinderCameraClass, CinderCamera::sStartCaptureMethodId );
		
// 		mWidth = JniHelper::Get()->CallStaticIntMethod( CinderCamera::sCinderCameraClass, CinderCamera::sGetWidthMethodId );
// 		mHeight = JniHelper::Get()->CallStaticIntMethod( CinderCamera::sCinderCameraClass, CinderCamera::sGetHeightMethodId );

// 		mCapturing = true;
// 	}	
// }

// void CinderCamera::stopCapture()
// {
// 	if( ! mInitialized ) {
// 		return;
// 	}

// 	if( JniHelper::Get()->AttachCurrentThread() ) {
// 		JniHelper::Get()->CallStaticVoidMethod( CinderCamera::sCinderCameraClass, CinderCamera::sStopCaptureMethodId );

// 		mCapturing = false;
// 	}	
// }

// /*
// const uint8_t * CinderCamera::lockPixels()
// {
// 	LOGI("CinderCamera::lockPixels");

// 	const uint8_t * result = nullptr;

// 	//LOGI( "Mark 1" );

// 	if( mCapturing ) {
// 		//LOGI( "Mark 2" );
// 		if( JniHelper::Get()->AttachCurrentThread() ) {
// 			//LOGI( "Mark 3" );
// 			jbyteArray jBytes = (jbyteArray)JniHelper::Get()->CallStaticObjectMethod( CinderCamera::sCinderCameraClass, CinderCamera::sLockPixelsMethodId );
// 			mLockedPixels = true;

// 			if( NULL != jBytes ) {
// 				//LOGI( "Mark 4" );
// 				jbyte* dataPtr = (jbyte*)JniHelper::Get()->GetByteArrayElements( jBytes, NULL );
// 				if( NULL != dataPtr ) {
// 					//LOGI( "Mark 5" );
// 					result = (const uint8_t *)dataPtr;
// 				}
// 			}
// 			JniHelper::Get()->DeatchCurrentThread();
// 		}	
// 	}

// 	return result;
// }

// void CinderCamera::unlockPixels()
// {
// 	LOGI("CinderCamera::unlockPixels");

// 	if( mLockedPixels ) {
// 		if( JniHelper::Get()->AttachCurrentThread() ) {
// 			JniHelper::Get()->CallStaticVoidMethod( CinderCamera::sCinderCameraClass, CinderCamera::sUnlockPixelsMethodId );
// 			JniHelper::Get()->DeatchCurrentThread();
// 		}
// 		mLockedPixels = false;
// 	}
// }

// */

// // Conversion from yuv nv21 to rgb24 adapted from
// // videonet conversion from YUV420 to RGB24
// // http://www.codeguru.com/cpp/g-m/multimedia/video/article.php/c7621
// static long int crv_tab[256];
// static long int cbu_tab[256];
// static long int cgu_tab[256];
// static long int cgv_tab[256];
// static long int tab_76309[256];
// static unsigned char clp[1024];         //for clip in CCIR601

// static bool gTableInited = false;

//  //
//  //Initialize conversion table for YUV420 to RGB
//  //
//  static void InitConvertTable()
//  {
//     long int crv,cbu,cgu,cgv;
//     int i,ind;

//     crv = 104597; cbu = 132201;  /* fra matrise i global.h */
//     cgu = 25675;  cgv = 53279;

//     for (i = 0; i < 256; i++) {
//        crv_tab[i] = (i-128) * crv;
//        cbu_tab[i] = (i-128) * cbu;
//        cgu_tab[i] = (i-128) * cgu;
//        cgv_tab[i] = (i-128) * cgv;
//        tab_76309[i] = 76309*(i-16);
//     }

//     for (i=0; i<384; i++)
//        clp[i] =0;
//     ind=384;
//     for (i=0;i<256; i++)
//         clp[ind++]=i;
//     ind=640;
//     for (i=0;i<384;i++)
//         clp[ind++]=255;

//     gTableInited = true;
//  }

//  static void ConvertYUV2RGB(const unsigned char *src0,const unsigned char *src1,unsigned char *dst_ori, int width,int height)
//  {
//  	if( ! gTableInited ) {
//  		InitConvertTable();
//  	}

//      register int y1,y2,u,v;
//      register const unsigned char *py1,*py2;
//      register int i,j, c1, c2, c3, c4;
//      register unsigned char *d1, *d2;

//      int width3 = 3*width;
//      py1=src0;
//      py2=py1+width;
//      d1=dst_ori;
//      d2=d1+width3;
//      for (j = 0; j < height; j += 2) {
//          for (i = 0; i < width; i += 2) {

//              v = *src1++;
//              u = *src1++;

//              c1 = crv_tab[v];
//              c2 = cgu_tab[u];
//              c3 = cgv_tab[v];
//              c4 = cbu_tab[u];

//              //up-left
//              y1 = tab_76309[*py1++];
//              *d1++ = clp[384+((y1 + c1)>>16)];
//              *d1++ = clp[384+((y1 - c2 - c3)>>16)];
//              *d1++ = clp[384+((y1 + c4)>>16)];

//              //down-left
//              y2 = tab_76309[*py2++];
//              *d2++ = clp[384+((y2 + c1)>>16)];
//              *d2++ = clp[384+((y2 - c2 - c3)>>16)];
//              *d2++ = clp[384+((y2 + c4)>>16)];

//              //up-right
//              y1 = tab_76309[*py1++];
//              *d1++ = clp[384+((y1 + c1)>>16)];
//              *d1++ = clp[384+((y1 - c2 - c3)>>16)];
//              *d1++ = clp[384+((y1 + c4)>>16)];

//              //down-right
//              y2 = tab_76309[*py2++];
//              *d2++ = clp[384+((y2 + c1)>>16)];
//              *d2++ = clp[384+((y2 - c2 - c3)>>16)];
//              *d2++ = clp[384+((y2 + c4)>>16)];
//          }
//          d1 += width3;
//          d2 += width3;
//          py1+=   width;
//          py2+=   width;
//      }
// }


// ci::Surface	CinderCamera::getSurface()
// {
// 	ci::Surface result = ci::Surface( mWidth, mHeight, false );

// 	if( mCapturing ) {
// 		bool hasPixels = false;

// 		// Copy pixels
// 		if( JniHelper::Get()->AttachCurrentThread() ) {
// 			jbyteArray jBytes = (jbyteArray)JniHelper::Get()->CallStaticObjectMethod( CinderCamera::sCinderCameraClass, CinderCamera::sLockPixelsMethodId );

// 			if( NULL != jBytes ) {
// 				size_t dataLength = JniHelper::Get()->GetArrayLength( jBytes );
// 				jbyte* dataPtr = (jbyte*)JniHelper::Get()->GetByteArrayElements( jBytes, NULL );
// 				if( ( NULL != dataPtr ) && ( dataLength > 0 ) ) {
// 					/*
// 					if( ! mBuffer ) {
// 						mBuffer = ci::Buffer( dataLength );
// 					}

// 					memcpy( mBuffer.getData(), (const void *)dataPtr, dataLength );
// 					hasPixels = true;
// 					*/

// 					const uint8_t * src = (const uint8_t *)dataPtr;
// 					uint8_t * dst = (uint8_t *)result.getData();
// 					ConvertYUV2RGB( src, src + (mWidth*mHeight), dst, mWidth, mHeight );					
// 				}
// 			}

// 			JniHelper::Get()->CallStaticVoidMethod( CinderCamera::sCinderCameraClass, CinderCamera::sUnlockPixelsMethodId );
// 		}

// /*
// 		// Now process
// 		if( hasPixels ) {
// 			const uint8_t * src = (const uint8_t *)mBuffer.getData();
// 			uint8_t * dst = (uint8_t *)result.getData();
// 			ConvertYUV2RGB( src, src + (mWidth*mHeight), dst, mWidth, mHeight );			
// 		}
// */		
// 	}

// 	return result;
// }

// }} // namespace cinder::android