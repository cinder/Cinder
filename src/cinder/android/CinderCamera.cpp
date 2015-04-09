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

jclass		CinderCamera::sCinderCameraClass		= nullptr;
jmethodID	CinderCamera::sInitializeMethodId 		= nullptr;
jmethodID 	CinderCamera::sHasFrontCameraMethodId	= nullptr;
jmethodID 	CinderCamera::sHasBackCameraMethodId 	= nullptr;
jmethodID 	CinderCamera::sStartCaptureMethodId 	= nullptr;
jmethodID 	CinderCamera::sStopCaptureMethodId 		= nullptr;
jmethodID 	CinderCamera::sLockPixelsMethodId 		= nullptr;
jmethodID 	CinderCamera::sUnlockPixelsMethodId 	= nullptr;
jmethodID 	CinderCamera::sGetWidthMethodId			= nullptr;
jmethodID 	CinderCamera::sGetHeightMethodId		= nullptr;

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
	const std::string className = "org/libcinder/hardware/CinderCamera";

	if( JniHelper::Get()->AttachCurrentThread() ) {
		jclass cinderCameraClass = JniHelper::Get()->RetrieveClass( className );
		if( nullptr != cinderCameraClass ) {
			CinderCamera::sCinderCameraClass = reinterpret_cast<jclass>( JniHelper::Get()->NewGlobalRef( cinderCameraClass ) );
		}

		if( nullptr != CinderCamera::sCinderCameraClass ) {
			CinderCamera::sInitializeMethodId 		= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "initialize", "()Z" );
			CinderCamera::sHasFrontCameraMethodId	= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "hasFrontCamera", "()Z" );
			CinderCamera::sHasBackCameraMethodId 	= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "hasBackCamera", "()Z" );
			CinderCamera::sStartCaptureMethodId 	= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "startCapture", "()V" );
			CinderCamera::sStopCaptureMethodId 		= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "stopCapture", "()V" );
			CinderCamera::sLockPixelsMethodId 		= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "lockPixels", "()[B" );
			CinderCamera::sUnlockPixelsMethodId 	= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "unlockPixels", "()V" );
			CinderCamera::sGetWidthMethodId 		= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "getWidth", "()I" );
			CinderCamera::sGetHeightMethodId 		= JniHelper::Get()->GetStaticMethodId( CinderCamera::sCinderCameraClass, "getHeight", "()I" );

			FN_LOG( CinderCamera::sInitializeMethodId );	
			FN_LOG( CinderCamera::sHasFrontCameraMethodId );	
			FN_LOG( CinderCamera::sHasBackCameraMethodId );	
			FN_LOG( CinderCamera::sStartCaptureMethodId );	
			FN_LOG( CinderCamera::sStopCaptureMethodId );	
			FN_LOG( CinderCamera::sLockPixelsMethodId );	
			FN_LOG( CinderCamera::sUnlockPixelsMethodId );	
			FN_LOG( CinderCamera::sGetWidthMethodId );	
			FN_LOG( CinderCamera::sGetHeightMethodId );				
		}
	}
}

void CinderCamera::destroyJni()
{
	if( JniHelper::Get()->AttachCurrentThread() ) {
		
		JniHelper::Get()->DeleteGlobalRef( CinderCamera::sCinderCameraClass );

		CinderCamera::sCinderCameraClass		= nullptr;
		CinderCamera::sInitializeMethodId 		= nullptr;
		CinderCamera::sHasFrontCameraMethodId	= nullptr;
		CinderCamera::sHasBackCameraMethodId 	= nullptr;
		CinderCamera::sStartCaptureMethodId 	= nullptr;
		CinderCamera::sStopCaptureMethodId 		= nullptr;
		CinderCamera::sLockPixelsMethodId 		= nullptr;
		CinderCamera::sUnlockPixelsMethodId 	= nullptr;	
		CinderCamera::sGetWidthMethodId 		= nullptr;
		CinderCamera::sGetHeightMethodId 		= nullptr;	
	}
}

bool CinderCamera::initialize()
{
	mInitialized = false;

	if( JniHelper::Get()->AttachCurrentThread() ) {
		mInitialized = (bool)JniHelper::Get()->CallStaticBooleanMethod( CinderCamera::sCinderCameraClass, CinderCamera::sInitializeMethodId );

		if( mInitialized ) {
			mHasFrontCamera = (bool)JniHelper::Get()->CallStaticBooleanMethod( CinderCamera::sCinderCameraClass, CinderCamera::sHasFrontCameraMethodId );
			mHasBackCamera = (bool)JniHelper::Get()->CallStaticBooleanMethod( CinderCamera::sCinderCameraClass, CinderCamera::sHasBackCameraMethodId );
		}
	}

	return mInitialized;	
}

void CinderCamera::startCapture()
{
	if( ! mInitialized ) {
		return;
	}

	if( JniHelper::Get()->AttachCurrentThread() ) {
		JniHelper::Get()->CallStaticVoidMethod( CinderCamera::sCinderCameraClass, CinderCamera::sStartCaptureMethodId );
		
		mWidth = JniHelper::Get()->CallStaticIntMethod( CinderCamera::sCinderCameraClass, CinderCamera::sGetWidthMethodId );
		mHeight = JniHelper::Get()->CallStaticIntMethod( CinderCamera::sCinderCameraClass, CinderCamera::sGetHeightMethodId );

		mCapturing = true;
	}	
}

