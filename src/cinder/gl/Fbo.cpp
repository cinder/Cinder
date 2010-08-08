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
GLint Fbo::sMaxAttachments = -1;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RenderBuffer::Obj
Renderbuffer::Obj::Obj()
{
	mWidth = mHeight = -1;
	mId = 0;
	mInternalFormat = 0;
	mSamples = mCoverageSamples = 0;
}

Renderbuffer::Obj::Obj( int aWidth, int aHeight, GLenum internalFormat, int msaaSamples, int coverageSamples )
	: mWidth( aWidth ), mHeight( aHeight ), mInternalFormat( internalFormat ), mSamples( msaaSamples ), mCoverageSamples( coverageSamples )
{
#if defined( CINDER_MSW )
	static bool csaaSupported = ( GLEE_NV_framebuffer_multisample_coverage != 0 );
#else
	static bool csaaSupported = false;
#endif

	glGenRenderbuffersEXT( 1, &mId );

	if( mSamples > Fbo::getMaxSamples() )
		mSamples = Fbo::getMaxSamples();

	if( ! csaaSupported )
		mCoverageSamples = 0;

	glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, mId );

#if defined( CINDER_MSW )
	if( mCoverageSamples ) // create a CSAA buffer
		glRenderbufferStorageMultisampleCoverageNV( GL_RENDERBUFFER_EXT, mCoverageSamples, mSamples, mInternalFormat, mWidth, mHeight );
	else
#endif
	if( mSamples ) // create a regular MSAA buffer
		glRenderbufferStorageMultisampleEXT( GL_RENDERBUFFER_EXT, mSamples, mInternalFormat, mWidth, mHeight );
	else
		glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, mInternalFormat, mWidth, mHeight );
}

