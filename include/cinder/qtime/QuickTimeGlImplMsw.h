/*
 Copyright (c) 2025, The Cinder Project, All rights reserved.

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

 Windows 64-bit Media Foundation GL implementation.
 Based on AX-MediaPlayer by Andrew Wright (@axjxwright).

 Hardware acceleration uses WGL_NV_DX_interop for D3D11/OpenGL texture sharing.
 Falls back to WIC/CPU path if hardware acceleration is unavailable.
 */

// This file is only meant to be included by QuickTimeGl.h
#pragma once

#include "cinder/Cinder.h"
#include "cinder/qtime/QuickTimeImplMsw.h"
#include "cinder/gl/Texture.h"

namespace cinder { namespace qtime {

typedef std::shared_ptr<class MovieGl>	MovieGlRef;

/** \brief Media Foundation movie playback as OpenGL textures (Windows 64-bit)
 *
 *  Uses IMFMediaEngine with optional hardware-accelerated D3D11/GL interop.
 *  When hardware acceleration is available, textures are shared directly
 *  between D3D11 and OpenGL via WGL_NV_DX_interop extension.
 *
 *  \remarks The destination OpenGL context must be current when MovieGl is constructed.
 *  A call to app::restoreWindowContext() can be used to force this.
 **/
class MovieGl : public MovieBase {
  public:
	virtual ~MovieGl();

	static MovieGlRef create( const Url& url ) { return MovieGlRef( new MovieGl( url ) ); }
	static MovieGlRef create( const fs::path& path ) { return MovieGlRef( new MovieGl( path ) ); }
	static MovieGlRef create( const MovieLoaderRef &loader ) { return MovieGlRef( new MovieGl( *loader ) ); }

	//! Returns the gl::Texture representing the Movie's current frame
	//! \note Texture is valid until the next call to getTexture() or until the MovieGl is destroyed
	gl::TextureRef	getTexture();

	//! Returns whether hardware acceleration (D3D11/GL interop) is being used
	bool isHardwareAccelerated() const { return mHardwareAccelerated; }

  protected:
	MovieGl( const Url& url );
	MovieGl( const fs::path& path );
	MovieGl( const MovieLoader& loader );

	void initGl( bool hardwareAccelerated = true );
	void initFromUrl( const Url& url );
	void initFromPath( const fs::path& filePath );

	gl::TextureRef		mTexture;
	bool				mHardwareAccelerated;

	//! Opaque holder for the current frame lease - keeps texture locked during draw
	struct FrameLeaseHolder;
	std::unique_ptr<FrameLeaseHolder>	mFrameLeaseHolder;
};

} } // namespace cinder::qtime
