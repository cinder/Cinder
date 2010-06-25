/*
Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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

#include "cinder/gl/gl.h" // must be first
#include "cinder/gl/Fbo.h"

using namespace std;

namespace cinder {
namespace gl {

GLint Fbo::sMaxSamples = -1;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fbo::Obj
Fbo::Obj::Obj()
{
	mId = mColorTextureId = mDepthTextureId = 0;
	mColorRenderBufferId = mDepthRenderBufferId = mResolveFramebufferId = 0;
}

Fbo::Obj::Obj( int width, int height )
	: mWidth( width ), mHeight( height )
{
	mId = mColorTextureId = mDepthTextureId = 0;
	mColorRenderBufferId = mDepthRenderBufferId = mResolveFramebufferId = 0;
}

Fbo::Obj::~Obj()
{
	if( mId )
		glDeleteFramebuffersEXT( 1, &mId );
	if( mResolveFramebufferId )
		glDeleteFramebuffersEXT( 1, &mResolveFramebufferId );
	if( mColorRenderBufferId )
		glDeleteRenderbuffersEXT( 1, &mColorRenderBufferId );
	if( mDepthRenderBufferId )
		glDeleteRenderbuffersEXT( 1, &mDepthRenderBufferId );
	if( mColorTextureId )
		glDeleteTextures( 1, &mColorTextureId );
	if( mDepthTextureId )
		glDeleteTextures( 1, &mDepthTextureId );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fbo::Format
Fbo::Format::Format()
{
	mTarget = GL_TEXTURE_2D;
	mColorInternalFormat = GL_RGBA8;
	mDepthInternalFormat = GL_DEPTH_COMPONENT24;
	mSamples = 0;
	mCoverageSamples = 0;
	mColorBuffer = true;
	mDepthBuffer = true;
	mStencilBuffer = false;
	mMipmapping = false;
	mWrapS = GL_CLAMP_TO_EDGE;
	mWrapT = GL_CLAMP_TO_EDGE;
	mMinFilter = GL_LINEAR;
	mMagFilter = GL_LINEAR;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fbo
void Fbo::init()
{
#if defined( CINDER_MSW )
	static bool csaaSupported = ( GLEE_NV_framebuffer_multisample_coverage != 0 );
#else
	static bool csaaSupported = false;
#endif
	bool useCSAA = csaaSupported && ( mObj->mFormat.mCoverageSamples > mObj->mFormat.mSamples );
	bool useMSAA = ( mObj->mFormat.mCoverageSamples > 0 ) || ( mObj->mFormat.mSamples > 0 );
	if( useCSAA )
		useMSAA = false;

	// allocate the framebuffer itself
	glGenFramebuffersEXT( 1, &mObj->mId );
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, mObj->mId );	

	// allocate the color buffer
	if( mObj->mFormat.mColorBuffer ) {
		glGenTextures( 1, &mObj->mColorTextureId );
		glBindTexture( getTarget(), mObj->mColorTextureId );
		glTexImage2D( getTarget(), 0, getFormat().getColorInternalFormat(), mObj->mWidth, mObj->mHeight, 0, GL_RGBA, GL_FLOAT, NULL );

		glTexParameteri( getTarget(), GL_TEXTURE_MIN_FILTER, mObj->mFormat.mMinFilter );
		glTexParameteri( getTarget(), GL_TEXTURE_MAG_FILTER, mObj->mFormat.mMagFilter );
		glTexParameteri( getTarget(), GL_TEXTURE_WRAP_S, mObj->mFormat.mWrapS );
		glTexParameteri( getTarget(), GL_TEXTURE_WRAP_T, mObj->mFormat.mWrapT );

		if( getFormat().hasMipMapping() )
			glGenerateMipmapEXT( getTarget() );
			
		mObj->mColorTexture = Texture( getTarget(), mObj->mColorTextureId, mObj->mWidth, mObj->mHeight, true );
	}
	else { // no color
		glDrawBuffer( GL_NONE );
		glReadBuffer( GL_NONE );	
	}
		
	if( ( ( ! useCSAA ) && ( ! useMSAA ) ) || ( ! initMultisample( useCSAA ) ) ) { // if we don't need any variety of multisampling or it failed to initialize
		// attach the color texture
		if( mObj->mFormat.mColorBuffer )
			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, getTarget(), mObj->mColorTextureId, 0 );
		
		// allocate and attach depth texture
		if( mObj->mFormat.mDepthBuffer ) {
			glGenTextures( 1, &mObj->mDepthTextureId );
			glBindTexture( getTarget(), mObj->mDepthTextureId );
			glTexImage2D( getTarget(), 0, getFormat().getDepthInternalFormat(), mObj->mWidth, mObj->mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
			glTexParameteri( getTarget(), GL_TEXTURE_MIN_FILTER, mObj->mFormat.mMinFilter );
			glTexParameteri( getTarget(), GL_TEXTURE_MAG_FILTER, mObj->mFormat.mMagFilter );
			glTexParameteri( getTarget(), GL_TEXTURE_WRAP_S, mObj->mFormat.mWrapS );
			glTexParameteri( getTarget(), GL_TEXTURE_WRAP_T, mObj->mFormat.mWrapT );
			glTexParameteri( getTarget(), GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );

			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, getTarget(), mObj->mDepthTextureId, 0 );
		
			mObj->mDepthTexture = Texture( getTarget(), mObj->mDepthTextureId, mObj->mWidth, mObj->mHeight, true );
		}

		FboExceptionInvalidSpecification exc;
		if( ! checkStatus( &exc ) ) { // failed creation; throw
			throw exc;
		}
	}
	
	mObj->mNeedsResolve = false;
	mObj->mNeedsMipmapUpdate = false;
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

bool Fbo::initMultisample( bool csaa )
{
	glGenRenderbuffersEXT( 1, &mObj->mDepthRenderBufferId );
	glGenRenderbuffersEXT( 1, &mObj->mColorRenderBufferId );
	glGenFramebuffersEXT( 1, &mObj->mResolveFramebufferId );

	// multisample, so we need to resolve from the FBO, bind the texture to the resolve FBO
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, mObj->mResolveFramebufferId ); 
	
	if( mObj->mFormat.mColorBuffer )
		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, getTarget(), mObj->mColorTextureId, 0 );

	// see if the resolve buffer is ok
	FboExceptionInvalidSpecification ignoredException;
	if( ! checkStatus( &ignoredException ) )
		return false;

	if( mObj->mFormat.mSamples > getMaxSamples() ) {
		mObj->mFormat.mSamples = getMaxSamples();
	}

	// setup the primary framebuffer
	if( mObj->mFormat.mColorBuffer ) {
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, mObj->mId );
		glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, mObj->mColorRenderBufferId );
#if defined( CINDER_MSW )
		if( csaa )
			glRenderbufferStorageMultisampleCoverageNV( GL_RENDERBUFFER_EXT, mObj->mFormat.mCoverageSamples, mObj->mFormat.mSamples, mObj->mFormat.mColorInternalFormat, mObj->mWidth, mObj->mHeight );
		else
#endif
			// create a regular MSAA color buffer
			glRenderbufferStorageMultisampleEXT( GL_RENDERBUFFER_EXT, mObj->mFormat.mSamples, mObj->mFormat.mColorInternalFormat, mObj->mWidth, mObj->mHeight );

		// attach the multisampled color buffer
		glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, mObj->mColorRenderBufferId );
	}

	if( mObj->mFormat.mDepthBuffer ) {
		glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, mObj->mDepthRenderBufferId );
		// create the multisampled depth buffer (with or without coverage sampling)
#if defined( CINDER_MSW )
		if( csaa )
			// create a coverage sampled MSAA depth buffer
			glRenderbufferStorageMultisampleCoverageNV( GL_RENDERBUFFER_EXT, mObj->mFormat.mCoverageSamples, mObj->mFormat.mSamples, mObj->mFormat.mDepthInternalFormat, mObj->mWidth, mObj->mHeight );
		else
#endif
			// create a regular (not coverage sampled) MSAA depth buffer
			glRenderbufferStorageMultisampleEXT( GL_RENDERBUFFER_EXT, mObj->mFormat.mSamples, mObj->mFormat.mDepthInternalFormat, mObj->mWidth, mObj->mHeight );

		// attach the depth buffer
		glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mObj->mDepthRenderBufferId );
	}

	// see if the primary framebuffer turned out ok
	return checkStatus( &ignoredException );
}

Fbo::Fbo( int width, int height, Format format )
	: mObj( shared_ptr<Obj>( new Obj( width, height ) ) )
{
	mObj->mFormat = format;
	init();
}

Fbo::Fbo( int width, int height, bool alpha, bool color, bool depth )
	: mObj( shared_ptr<Obj>( new Obj( width, height ) ) )
{
	Format format;
	mObj->mFormat.mColorInternalFormat = ( alpha ) ? GL_RGBA8 : GL_RGB8;
	mObj->mFormat.mDepthBuffer = depth;
	mObj->mFormat.mColorBuffer = color;
	init();
}

Texture& Fbo::getTexture()
{
	resolveTexture();
	updateMipmaps( true );
	return mObj->mColorTexture;
}

Texture& Fbo::getDepthTexture()
{
	return mObj->mDepthTexture;
}

void Fbo::bindTexture( int textureUnit )
{
	resolveTexture();
	mObj->mColorTexture.bind( textureUnit );
	updateMipmaps( false );
}

void Fbo::unbindTexture()
{
	glBindTexture( getTarget(), 0 );
}

void Fbo::bindDepthTexture( int textureUnit )
{
	mObj->mDepthTexture.bind( textureUnit );
}

void Fbo::resolveTexture() const
{
	if( ! mObj->mNeedsResolve )
		return;
		
	// if this FBO is multisampled, resolve it, so it can be displayed
	if ( mObj->mResolveFramebufferId ) {
		GLint oldFb;
		glGetIntegerv( GL_FRAMEBUFFER_BINDING_EXT, &oldFb );
		glBindFramebufferEXT( GL_READ_FRAMEBUFFER_EXT, mObj->mId );
		glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER_EXT, mObj->mResolveFramebufferId );
		glBlitFramebufferEXT( 0, 0, mObj->mWidth, mObj->mHeight, 0, 0, mObj->mWidth, mObj->mHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST );

		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, oldFb );
	}

	mObj->mNeedsResolve = false;
}

void Fbo::updateMipmaps( bool bindFirst ) const
{
	if( ! mObj->mNeedsMipmapUpdate )
		return;
	
	if( bindFirst ) {
		SaveTextureBindState state( getTarget() );
		mObj->mColorTexture.bind();
		glGenerateMipmapEXT( getTarget() );
	}
	else {
		glGenerateMipmapEXT( getTarget() );
	}

	mObj->mNeedsMipmapUpdate = false;
}

void Fbo::bindFramebuffer()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, mObj->mId );
	if( mObj->mResolveFramebufferId ) {
		mObj->mNeedsResolve = true;
	}
	if( mObj->mFormat.hasMipMapping() ) {
		mObj->mNeedsMipmapUpdate = true;
	}
}

void Fbo::unbindFramebuffer()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

bool Fbo::checkStatus( FboExceptionInvalidSpecification *resultExc )
{
	GLenum status;
	status = (GLenum) glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
	switch( status ) {
		case GL_FRAMEBUFFER_COMPLETE_EXT:
		break;
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			*resultExc = FboExceptionInvalidSpecification( "Unsupported framebuffer format" );
		return false;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			*resultExc = FboExceptionInvalidSpecification( "Framebuffer incomplete: missing attachment" );
		return false;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			*resultExc = FboExceptionInvalidSpecification( "Framebuffer incomplete: duplicate attachment" );
		return false;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			*resultExc = FboExceptionInvalidSpecification( "Framebuffer incomplete: attached images must have same dimensions" );
		return false;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			*resultExc = FboExceptionInvalidSpecification( "Framebuffer incomplete: attached images must have same format" );
		return false;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			*resultExc = FboExceptionInvalidSpecification( "Framebuffer incomplete: missing draw buffer" );
		return false;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			*resultExc = FboExceptionInvalidSpecification( "Framebuffer incomplete: missing read buffer" );
		return false;
		default:
			*resultExc = FboExceptionInvalidSpecification( "Framebuffer invalid: unknown reason" );
		return false;
    }
	
    return true;
}


GLint Fbo::getMaxSamples()
{
	if( sMaxSamples < 0 ) {
		if( ( ! gl::isExtensionAvailable( "GL_EXT_framebuffer_multisample" ) ) || ( ! gl::isExtensionAvailable( "GLEE_EXT_framebuffer_blit" ) ) ) {
			sMaxSamples = 0;
		}
		else
			glGetIntegerv( GL_MAX_SAMPLES_EXT, &sMaxSamples);	
	}
	
	return sMaxSamples;
}

FboExceptionInvalidSpecification::FboExceptionInvalidSpecification( const string &message ) throw()
	: FboException()
{
	strncpy( mMessage, message.c_str(), 255 );
}

} } // namespace cinder::gl
