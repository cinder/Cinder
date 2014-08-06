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

#import "AppImplCocoaTouchRendererGl.h"
#import <QuartzCore/QuartzCore.h>

#include "cinder/gl/gl.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Environment.h"

@implementation AppImplCocoaTouchRendererGl

- (id)initWithFrame:(CGRect)frame cinderView:(UIView*)aCinderView app:(cinder::app::App*)aApp renderer:(cinder::app::RendererGl*)aRenderer sharedRenderer:(cinder::app::RendererGlRef)sharedRenderer
{
	mCinderView = aCinderView;
	mApp = aApp;
	// Get the layer
	CAEAGLLayer *eaglLayer = (CAEAGLLayer *)mCinderView.layer;
	
	eaglLayer.opaque = TRUE;
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
									[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
	
	mBackingWidth = 0;
	mBackingHeight = 0;
	mMsaaSamples = cinder::app::RendererGl::sAntiAliasingSamples[aRenderer->getOptions().getAntiAliasing()];
	mUsingMsaa = mMsaaSamples > 0;
	mUsingStencil = aRenderer->getOptions().getStencil();
	mObjectTracking = aRenderer->getOptions().getObjectTracking();
	
	[self allocateGraphics:sharedRenderer];
	
	return self;
}

- (void)allocateGraphics:(cinder::app::RendererGlRef)sharedRenderer
{
	if( sharedRenderer ) {
		EAGLSharegroup *sharegroup = [sharedRenderer->getEaglContext() sharegroup];
		mContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup:sharegroup];
	}
	else
		mContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	
	if( ! mContext ) {
		[self release];
		return;
	}
	else
		[EAGLContext setCurrentContext:mContext];
	
	cinder::gl::Environment::setEs2();
	
	// force Cinder's context to be allocated
	std::shared_ptr<cinder::gl::Context::PlatformData> platformData( new cinder::gl::PlatformDataIos( mContext ) );
	platformData->mObjectTracking = mObjectTracking;
	mCinderContext = cinder::gl::Context::createFromExisting( platformData );
	mCinderContext->makeCurrent();

	// Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
	glGenFramebuffers( 1, &mViewFramebuffer );
	glGenRenderbuffers( 1, &mViewRenderBuffer );
	mCinderContext->bindFramebuffer( GL_FRAMEBUFFER, mViewFramebuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, mViewRenderBuffer );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mViewRenderBuffer );
	
	if( mUsingMsaa ) {
		glGenFramebuffers( 1, &mMsaaFramebuffer );
		glGenRenderbuffers( 1, &mMsaaRenderBuffer );
		
		mCinderContext->bindFramebuffer( GL_FRAMEBUFFER, mMsaaFramebuffer );
		glBindRenderbuffer( GL_RENDERBUFFER, mMsaaRenderBuffer );
		
		glRenderbufferStorageMultisampleAPPLE( GL_RENDERBUFFER, mMsaaSamples, GL_RGBA8_OES, 0, 0 );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mMsaaRenderBuffer );
		
		if( ! mUsingStencil ) {
			glGenRenderbuffers( 1, &mDepthRenderBuffer );
			glBindRenderbuffer( GL_RENDERBUFFER, mDepthRenderBuffer );
			glRenderbufferStorageMultisampleAPPLE( GL_RENDERBUFFER, mMsaaSamples, GL_DEPTH_COMPONENT16, 0, 0  );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderBuffer );
		}
		else {
			glGenRenderbuffers( 1, &mDepthRenderBuffer );
			glBindRenderbuffer( GL_RENDERBUFFER, mDepthRenderBuffer );
			glRenderbufferStorageMultisampleAPPLE( GL_RENDERBUFFER, mMsaaSamples, GL_DEPTH24_STENCIL8_OES, 0, 0  );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderBuffer );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderBuffer );
		}
	}
	else {
		if( ! mUsingStencil ) {
			glGenRenderbuffers( 1, &mDepthRenderBuffer );
			glBindRenderbuffer( GL_RENDERBUFFER, mDepthRenderBuffer );
			glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 0, 0 );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderBuffer );
		}
		else {
			glGenRenderbuffers( 1, &mDepthRenderBuffer );
			glBindRenderbuffer( GL_RENDERBUFFER, mDepthRenderBuffer );
			glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, 0, 0 );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderBuffer );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderBuffer );
		}
	}
}

