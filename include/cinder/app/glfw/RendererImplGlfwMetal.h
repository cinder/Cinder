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

#pragma once

#include "cinder/app/glfw/RendererImplGlfw.h"
#include <mutex>

#if defined( CINDER_MAC )
#include <CoreFoundation/CoreFoundation.h>

#if defined( __OBJC__ )
	@class CAMetalLayer;
	@protocol MTLDevice;
	@protocol MTLCommandQueue;
	@protocol CAMetalDrawable;
	@class NSWindow;
	@class NSView;
#else
	typedef void CAMetalLayer;
	typedef void MTLDevice;
	typedef void MTLCommandQueue;
	typedef void CAMetalDrawable;
	typedef void NSWindow;
	class NSView;
#endif

namespace cinder { namespace app {

class RendererMetal;

class RendererImplGlfwMetal : public RendererImplGlfw {
  public:
	RendererImplGlfwMetal( class RendererMetal *aRenderer );
	virtual ~RendererImplGlfwMetal();

	virtual void	prepareWindowHints() override;
	virtual bool	initialize( GLFWwindow *window, RendererRef sharedRenderer ) override;
	virtual void	kill() override;
	virtual void	defaultResize() const override;
	virtual void	swapBuffers() const override;
	virtual void	makeCurrentContext( bool force = false ) override;

	// Metal-specific methods
	void*			getMetalDevice() const;
	void*			getMetalCommandQueue() const;
	void*			getMetalLayer() const;

	void			beginFrame();
	void			endFrame();

  protected:
	void			configureMetalLayer( CAMetalLayer *metalLayer, NSView *contentView ) const;
	CAMetalLayer*	ensureMetalLayer( NSWindow *nsWindow ) const;
	void			updateLayerDrawableSize( CAMetalLayer *metalLayer, NSWindow *nsWindow ) const;

	class RendererMetal		*mRenderer;
	GLFWwindow				*mGlfwWindow;

	CFTypeRef				mMetalDevice;			// id<MTLDevice>
	CFTypeRef				mMetalCommandQueue;		// id<MTLCommandQueue>
	mutable std::recursive_mutex	mMetalLayerMutex;		// Protects layer/device access
};

} } // namespace cinder::app

#endif // CINDER_MAC
