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

#include "cinder/app/Renderer.h"

#if defined( CINDER_MAC )

typedef struct GLFWwindow	GLFWwindow;

namespace cinder { namespace app {

class RendererImplGlfw;

typedef std::shared_ptr<class RendererMetal>	RendererMetalRef;

class CI_API RendererMetal : public Renderer {
  public:
	struct CI_API Options {
	  public:
		Options() {
			mMsaaSamples = 1;
			mDepthBufferBits = 24;
			mStencilBufferBits = 8;
		}

		//! Sets the number of samples used for Multisample Anti-Aliasing (MSAA). Valid values are 1, 2, 4, 8. Defaults to 1 (no MSAA).
		Options&	msaa( int samples ) { mMsaaSamples = samples; return *this; }
		//! Returns the number of samples used for Multisample Anti-Aliasing (MSAA).
		int			getMsaa() const { return mMsaaSamples; }

		//! Sets the number of bits dedicated to the depth buffer. Default is 24.
		Options&	depthBufferDepth( int depthBufferBits ) { mDepthBufferBits = depthBufferBits; return *this; }
		//! Returns the number of bits dedicated to the depth buffer. Default is 24.
		int			getDepthBufferDepth() const { return mDepthBufferBits; }

		//! Sets the number of bits dedicated to the stencil buffer. Default is 8.
		Options&	stencilBufferDepth( int stencilBufferBits ) { mStencilBufferBits = stencilBufferBits; return *this; }
		//! Returns the number of bits dedicated to the stencil buffer. Default is 8.
		int			getStencilBufferDepth() const { return mStencilBufferBits; }

	  protected:
		int		mMsaaSamples;
		int		mDepthBufferBits;
		int		mStencilBufferBits;
	};

	RendererMetal( const Options &options = Options() );
	~RendererMetal();

	static RendererMetalRef	create( const Options &options = Options() )	{ return RendererMetalRef( new RendererMetal( options ) ); }
	RendererRef				clone() const override							{ return RendererMetalRef( new RendererMetal( *this ) ); }

	virtual void			setup( void* nativeWindow, RendererRef sharedRenderer ) override;
	RendererImplGlfw*		getGlfwRendererImpl();

	const Options&	getOptions() const { return mOptions; }

	void			startDraw() override;
	void			finishDraw() override;
	void			defaultResize() override;
	void			makeCurrentContext( bool force = false ) override;
	void			swapBuffers() override;
	Surface8u		copyWindowSurface( const Area &area, int32_t windowHeightPixels ) override;

	//! Overrides Renderer's start draw implementation for custom hooks. Only useful in advanced use cases.
	void setStartDrawFn( const std::function<void( Renderer* )>& function ) { mStartDrawFn = function; }
	//! Overrides Renderer's finish draw implementation for custom hooks. Only useful in advanced use cases.
	void setFinishDrawFn( const std::function<void( Renderer* )>& function ) { mFinishDrawFn = function; }

	// Metal-specific accessors
	void*	getMetalDevice() const;
	void*	getMetalCommandQueue() const;
	void*	getMetalLayer() const;

protected:
	RendererMetal( const RendererMetal &renderer );

	Options		mOptions;

	// GLFW rendering (macOS only for Metal)
	class RendererImplGlfwMetal		*mImpl;
    RendererImplGlfwMetal		    *getImpl() { return mImpl; }

	std::function<void( Renderer* )> mStartDrawFn;
	std::function<void( Renderer* )> mFinishDrawFn;
};

} } // namespace cinder::app

#endif // CINDER_MAC