- (EAGLContext*)getEaglContext
{
	return mContext;
}

- (void)layoutSubviews
{
	[EAGLContext setCurrentContext:mContext];
	// Allocate color buffer backing based on the current layer size
	mCinderContext->bindFramebuffer( GL_FRAMEBUFFER, mViewFramebuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, mViewRenderBuffer );
	[mContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)mCinderView.layer];
	glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &mBackingWidth );
	glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &mBackingHeight );
	
	if( mUsingMsaa ) {
		mCinderContext->bindFramebuffer( GL_FRAMEBUFFER, mMsaaFramebuffer );
		if( ! mUsingStencil ) {
			glBindRenderbuffer( GL_RENDERBUFFER, mDepthRenderBuffer );
			glRenderbufferStorageMultisampleAPPLE( GL_RENDERBUFFER, mMsaaSamples, GL_DEPTH_COMPONENT16, mBackingWidth, mBackingHeight );
			glBindRenderbuffer( GL_RENDERBUFFER, mMsaaRenderBuffer );
			glRenderbufferStorageMultisampleAPPLE( GL_RENDERBUFFER, mMsaaSamples, GL_RGBA8_OES, mBackingWidth, mBackingHeight );
		}
		else {
			glBindRenderbuffer( GL_RENDERBUFFER, mDepthRenderBuffer );
			glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, mMsaaSamples, GL_DEPTH24_STENCIL8_OES, mBackingWidth, mBackingWidth );
			glBindRenderbuffer( GL_RENDERBUFFER, mMsaaRenderBuffer );
			glRenderbufferStorageMultisampleAPPLE( GL_RENDERBUFFER, mMsaaSamples, GL_RGBA8_OES, mBackingWidth, mBackingHeight );
		}
	}
	else {
		if ( ! mUsingStencil ) {
			glBindRenderbuffer( GL_RENDERBUFFER, mDepthRenderBuffer );
			glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mBackingWidth, mBackingHeight );
		}
		else {
			glBindRenderbuffer( GL_RENDERBUFFER, mDepthRenderBuffer );
			glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, mBackingWidth, mBackingHeight );
		}
	}
	
	if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ) {
		NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
	}
}

- (void)makeCurrentContext
{
	mCinderContext->makeCurrent();
    
	// This application only creates a single default framebuffer which is already bound at this point.
	// This call is redundant, but needed if dealing with multiple framebuffers.
	if( mUsingMsaa ) {
		mCinderContext->bindFramebuffer( GL_FRAMEBUFFER, mMsaaFramebuffer );
	}
	else {
		mCinderContext->bindFramebuffer( GL_FRAMEBUFFER, mViewFramebuffer );
	}
//	ci::gl::viewport( 0, 0, mBackingWidth, mBackingHeight );
}

- (void)flushBuffer
{
	if( mUsingMsaa ) {		
		mCinderContext->bindFramebuffer( GL_READ_FRAMEBUFFER_APPLE, mMsaaFramebuffer );
		mCinderContext->bindFramebuffer( GL_DRAW_FRAMEBUFFER_APPLE, mViewFramebuffer );
		
		glResolveMultisampleFramebufferAPPLE();

		mCinderContext->bindFramebuffer( GL_FRAMEBUFFER, mMsaaFramebuffer );
		GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT };
		glDiscardFramebufferEXT( GL_FRAMEBUFFER, 2, attachments );
	}
	
    glBindRenderbuffer( GL_RENDERBUFFER, mViewRenderBuffer );
    [mContext presentRenderbuffer:GL_RENDERBUFFER];
	
	if( mUsingMsaa ) {
	}
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