Renderbuffer::Obj::~Obj()
{
	if( mId )
		glDeleteRenderbuffersEXT( 1, &mId );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Renderbuffer::Renderbuffer
Renderbuffer::Renderbuffer( int width, int height, GLenum internalFormat )
	: mObj( new Obj( width, height, internalFormat, 0, 0 ) )
{
}
Renderbuffer::Renderbuffer( int width, int height, GLenum internalFormat, int msaaSamples, int coverageSamples )
	: mObj( new Obj( width, height, internalFormat, msaaSamples, coverageSamples ) )
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fbo::Obj
Fbo::Obj::Obj()
{
	mId = 0;
	mResolveFramebufferId = 0;
}

Fbo::Obj::Obj( int width, int height )
	: mWidth( width ), mHeight( height )
{
	mId = 0;
	mResolveFramebufferId = 0;
}

Fbo::Obj::~Obj()
{
	if( mId )
		glDeleteFramebuffersEXT( 1, &mId );
	if( mResolveFramebufferId )
		glDeleteFramebuffersEXT( 1, &mResolveFramebufferId );
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
	mNumColorBuffers = 1;
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

	Texture::Format textureFormat;
	textureFormat.setTarget( getTarget() );
	textureFormat.setInternalFormat( getFormat().getColorInternalFormat() );
	textureFormat.setWrap( mObj->mFormat.mWrapS, mObj->mFormat.mWrapT );
	textureFormat.setMinFilter( mObj->mFormat.mMinFilter );
	textureFormat.setMagFilter( mObj->mFormat.mMagFilter );
	textureFormat.enableMipmapping( getFormat().hasMipMapping() );

	// allocate the color buffers
	for( int c = 0; c < mObj->mFormat.mNumColorBuffers; ++c ) {
		mObj->mColorTextures.push_back( Texture( mObj->mWidth, mObj->mHeight, textureFormat ) );
	}
	
	if( mObj->mFormat.mNumColorBuffers == 0 ) { // no color
		glDrawBuffer( GL_NONE );
		glReadBuffer( GL_NONE );	
	}
		
	if( ( ( ! useCSAA ) && ( ! useMSAA ) ) || ( ! initMultisample( useCSAA ) ) ) { // if we don't need any variety of multisampling or it failed to initialize
		// attach all the textures to the framebuffer
		vector<GLenum> drawBuffers;
		for( size_t c = 0; c < mObj->mColorTextures.size(); ++c ) {
			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + c, getTarget(), mObj->mColorTextures[c].getTextureId(), 0 );
			drawBuffers.push_back( GL_COLOR_ATTACHMENT0_EXT + c );
		}
		if( ! drawBuffers.empty() )
			glDrawBuffers( drawBuffers.size(), &drawBuffers[0] );

		// allocate and attach depth texture
		if( mObj->mFormat.mDepthBuffer ) {
			GLuint depthTextureId;
			glGenTextures( 1, &depthTextureId );
			glBindTexture( getTarget(), depthTextureId );
			glTexImage2D( getTarget(), 0, getFormat().getDepthInternalFormat(), mObj->mWidth, mObj->mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
			glTexParameteri( getTarget(), GL_TEXTURE_MIN_FILTER, mObj->mFormat.mMinFilter );
			glTexParameteri( getTarget(), GL_TEXTURE_MAG_FILTER, mObj->mFormat.mMagFilter );
			glTexParameteri( getTarget(), GL_TEXTURE_WRAP_S, mObj->mFormat.mWrapS );
			glTexParameteri( getTarget(), GL_TEXTURE_WRAP_T, mObj->mFormat.mWrapT );
			glTexParameteri( getTarget(), GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );
			mObj->mDepthTexture = Texture( getTarget(), depthTextureId, mObj->mWidth, mObj->mHeight, true );

			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, getTarget(), mObj->mDepthTexture.getTextureId(), 0 );
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
	glGenFramebuffersEXT( 1, &mObj->mResolveFramebufferId );
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, mObj->mResolveFramebufferId ); 
	
	// bind all of the color buffers to the resolve FB's attachment points
	vector<GLenum> drawBuffers;
	for( size_t c = 0; c < mObj->mColorTextures.size(); ++c ) {
		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + c, getTarget(), mObj->mColorTextures[c].getTextureId(), 0 );
		drawBuffers.push_back( GL_COLOR_ATTACHMENT0_EXT + c );
	}

	if( ! drawBuffers.empty() )
		glDrawBuffers( drawBuffers.size(), &drawBuffers[0] );

	// see if the resolve buffer is ok
	FboExceptionInvalidSpecification ignoredException;
	if( ! checkStatus( &ignoredException ) )
		return false;

	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, mObj->mId );

	if( mObj->mFormat.mSamples > getMaxSamples() ) {
		mObj->mFormat.mSamples = getMaxSamples();
	}

	// setup the multisampled color renderbuffers
	for( int c = 0; c < mObj->mFormat.mNumColorBuffers; ++c ) {
		mObj->mMultisampleColorRenderbuffers.push_back( Renderbuffer( mObj->mWidth, mObj->mHeight, mObj->mFormat.mColorInternalFormat, mObj->mFormat.mSamples, mObj->mFormat.mCoverageSamples ) );

		// attach the multisampled color buffer
		glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + c, GL_RENDERBUFFER_EXT, mObj->mMultisampleColorRenderbuffers.back().getId() );
	}
	
	if( ! drawBuffers.empty() )
		glDrawBuffers( drawBuffers.size(), &drawBuffers[0] );

	if( mObj->mFormat.mDepthBuffer ) {
		// create the multisampled depth Renderbuffer
		mObj->mMultisampleDepthRenderbuffer = Renderbuffer( mObj->mWidth, mObj->mHeight, mObj->mFormat.mDepthInternalFormat, mObj->mFormat.mSamples, mObj->mFormat.mCoverageSamples );

		// attach the depth Renderbuffer
		glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mObj->mMultisampleDepthRenderbuffer.getId() );
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
	mObj->mFormat.mNumColorBuffers = color ? 1 : 0;
	init();
}

Texture& Fbo::getTexture( int attachment )
{
	resolveTextures();
	updateMipmaps( true, attachment );
	return mObj->mColorTextures[attachment];
}

Texture& Fbo::getDepthTexture()
{
	return mObj->mDepthTexture;
}

void Fbo::bindTexture( int textureUnit, int attachment )
{
	resolveTextures();
	mObj->mColorTextures[attachment].bind( textureUnit );
	updateMipmaps( false, attachment );
}

void Fbo::unbindTexture()
{
	glBindTexture( getTarget(), 0 );
}

void Fbo::bindDepthTexture( int textureUnit )
{
	mObj->mDepthTexture.bind( textureUnit );
}

