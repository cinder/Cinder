/*
 Copyright (c) 2013, The Cinder Project: http://libcinder.org All rights reserved.
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

// This path is not used on 64-bit Mac or Windows. On the Mac we only use this path for <=Mac OS 10.7
#include "cinder/Cinder.h"

#if ( defined( CINDER_MAC ) && ( ! defined( __LP64__ ) ) && ( MAC_OS_X_VERSION_MIN_REQUIRED < 1080 ) ) || ( defined( CINDER_MSW ) && ( ! defined( _WIN64 ) ) )

#include "cinder/qtime/QuickTimeGlImplLegacy.h"
#include "cinder/qtime/QuickTimeUtils.h"

#if defined( CINDER_MSW )
	#pragma push_macro( "__STDC_CONSTANT_MACROS" )
	#pragma push_macro( "_STDINT_H" )
		#undef __STDC_CONSTANT_MACROS
		#if _MSC_VER >= 1600 // VC10 or greater
			#define _STDINT_H
			#define __FP__
		#endif
		#include <QTML.h>
		#include <CVPixelBuffer.h>
		#include <ImageCompression.h>
		#include <Movies.h>
		#include <GXMath.h>
	#pragma pop_macro( "_STDINT_H" )
	#pragma pop_macro( "__STDC_CONSTANT_MACROS" )
	// this call is improperly defined as Mac-only in the headers
	extern "C" {
	EXTERN_API_C( OSStatus ) QTPixelBufferContextCreate( CFAllocatorRef, CFDictionaryRef, QTVisualContextRef* );
	}
#endif

namespace cinder { namespace qtime {

MovieGlRef MovieGl::create( const MovieLoaderRef &loader )
{
	return std::shared_ptr<MovieGl>( new MovieGl( *loader ) );
}

MovieGl::Obj::Obj()
	: MovieBase::Obj()
{
}

MovieGl::Obj::~Obj()
{
	// see note on prepareForDestruction()
	prepareForDestruction();
}

MovieGl::MovieGl( const MovieLoader &loader )
	: MovieBase(), mObj( new Obj() )
{
	MovieBase::initFromLoader( loader );
	allocateVisualContext();
}

MovieGl::MovieGl( const fs::path &path )
	: MovieBase(), mObj( new Obj() )
{
	MovieBase::initFromPath( path );
	allocateVisualContext();
}

MovieGl::MovieGl( const void *data, size_t dataSize, const std::string &fileNameHint, const std::string &mimeTypeHint )
	: MovieBase(), mObj( new Obj() )
{
	MovieBase::initFromMemory( data, dataSize, fileNameHint, mimeTypeHint );
	allocateVisualContext();	
}

MovieGl::MovieGl( DataSourceRef dataSource, const std::string mimeTypeHint )
	: MovieBase(), mObj( new Obj() )
{
	MovieBase::initFromDataSource( dataSource, mimeTypeHint );
	allocateVisualContext();
}

void MovieGl::allocateVisualContext()
{
#if defined( CINDER_MAC )
	CGLContextObj cglContext = ::CGLGetCurrentContext();
	CGLPixelFormatObj cglPixelFormat = ::CGLGetPixelFormat( cglContext );

	// Creates a new OpenGL texture context for a specified OpenGL context and pixel format
	::QTOpenGLTextureContextCreate( kCFAllocatorDefault, cglContext, cglPixelFormat, NULL, (QTVisualContextRef*)&getObj()->mVisualContext );
	::SetMovieVisualContext( getObj()->mMovie, (QTVisualContextRef)getObj()->mVisualContext );
#else
	CFMutableDictionaryRef visualContextOptions = initQTVisualContextOptions( getObj()->mWidth, getObj()->mHeight, hasAlpha() );
	::QTPixelBufferContextCreate( kCFAllocatorDefault, visualContextOptions, &(getObj()->mVisualContext) );
	::CFRelease( visualContextOptions );

	::SetMovieVisualContext( getObj()->mMovie, getObj()->mVisualContext );
#endif
}

#if defined( CINDER_MAC )
static void CVOpenGLTextureDealloc( gl::Texture *texture, void *refcon )
{
	CVOpenGLTextureRelease( (CVImageBufferRef)(refcon) );
	delete texture;
}

#endif // defined( CINDER_MAC )

void MovieGl::Obj::releaseFrame()
{
	mTexture.reset();
}

void MovieGl::Obj::newFrame( CVImageBufferRef cvImage )
{
#if defined( CINDER_MAC )
	CVOpenGLTextureRef imgRef = reinterpret_cast<CVOpenGLTextureRef>( cvImage );
	GLenum target = ::CVOpenGLTextureGetTarget( imgRef );
	GLuint name = ::CVOpenGLTextureGetName( imgRef );
	bool flipped = ::CVOpenGLTextureIsFlipped( imgRef );
	mTexture = gl::Texture2d::create( target, name, mWidth, mHeight, true, std::bind( CVOpenGLTextureDealloc, std::placeholders::_1, imgRef ) );
	vec2 t0, lowerRight, t2, upperLeft;
	::CVOpenGLTextureGetCleanTexCoords( imgRef, &t0.x, &lowerRight.x, &t2.x, &upperLeft.x );
	mTexture->setCleanSize( std::max( upperLeft.x, lowerRight.x ), std::max( upperLeft.y, lowerRight.y ) );
	mTexture->setTopDown( flipped );
#else
	// on Windows this is actually a CVPixelBufferRef, which we will convert into a texture
	CVPixelBufferRef imgRef = reinterpret_cast<CVPixelBufferRef>( cvImage );
	CVPixelBufferLockBaseAddress( imgRef, 0 );
	uint8_t *ptr = reinterpret_cast<uint8_t*>( CVPixelBufferGetBaseAddress( imgRef ) );
	
	int32_t rowBytes = CVPixelBufferGetBytesPerRow( imgRef );
	OSType type = CVPixelBufferGetPixelFormatType( imgRef );
	size_t width = CVPixelBufferGetWidth( imgRef );
	size_t height = CVPixelBufferGetHeight( imgRef );
	SurfaceChannelOrder sco = SurfaceChannelOrder::BGRA;
	if( type == k24RGBPixelFormat )
		sco = SurfaceChannelOrder::RGB;
	else if( type == k32ARGBPixelFormat )
		sco = SurfaceChannelOrder::ARGB;
	else if( type == k24BGRPixelFormat )
		sco = SurfaceChannelOrder::BGR;
	else if( type == k32BGRAPixelFormat )
		sco = SurfaceChannelOrder::BGRA;
	
	if( ! mTextureCache ) {
		gl::Texture::Format format;
#if ! defined( CINDER_GL_ES )
		format.setTargetRect();
#endif
		format.loadTopDown();
		mTextureCache = gl::Texture2dCache::create( Surface8u( ptr, width, height, rowBytes, sco ), format );
	}
	mTexture = mTextureCache->cache( Surface8u( ptr, width, height, rowBytes, sco ) );
	
	::CVBufferRelease( imgRef );
#endif
}

const gl::TextureRef MovieGl::getTexture()
{
	updateFrame();

	mObj->lock();
		gl::TextureRef result = mObj->mTexture;
	mObj->unlock();

	return result;
}


} } // namespace cinder::qtime

#endif // ! defined( __LP64__ )