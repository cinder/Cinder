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

#include "cinder/app/RendererMetal.h"
#include <glfw/glfw3.h>

#if defined( CINDER_MAC )

//#include "cinder/app/glfw/RendererImplGlfw.h"
#include "cinder/app/glfw/RendererImplGlfwMetal.h"
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

namespace cinder { namespace app {

RendererMetal::RendererMetal( const RendererMetal::Options &options )
	: Renderer(), mImpl( nullptr ), mOptions( options )
{}

RendererMetal::RendererMetal( const RendererMetal &renderer )
	: Renderer( renderer ), mImpl( nullptr ), mOptions( renderer.mOptions )
{}

RendererMetal::~RendererMetal()
{
	delete mImpl;
}

void RendererMetal::setup( void* nativeWindow, RendererRef sharedRenderer )
{
	if( ! mImpl ) {
		mImpl = new RendererImplGlfwMetal( this );
	}

	if( ! mImpl->initialize( (GLFWwindow*)nativeWindow, sharedRenderer ) ) {
		throw ExcRendererAllocation( "RendererMetalGlfw initialization failed." );
	}
}

RendererImplGlfw* RendererMetal::getGlfwRendererImpl()
{
	return mImpl;
}

void RendererMetal::startDraw()
{
	if( mStartDrawFn ) {
		mStartDrawFn( this );
	}
	else {
		// Begin a new Metal frame
		if( mImpl )
            mImpl->beginFrame();
	}
}

void RendererMetal::makeCurrentContext( bool force )
{
	mImpl->makeCurrentContext( force );
}

void RendererMetal::swapBuffers()
{
	mImpl->swapBuffers();
}

void RendererMetal::finishDraw()
{
	if( mFinishDrawFn ) {
		mFinishDrawFn( this );
	}
	else {
		// End the Metal frame and present
		if( mImpl )
            mImpl->endFrame();
	}
}

void RendererMetal::defaultResize()
{
	mImpl->defaultResize();
}

void* RendererMetal::getMetalDevice() const
{
	if( mImpl )
        return mImpl->getMetalDevice();
	return nullptr;
}

void* RendererMetal::getMetalCommandQueue() const
{
	if( mImpl )
		return mImpl->getMetalCommandQueue();

	return nullptr;
}

void* RendererMetal::getMetalLayer() const
{
    if( mImpl )
        return mImpl->getMetalLayer();

    return nullptr;
}

Surface8u RendererMetal::copyWindowSurface( const Area &area, int32_t windowHeightPixels )
{
	@autoreleasepool {
		// Get Metal layer from the renderer implementation
		CAMetalLayer* metalLayer = (__bridge CAMetalLayer*)getMetalLayer();
		if( ! metalLayer ) {
			return Surface8u( area.getWidth(), area.getHeight(), false );
		}

		// Get the current drawable's texture
		id<CAMetalDrawable> drawable = [metalLayer nextDrawable];
		if( ! drawable ) {
			return Surface8u( area.getWidth(), area.getHeight(), false );
		}

		id<MTLTexture> sourceTexture = drawable.texture;
		id<MTLDevice> device = (__bridge id<MTLDevice>)getMetalDevice();
		id<MTLCommandQueue> commandQueue = (__bridge id<MTLCommandQueue>)getMetalCommandQueue();

		if( ! sourceTexture || ! device || ! commandQueue ) {
			return Surface8u( area.getWidth(), area.getHeight(), false );
		}

		// Create a readable texture descriptor
		MTLTextureDescriptor* readableDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
																								 width:area.getWidth()
																								height:area.getHeight()
																							 mipmapped:NO];
		readableDesc.usage = MTLTextureUsageShaderWrite | MTLTextureUsageShaderRead;
		readableDesc.storageMode = MTLStorageModeShared;  // Shared for CPU readback

		id<MTLTexture> readableTexture = [device newTextureWithDescriptor:readableDesc];
		if( ! readableTexture ) {
			return Surface8u( area.getWidth(), area.getHeight(), false );
		}

		// Create command buffer and blit encoder to copy the texture
		id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
		id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];

		// Copy from drawable texture to readable texture
		[blitEncoder copyFromTexture:sourceTexture
						 sourceSlice:0
						 sourceLevel:0
						sourceOrigin:MTLOriginMake( area.getX1(), area.getY1(), 0 )
						  sourceSize:MTLSizeMake( area.getWidth(), area.getHeight(), 1 )
						   toTexture:readableTexture
					destinationSlice:0
					destinationLevel:0
				   destinationOrigin:MTLOriginMake( 0, 0, 0 )];

		[blitEncoder endEncoding];
		[commandBuffer commit];
		[commandBuffer waitUntilCompleted];

		// Create surface and copy pixel data
		Surface8u surface( area.getWidth(), area.getHeight(), false, SurfaceChannelOrder::BGRA );

		// Get bytes from the readable texture
		NSUInteger bytesPerRow = area.getWidth() * 4;  // BGRA = 4 bytes per pixel
		[readableTexture getBytes:surface.getData()
					  bytesPerRow:bytesPerRow
					   fromRegion:MTLRegionMake2D( 0, 0, area.getWidth(), area.getHeight() )
					  mipmapLevel:0];

		// Metal uses top-left origin, but Surface8u expects bottom-left for OpenGL compatibility
		// Flip the surface vertically
		Surface8u flipped( area.getWidth(), area.getHeight(), false, SurfaceChannelOrder::BGRA );
		for( int32_t y = 0; y < area.getHeight(); ++y ) {
			std::memcpy( flipped.getData( ivec2( 0, area.getHeight() - 1 - y ) ),
						 surface.getData( ivec2( 0, y ) ),
						 area.getWidth() * 4 );
		}

		[readableTexture release];

		return flipped;
	}
}

} } // namespace cinder::app

#endif // CINDER_MAC
