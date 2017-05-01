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

#include "cinder/gl/platform.h"
#import "cinder/app/cocoa/RendererImplGlMac.h"
#import "cinder/app/cocoa/CinderViewMac.h"

#include "cinder/gl/Context.h"
#include "cinder/gl/Environment.h"
#include "cinder/Camera.h"
#include "cinder/Log.h"

#include <iostream>

#import <Cocoa/Cocoa.h>

// This is only here so that we can override isOpaque, which is necessary
// for the ScreenSaverView to show it
@interface RendererImplGlMacTransparentView : NSOpenGLView 
@end

@implementation RendererImplGlMacTransparentView

- (BOOL)isOpaque
{
	return NO;
}

// For whatever reasons, rightMouseDown does not get propagated along the normal responder chain
- (void)rightMouseDown:(NSEvent *)theEvent
{
	[[self superview] rightMouseDown:theEvent];
}

@end

@implementation RendererImplGlMac

- (id)initWithFrame:(NSRect)frame cinderView:(NSView*)cinderView renderer:(cinder::app::RendererGl *)renderer sharedRenderer:(cinder::app::RendererGlRef)sharedRenderer withRetina:(BOOL)retinaEnabled
{
	self = [super init];

	mCinderView = cinderView;
	mRenderer = renderer;

	cinder::app::RendererGl::Options options = mRenderer->getOptions();
	NSOpenGLPixelFormat* fmt = [RendererImplGlMac defaultPixelFormat:options];
	GLint aaSamples;
	[fmt getValues:&aaSamples forAttribute:NSOpenGLPFASamples forVirtualScreen:0];

	NSRect bounds = NSMakeRect( 0, 0, frame.size.width, frame.size.height );
	mView = [[RendererImplGlMacTransparentView alloc] initWithFrame:bounds pixelFormat:fmt];
	if( ! mView )
		CI_LOG_E( "Unable to allocate GL view" );

	// if we've been passed a context to share with, replace our NSOpenGLContext with a new one that shares
	if( sharedRenderer ) {
		auto sharedRendererPtr = sharedRenderer.get();
		assert( typeid( *sharedRendererPtr ) == typeid( cinder::app::RendererGl ) );
		NSOpenGLContext *newContext = [[NSOpenGLContext alloc] initWithFormat:fmt shareContext:sharedRenderer->getNsOpenGlContext()];
		[mView setOpenGLContext:newContext];
		[newContext release];
	}

	[mCinderView addSubview:mView];
	
	if( retinaEnabled )
		[mView setWantsBestResolutionOpenGLSurface:YES];
	
	cinder::gl::Environment::setCore();
	
	CGLContextObj cglContext = (CGLContextObj)[[mView openGLContext] CGLContextObj];
	::CGLSetCurrentContext( cglContext );
	auto platformData = std::shared_ptr<cinder::gl::Context::PlatformData>( new cinder::gl::PlatformDataMac( cglContext ) );
	platformData->mObjectTracking = options.getObjectTracking();
	mContext = cinder::gl::Context::createFromExisting( platformData );
	mContext->makeCurrent();

	GLint swapInterval = 1;
	[[mView openGLContext] setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];

	return self;
}

- (NSOpenGLView*)view
{
	return mView;
}

- (CGLContextObj)getCglContext
{
	return (CGLContextObj)[[mView openGLContext] CGLContextObj];
}

- (CGLPixelFormatObj)getCglPixelFormat
{
	return (CGLPixelFormatObj)[[mView pixelFormat] CGLPixelFormatObj];
}

- (NSOpenGLContext *)getNsOpenGlContext
{
	return [mView openGLContext];
}

- (NSOpenGLPixelFormat *)getNSOpenGLPixelFormat
{
	return [mView pixelFormat];
}

- (void)makeCurrentContext:(bool)force
{
	mContext->makeCurrent( force );
}

- (void)flushBuffer
{	
	[[NSOpenGLContext currentContext] flushBuffer];
}

- (void)drawRect:(NSRect)rect
{
	[mView drawRect:rect];
}

- (void)setFrameSize:(CGSize)newSize
{
//	[super setFrameSize:newSize];
	[mView setFrameSize:NSSizeFromCGSize( newSize )];
}

- (void)defaultResize
{
	NSSize nsSize = [mView frame].size;
	NSSize backingSize = [mView convertSizeToBacking:nsSize];
	cinder::gl::viewport( 0, 0, backingSize.width, backingSize.height );
	cinder::gl::setMatricesWindow( nsSize.width, nsSize.height );
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

+ (NSOpenGLPixelFormat*)defaultPixelFormat:(cinder::app::RendererGl::Options)rendererOptions
{
	NSOpenGLPixelFormat *result = nil;
    std::vector<NSOpenGLPixelFormatAttribute> attributes;
    
    attributes.push_back( NSOpenGLPFADoubleBuffer );
	
	attributes.push_back( NSOpenGLPFAOpenGLProfile );
	attributes.push_back( NSOpenGLProfileVersion3_2Core );
	
    attributes.push_back( NSOpenGLPFADepthSize );
	attributes.push_back( (NSOpenGLPixelFormatAttribute) rendererOptions.getDepthBufferDepth() );
	
    if( rendererOptions.getStencil() ) {
		attributes.push_back( kCGLPFAStencilSize );
		attributes.push_back( (CGLPixelFormatAttribute) 8 );
	}
	
	if( rendererOptions.getMsaa() > 0 ) {
		attributes.push_back( NSOpenGLPFASampleBuffers );
		attributes.push_back( (NSOpenGLPixelFormatAttribute) 1 );
		attributes.push_back( NSOpenGLPFASamples );
		attributes.push_back( (NSOpenGLPixelFormatAttribute) rendererOptions.getMsaa() );
		attributes.push_back( NSOpenGLPFAMultisample );
	}
	
	attributes.push_back( (NSOpenGLPixelFormatAttribute) 0 );
		
	result = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes.data()];

	assert( result );
	return [result autorelease];
}

@end
