/*
 Copyright (c) 2013, The Cinder Project, All rights reserved.
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

#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"

// forward declarations
namespace cinder { namespace gl {
	typedef std::shared_ptr<class Texture2d>		Texture2dRef;
	typedef Texture2dRef							TextureRef;
} }

namespace cinder { namespace gl {

class CI_API ShaderDef {
  public:
	ShaderDef();

	ShaderDef&		color();	
	ShaderDef&		texture( const TextureRef &tex = TextureRef() );
	ShaderDef&		texture( GLenum target );
	ShaderDef&		lambert();
	// Used by draw(TextureRef&) stock shader; scales ciPosition and ciTexCoord according to
	// uniform "uPositionScale", "uPositionOffset", "uTexCoord0Scale", "uTexCoord0Offset"
	ShaderDef&		uniformBasedPosAndTexCoord();

	bool			isTextureSwizzleDefault() const;
	std::string		getTextureSwizzleString() const;	

	bool operator<( const ShaderDef &rhs ) const;
	
  protected:
	bool					mTextureMapping;
	bool					mTextureMappingRectangleArb;
#if defined( CINDER_ANDROID )
	bool 					mTextureMappingExternalOes;
#endif	
	std::array<GLint,4>		mTextureSwizzleMask;
	bool					mUniformBasedPosAndTexCoord;

	bool					mColor;
	bool					mLambert;
	
	friend class EnvironmentCore;
	friend class EnvironmentEs;
};
	
} } // namespace cinder::gl