void CinderCamera::stopCapture()
{
	if( ! mInitialized ) {
		return;
	}

	if( JniHelper::Get()->AttachCurrentThread() ) {
		JniHelper::Get()->CallStaticVoidMethod( CinderCamera::sCinderCameraClass, CinderCamera::sStopCaptureMethodId );

		mCapturing = false;
	}	
}

/*
const uint8_t * CinderCamera::lockPixels()
{
	LOGI("CinderCamera::lockPixels");

	const uint8_t * result = nullptr;

	//LOGI( "Mark 1" );

	if( mCapturing ) {
		//LOGI( "Mark 2" );
		if( JniHelper::Get()->AttachCurrentThread() ) {
			//LOGI( "Mark 3" );
			jbyteArray jBytes = (jbyteArray)JniHelper::Get()->CallStaticObjectMethod( CinderCamera::sCinderCameraClass, CinderCamera::sLockPixelsMethodId );
			mLockedPixels = true;

			if( NULL != jBytes ) {
				//LOGI( "Mark 4" );
				jbyte* dataPtr = (jbyte*)JniHelper::Get()->GetByteArrayElements( jBytes, NULL );
				if( NULL != dataPtr ) {
					//LOGI( "Mark 5" );
					result = (const uint8_t *)dataPtr;
				}
			}
			JniHelper::Get()->DeatchCurrentThread();
		}	
	}

	return result;
}

void CinderCamera::unlockPixels()
{
	LOGI("CinderCamera::unlockPixels");

	if( mLockedPixels ) {
		if( JniHelper::Get()->AttachCurrentThread() ) {
			JniHelper::Get()->CallStaticVoidMethod( CinderCamera::sCinderCameraClass, CinderCamera::sUnlockPixelsMethodId );
			JniHelper::Get()->DeatchCurrentThread();
		}
		mLockedPixels = false;
	}
}

*/

// Conversion from yuv nv21 to rgb24 adapted from
// videonet conversion from YUV420 to RGB24
// http://www.codeguru.com/cpp/g-m/multimedia/video/article.php/c7621
static long int crv_tab[256];
static long int cbu_tab[256];
static long int cgu_tab[256];
static long int cgv_tab[256];
static long int tab_76309[256];
static unsigned char clp[1024];         //for clip in CCIR601

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

    for (i=0; i<384; i++)
       clp[i] =0;
    ind=384;
    for (i=0;i<256; i++)
        clp[ind++]=i;
    ind=640;
    for (i=0;i<384;i++)
        clp[ind++]=255;

    gTableInited = true;
 }

 static void ConvertYUV2RGB(const unsigned char *src0,const unsigned char *src1,unsigned char *dst_ori, int width,int height)
 {
 	if( ! gTableInited ) {
 		InitConvertTable();
 	}

     register int y1,y2,u,v;
     register const unsigned char *py1,*py2;
     register int i,j, c1, c2, c3, c4;
     register unsigned char *d1, *d2;

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


ci::Surface	CinderCamera::getSurface()
{
	ci::Surface result = ci::Surface( mWidth, mHeight, false );

	if( mCapturing ) {
		bool hasPixels = false;

		// Copy pixels
		if( JniHelper::Get()->AttachCurrentThread() ) {
			jbyteArray jBytes = (jbyteArray)JniHelper::Get()->CallStaticObjectMethod( CinderCamera::sCinderCameraClass, CinderCamera::sLockPixelsMethodId );

			if( NULL != jBytes ) {
				size_t dataLength = JniHelper::Get()->GetArrayLength( jBytes );
				jbyte* dataPtr = (jbyte*)JniHelper::Get()->GetByteArrayElements( jBytes, NULL );
				if( ( NULL != dataPtr ) && ( dataLength > 0 ) ) {
					/*
					if( ! mBuffer ) {
						mBuffer = ci::Buffer( dataLength );
					}

					memcpy( mBuffer.getData(), (const void *)dataPtr, dataLength );
					hasPixels = true;
					*/

					const uint8_t * src = (const uint8_t *)dataPtr;
					uint8_t * dst = (uint8_t *)result.getData();
					ConvertYUV2RGB( src, src + (mWidth*mHeight), dst, mWidth, mHeight );					
				}
			}

			JniHelper::Get()->CallStaticVoidMethod( CinderCamera::sCinderCameraClass, CinderCamera::sUnlockPixelsMethodId );
		}

/*
		// Now process
		if( hasPixels ) {
			const uint8_t * src = (const uint8_t *)mBuffer.getData();
			uint8_t * dst = (uint8_t *)result.getData();
			ConvertYUV2RGB( src, src + (mWidth*mHeight), dst, mWidth, mHeight );			
		}
*/		
	}

	return result;
}

}} // namespace cinder::android