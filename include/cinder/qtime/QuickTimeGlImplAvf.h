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

// This file is only meant to be included by QuickTimeGl.h
#pragma once

#include "cinder/Cinder.h"
#include "cinder/qtime/QuickTimeImplAvf.h"
#include "cinder/gl/platform.h"

// Forward declarations of CoreVideo types
typedef struct __CVBuffer *CVBufferRef;
typedef CVBufferRef CVImageBufferRef;
#if defined( CINDER_COCOA_TOUCH )
	typedef struct __CVOpenGLESTextureCache *CVOpenGLESTextureCacheRef;
	typedef CVImageBufferRef CVOpenGLESTextureRef;
#elif defined( CINDER_MAC )
	typedef struct __CVOpenGLTextureCache *CVOpenGLTextureCacheRef;
	typedef CVImageBufferRef CVOpenGLTextureRef;
#endif

namespace cinder { namespace qtime {
	
typedef std::shared_ptr<class MovieGl>	MovieGlRef;
/** \brief QuickTime movie playback as OpenGL textures
 *	Textures are always bound to the \c GL_TEXTURE_RECTANGLE_ARB target
 *	\remarks On Mac OS X, the destination CGLContext must be the current context when the MovieGl is constructed. If that doesn't mean anything to you, you should be fine. A call to app::restoreWindowContext() can be used to force this to be the case.
 **/
class MovieGl : public MovieBase {
  public:
	virtual ~MovieGl();
	
	static MovieGlRef create( const Url& url ) { return MovieGlRef( new MovieGl( url ) ); }
	static MovieGlRef create( const fs::path& path ) { return MovieGlRef( new MovieGl( path ) ); }
	static MovieGlRef create( const MovieLoaderRef &loader ) { return MovieGlRef( new MovieGl( *loader ) ); }
	
	//! Returns the gl::Texture representing the Movie's current frame, bound to the \c GL_TEXTURE_RECTANGLE_ARB target
	gl::TextureRef	getTexture();
	
  protected:
	MovieGl( const Url& url );
	MovieGl( const fs::path& path );
	MovieGl( const MovieLoader& loader );
	
	NSDictionary*	avPlayerItemOutputDictionary() const override;
	void			allocateVisualContext() override;
	void			deallocateVisualContext() override;
	void			newFrame( CVImageBufferRef cvImage ) override;
	void			releaseFrame() override;
	
#if defined( CINDER_COCOA_TOUCH )
	CVOpenGLESTextureCacheRef	mVideoTextureCacheRef;
	CVOpenGLESTextureRef		mVideoTextureRef;
#else
	class TextureCache;
	std::shared_ptr<TextureCache>		mTextureCache;
#endif
	
	gl::TextureRef		mTexture;
};

} } // namespace cinder::qtime
