/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.
 
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

#import "RendererImplGlCocoaTouch.h"
#import <QuartzCore/QuartzCore.h>

#include "cinder/gl/platform.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Environment.h"
#include "cinder/Log.h"

#if defined( CINDER_GL_ES_2 )
	#define GL_DEPTH_COMPONENT24						GL_DEPTH_COMPONENT24_OES
	#define GL_DEPTH24_STENCIL8							GL_DEPTH24_STENCIL8_OES
	#define GL_RGBA8									GL_RGBA8_OES
	#define glRenderbufferStorageMultisample			glRenderbufferStorageMultisampleAPPLE
	#define GL_READ_FRAMEBUFFER							GL_READ_FRAMEBUFFER_APPLE
	#define GL_DRAW_FRAMEBUFFER							GL_DRAW_FRAMEBUFFER_APPLE
	#define GL_MAX_SAMPLES								GL_MAX_SAMPLES_APPLE
#endif

@implementation RendererImplGlCocoaTouch

- (id)initWithFrame:(CGRect)frame cinderView:(UIView *)cinderView renderer:(cinder::app::RendererGl *)renderer sharedRenderer:(cinder::app::RendererGlRef)sharedRenderer
{
	mCinderView = cinderView;
	// Get the layer
	CAEAGLLayer *eaglLayer = (CAEAGLLayer *)mCinderView.layer;
	
	eaglLayer.opaque = TRUE;
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
									[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
	
	mBackingWidth = 0;
	mBackingHeight = 0;
	
	mMsaaSamples = renderer->getOptions().getMsaa();
	
	mUsingStencil = renderer->getOptions().getStencil();
	mObjectTracking = renderer->getOptions().getObjectTracking();
	mDepthInternalFormat = ( renderer->getOptions().getDepthBufferDepth() == 24 ) ? GL_DEPTH_COMPONENT24 : GL_DEPTH_COMPONENT16;
	if( mUsingStencil )
		mDepthInternalFormat = GL_DEPTH24_STENCIL8;
	mColorInternalFormat = GL_RGBA8;
	
	[self allocateGraphics:sharedRenderer];
	// make sure that mContext was set properly; if not we failed to allocate
	if( ! mContext )
		throw cinder::app::ExcRendererAllocation( "Failed to allocate GL context" );
	
	return self;
}

- (void)allocateGraphics:(cinder::app::RendererGlRef)sharedRenderer
{
#if defined( CINDER_GL_ES_3 )
	auto api = kEAGLRenderingAPIOpenGLES3;
#else
	auto api = kEAGLRenderingAPIOpenGLES2;
#endif
	if( sharedRenderer ) {
		EAGLSharegroup *sharegroup = [sharedRenderer->getEaglContext() sharegroup];
		mContext = [[EAGLContext alloc] initWithAPI:api sharegroup:sharegroup];
	}
	else
		mContext = [[EAGLContext alloc] initWithAPI:api];
	
	if( ! mContext ) {
		[self release];
		return;
	}
	else
		[EAGLContext setCurrentContext:mContext];
	
	cinder::gl::Environment::setEs();

	// setup msaa samples and clamp to max on this hardware
	GLint maxSamples;
	glGetIntegerv( GL_MAX_SAMPLES, &maxSamples);
	if( mMsaaSamples > maxSamples )
		mMsaaSamples = maxSamples;
	mUsingMsaa = mMsaaSamples > 0;

	// force Cinder's context to be allocated
	std::shared_ptr<cinder::gl::Context::PlatformData> platformData( new cinder::gl::PlatformDataIos( mContext ) );
	platformData->mObjectTracking = mObjectTracking;
	mCinderContext = cinder::gl::Context::createFromExisting( platformData );
	mCinderContext->makeCurrent();

	// Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
	glGenFramebuffers( 1, &mViewFramebuffer );
	glGenRenderbuffers( 1, &mViewRenderbuffer );
	mCinderContext->bindFramebuffer( GL_FRAMEBUFFER, mViewFramebuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, mViewRenderbuffer );

	// This call associates the storage for the current render buffer with the EAGLDrawable (our CAEAGLLayer)
	// allowing us to draw into a buffer that will later be rendered to the screen wherever the layer is (which corresponds with our view).
	[mContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)mCinderView.layer];
	
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mViewRenderbuffer );
	
	glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &mBackingWidth );
	glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &mBackingHeight );
	
	if( ! mUsingMsaa ) {
		// setup depth (+stencil) buffer
		glGenRenderbuffers( 1, &mDepthRenderbuffer );
		glBindRenderbuffer( GL_RENDERBUFFER, mDepthRenderbuffer );
		glRenderbufferStorage( GL_RENDERBUFFER, mDepthInternalFormat, mBackingWidth, mBackingHeight );
		if( mUsingStencil ) {
#if defined( CINDER_GL_ES_2 )
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer );
#else
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer );
#endif
		}
		else
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer );
	}
	else {
		// for MSAA we setup a parallel Framebuffer; the non-MSAA framebuffer doesn't get a depthbuffer		
		glGenFramebuffers( 1, &mMsaaFramebuffer );
		glGenRenderbuffers( 1, &mMsaaRenderbuffer );
		mCinderContext->bindFramebuffer( GL_FRAMEBUFFER, mMsaaFramebuffer );
		glBindRenderbuffer( GL_RENDERBUFFER, mMsaaRenderbuffer );
		glRenderbufferStorageMultisample( GL_RENDERBUFFER, mMsaaSamples, mColorInternalFormat, mBackingWidth, mBackingHeight );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mMsaaRenderbuffer );

		// depth (+stencil) buffer
		glGenRenderbuffers( 1, &mDepthRenderbuffer );
		glBindRenderbuffer( GL_RENDERBUFFER, mDepthRenderbuffer );
		glRenderbufferStorageMultisample( GL_RENDERBUFFER, mMsaaSamples, mDepthInternalFormat, mBackingWidth, mBackingHeight );
		
		if( mUsingStencil ) {
#if defined( CINDER_GL_ES_2 )
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer );
#else
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer );
#endif
		}
		else
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer );
	}

	if( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ) {
		CI_LOG_E( "Failed to allocate application framebuffer" );
	}
}

