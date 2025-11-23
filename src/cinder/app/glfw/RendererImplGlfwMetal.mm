/*
 Copyright (c) 2025, The Cinder Project, All rights reserved.

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

#include "cinder/app/glfw/RendererImplGlfwMetal.h"
#include "cinder/app/RendererMetal.h"
#include "cinder/Log.h"
#include <algorithm>

#if defined( CINDER_MAC )

#define GLFW_EXPOSE_NATIVE_COCOA
#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#import <QuartzCore/CATransaction.h>
#import <QuartzCore/QuartzCore.h>
#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>

namespace {
static const void *kCinderMetalLayerKey = &kCinderMetalLayerKey;

static CAMetalLayer* cinderGetMetalLayerForView( NSView *view )
{
	return view ? (__bridge CAMetalLayer*)objc_getAssociatedObject( view, kCinderMetalLayerKey ) : nil;
}

static void cinderSetMetalLayerForView( NSView *view, CAMetalLayer *layer )
{
	if( ! view )
		return;
	objc_setAssociatedObject( view, kCinderMetalLayerKey, layer, OBJC_ASSOCIATION_RETAIN_NONATOMIC );
}

static CALayer* cinderEnsureHostLayer( NSView *view )
{
	if( ! view )
		return nil;
	if( ! view.wantsLayer ) {
		view.wantsLayer = YES;
	}
	CALayer *hostLayer = view.layer;
	if( ! hostLayer ) {
		view.layer = [CALayer layer];
		hostLayer = view.layer;
	}
	hostLayer.contentsGravity = kCAGravityResize;
	hostLayer.needsDisplayOnBoundsChange = YES;
	hostLayer.masksToBounds = YES;
	return hostLayer;
}

static void cinderAttachMetalLayerToView( NSView *view, CAMetalLayer *metalLayer )
{
	if( ! view || ! metalLayer )
		return;

	CALayer *hostLayer = cinderEnsureHostLayer( view );
	if( ! hostLayer )
		return;

	if( metalLayer.superlayer != hostLayer ) {
		[metalLayer removeFromSuperlayer];
		[hostLayer addSublayer:metalLayer];
	}
	metalLayer.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
	metalLayer.frame = hostLayer.bounds;
}
} // anonymous namespace

namespace cinder { namespace app {

RendererImplGlfwMetal::RendererImplGlfwMetal( RendererMetal *aRenderer )
	: mRenderer( aRenderer ), mGlfwWindow( nullptr ),
	  mMetalDevice( nullptr ), mMetalCommandQueue( nullptr )
{
}

RendererImplGlfwMetal::~RendererImplGlfwMetal()
{
	kill();
}

void RendererImplGlfwMetal::prepareWindowHints()
{
	// Tell GLFW we're not using OpenGL
	::glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
}

bool RendererImplGlfwMetal::initialize( GLFWwindow *window, RendererRef sharedRenderer )
{
	mGlfwWindow = window;

	if( sharedRenderer ) {
		CI_LOG_E( "RendererImplGlfwMetal does not support shared renderers yet." );
	}

	@autoreleasepool {
		// Create Metal device
		id<MTLDevice> device = MTLCreateSystemDefaultDevice();
		if( ! device ) {
			CI_LOG_E( "Failed to create Metal device" );
			return false;
		}
		mMetalDevice = (__bridge_retained CFTypeRef)device;

		// Create command queue
		id<MTLCommandQueue> commandQueue = [device newCommandQueue];
		if( ! commandQueue ) {
			CI_LOG_E( "Failed to create Metal command queue" );
			return false;
		}
		mMetalCommandQueue = (__bridge_retained CFTypeRef)commandQueue;

		// Get the NSWindow and set up the Metal layer
		NSWindow *nsWindow = glfwGetCocoaWindow( mGlfwWindow );
		if( ! nsWindow ) {
			CI_LOG_E( "Failed to get NSWindow from GLFW window" );
			return false;
		}

		CAMetalLayer *metalLayer = [CAMetalLayer layer];
		configureMetalLayer( metalLayer, nsWindow.contentView );
		updateLayerDrawableSize( metalLayer, nsWindow );
	}

	return true;
}

void RendererImplGlfwMetal::kill()
{
	std::lock_guard<std::recursive_mutex> lock( mMetalLayerMutex );

	if( mMetalCommandQueue ) {
		CFRelease( mMetalCommandQueue );
		mMetalCommandQueue = nullptr;
	}
	if( mMetalDevice ) {
		CFRelease( mMetalDevice );
		mMetalDevice = nullptr;
	}
}

void RendererImplGlfwMetal::defaultResize() const
{
	std::lock_guard<std::recursive_mutex> lock( mMetalLayerMutex );

	if( ! mGlfwWindow )
		return;

	@autoreleasepool {
		NSWindow *nsWindow = glfwGetCocoaWindow( mGlfwWindow );
		if( ! nsWindow )
			return;

		CAMetalLayer *metalLayer = ensureMetalLayer( nsWindow );
		updateLayerDrawableSize( metalLayer, nsWindow );
	}
}

void RendererImplGlfwMetal::makeCurrentContext( bool force )
{
	// Metal doesn't have a "current context" concept like OpenGL
	// This is a no-op for Metal
}

void RendererImplGlfwMetal::beginFrame()
{
	// Lock the mutex to protect Metal layer access during rendering
	mMetalLayerMutex.lock();
}

void RendererImplGlfwMetal::endFrame()
{
	// Unlock the mutex after rendering is complete
	mMetalLayerMutex.unlock();
}

void RendererImplGlfwMetal::swapBuffers() const
{
	// Metal presentation is handled in endFrame()
	// This is a no-op for Metal
}

void* RendererImplGlfwMetal::getMetalDevice() const
{
	return const_cast<void*>( mMetalDevice );
}

void* RendererImplGlfwMetal::getMetalCommandQueue() const
{
	return const_cast<void*>( mMetalCommandQueue );
}

void* RendererImplGlfwMetal::getMetalLayer() const
{
	std::lock_guard<std::recursive_mutex> lock( mMetalLayerMutex );

	if( ! mGlfwWindow )
		return nullptr;

	@autoreleasepool {
		NSWindow *nsWindow = glfwGetCocoaWindow( mGlfwWindow );
		if( ! nsWindow )
			return nullptr;

		CAMetalLayer *metalLayer = ensureMetalLayer( nsWindow );
		updateLayerDrawableSize( metalLayer, nsWindow );
		return (__bridge void*)metalLayer;
	}
}

void RendererImplGlfwMetal::configureMetalLayer( CAMetalLayer *metalLayer, NSView *contentView ) const
{
	if( ! metalLayer )
		return;

	if( mMetalDevice ) {
		metalLayer.device = (__bridge id<MTLDevice>)mMetalDevice;
	}

	metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
	metalLayer.opaque = YES;
	metalLayer.framebufferOnly = YES;
	metalLayer.needsDisplayOnBoundsChange = NO;
	if( [metalLayer respondsToSelector:@selector(setAllowsNextDrawableTimeout:)] ) {
		metalLayer.allowsNextDrawableTimeout = NO;
	}
	if( [metalLayer respondsToSelector:@selector(setDisplaySyncEnabled:)] ) {
		metalLayer.displaySyncEnabled = NO;
	}
	if( [metalLayer respondsToSelector:@selector(setMaximumDrawableCount:)] ) {
		metalLayer.maximumDrawableCount = 3;
	}
	if( [metalLayer respondsToSelector:@selector(setPresentsWithTransaction:)] ) {
		metalLayer.presentsWithTransaction = NO;
	}
	// Prevent implicit Core Animation fades when we update frame/drawableSize
	metalLayer.actions = @{
		@"bounds": [NSNull null],
		@"position": [NSNull null],
		@"contents": [NSNull null],
		@"opacity": [NSNull null]
	};

	if( contentView ) {
		cinderSetMetalLayerForView( contentView, metalLayer );
		if( [contentView respondsToSelector:@selector(setLayerContentsRedrawPolicy:)] ) {
			contentView.layerContentsRedrawPolicy = NSViewLayerContentsRedrawNever;
		}
		if( [contentView respondsToSelector:@selector(setCanDrawSubviewsIntoLayer:)] ) {
			contentView.canDrawSubviewsIntoLayer = YES;
		}
		cinderAttachMetalLayerToView( contentView, metalLayer );
	}
}

CAMetalLayer* RendererImplGlfwMetal::ensureMetalLayer( NSWindow *nsWindow ) const
{
	if( ! nsWindow )
		return nil;

	NSView *contentView = nsWindow.contentView;
	if( ! contentView )
		return nil;

	CAMetalLayer *metalLayer = cinderGetMetalLayerForView( contentView );
	if( ! metalLayer ) {
		CI_LOG_W( "CAMetalLayer was missing on NSView, creating..." );
		metalLayer = [CAMetalLayer layer];
		configureMetalLayer( metalLayer, contentView );
	}
	else {
		cinderAttachMetalLayerToView( contentView, metalLayer );
	}
	return metalLayer;
}

void RendererImplGlfwMetal::updateLayerDrawableSize( CAMetalLayer *metalLayer, NSWindow *nsWindow ) const
{
	if( ! metalLayer )
		return;

	CGSize drawableSize = CGSizeZero;
	CGRect layerFrame = CGRectZero;
	CGFloat contentsScale = 1.0f;

	if( nsWindow && nsWindow.contentView ) {
		NSView *contentView = nsWindow.contentView;
		layerFrame = contentView.bounds;

		if( [nsWindow respondsToSelector:@selector(backingScaleFactor)] ) {
			contentsScale = std::max<CGFloat>( 1.0f, nsWindow.backingScaleFactor );
		}

		drawableSize = CGSizeMake( layerFrame.size.width * contentsScale, layerFrame.size.height * contentsScale );
	}
	else if( mGlfwWindow ) {
		int width = 0, height = 0;
		::glfwGetFramebufferSize( mGlfwWindow, &width, &height );
		if( width <= 0 || height <= 0 )
			return;

		drawableSize = CGSizeMake( width, height );
		layerFrame.size = CGSizeMake( width, height );
	}
	else if( metalLayer.superlayer ) {
		layerFrame = metalLayer.superlayer.bounds;
		drawableSize = layerFrame.size;
	}

	if( drawableSize.width <= 0 || drawableSize.height <= 0 )
		return;

	[CATransaction begin];
	[CATransaction setDisableActions:YES];
	metalLayer.drawableSize = drawableSize;
	metalLayer.frame = layerFrame;
	metalLayer.contentsScale = contentsScale;
	[CATransaction commit];
}

} } // namespace cinder::app

#endif // CINDER_MAC
