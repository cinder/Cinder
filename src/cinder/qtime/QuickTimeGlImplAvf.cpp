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
#include "cinder/gl/scoped.h"

// This path is not used on 64-bit Mac or Windows. On the Mac we only use this path for >=Mac OS 10.8
#if ( defined( CINDER_MAC ) && ( MAC_OS_X_VERSION_MIN_REQUIRED >= 1080 ) ) || ( defined( CINDER_MSW ) && ( ! defined( _WIN64 ) ) ) || defined( CINDER_COCOA_TOUCH )

#include "cinder/qtime/QuickTimeGlImplAvf.h"
#include "cinder/app/AppBase.h"
#include "cinder/app/RendererGl.h"

#include <CoreVideo/CoreVideo.h>
#include <CoreVideo/CVBase.h>

#import <Foundation/NSDictionary.h>
#import <Foundation/NSValue.h>
#if defined( CINDER_MAC )
	#include <CoreVideo/CVOpenGLTextureCache.h>
	#include <CoreVideo/CVOpenGLTexture.h>
#else
	#include <CoreVideo/CVOpenGLESTextureCache.h>
#endif

namespace cinder { namespace qtime {

/////////////////////////////////////////////////////////////////////////////////
// MovieGl::TextureCache
#if defined( CINDER_MAC )
class MovieGl::TextureCache : public std::enable_shared_from_this<MovieGl::TextureCache> {
  public:
	~TextureCache();
	gl::TextureRef		add( CVImageBufferRef cvImage );
	void				remove( GLuint texId );
	
