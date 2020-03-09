/*
 Copyright (c) 2013, The Cinder Project, All rights reserved.

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
#include "cinder/gl/platform.h"

#if defined( CINDER_MAC )
	#if defined __OBJC__
		@class RendererImplGlMac;
		@class NSOpenGLContext;
	#else
		class RendererImplGlMac;
		class NSOpenGLContext;
	#endif
	typedef struct _CGLContextObject	   *CGLContextObj;
	typedef struct _CGLPixelFormatObject   *CGLPixelFormatObj;
#elif defined( CINDER_COCOA_TOUCH )
	#if defined __OBJC__
		typedef struct CGContext * CGContextRef;
		@class RendererImplGlCocoaTouch;
		@class EAGLContext;
	#else
		typedef struct CGContext * CGContextRef;
		class RendererImplGlCocoaTouch;
		class EAGLContext;
	#endif
#elif defined( CINDER_LINUX )
	typedef struct GLFWwindow	GLFWwindow;
#endif

namespace cinder { namespace gl {

class Context;
typedef std::shared_ptr<Context>		ContextRef;

} } // cinder::gl

namespace cinder { namespace app {

typedef std::shared_ptr<class RendererGl>	RendererGlRef;
class CI_API RendererGl : public Renderer {
  public:
	struct CI_API Options {
	  public:
		Options() {
#if defined( CINDER_COCOA_TOUCH )
			mMsaaSamples = 0;
			mCoreProfile = false;
			mVersion = std::pair<int,int>( 2, 0 );
#else
			mMsaaSamples = 0;
			mCoreProfile = true;
			mVersion = std::pair<int,int>( 3, 2 );
#endif
#if ! defined( CINDER_GL_ES )
			mDebugContext = false;
			mDebugLogSeverity = 0;
			mDebugBreakSeverity = 0;
			mMultiGpuEnabledNV = false;
			mMultiGpuModeNV = MultiGpuModeNV::SINGLE;
#endif
			mObjectTracking = false;
			mStencil = false;
			mDepthBufferBits = 24;
			mColorBpc = 8;
		}

		Options&	coreProfile( bool enable = true ) { mCoreProfile = enable; return *this; }
		bool		getCoreProfile() const { return mCoreProfile; }
		void		setCoreProfile( bool enable ) { mCoreProfile = enable; }

		Options&			version( int major, int minor ) { mVersion = std::make_pair( major, minor ); return *this; }
		Options&			version( std::pair<int,int> version ) { mVersion = version; return *this; }
		std::pair<int,int>	getVersion() const { return mVersion; }
		void				setVersion( int major, int minor ) { mVersion = std::make_pair( major, minor ); }
		void				setVersion( std::pair<int,int> version ) { mVersion = version; }

		//! Sets the number of samples used for Multisample Anti-Aliasing (MSAA). Valid values are powers of 2 (0, 2, 4, 8, 16). Defaults to \c 0.
		Options&	msaa( int samples ) { mMsaaSamples = samples; return *this; }
		//! Returns the number of samples used for Multisample Anti-Aliasing (MSAA).
		int			getMsaa() const { return mMsaaSamples; }

#if ! defined( CINDER_GL_ES )
		enum class MultiGpuModeNV { SINGLE, AFR, MULTICAST, MULTI_DISPLAY_MULTICAST };

		//! Enables a debug context (per \c ARB_debug_output). Currently only implemented by MSW and Linux GL implementations. By default this is made \c GL_DEBUG_OUTPUT_SYNCHRONOUS
		Options&	debug() { mDebugContext = true; return *this; }
		//! Returns whether the context has debug enabled
		bool		getDebug() const { return mDebugContext; }
		//! Enables logging of an error of a given severity (or greater), such as \c GL_DEBUG_SEVERITY_NOTIFICATION. Implicitly enables the debug context. Off (\c 0) by default.
		Options&	debugLog( GLenum severity = GL_DEBUG_SEVERITY_NOTIFICATION ) { mDebugLogSeverity = severity; mDebugContext = true; return *this; }
		//! Returns the severity threshold for debug logging, such as \c GL_DEBUG_SEVERITY_LOW. A value of \c 0 indicates debugLog is off.
		GLenum		getDebugLogSeverity() const { return mDebugLogSeverity; }
		//! Enables breaking on an error of a given severity (or greater), such as \c GL_DEBUG_SEVERITY_HIGH. Implicitly enables the debug context. Off (\c 0) by default.
		Options&	debugBreak( GLenum severity = GL_DEBUG_SEVERITY_HIGH ) { mDebugBreakSeverity = severity; mDebugContext = true; return *this; }
		//! Returns the severity threshold for debug breaking. A value of \c 0 indicates debugBreak is off.
		GLenum		getDebugBreakSeverity() const { return mDebugBreakSeverity; }
		//! (NVIDIA ONLY) Specify the multi-GPU strategy (SLI mode) to be WGL_CONTEXT_MULTIGPU_ATTRIB_SINGLE_NV.
		Options&	multiGpuSingleNV() { mMultiGpuEnabledNV = true; mMultiGpuModeNV = MultiGpuModeNV::SINGLE; return *this; }
		//! (NVIDIA ONLY) Specify the multi-GPU strategy (SLI mode) to be WGL_CONTEXT_MULTIGPU_ATTRIB_AFR_NV.
		Options&	multiGpuAFRNV() { mMultiGpuEnabledNV = true; mMultiGpuModeNV = MultiGpuModeNV::AFR; return *this; }
		//! (NVIDIA ONLY) Specify the multi-GPU strategy (SLI mode) to be WGL_CONTEXT_MULTIGPU_ATTRIB_MULTICAST_NV.
		Options&	multiGpuMulticastNV() { mMultiGpuEnabledNV = true; mMultiGpuModeNV = MultiGpuModeNV::MULTICAST; return *this; }
		//! (NVIDIA ONLY) Specify the multi-GPU strategy (SLI mode) to be WGL_CONTEXT_MULTIGPU_ATTRIB_MULTI_DISPLAY_MULTICAST_NV.
		Options&	multiGpuMultiDisplayMulticastNV() { mMultiGpuEnabledNV = true; mMultiGpuModeNV = MultiGpuModeNV::MULTI_DISPLAY_MULTICAST; return *this; }
		//! (NVIDIA ONLY) Returns whether multi-GPU (SLI mode) is active or not.
		bool		isMultiGpuEnabledNV() const { return mMultiGpuEnabledNV; }
		//! (NVIDIA ONLY) Returns the multi-GPU strategy (SLI mode).
		MultiGpuModeNV	getMultiGpuModeNV() const { return mMultiGpuModeNV; }
#endif

		//! Enables Context-level tracking of live objects. Defaults to \c false.
		Options&	objectTracking( bool objectTracking = true ) { mObjectTracking = objectTracking; return *this; }
		//! Returns whether Context-level tracking of live objects is enabled. Defaults to \c false.
		bool		getObjectTracking() const { return mObjectTracking; }

		//! Sets the number of bits dedicated to the depth buffer. Default is \c 24.
		Options&	depthBufferDepth( int depthBufferBits ) { mDepthBufferBits = depthBufferBits; return *this; }
		//! Returns the number of bits dedicated to the depth buffer. Default is 24.
		int			getDepthBufferDepth() const { return mDepthBufferBits; }
		//! Sets the number of bits dedicated to the depth buffer. Default is \c 24.
		void		setDepthBufferDepth( int depthBufferBits ) { mDepthBufferBits = depthBufferBits; }

		//! Enables or disables a stencil buffer. Default is \c false
		Options&	stencil( bool createStencil = true ) { mStencil = createStencil; return *this; }
		//! Returns whether a stenci buffer is enabled. Default is \c false
		bool		getStencil() const { return mStencil; }
		//! Enables or disables a stencil buffer. Default is \c false
		void		setStencil( bool createStencil = true ) { mStencil = createStencil; }

		//! Sets the number of bits per color channel. Default is \c 8 but can be \c 10 on professional GPUs (Quadro/FireGL)
		Options&	colorChannelDepth( int colorBitsPerChannel ) { mColorBpc = colorBitsPerChannel; return *this; }
		//! Returns the number of bits per color channel. Default is \c 8 but can be \c 10 on professional GPUs (Quadro/FireGL)
		int			getColorChannelDepth() const { return mColorBpc; }

	  protected:
		bool					mCoreProfile;
		std::pair<int,int>		mVersion;
		int						mMsaaSamples;
		bool					mStencil;
		int						mColorBpc, mDepthBufferBits;
#if ! defined( CINDER_GL_ES )
		bool					mDebugContext;
		GLenum					mDebugLogSeverity; // initial value of 0 means debug logging is disabled
		GLenum					mDebugBreakSeverity; // initial value of 0 means debug break is disabled
		bool					mMultiGpuEnabledNV;
		MultiGpuModeNV			mMultiGpuModeNV;
#endif
		bool					mObjectTracking;
	};


	RendererGl( const Options &options = Options() );
	~RendererGl();

	static RendererGlRef	create( const Options &options = Options() )	{ return RendererGlRef( new RendererGl( options ) ); }
	RendererRef				clone() const override							{ return RendererGlRef( new RendererGl( *this ) ); }

#if defined( CINDER_COCOA )
	#if defined( CINDER_MAC )
		void						setup( CGRect frame, NSView *cinderView, RendererRef sharedRenderer, bool retinaEnabled ) override;
		CGLContextObj				getCglContext() override;
		CGLPixelFormatObj			getCglPixelFormat() override;
		virtual NSOpenGLContext*	getNsOpenGlContext();
	#elif defined( CINDER_COCOA_TOUCH )
		void						setup( const Area &frame, UIView *cinderView, RendererRef sharedRenderer ) override;
		bool						isEaglLayer() const override { return true; }
		EAGLContext*				getEaglContext() const;
	#endif
	void	setFrameSize( int width, int height ) override;
#elif defined( CINDER_MSW_DESKTOP )
	virtual void	setup( WindowImplMsw *windowImpl, RendererRef sharedRenderer );
	virtual void	kill();
	virtual HWND	getHwnd() const override;
	virtual HDC		getDc() const override;
	virtual void	prepareToggleFullScreen();
	virtual void	finishToggleFullScreen();
#elif defined( CINDER_UWP )
	void			setup( ::Platform::Agile<Windows::UI::Core::CoreWindow> wnd, RendererRef sharedRenderer ) override;
	void			prepareToggleFullScreen();
	void			finishToggleFullScreen();
#elif defined( CINDER_ANDROID )
	virtual void	setup( ANativeWindow *nativeWindow, RendererRef sharedRenderer ) override;
#elif defined( CINDER_LINUX )
#if defined( CINDER_HEADLESS )
	virtual void	setup( ci::ivec2 renderSize, RendererRef sharedRenderer ) override;
#else
	virtual void	setup( void* nativeWindow, RendererRef sharedRenderer ) override;
#endif
#endif

	const Options&	getOptions() const { return mOptions; }

	void			startDraw() override;
	void			finishDraw() override;
	void			defaultResize() override;
	void			makeCurrentContext( bool force = false ) override;
	void			swapBuffers()override;
	Surface8u		copyWindowSurface( const Area &area, int32_t windowHeightPixels ) override;

	//! Overrides Renderer's start draw implementation for custom hooks. Only useful in advanced use cases.
	void setStartDrawFn( const std::function<void( Renderer* )>& function ) { mStartDrawFn = function; }
	//! Overrides Renderer's finish draw implementation for custom hooks. Only useful in advanced use cases.
	void setFinishDrawFn( const std::function<void( Renderer* )>& function ) { mFinishDrawFn = function; }

protected:
	RendererGl( const RendererGl &renderer );

	Options		mOptions;
#if defined( CINDER_MAC )
	RendererImplGlMac		*mImpl;
#elif defined( CINDER_COCOA_TOUCH )
	RendererImplGlCocoaTouch	*mImpl;
#elif defined( CINDER_MSW_DESKTOP )
	#if defined( CINDER_GL_ANGLE )
		class RendererImplGlAngle	*mImpl;
		friend class				RendererImplGlAngle;
	#else
		class RendererImplGlMsw		*mImpl;
		friend class				RendererImplGlMsw;
	#endif
	WindowImplMsw					*mWindowImpl;
#elif defined( CINDER_UWP )
	class RendererImplGlAngle	*mImpl;
	friend class				RendererImplGlAngle;
	::Platform::Agile<Windows::UI::Core::CoreWindow>	mWnd;
#elif defined( CINDER_ANDROID )
	class RendererGlAndroid		*mImpl;
	RendererGlAndroid		  *getImpl() { return mImpl; }
	friend class WindowImplAndroid;
#elif defined( CINDER_LINUX )
	class RendererGlLinux	*mImpl;
	RendererGlLinux			*getImpl() { return mImpl; }
	friend class WindowImplLinux;
#endif

	std::function<void( Renderer* )> mStartDrawFn;
	std::function<void( Renderer* )> mFinishDrawFn;
};

} } // namespace cinder::app
