/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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

#include "cinder/Cinder.h"
#include "cinder/Url.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"

#include <string>

#if defined( CINDER_MAC )
	#if ! defined( __LP64__ )
		#include <QuickTime/QuickTime.h>
	#else
		#include <CoreVideo/CoreVideo.h>
	#endif
#else
	#pragma push_macro( "__STDC_CONSTANT_MACROS" )
	#pragma push_macro( "_STDINT_H" )
		#undef __STDC_CONSTANT_MACROS
		#if _MSC_VER >= 1600 // VC10 or greater
			#define _STDINT_H
		#endif
		#include <QTML.h>
		#include <Movies.h>
	#pragma pop_macro( "_STDINT_H" )
	#pragma pop_macro( "__STDC_CONSTANT_MACROS" )
#endif

namespace cinder { namespace qtime {

#if ! defined( __LP64__ )
//! \cond
bool dictionarySetValue( CFMutableDictionaryRef dict, CFStringRef key, SInt32 value );
bool dictionarySetPixelBufferPixelFormatType( bool alpha, CFMutableDictionaryRef dict );
bool dictionarySetPixelBufferSize( const unsigned int width, const unsigned int height, CFMutableDictionaryRef dict );
bool dictionarySetPixelBufferBytesPerRowAlignment( CFMutableDictionaryRef dict );
void dictionarySetPixelBufferOpenGLCompatibility( CFMutableDictionaryRef dict );
bool dictionarySetPixelBufferOptions( unsigned int width, unsigned int height, bool alpha, CFMutableDictionaryRef *pixelBufferOptions );
CFMutableDictionaryRef initQTVisualContextOptions( int width, int height, bool alpha );

::Movie openMovieFromUrl( const Url &url );
::Movie openMovieFromPath( const std::string &path );
::Movie openMovieFromMemory( const void *data, size_t dataSize, const std::string &fileNameHint, const std::string &mimeTypeHint );

//! \endcond

//! Designed to be the deallocator for surfaces returned by convertToPixelBufferToSurface
static void CVPixelBufferDealloc( void *refcon );
//! Makes a cinder::Surface form a CVPixelBufferRef, setting a proper deallocation function to free the CVPixelBufferRef upon the destruction of the Surface::Obj
Surface8u convertCVPixelBufferToSurface( CVPixelBufferRef pixelBufferRef );

#endif // ! defined( __LP64__ )

typedef shared_ptr<class ImageTargetCvPixelBuffer> ImageTargetCvPixelBufferRef;

class ImageTargetCvPixelBuffer : public ci::ImageTarget {
  public:
	static ImageTargetCvPixelBufferRef createRef( ImageSourceRef imageSource, bool convertToYpCbCr = false );
	~ImageTargetCvPixelBuffer();

	virtual void*		getRowPointer( int32_t row );
	virtual void		finalize();

	::CVPixelBufferRef	getCvPixelBuffer() const { return mPixelBufferRef; }

  protected:
	ImageTargetCvPixelBuffer( ImageSourceRef imageSource, bool convertToYpCbCr );
	
	void		convertDataToYpCbCr();
	void		convertDataToAYpCbCr();

	::CVPixelBufferRef	mPixelBufferRef;
	size_t				mRowBytes;
	uint8_t				*mData;
	bool				mConvertToYpCbCr;
};

//! Creates a CVPixelBufferRef from an ImageSource. Release the result with CVPixelBufferRelease(). If \a convertToYpCbCr the resulting CVPixelBuffer will be in either \c k444YpCbCr8CodecType or \c k4444YpCbCrA8PixelFormat
CVPixelBufferRef createCvPixelBuffer( ImageSourceRef imageSource, bool convertToYpCbCr = false );

#if ! defined( __LP64__ )
typedef shared_ptr<class ImageTargetGWorld> ImageTargetGWorldRef;

class ImageTargetGWorld : public ci::ImageTarget {
  public:
	static ImageTargetGWorldRef createRef( ImageSourceRef imageSource );

	virtual void*		getRowPointer( int32_t row );
	virtual void		finalize();

	::GWorldPtr			getGWorld() const { return mGWorld; }

  protected:
	ImageTargetGWorld( ImageSourceRef imageSource );
	
	::GWorldPtr			mGWorld;
	::PixMapHandle		mPixMap;
	size_t				mRowBytes;
	uint8_t				*mData;
};

//! Creates a GWorld from an ImageSource. Currently always creates a 32-bit RGBA GWorld. Dispose of the results using DisposeGWorld().
GWorldPtr createGWorld( ImageSourceRef imageSource );
#endif // defined( __LP64__ )

} } // namespace cinder::qtime