  private:
	std::vector<GLuint>		mTextures;
};

MovieGl::TextureCache::~TextureCache()
{
	for( auto &texId : mTextures )
		glDeleteTextures( 1, &texId );
}

gl::TextureRef MovieGl::TextureCache::add( CVImageBufferRef cvImage )
{
	IOSurfaceRef ioSurface = ::CVPixelBufferGetIOSurface( cvImage );

	::IOSurfaceIncrementUseCount( ioSurface );
	GLsizei texWidth = (GLsizei)::IOSurfaceGetWidth( ioSurface );
	GLsizei texHeight = (GLsizei)::IOSurfaceGetHeight( ioSurface );

	// find an available id or generate one if necessary
	if( mTextures.empty() ) {
		GLuint newId;
		glGenTextures( 1, &newId );
		mTextures.push_back( newId );
	}
	GLuint texId = mTextures.back();
	mTextures.pop_back();

	auto sharedThis = shared_from_this();
	auto deleter = [cvImage, ioSurface, sharedThis] ( gl::Texture *texture ) {
		::IOSurfaceDecrementUseCount( ioSurface );
		sharedThis->remove( texture->getId() );
		delete texture;
		::CVPixelBufferRelease( cvImage );
	};
		
	auto result = gl::Texture2d::create( GL_TEXTURE_RECTANGLE, texId, texWidth, texHeight, true, deleter );
	result->setTopDown( true );

	gl::ScopedTextureBind bind( result );
	CGLTexImageIOSurface2D( app::AppBase::get()->getRenderer()->getCglContext(), GL_TEXTURE_RECTANGLE, GL_RGBA, texWidth, texHeight, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, ioSurface, 0);        
	
	return result;
}

void MovieGl::TextureCache::remove( GLuint texId )
{
	mTextures.push_back( texId );
}

#endif

/////////////////////////////////////////////////////////////////////////////////
// MovieGl
MovieGl::MovieGl( const Url& url )
#if defined( CINDER_COCOA_TOUCH )
	: mVideoTextureRef( nullptr ), mVideoTextureCacheRef( nullptr )
#else
	: mTextureCache( new TextureCache() )
#endif
{
	MovieBase::initFromUrl( url );
}

MovieGl::MovieGl( const fs::path& path )
#if defined( CINDER_COCOA_TOUCH )
	: mVideoTextureRef( nullptr ), mVideoTextureCacheRef( nullptr )
#else
	: mTextureCache( new TextureCache() )
#endif
{
	MovieBase::initFromPath( path );
}
	
MovieGl::MovieGl( const MovieLoader &loader )
#if defined( CINDER_COCOA_TOUCH )
	: mVideoTextureRef( nullptr ), mVideoTextureCacheRef( nullptr )
#else
	: mTextureCache( new TextureCache() )
#endif
{
	MovieBase::initFromLoader( loader );
}
		
MovieGl::~MovieGl()
{
	deallocateVisualContext();
}

gl::TextureRef MovieGl::getTexture()
{
	updateFrame();
	
	lock();
	gl::TextureRef result = mTexture;
	unlock();
	
	return result;
}

NSDictionary* MovieGl::avPlayerItemOutputDictionary() const
{
	return [NSDictionary dictionaryWithObjectsAndKeys:
				[NSNumber numberWithInt:kCVPixelFormatType_32BGRA], kCVPixelBufferPixelFormatTypeKey,
				[NSDictionary dictionary], kCVPixelBufferIOSurfacePropertiesKey,
				[NSNumber numberWithBool:YES], kCVPixelBufferOpenGLCompatibilityKey, nil];
}
	
void MovieGl::allocateVisualContext()
{
#if defined( CINDER_COCOA_TOUCH )
	if( ! mVideoTextureCacheRef ) {
		CVReturn err = 0;

		app::RendererGl *renderer = dynamic_cast<app::RendererGl*>( app::AppBase::get()->getRenderer().get() );
		if( renderer ) {
			EAGLContext* context = renderer->getEaglContext();
			err = ::CVOpenGLESTextureCacheCreate( kCFAllocatorDefault, NULL, context, NULL, &mVideoTextureCacheRef );
		}
		else {
			throw AvfTextureErrorExc();
		}
	}	
#endif
}

void MovieGl::deallocateVisualContext()
{
#if defined( CINDER_COCOA_TOUCH )
	if( mVideoTextureRef ) {
		::CFRelease( mVideoTextureRef );
		mVideoTextureRef = nullptr;
	}
	
	if( mVideoTextureCacheRef ) {
		::CVOpenGLESTextureCacheFlush( mVideoTextureCacheRef, 0 );
		::CFRelease( mVideoTextureCacheRef );
		mVideoTextureCacheRef = nullptr;
	}
#endif
}

void MovieGl::newFrame( CVImageBufferRef cvImage )
{
#if defined( CINDER_COCOA_TOUCH )
	CVReturn err = 0;
	::CVPixelBufferLockBaseAddress( cvImage, kCVPixelBufferLock_ReadOnly );

	::CVOpenGLESTextureCacheFlush( mVideoTextureCacheRef, 0 ); // Periodic texture cache flush every frame

	CVOpenGLESTextureRef videoTextureRef;
	err = ::CVOpenGLESTextureCacheCreateTextureFromImage( kCFAllocatorDefault,		 // CFAllocatorRef allocator
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
															&videoTextureRef );      // CVOpenGLESTextureRef *textureOut
	
	if( err )
		throw AvfTextureErrorExc();
	
	GLenum target = ::CVOpenGLESTextureGetTarget( videoTextureRef );
	GLuint name = ::CVOpenGLESTextureGetName( videoTextureRef );
	bool topDown = ::CVOpenGLESTextureIsFlipped( videoTextureRef );
	
	// custom deleter fires when last reference to Texture goes out of scope
	auto deleter = [cvImage, videoTextureRef] ( gl::Texture *texture ) {
		::CVPixelBufferUnlockBaseAddress( cvImage, kCVPixelBufferLock_ReadOnly );
		::CVBufferRelease( cvImage );
		::CFRelease( videoTextureRef );
		delete texture;
	};
	
	mTexture = gl::Texture2d::create( target, name, mWidth, mHeight, true, deleter );
	
	// query and set clean bounds
	vec2 lowerLeft, lowerRight, upperRight, upperLeft;
	::CVOpenGLESTextureGetCleanTexCoords( videoTextureRef, &lowerLeft.x, &lowerRight.x, &upperRight.x, &upperLeft.x );
	if( target == GL_TEXTURE_2D )
		mTexture->setCleanBounds( Area( (int32_t)(upperLeft.x * mWidth), (int32_t)(upperLeft.y * mHeight),
			(int32_t)(lowerRight.x * mWidth ), (int32_t)(lowerRight.y * mHeight ) ) );
	else
		mTexture->setCleanBounds( Area( (int32_t)upperLeft.x, (int32_t)upperLeft.y, (int32_t)lowerRight.x, (int32_t)lowerRight.y ) );
	
	mTexture->setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
	mTexture->setTopDown( topDown );
	
#elif defined( CINDER_MAC )

	mTexture = mTextureCache->add( cvImage );

#endif
}

void MovieGl::releaseFrame()
{
	mTexture.reset();
}
	
} } // namespace cinder::qtime

#endif