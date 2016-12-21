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

#include "cinder/gl/Shader.h"

#include "cinder/Utilities.h"
#include "cinder/gl/Texture.h"

using namespace std;

namespace cinder { namespace gl {

ShaderDef::ShaderDef()
#if defined( CINDER_ANDROID ) 
	: mTextureMapping( false ), mTextureMappingRectangleArb( false ), mTextureMappingExternalOes( false ), mColor( false ), mLambert(false), mUniformBasedPosAndTexCoord( false )
#else
	: mTextureMapping( false ), mTextureMappingRectangleArb( false ), mColor( false ), mLambert( false ), mUniformBasedPosAndTexCoord( false )
#endif	
{
	mTextureSwizzleMask[0] = GL_RED;
	mTextureSwizzleMask[1] = GL_GREEN; 
	mTextureSwizzleMask[2] = GL_BLUE; 
	mTextureSwizzleMask[3] = GL_ALPHA;
}

ShaderDef& ShaderDef::texture( const TextureRef &texture )
{
	mTextureMapping = true;
#if ! defined( CINDER_GL_ES )
	if( texture && texture->getTarget() == GL_TEXTURE_RECTANGLE_ARB )
		mTextureMappingRectangleArb = true;
#elif defined( CINDER_ANDROID )
	if( texture && ( texture->getTarget() == GL_TEXTURE_EXTERNAL_OES ) ) {
		mTextureMappingExternalOes = true;
	}
#endif
	if( texture && ( ! TextureBase::supportsHardwareSwizzle() ) )
		mTextureSwizzleMask = texture->getSwizzleMask();

	return *this;
}

ShaderDef& ShaderDef::texture( GLenum target )
{
	mTextureMapping = true;
#if ! defined( CINDER_GL_ES )
	if( target == GL_TEXTURE_RECTANGLE_ARB )
		mTextureMappingRectangleArb = true;
#elif defined( CINDER_ANDROID )
	if( target == GL_TEXTURE_EXTERNAL_OES ) {
		mTextureMappingExternalOes = true;
	}
#endif
	return *this;
}

ShaderDef& ShaderDef::uniformBasedPosAndTexCoord()
{
	mUniformBasedPosAndTexCoord = true;
	return *this;
}

ShaderDef& ShaderDef::color()
{
	mColor = true;
	return *this;
}

ShaderDef& ShaderDef::lambert()
{
	mLambert = true;
	return *this;
}

bool ShaderDef::isTextureSwizzleDefault() const
{
	return mTextureSwizzleMask[0] == GL_RED &&
			mTextureSwizzleMask[1] == GL_GREEN && 
			mTextureSwizzleMask[2] == GL_BLUE && 
			mTextureSwizzleMask[3] == GL_ALPHA;
}

// this only works with RGBA values
std::string ShaderDef::getTextureSwizzleString() const
{
	string result;
	for( int i = 0; i < 4; ++i ) {
		if( mTextureSwizzleMask[i] == GL_RED )
			result += "r";
		else if( mTextureSwizzleMask[i] == GL_GREEN )
			result += "g";
		else if( mTextureSwizzleMask[i] == GL_BLUE )
			result += "b";
		else
			result += "a";
	}
	
	return result;
}


bool ShaderDef::operator<( const ShaderDef &rhs ) const
{
	if( rhs.mTextureMapping != mTextureMapping )
		return rhs.mTextureMapping;
#if ! defined( CINDER_GL_ES )
	if( rhs.mTextureMappingRectangleArb != mTextureMappingRectangleArb )
		return rhs.mTextureMappingRectangleArb;
#elif defined( CINDER_ANDROID )
	if( rhs.mTextureMappingExternalOes != mTextureMappingExternalOes )
		return rhs.mTextureMappingExternalOes;
#endif
	if( rhs.mUniformBasedPosAndTexCoord != mUniformBasedPosAndTexCoord )
		return rhs.mUniformBasedPosAndTexCoord;
	if( rhs.mColor != mColor )
		return rhs.mColor;
	else if( rhs.mTextureSwizzleMask[0] != mTextureSwizzleMask[0] )
		return mTextureSwizzleMask[0] < rhs.mTextureSwizzleMask[0];
	else if( rhs.mTextureSwizzleMask[1] != mTextureSwizzleMask[1] )
		return mTextureSwizzleMask[1] < rhs.mTextureSwizzleMask[1];	
	else if( rhs.mTextureSwizzleMask[2] != mTextureSwizzleMask[2] )
		return mTextureSwizzleMask[2] < rhs.mTextureSwizzleMask[2];	
	else if( rhs.mTextureSwizzleMask[3] != mTextureSwizzleMask[3] )
		return mTextureSwizzleMask[3] < rhs.mTextureSwizzleMask[3];	
	if( rhs.mLambert != mLambert )
		return rhs.mLambert;
	
	return false;
}

} } // namespace cinder::gl