- (EAGLContext*)getEaglContext
{
	return mContext;
}

- (void)layoutSubviews
{
	GLint backingWidth = mCinderView.layer.bounds.size.width * mCinderView.layer.contentsScale;
	GLint backingHeight = mCinderView.layer.bounds.size.height * mCinderView.layer.contentsScale;

	// test to see if this is already the resolution we setup in allocateGraphics()
	if( (mBackingWidth == backingWidth) && (mBackingHeight == backingHeight) )
		return;

	mBackingWidth = backingWidth;
	mBackingHeight = backingHeight;

	mCinderContext->makeCurrent();
	mCinderContext->bindFramebuffer( GL_FRAMEBUFFER, mViewFramebuffer );

	// Allocate color buffer backing based on the current layer size
	glBindRenderbuffer( GL_RENDERBUFFER, mViewRenderbuffer );

	[mContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)mCinderView.layer];

	if( ! mUsingMsaa ) {
		glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer );
		glRenderbufferStorage(GL_RENDERBUFFER, mDepthInternalFormat, mBackingWidth, mBackingHeight );
	}
	else {
		glBindRenderbuffer( GL_RENDERBUFFER, mDepthRenderbuffer );
		glRenderbufferStorageMultisample( GL_RENDERBUFFER, mMsaaSamples, mDepthInternalFormat, mBackingWidth, mBackingHeight );
		glBindRenderbuffer( GL_RENDERBUFFER, mMsaaRenderbuffer );
		glRenderbufferStorageMultisample( GL_RENDERBUFFER, mMsaaSamples, mColorInternalFormat, mBackingWidth, mBackingHeight );
	}
	
	if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ) {
		CI_LOG_E( "Failed to reallocate application framebuffer" );
	}
}

- (void)makeCurrentContext:(bool)force
{
	mCinderContext->makeCurrent( force );
    
	// This application only creates a single default framebuffer which is already bound at this point.
	// This call is redundant, but needed if dealing with multiple framebuffers.
	if( mUsingMsaa ) {
		mCinderContext->bindFramebuffer( GL_FRAMEBUFFER, mMsaaFramebuffer );
	}
	else {
		mCinderContext->bindFramebuffer( GL_FRAMEBUFFER, mViewFramebuffer );
	}
}

- (void)flushBuffer
{
	if( mUsingMsaa ) {
		mCinderContext->bindFramebuffer( GL_READ_FRAMEBUFFER, mMsaaFramebuffer );
		mCinderContext->bindFramebuffer( GL_DRAW_FRAMEBUFFER, mViewFramebuffer );
#if defined( CINDER_GL_ES_2 )
		glResolveMultisampleFramebufferAPPLE();
#else
		glBlitFramebuffer( 0, 0, mBackingWidth, mBackingHeight, 0, 0, mBackingWidth, mBackingHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST );
#endif

		mCinderContext->bindFramebuffer( GL_FRAMEBUFFER, mMsaaFramebuffer );
		GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT };
#if defined( CINDER_GL_ES_2 )
		glDiscardFramebufferEXT( GL_FRAMEBUFFER, 2, attachments );
#else
		glInvalidateFramebuffer( GL_FRAMEBUFFER, 2, attachments );
#endif
	}
	
	mCinderContext->bindFramebuffer( GL_FRAMEBUFFER, mViewFramebuffer );
    glBindRenderbuffer( GL_RENDERBUFFER, mViewRenderbuffer );
    [mContext presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)setFrameSize:(CGSize)newSize
{
	[self layoutSubviews];
}

- (void)defaultResize
{
	cinder::gl::setMatricesWindow( mCinderView.bounds.size.width, mCinderView.bounds.size.height );
	cinder::gl::viewport( 0, 0, mBackingWidth, mBackingHeight );
}

- (BOOL)needsDrawRect
{
	return NO;
}

@end
