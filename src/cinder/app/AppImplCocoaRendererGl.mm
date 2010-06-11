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

#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>

#import "cinder/app/AppImplCocoaRendererGl.h"
#import "cinder/app/CinderView.h"

#import "cinder/app/App.h"
#include "cinder/app/Renderer.h"
#include <iostream>

// This is only here so that we can override isOpaque, which is necessary
// for the ScreenSaverView to show it
@interface AppImplCocoaTransparentGlView : NSOpenGLView 
{}
@end

@implementation AppImplCocoaTransparentGlView
- (BOOL)isOpaque
{ return NO; }
// For whatever reasons, rightMouseDown does not get propagated along the normal responder chain
- (void)rightMouseDown:(NSEvent *)theEvent
{
	[[self superview] rightMouseDown:theEvent];
}

@end

@implementation AppImplCocoaRendererGl

- (id)initWithFrame:(NSRect)frame cinderView:(NSView*)aCinderView app:(cinder::app::App*)aApp renderer:(cinder::app::RendererGl*)aRenderer
{
//	self = [super initWithFrame:frame cinderView:aCinderView app:aApp];
	app = aApp;
	cinderView = aCinderView;
	
	renderer = aRenderer;
	
	NSOpenGLPixelFormat* fmt = [AppImplCocoaRendererGl defaultPixelFormat:renderer->getAntiAliasing()];
	GLint aaSamples;
	[fmt getValues:&aaSamples forAttribute:NSOpenGLPFASamples forVirtualScreen:0];
	renderer->setAntiAliasing( aaSamples );

	view = [[AppImplCocoaTransparentGlView alloc] initWithFrame:frame pixelFormat:fmt];
	[cinderView addSubview:view];
	[[view openGLContext] makeCurrentContext];

	GLint swapInterval = 1;
	[[view openGLContext] setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];

	return self;
}

- (NSOpenGLView*)view
{
	return view;
}

- (CGLContextObj)getCglContext
{
	return (CGLContextObj)[[view openGLContext] CGLContextObj];
}

- (CGLPixelFormatObj)getCglPixelFormat
{
	return (CGLPixelFormatObj)[[view pixelFormat] CGLPixelFormatObj];
}

- (NSOpenGLContext *)getNSOpenGLContext
{
	return [view openGLContext];
}

- (NSOpenGLPixelFormat *)getNSOpenGLPixelFormat
{
	return [view pixelFormat];
}

- (void)makeCurrentContext
{
	[[view openGLContext] makeCurrentContext];
}

- (void)flushBuffer
{	
	[[NSOpenGLContext currentContext] flushBuffer];
}

- (void)drawRect:(NSRect)rect
{
	[view drawRect:rect];
}

- (void)setFrameSize:(CGSize)newSize
{
//	[super setFrameSize:newSize];
	[view setFrameSize:NSSizeFromCGSize( newSize )];
}

- (void)defaultResize
{
	glViewport( 0, 0, app->getWindowWidth(), app->getWindowHeight() );
	cinder::CameraPersp cam( app->getWindowWidth(), app->getWindowHeight(), 60.0f );

	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( cam.getProjectionMatrix().m );

	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( cam.getModelViewMatrix().m );
	glScalef( 1.0f, -1.0f, 1.0f );           // invert Y axis so increasing Y goes down.
	glTranslatef( 0.0f, (float)-app->getWindowHeight(), 0.0f );       // shift origin up to upper-left corner.
}

- (BOOL)acceptsFirstResponder
{
	return NO;
}

- (void)dealloc 
{
	[super dealloc];
}

// For whatever reason, NSOpenGLView doesn't seem to get a drawRect call in response to a setNeedsDisplay:YES
- (BOOL)needsDrawRect
{
	return YES;
}

+ (NSOpenGLPixelFormat*)defaultPixelFormat:(int)antialiasLevel
{
	if( antialiasLevel == cinder::app::RendererGl::AA_NONE ) {
		NSOpenGLPixelFormatAttribute attributes [] = {
			NSOpenGLPFAWindow,
			NSOpenGLPFADoubleBuffer,
			NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)24,
	        NSOpenGLPFASampleBuffers, (NSOpenGLPixelFormatAttribute)1,
			NSOpenGLPFANoRecovery,
			(NSOpenGLPixelFormatAttribute)0
		};
		
		return [[[NSOpenGLPixelFormat alloc] 
				 initWithAttributes:attributes] autorelease];
	}
	else {
		NSOpenGLPixelFormatAttribute attributes [] = {
			NSOpenGLPFAWindow,
			NSOpenGLPFADoubleBuffer,
			NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)24,
	        NSOpenGLPFASampleBuffers, (NSOpenGLPixelFormatAttribute)1, 
			NSOpenGLPFASamples, (NSOpenGLPixelFormatAttribute)cinder::app::RendererGl::sAntiAliasingSamples[antialiasLevel],
			NSOpenGLPFANoRecovery,
			(NSOpenGLPixelFormatAttribute)0
		};
		
		
		return [[[NSOpenGLPixelFormat alloc] 
				 initWithAttributes:attributes] autorelease];
	}
}

@end
