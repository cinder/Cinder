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

#import "cinder/app/AppBase.h"
#import "cinder/app/RendererGl.h"

#import <Foundation/Foundation.h>

@class AppImplMac;
@class CinderViewMac;
@class NSOpenGLView;
@class NSOpenGLPixelFormat;

@interface RendererImplGlMac : NSObject {
	NSOpenGLView*					mView;
	cinder::app::RendererGl*		mRenderer;		// equivalent of a weak_ptr; 'renderer' actually owns us
	NSView*							mCinderView;
	cinder::gl::ContextRef			mContext;
}

- (id)initWithFrame:(NSRect)frame cinderView:(NSView*)cinderView renderer:(cinder::app::RendererGl *)renderer sharedRenderer:(cinder::app::RendererGlRef)sharedRenderer withRetina:(BOOL)retinaEnabled;
- (NSOpenGLView*)view;

- (void)makeCurrentContext:(bool)force;
- (CGLContextObj)getCglContext;
- (CGLPixelFormatObj)getCglPixelFormat;
- (NSOpenGLContext*)getNsOpenGlContext;
- (void)flushBuffer;
- (void)setFrameSize:(CGSize)newSize;
- (void)defaultResize;

- (BOOL)needsDrawRect;

+ (NSOpenGLPixelFormat*)defaultPixelFormat: (cinder::app::RendererGl::Options)rendererOptions;

@end
