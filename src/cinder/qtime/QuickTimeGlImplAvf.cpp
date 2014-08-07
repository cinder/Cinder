/*
 Copyright (c) 2014, The Cinder Project, All rights reserved.
 
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

#include "cinder/Cinder.h"

// This path is not used on 64-bit Mac or Windows. On the Mac we only use this path for >=Mac OS 10.8
#if ( defined( CINDER_MAC ) && ( MAC_OS_X_VERSION_MIN_REQUIRED >= 1080 ) ) || ( defined( CINDER_MSW ) && ( ! defined( _WIN64 ) ) )

#include "cinder/qtime/QuickTimeGlImplAvf.h"
#include "cinder/app/App.h"

namespace cinder { namespace qtime {
/////////////////////////////////////////////////////////////////////////////////
// MovieGl
MovieGl::MovieGl( const Url& url ) : MovieBase(), mVideoTextureRef(NULL), mVideoTextureCacheRef(NULL)
{
	MovieBase::initFromUrl( url );
}

MovieGl::MovieGl( const fs::path& path ) : MovieBase(), mVideoTextureRef(NULL), mVideoTextureCacheRef(NULL)
{
	MovieBase::initFromPath( path );
}
	
MovieGl::MovieGl( const MovieLoader& loader ) : MovieBase(), mVideoTextureRef(NULL), mVideoTextureCacheRef(NULL)
{
	MovieBase::initFromLoader(loader);
}
		
MovieGl::~MovieGl()
{
	deallocateVisualContext();
}
	
bool MovieGl::hasAlpha() const
{
	if (!mVideoTextureRef) return false;
	
	CVPixelBufferLockBaseAddress( mVideoTextureRef, 0 );
	OSType type = CVPixelBufferGetPixelFormatType(mVideoTextureRef);
	CVPixelBufferUnlockBaseAddress( mVideoTextureRef, 0 );
#if defined ( CINDER_COCOA_TOUCH)
	return (type == kCVPixelFormatType_32ARGB ||
			type == kCVPixelFormatType_32BGRA ||
			type == kCVPixelFormatType_32ABGR ||
			type == kCVPixelFormatType_32RGBA ||
			type == kCVPixelFormatType_64ARGB);
#elif defined ( CINDER_COCOA )
	return (type == k32ARGBPixelFormat || type == k32BGRAPixelFormat);
#endif
	
	/*
	CGColorSpaceRef color_space = CVImageBufferGetColorSpace(mVideoTextureRef);
	size_t components = CGColorSpaceGetNumberOfComponents(color_space);
	return components > 3;
	*/
}

gl::TextureRef MovieGl::getTexture()
{
	updateFrame();
	
	lock();
	gl::TextureRef result = mTexture;
	unlock();
	
	return result;
}
	
void MovieGl::allocateVisualContext()
{
	if(mVideoTextureCacheRef == NULL) {
		CVReturn err = nil;
#if defined( CINDER_COCOA_TOUCH )
		app::RendererGl *renderer = dynamic_cast<app::RendererGl*>( app::App::get()->getRenderer().get() );
		if( renderer ) {
			EAGLContext* context = renderer->getEaglContext();
			err = CVOpenGLESTextureCacheCreate( kCFAllocatorDefault, NULL, context, NULL, &mVideoTextureCacheRef );
		}
		else {
			throw AvfTextureErrorExc();
		}
		
#elif defined( CINDER_COCOA )
		CGLContextObj context = app::App::get()->getRenderer()->getCglContext();
		CGLPixelFormatObj pixelFormat = app::App::get()->getRenderer()->getCglPixelFormat();
		err = CVOpenGLTextureCacheCreate(kCFAllocatorDefault, NULL, context, pixelFormat, NULL, &mVideoTextureCacheRef);
		CVOpenGLTextureCacheRetain(mVideoTextureCacheRef);
		
#endif
		if( err )
			throw AvfTextureErrorExc();
	}
}

void MovieGl::deallocateVisualContext()
{
	if(mVideoTextureRef) {
		CFRelease(mVideoTextureRef);
		mVideoTextureRef = NULL;
	}
	
	if(mVideoTextureCacheRef) {
#if defined( CINDER_COCOA_TOUCH )
		CVOpenGLESTextureCacheFlush(mVideoTextureCacheRef, 0);
#elif defined( CINDER_COCOA )
		CVOpenGLTextureCacheFlush(mVideoTextureCacheRef, 0);
#endif
		CFRelease(mVideoTextureCacheRef);
		mVideoTextureCacheRef = NULL;
	}
}

