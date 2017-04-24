/*
 Copyright (c) 2017, The Cinder Project, All rights reserved.

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

#include "cinder/gl/Sampler.h"
#include "cinder/gl/Environment.h"
#include "cinder/gl/scoped.h"
#include "cinder/gl/Context.h"

#if defined( CINDER_GL_HAS_SAMPLERS )

namespace cinder { namespace gl {

Sampler::Sampler()
	: mId( 0 ), mFormat()
{
}

Sampler::Sampler( const Format &format )
	: mFormat( format )
{
	glGenSamplers( 1, &mId );
	initParams();
}

Sampler::Sampler( Sampler &&rhs )
	: mFormat( rhs.mFormat )
{
	mId = rhs.mId;
	rhs.mId = 0;
}

Sampler::~Sampler()
{
	if( mId )
		glDeleteSamplers( 1, &mId );
}

Sampler& Sampler::operator=( Sampler &&rhs )
{
	if( mId )
		glDeleteSamplers( 1, &mId );
	mFormat = rhs.mFormat;
	mId = rhs.mId;
	rhs.mId = 0;
	return *this;
}

void Sampler::initParams()
{
	glSamplerParameteri( mId, GL_TEXTURE_WRAP_S, mFormat.mWrapS );
	glSamplerParameteri( mId, GL_TEXTURE_WRAP_T, mFormat.mWrapT );
	glSamplerParameteri( mId, GL_TEXTURE_WRAP_R, mFormat.mWrapR );
	
	glSamplerParameteri( mId, GL_TEXTURE_MIN_FILTER, mFormat.mMinFilter );
	glSamplerParameteri( mId, GL_TEXTURE_MAG_FILTER, mFormat.mMagFilter );
	
	glSamplerParameteri( mId, GL_TEXTURE_COMPARE_MODE, mFormat.mCompareMode );
	glSamplerParameteri( mId, GL_TEXTURE_COMPARE_FUNC, mFormat.mCompareFunc );

#if ! defined( CINDER_GL_ES )
	glSamplerParameterf( mId, GL_TEXTURE_MIN_LOD, mFormat.mMinLod );
	glSamplerParameterf( mId, GL_TEXTURE_MAX_LOD, mFormat.mMaxLod );
	glSamplerParameterfv( mId, GL_TEXTURE_BORDER_COLOR, &mFormat.mBorderColor.r );
#endif

	if( mFormat.mMaxAnisotropy > 1.0f )
		glSamplerParameterf( mId, GL_TEXTURE_MAX_ANISOTROPY_EXT, mFormat.mMaxAnisotropy );

	gl::env()->objectLabel( GL_SAMPLER, mId, (GLsizei)mFormat.mLabel.size(), mFormat.mLabel.c_str() );
}

void Sampler::setWrap( GLenum wrapS, GLenum wrapT )
{
	setWrapS( wrapS );
	setWrapT( wrapT );
}

void Sampler::setWrap( GLenum wrapS, GLenum wrapT, GLenum wrapR )
{
	setWrapS( wrapS );
	setWrapT( wrapT );
	setWrapR( wrapR );
}

void Sampler::setWrapS( GLenum wrapS )
{
	mFormat.setWrapS( wrapS );
	glSamplerParameteri( mId, GL_TEXTURE_WRAP_S, mFormat.mWrapS );
}

void Sampler::setWrapT( GLenum wrapT )
{
	mFormat.setWrapT( wrapT );
	glSamplerParameteri( mId, GL_TEXTURE_WRAP_T, mFormat.mWrapT );
}

void Sampler::setWrapR( GLenum wrapR )
{
	mFormat.setWrapR( wrapR );
	glSamplerParameteri( mId, GL_TEXTURE_WRAP_R, mFormat.mWrapR );
}

void Sampler::setMinFilter( GLenum minFilter )
{
	mFormat.setMinFilter( minFilter );
	glSamplerParameteri( mId, GL_TEXTURE_MIN_FILTER, mFormat.mMinFilter );
}

void Sampler::setMagFilter( GLenum magFilter )
{
	mFormat.setMagFilter( magFilter );
	glSamplerParameteri( mId, GL_TEXTURE_MAG_FILTER, mFormat.mMagFilter );
}

void Sampler::setCompareMode( GLenum compareMode )
{
	mFormat.setCompareMode( compareMode );
	glSamplerParameteri( mId, GL_TEXTURE_COMPARE_MODE, mFormat.mCompareMode );
}

void Sampler::setCompareFunc( GLenum compareFunc )
{
	mFormat.setCompareFunc( compareFunc );
	glSamplerParameteri( mId, GL_TEXTURE_COMPARE_FUNC, mFormat.mCompareFunc );
}

#if ! defined( CINDER_GL_ES )
void Sampler::setBorderColor( const ColorA &color )
{
	mFormat.setBorderColor( color );
	glSamplerParameterfv( mId, GL_TEXTURE_BORDER_COLOR, &mFormat.mBorderColor.r );
}

void Sampler::setMinLod( float minLod )
{
	mFormat.setMinLod( minLod );
	glSamplerParameterf( mId, GL_TEXTURE_MIN_LOD, mFormat.mMinLod );
}

void Sampler::setMaxLod( float maxLod )
{
	mFormat.setMaxLod( maxLod );
	glSamplerParameterf( mId, GL_TEXTURE_MAX_LOD, mFormat.mMaxLod );
}
#endif // defined( CINDER_GL_ES )

void Sampler::setMaxAnisotropy( float maxAnisotropy )
{
	mFormat.setMaxAnisotropy( maxAnisotropy );
	glSamplerParameterf( mId, GL_TEXTURE_MAX_ANISOTROPY_EXT, mFormat.mMaxAnisotropy );
}

void Sampler::setLabel( const std::string &label )
{
	mFormat.setLabel( label );
	gl::env()->objectLabel( GL_SAMPLER, mId, (GLsizei)label.size(), label.c_str() );
}

void Sampler::bind( uint8_t textureUnit ) const
{
	gl::context()->bindSampler( textureUnit, mId );
}

void Sampler::unbind( uint8_t textureUnit ) const
{
	gl::context()->bindSampler( textureUnit, 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Sampler::Format

Sampler::Format::Format()
{
	mWrapS = mWrapT = mWrapR = GL_REPEAT;
	mMinFilter = GL_LINEAR; 
	mMagFilter = GL_LINEAR;
	mCompareMode = GL_NONE; 
	mCompareFunc = GL_LEQUAL;

#if ! defined( CINDER_GL_ES )
	mBorderColor = { 0, 0, 0, 0 };
	mMinLod = -1000.0f;
	mMaxLod = 1000.0f;	
#endif

	mMaxAnisotropy = -1.0f;
}

void Sampler::Format::copy( const Format &rhs )
{
	mWrapS = rhs.mWrapS;
	mWrapT = rhs.mWrapT;
	mWrapR = rhs.mWrapR;
	mMinFilter = rhs.mMinFilter;
	mMagFilter = rhs.mMagFilter;
	mCompareMode = rhs.mCompareMode;
	mCompareFunc = rhs.mCompareFunc;

#if ! defined( CINDER_GL_ES )
	mBorderColor = rhs.mBorderColor;
	mMinLod = rhs.mMinLod;
	mMaxLod = rhs.mMaxLod;
#endif

	mMaxAnisotropy = rhs.mMaxAnisotropy;
	mLabel = rhs.mLabel;
}

} } // namespace cinder::gl

#endif // defined( CINDER_GL_HAS_SAMPLERS )
