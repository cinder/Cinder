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

// This file is only meant to be included by QuickTimeGl.h
#pragma once

#include "cinder/gl/platform.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/gl/Texture.h"

namespace cinder { namespace qtime {

class MovieGl;
typedef std::shared_ptr<MovieGl>	MovieGlRef;
/** \brief QuickTime movie playback as OpenGL textures
 *	Textures are always bound to the \c GL_TEXTURE_RECTANGLE_ARB target
 *	\remarks On Mac OS X, the destination CGLContext must be the current context when the MovieGl is constructed. If that doesn't mean anything to you, you should be fine. A call to app::restoreWindowContext() can be used to force this to be the case.
**/
class MovieGl : public MovieBase {
  public:
	static MovieGlRef create( const fs::path &path ) { return std::shared_ptr<MovieGl>( new MovieGl( path ) ); }
	static MovieGlRef create( const MovieLoaderRef &loader );
	static MovieGlRef create( const void *data, size_t dataSize, const std::string &fileNameHint, const std::string &mimeTypeHint = "" )
		 { return std::shared_ptr<MovieGl>( new MovieGl( data, dataSize, fileNameHint, mimeTypeHint ) ); }
	static MovieGlRef create( DataSourceRef dataSource, const std::string mimeTypeHint = "" )
		 { return std::shared_ptr<MovieGl>( new MovieGl( dataSource, mimeTypeHint ) ); }

	//! Returns the gl::Texture representing the Movie's current frame, bound to the \c GL_TEXTURE_RECTANGLE_ARB target
	const gl::TextureRef	getTexture();

  protected:
	MovieGl() : MovieBase() {}
	MovieGl( const fs::path &path );
	MovieGl( const class MovieLoader &loader );
	//! Constructs a MovieGl from a block of memory of size \a dataSize pointed to by \a data, which must not be disposed of during the lifetime of the movie.
	/** \a fileNameHint and \a mimeTypeHint provide important hints to QuickTime about the contents of the file. Omit both of them at your peril. "video/quicktime" is often a good choice for \a mimeTypeHint. **/
	MovieGl( const void *data, size_t dataSize, const std::string &fileNameHint, const std::string &mimeTypeHint = "" );
	MovieGl( DataSourceRef dataSource, const std::string mimeTypeHint = "" );

	void				allocateVisualContext();

	struct Obj : public MovieBase::Obj {
		Obj();
		~Obj();

		virtual void		releaseFrame();
		virtual void		newFrame( CVImageBufferRef cvImage );
		
		gl::TextureRef			mTexture;
#if defined( CINDER_MSW )
		gl::Texture2dCacheRef	mTextureCache;
#endif
	};
 	
	std::shared_ptr<Obj>				mObj;
	virtual MovieBase::Obj*		getObj() const { return mObj.get(); }

  public:
	//@{
	//! Emulates shared_ptr-like behavior
	typedef std::shared_ptr<Obj> MovieGl::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &MovieGl::mObj; }
	void reset() { mObj.reset(); }
	//@}  
};

} } // namespace cinder::qtime