void MovieGl::newFrame( CVImageBufferRef cvImage )
{
	CVPixelBufferLockBaseAddress(cvImage, kCVPixelBufferLock_ReadOnly);
	
	if (mVideoTextureRef) {
		CFRelease(mVideoTextureRef);
		mVideoTextureRef = NULL;
	}
#if defined( CINDER_COCOA_TOUCH )
	CVOpenGLESTextureCacheFlush(mVideoTextureCacheRef, 0); // Periodic texture cache flush every frame
#elif defined( CINDER_COCOA )
	CVOpenGLTextureCacheFlush(mVideoTextureCacheRef, 0); // Periodic texture cache flush every frame
#endif
	
	CVReturn err = nil;
	
#if defined( CINDER_COCOA_TOUCH )
	err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,     // CFAllocatorRef allocator
													   mVideoTextureCacheRef,   // CVOpenGLESTextureCacheRef textureCache
													   cvImage,                 // CVImageBufferRef sourceImage
													   NULL,                    // CFDictionaryRef textureAttributes
													   GL_TEXTURE_2D,           // GLenum target
													   GL_RGBA,                 // GLint internalFormat
													   mWidth,                  // GLsizei width
													   mHeight,                 // GLsizei height
													   GL_BGRA,                 // GLenum format
													   GL_UNSIGNED_BYTE,        // GLenum type
													   0,                       // size_t planeIndex
													   &mVideoTextureRef);      // CVOpenGLESTextureRef *textureOut
	
#elif defined( CINDER_MAC )
	err = CVOpenGLTextureCacheCreateTextureFromImage(kCFAllocatorDefault,       // CFAllocatorRef allocator
													 mVideoTextureCacheRef,     // CVOpenGLESTextureCacheRef textureCache
													 cvImage,                   // CVImageBufferRef sourceImage
													 NULL,                      // CFDictionaryRef textureAttributes
													 &mVideoTextureRef);        // CVOpenGLTextureRef *textureOut
#endif
	
	if (err) {
		throw AvfTextureErrorExc();
		return;
	}
	
#if defined( CINDER_COCOA_TOUCH )
	GLenum target = CVOpenGLESTextureGetTarget( mVideoTextureRef );
	GLuint name = CVOpenGLESTextureGetName( mVideoTextureRef );
	bool flipped = !CVOpenGLESTextureIsFlipped( mVideoTextureRef );
	mTexture = gl::Texture::create( target, name, mWidth, mHeight, true );
	Vec2f t0, lowerRight, t2, upperLeft;
	::CVOpenGLESTextureGetCleanTexCoords( mVideoTextureRef, &t0.x, &lowerRight.x, &t2.x, &upperLeft.x );
//	mTexture.setCleanTexCoords( std::max( upperLeft.x, lowerRight.x ), std::max( upperLeft.y, lowerRight.y ) );
//	mTexture.setFlipped( flipped );
#elif defined( CINDER_MAC )	
	GLenum target = CVOpenGLTextureGetTarget( mVideoTextureRef );
	GLuint name = CVOpenGLTextureGetName( mVideoTextureRef );
	bool flipped = ! CVOpenGLTextureIsFlipped( mVideoTextureRef );
	mTexture = gl::Texture::create( target, name, mWidth, mHeight, true );
	Vec2f t0, lowerRight, t2, upperLeft;
	::CVOpenGLTextureGetCleanTexCoords( mVideoTextureRef, &t0.x, &lowerRight.x, &t2.x, &upperLeft.x );
//	mTexture.setCleanTexCoords( std::max( upperLeft.x, lowerRight.x ), std::max( upperLeft.y, lowerRight.y ) );
//	mTexture.setFlipped( flipped );
	//mTexture.setDeallocator( CVPixelBufferDealloc, mVideoTextureRef );	// do we want to do this?
	
#endif

	::CVPixelBufferUnlockBaseAddress( cvImage, kCVPixelBufferLock_ReadOnly );
	::CVPixelBufferRelease( cvImage );
}

void MovieGl::releaseFrame()
{
	mTexture.reset();
}
	
} } // namespace cinder::qtime

#endif