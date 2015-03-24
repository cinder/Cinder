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

#import <UIKit/UIKit.h>

#import "cinder/app/AppBase.h"
#import "cinder/app/RendererGl.h"

@interface RendererImplGlCocoaTouch : NSObject
{
	cinder::app::RendererGl			*mRenderer; // equivalent of a weak_ptr; 'renderer' owns this // TODO: remove, this is unused
	UIView							*mCinderView;
	EAGLContext						*mContext;
	cinder::gl::ContextRef			mCinderContext;

	// The pixel dimensions of the CAEAGLLayer
	GLint 			mBackingWidth, mBackingHeight;
	
	// The dimensions of the CAEAGLLayer in points
	GLint			mPointsWidth, mPointsHeight;
	
	// The OpenGL names for the framebuffer and renderbuffer used to render to this view
	GLuint 			mViewFramebuffer, mViewRenderbuffer, mDepthRenderbuffer;
	GLuint			mMsaaFramebuffer, mMsaaRenderbuffer;
	GLint			mColorInternalFormat, mDepthInternalFormat;
	BOOL			mUsingMsaa;
    BOOL            mUsingStencil;
	BOOL			mObjectTracking;
	int				mMsaaSamples;
}

- (id)initWithFrame:(CGRect)frame cinderView:(UIView *)cinderView renderer:(cinder::app::RendererGl *)renderer sharedRenderer:(cinder::app::RendererGlRef)sharedRenderer;

- (EAGLContext*)getEaglContext;
- (void)makeCurrentContext:(bool)force;
- (void)flushBuffer;
- (void)setFrameSize:(CGSize)newSize;
- (void)defaultResize;

- (BOOL)needsDrawRect;

@end
