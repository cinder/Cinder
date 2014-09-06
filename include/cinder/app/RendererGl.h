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
#include "cinder/gl/gl.h"

#if defined( CINDER_MAC )
	#if defined __OBJC__
		@class AppImplCocoaRendererGl;
		@class NSOpenGLContext;
	#else
		class AppImplCocoaRendererGl;
		class NSOpenGLContext;
	#endif
	typedef struct _CGLContextObject       *CGLContextObj;
	typedef struct _CGLPixelFormatObject   *CGLPixelFormatObj;
#elif defined( CINDER_COCOA_TOUCH )
	#if defined __OBJC__
		typedef struct CGContext * CGContextRef;
		@class AppImplCocoaTouchRendererGl;
		@class EAGLContext;
	#else
		typedef struct CGContext * CGContextRef;
		class AppImplCocoaTouchRendererGl;
		class EAGLContext;
	#endif
#endif

namespace cinder { namespace gl {

class Context;
typedef std::shared_ptr<Context>		ContextRef;

} } // cinder::gl

namespace cinder { namespace app {

typedef std::shared_ptr<class RendererGl>	RendererGlRef;
class RendererGl : public Renderer {
  public:
	struct Options {
	  public:
		Options() {
#if defined( CINDER_COCOA_TOUCH )
			mAntiAliasing = AA_NONE;
			mCoreProfile = false;
			mVersion = std::pair<int,int>( 2, 0 );
#else
			mAntiAliasing = AA_MSAA_16;
			mCoreProfile = true;
			mVersion = std::pair<int,int>( 3, 2 );	
#endif
#if ! defined( CINDER_GL_ES )
			mDebugContext = false;
			mDebugLogSeverity = 0;
			mDebugBreakSeverity = 0;
#endif
			mObjectTracking = false;
			mStencil = false;
			mDepthBufferBits = 24;
		}

		Options&	coreProfile( bool enable = true ) { mCoreProfile = enable; return *this; }
		bool		getCoreProfile() const { return mCoreProfile; }
		void		setCoreProfile( bool enable ) { mCoreProfile = enable; }
		
		Options&			version( int major, int minor ) { mVersion = std::make_pair( major, minor ); return *this; }
		Options&			version( std::pair<int,int> version ) { mVersion = version; return *this; }
		std::pair<int,int>	getVersion() const { return mVersion; }
		void				setVersion( int major, int minor ) { mVersion = std::make_pair( major, minor ); }
		void				setVersion( std::pair<int,int> version ) { mVersion = version; }
		
		Options&	antiAliasing( int amount ) { mAntiAliasing = amount; return *this; }
		int			getAntiAliasing() const { return mAntiAliasing; }
		void		setAntiAliasing( int amount ) { mAntiAliasing = amount; }

#if ! defined( CINDER_GL_ES )
		//! Enables a debug context (per \c ARB_debug_output). Currently only implemented by MSW GL implementations. By default this is made GL_DEBUG_OUTPUT_SYNCHRONOUS
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
		
	  protected:
		bool					mCoreProfile;
		std::pair<int,int>		mVersion;
		int						mAntiAliasing;
		bool					mStencil;
		int						mDepthBufferBits;
#if ! defined( CINDER_GL_ES )
		bool					mDebugContext;
		GLenum					mDebugLogSeverity; // initial value of 0 means debug logging is disabled
		GLenum					mDebugBreakSeverity; // initial value of 0 means debug break is disabled
#endif
		bool					mObjectTracking;
	};


	RendererGl( const Options &options = Options() );
	~RendererGl();

	static RendererGlRef	create( const Options &options = Options() ) { return RendererGlRef( new RendererGl( options ) ); }
	virtual RendererRef		clone() const { return RendererGlRef( new RendererGl( *this ) ); }
 
#if defined( CINDER_COCOA )
	#if defined( CINDER_MAC )
		virtual void setup( App *aApp, CGRect frame, NSView *cinderView, RendererRef sharedRenderer, bool retinaEnabled );
		virtual CGLContextObj			getCglContext();
		virtual CGLPixelFormatObj		getCglPixelFormat();
		virtual NSOpenGLContext*		getNsOpenGlContext();		
	#elif defined( CINDER_COCOA_TOUCH )
		virtual void 	setup( App *aApp, const Area &frame, UIView *cinderView, RendererRef sharedRenderer );
		virtual bool 	isEaglLayer() const { return true; }
		EAGLContext*	getEaglContext() const;
	#endif
	virtual void	setFrameSize( int width, int height );
#elif defined( CINDER_MSW )
	virtual void	setup( App *aApp, HWND wnd, HDC dc, RendererRef sharedRenderer );
	virtual void	kill();
	virtual HWND	getHwnd() { return mWnd; }
	virtual void	prepareToggleFullScreen();
	virtual void	finishToggleFullScreen();
#endif

	static const int sAntiAliasingSamples[];
	enum	{ AA_NONE = 0, AA_MSAA_2, AA_MSAA_4, AA_MSAA_6, AA_MSAA_8, AA_MSAA_16, AA_MSAA_32 };
	const Options&	getOptions() const { return mOptions; }

	virtual void	startDraw();
	virtual void	finishDraw();
	virtual void	defaultResize();
	virtual void	makeCurrentContext();
	virtual Surface	copyWindowSurface( const Area &area );
	
 protected:
	RendererGl( const RendererGl &renderer );

	Options		mOptions;
#if defined( CINDER_MAC )
	AppImplCocoaRendererGl		*mImpl;
#elif defined( CINDER_COCOA_TOUCH )
	AppImplCocoaTouchRendererGl	*mImpl;
#elif defined( CINDER_MSW )
	#if defined( CINDER_GL_ANGLE )
		class AppImplMswRendererAngle	*mImpl;
		friend class					AppImplMswRendererAngle;
	#else
		class AppImplMswRendererGl		*mImpl;
		friend class					AppImplMswRendererGl;
	#endif
	HWND						mWnd;
#endif
};

} } // namespace cinder::app