void Fbo::resolveTextures() const
{
	if( ! mObj->mNeedsResolve )
		return;
		
	// if this FBO is multisampled, resolve it, so it can be displayed
	if ( mObj->mResolveFramebufferId ) {
		SaveFramebufferBinding saveFboBinding;

		glBindFramebufferEXT( GL_READ_FRAMEBUFFER_EXT, mObj->mId );
		glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER_EXT, mObj->mResolveFramebufferId );
		
		for( size_t c = 0; c < mObj->mColorTextures.size(); ++c ) {
			glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + c );
			glReadBuffer( GL_COLOR_ATTACHMENT0_EXT + c );
			GLbitfield bitfield = GL_COLOR_BUFFER_BIT;
			if( mObj->mDepthTexture )
				bitfield |= GL_DEPTH_BUFFER_BIT;
			glBlitFramebufferEXT( 0, 0, mObj->mWidth, mObj->mHeight, 0, 0, mObj->mWidth, mObj->mHeight, bitfield, GL_NEAREST );
		}

		// restore the draw buffers to the default for the antialiased (non-resolve) framebuffer
		vector<GLenum> drawBuffers;
		for( size_t c = 0; c < mObj->mColorTextures.size(); ++c )
			drawBuffers.push_back( GL_COLOR_ATTACHMENT0_EXT + c );
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, mObj->mId );
		glDrawBuffers( drawBuffers.size(), &drawBuffers[0] );
	}

	mObj->mNeedsResolve = false;
}

void Fbo::updateMipmaps( bool bindFirst, int attachment ) const
{
	if( ! mObj->mNeedsMipmapUpdate )
		return;
	
	if( bindFirst ) {
		SaveTextureBindState state( getTarget() );
		mObj->mColorTextures[attachment].bind();
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
		if( ( ! gl::isExtensionAvailable( "GL_EXT_framebuffer_multisample" ) ) || ( ! gl::isExtensionAvailable( "GL_EXT_framebuffer_blit" ) ) ) {
			sMaxSamples = 0;
		}
		else
			glGetIntegerv( GL_MAX_SAMPLES_EXT, &sMaxSamples);	
	}
	
	return sMaxSamples;
}

GLint Fbo::getMaxAttachments()
{
	if( sMaxAttachments < 0 ) {
		glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &sMaxAttachments );
	}
	
	return sMaxAttachments;
}

void Fbo::blitTo( Fbo dst, const Area &srcArea, const Area &dstArea, GLenum filter, GLbitfield mask ) const
{
	SaveFramebufferBinding saveFboBinding;

	glBindFramebufferEXT( GL_READ_FRAMEBUFFER_EXT, mObj->mId );
	glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER_EXT, dst.getId() );		
	glBlitFramebufferEXT( srcArea.getX1(), srcArea.getY1(), srcArea.getX2(), srcArea.getY2(), dstArea.getX1(), dstArea.getY1(), dstArea.getX2(), dstArea.getY2(), mask, filter );
}

void Fbo::blitToScreen( const Area &srcArea, const Area &dstArea, GLenum filter, GLbitfield mask ) const
{
	SaveFramebufferBinding saveFboBinding;

	glBindFramebufferEXT( GL_READ_FRAMEBUFFER_EXT, mObj->mId );
	glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER_EXT, 0 );		
	glBlitFramebufferEXT( srcArea.getX1(), srcArea.getY1(), srcArea.getX2(), srcArea.getY2(), dstArea.getX1(), dstArea.getY1(), dstArea.getX2(), dstArea.getY2(), mask, filter );
}

void Fbo::blitFromScreen( const Area &srcArea, const Area &dstArea, GLenum filter, GLbitfield mask )
{
	SaveFramebufferBinding saveFboBinding;

	glBindFramebufferEXT( GL_READ_FRAMEBUFFER_EXT, GL_NONE );
	glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER_EXT, mObj->mId );		
	glBlitFramebufferEXT( srcArea.getX1(), srcArea.getY1(), srcArea.getX2(), srcArea.getY2(), dstArea.getX1(), dstArea.getY1(), dstArea.getX2(), dstArea.getY2(), mask, filter );
}

FboExceptionInvalidSpecification::FboExceptionInvalidSpecification( const string &message ) throw()
	: FboException()
{
	strncpy( mMessage, message.c_str(), 255 );
}

} } // namespace cinder::gl
