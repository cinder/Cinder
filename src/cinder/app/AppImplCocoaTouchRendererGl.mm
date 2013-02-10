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

@interface AppImplCocoaTouchRendererGl ()

- (void)layoutSubviews;
- (void)allocateGraphics:(cinder::app::RendererGlRef)sharedRenderer;

@end

@implementation AppImplCocoaTouchRendererGl

- (id)initWithFrame:(CGRect)frame cinderView:(UIView*)cinderView app:(cinder::app::App*)app renderer:(cinder::app::RendererGl*)renderer sharedRenderer:(cinder::app::RendererGlRef)sharedRenderer
{
	mCinderView = cinderView;
	mApp = app;

	CAEAGLLayer *eaglLayer = (CAEAGLLayer *)cinderView.layer;
	eaglLayer.opaque = TRUE;
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
										[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

	mBackingWidth	= 0;
	mBackingHeight	= 0;
	
	mPointsWidth	= 0;
	mPointsHeight	= 0;
	
	
	mMsaaSamples = cinder::app::RendererGl::sAntiAliasingSamples[renderer->getAntiAliasing()];
	mUsingMsaa = mMsaaSamples > 0;

	[self allocateGraphics:sharedRenderer];

	return self;	
}

- (void)allocateGraphics:(cinder::app::RendererGlRef)sharedRenderer
{
	if( sharedRenderer ) {
		EAGLSharegroup *sharegroup = [sharedRenderer->getEaglContext() sharegroup];
		mContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1 sharegroup:sharegroup];
	}
	else
		mContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
	
	if( ( ! mContext ) || ( ! [EAGLContext setCurrentContext:mContext] ) ) {
		[self release];
		return;
	}
	
	// Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
	glGenFramebuffersOES( 1, &mViewFramebuffer );
	glGenRenderbuffersOES( 1, &mViewRenderBuffer );
	glBindFramebufferOES( GL_FRAMEBUFFER_OES, mViewFramebuffer );
	glBindRenderbufferOES( GL_RENDERBUFFER_OES, mViewRenderBuffer );
	glFramebufferRenderbufferOES( GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, mViewRenderBuffer );

	if( mUsingMsaa ) {
		glGenFramebuffersOES( 1, &mMsaaFramebuffer );
		glGenRenderbuffersOES( 1, &mMsaaRenderBuffer );
		
		glBindFramebufferOES( GL_FRAMEBUFFER_OES, mMsaaFramebuffer );
		glBindRenderbufferOES( GL_RENDERBUFFER_OES, mMsaaRenderBuffer );
		
		glRenderbufferStorageMultisampleAPPLE( GL_RENDERBUFFER_OES, mMsaaSamples, GL_RGB5_A1_OES, 0, 0 );
		glFramebufferRenderbufferOES( GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, mMsaaRenderBuffer );

		glGenRenderbuffersOES( 1, &mDepthRenderBuffer );		
		glBindRenderbufferOES( GL_RENDERBUFFER_OES, mDepthRenderBuffer );
		glRenderbufferStorageMultisampleAPPLE( GL_RENDERBUFFER_OES, mMsaaSamples, GL_DEPTH_COMPONENT16_OES, 0, 0  );
		glFramebufferRenderbufferOES( GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, mDepthRenderBuffer );
	}
	else {
		glGenRenderbuffersOES( 1, &mDepthRenderBuffer );
		glBindRenderbufferOES( GL_RENDERBUFFER_OES, mDepthRenderBuffer );
		glRenderbufferStorageOES( GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, 0, 0 );
		glFramebufferRenderbufferOES( GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, mDepthRenderBuffer );
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
	glBindFramebufferOES( GL_FRAMEBUFFER_OES, mViewFramebuffer );
	glBindRenderbufferOES( GL_RENDERBUFFER_OES, mViewRenderBuffer );
	[mContext renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)mCinderView.layer];
	glGetRenderbufferParameterivOES( GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &mBackingWidth );
	glGetRenderbufferParameterivOES( GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &mBackingHeight );

	if( mUsingMsaa ) {
		glBindFramebufferOES( GL_FRAMEBUFFER_OES, mMsaaFramebuffer );
		glBindRenderbufferOES( GL_RENDERBUFFER_OES, mDepthRenderBuffer );
		glRenderbufferStorageMultisampleAPPLE( GL_RENDERBUFFER_OES, mMsaaSamples, GL_DEPTH_COMPONENT16_OES, mBackingWidth, mBackingHeight );
		glBindRenderbufferOES( GL_RENDERBUFFER_OES, mMsaaRenderBuffer );
		glRenderbufferStorageMultisampleAPPLE( GL_RENDERBUFFER_OES, mMsaaSamples, GL_RGB5_A1_OES, mBackingWidth, mBackingHeight );		
	}
	else {
		glBindRenderbufferOES( GL_RENDERBUFFER_OES, mDepthRenderBuffer );
		glRenderbufferStorageOES( GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, mBackingWidth, mBackingHeight );
	}

	if( glCheckFramebufferStatusOES( GL_FRAMEBUFFER_OES ) != GL_FRAMEBUFFER_COMPLETE_OES ) {
		NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
	}
}

- (void)makeCurrentContext
{
	[EAGLContext setCurrentContext:mContext];
    
	// This application only creates a single default framebuffer which is already bound at this point.
	// This call is redundant, but needed if dealing with multiple framebuffers.
	if( mUsingMsaa ) {
		glBindFramebufferOES( GL_FRAMEBUFFER_OES, mMsaaFramebuffer );
	}
	else {
		glBindFramebufferOES( GL_FRAMEBUFFER_OES, mViewFramebuffer );
	}
    
	glViewport( 0, 0, mBackingWidth, mBackingHeight );
	
}

- (void)flushBuffer
{
	if( mUsingMsaa ) {
		GLenum attachments[] = { GL_DEPTH_ATTACHMENT_OES };
		glDiscardFramebufferEXT( GL_READ_FRAMEBUFFER_APPLE, 1, attachments ); 
		
		glBindFramebufferOES( GL_READ_FRAMEBUFFER_APPLE, mMsaaFramebuffer );
		glBindFramebufferOES( GL_DRAW_FRAMEBUFFER_APPLE, mViewFramebuffer );
		
		glResolveMultisampleFramebufferAPPLE();	
	}

    glBindRenderbufferOES( GL_RENDERBUFFER_OES, mViewRenderBuffer );
    [mContext presentRenderbuffer:GL_RENDERBUFFER_OES];
}

- (void)setFrameSize:(CGSize)newSize
{
	[self layoutSubviews];
}

- (void)defaultResize
{
	glViewport( 0, 0, mBackingWidth, mBackingHeight );
	ci::gl::setMatricesWindowPersp( mCinderView.bounds.size.width, mCinderView.bounds.size.height );
}

- (BOOL)needsDrawRect
{
	return NO;
}

@end
