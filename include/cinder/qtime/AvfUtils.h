/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.
 
 This code is intended for use with the Cinder C++ library: http://libcinder.org
 
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

// This path is used on iOS or Mac OS X 10.8+
#if defined( CINDER_COCOA_TOUCH ) || ( defined( CINDER_MAC ) && ( MAC_OS_X_VERSION_MIN_REQUIRED >= 1080 ) )

#include "cinder/Cinder.h"
#include "cinder/Url.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"

#include <string>

#if defined( CINDER_COCOA )
	#include <CoreVideo/CoreVideo.h>
#endif

namespace cinder { namespace qtime {

bool setAudioSessionModes();
bool dictionarySetValue( CFMutableDictionaryRef dict, CFStringRef key, SInt32 value );
bool dictionarySetPixelBufferPixelFormatType( bool alpha, CFMutableDictionaryRef dict );
bool dictionarySetPixelBufferSize( const unsigned int width, const unsigned int height, CFMutableDictionaryRef dict );
bool dictionarySetPixelBufferBytesPerRowAlignment( CFMutableDictionaryRef dict );
void dictionarySetPixelBufferOpenGLCompatibility( CFMutableDictionaryRef dict );
bool dictionarySetPixelBufferOptions( unsigned int width, unsigned int height, bool alpha, CFMutableDictionaryRef *pixelBufferOptions );
CFMutableDictionaryRef initQTVisualContextOptions( int width, int height, bool alpha );

//! Designed to be the deallocator for surfaces returned by convertToPixelBufferToSurface
void CVPixelBufferDealloc( void *refcon );
//! Makes a cinder::Surface form a CVPixelBufferRef, setting a proper deallocation function to free the CVPixelBufferRef upon the destruction of the Surface::Obj
Surface8uRef convertCvPixelBufferToSurface( CVPixelBufferRef pixelBufferRef );

typedef std::shared_ptr<class ImageTargetCvPixelBuffer> ImageTargetCvPixelBufferRef;

class ImageTargetCvPixelBuffer : public cinder::ImageTarget {
  public:
	static ImageTargetCvPixelBufferRef createRef( ImageSourceRef imageSource, bool convertToYpCbCr = false );
	static ImageTargetCvPixelBufferRef createRef( ImageSourceRef imageSource, CVPixelBufferPoolRef pbPool );
	~ImageTargetCvPixelBuffer();

	virtual void*		getRowPointer( int32_t row );
	virtual void		finalize();

	::CVPixelBufferRef	getCvPixelBuffer() const { return mPixelBufferRef; }

  protected:
	ImageTargetCvPixelBuffer( ImageSourceRef imageSource, bool convertToYpCbCr );
	ImageTargetCvPixelBuffer( ImageSourceRef imageSource, CVPixelBufferPoolRef pbPool );
	
	void		convertDataToYpCbCr();
	void		convertDataToAYpCbCr();

	::CVPixelBufferRef	mPixelBufferRef;
	size_t				mRowBytes;
	uint8_t				*mData;
	bool				mConvertToYpCbCr;
};

//! Creates a CVPixelBufferRef from an ImageSource. Release the result with CVPixelBufferRelease(). If \a convertToYpCbCr the resulting CVPixelBuffer will be in either \c k444YpCbCr8CodecType or \c k4444YpCbCrA8PixelFormat
CVPixelBufferRef createCvPixelBuffer( ImageSourceRef imageSource, bool convertToYpCbCr = false );

CVPixelBufferRef createCvPixelBuffer( ImageSourceRef imageSource, CVPixelBufferPoolRef pbPool );

} } // namespace cinder::qtime

#endif
