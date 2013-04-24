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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/gl/gl.h"  // necessary to give GLee the jump on Cocoa.h
#include "cinder/Surface.h"
#include "cinder/Display.h"

#if defined( CINDER_MAC )
	#include <ApplicationServices/ApplicationServices.h>
	#include <CoreFoundation/CoreFoundation.h>
	#if defined __OBJC__
		@class AppImplCocoaRendererQuartz;
		@class AppImplCocoaRendererGl;
		@class NSOpenGLContext;
		@class NSView;
	#else
		class AppImplCocoaRendererQuartz;
		class AppImplCocoaRendererGl;
		class NSOpenGLContext;
		class NSView;
	#endif
	typedef struct _CGLContextObject       *CGLContextObj;
	typedef struct _CGLPixelFormatObject   *CGLPixelFormatObj;
#elif defined( CINDER_COCOA_TOUCH )
	#if defined __OBJC__
		typedef struct CGContext * CGContextRef;
		@class AppImplCocoaTouchRendererGl;
		@class AppImplCocoaTouchRendererQuartz;
		@class UIView;
		@class EAGLContext;
	#else
		typedef struct CGContext * CGContextRef;
		class AppImplCocoaTouchRendererGl;
		class AppImplCocoaTouchRendererQuartz;
		class UIView;
		class EAGLContext;
	#endif
#endif


namespace cinder { namespace app {

class App;

typedef std::shared_ptr<class Renderer>		RendererRef;
class Renderer {
 public:
	virtual ~Renderer() {};
	
	virtual RendererRef	clone() const = 0;
	
#if defined( CINDER_COCOA )
	#if defined( CINDER_MAC )
		virtual void	setup( App *aApp, CGRect frame, NSView *cinderView, RendererRef sharedRenderer, bool retinaEnabled ) = 0;
		virtual CGContextRef			getCgContext() { throw; } // the default behavior is failure
		virtual CGLContextObj			getCglContext() { throw; } // the default behavior is failure
		virtual CGLPixelFormatObj		getCglPixelFormat() { throw; } // the default behavior is failure
	#elif defined( CINDER_COCOA_TOUCH )
		virtual void	setup( App *aApp, const Area &frame, UIView *cinderView, RendererRef sharedRenderer ) = 0;
		virtual bool	isEaglLayer() const { return false; }
	#endif


	virtual void	setFrameSize( int width, int height ) {}		

#elif defined( CINDER_MSW )
	virtual void setup( App *aApp, HWND wnd, HDC dc, RendererRef sharedRenderer ) = 0;

	virtual void prepareToggleFullScreen() {}
	virtual void finishToggleFullScreen() {}
	virtual void kill() {}

	virtual HWND				getHwnd() = 0;
	virtual HDC					getDc() { return NULL; }
#endif

	virtual Surface	copyWindowSurface( const Area &area ) = 0;

	virtual void startDraw() {}
	virtual void finishDraw() {}		
	virtual void makeCurrentContext() {}
	virtual void defaultResize() {}

 protected:
 	Renderer() : mApp( 0 ) {}
	Renderer( const Renderer &renderer );

	App			*mApp;
};

typedef std::shared_ptr<class RendererGl>	RendererGlRef;
class RendererGl : public Renderer {
  public:
#if defined( CINDER_COCOA_TOUCH )
	RendererGl( int aAntiAliasing = AA_MSAA_4 );
#else
	RendererGl( int aAntiAliasing = AA_MSAA_16 );
#endif
	~RendererGl();

	static RendererGlRef	create( int antiAliasing = AA_MSAA_16 ) { return RendererGlRef( new RendererGl( antiAliasing ) ); }
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

	enum	{ AA_NONE = 0, AA_MSAA_2, AA_MSAA_4, AA_MSAA_6, AA_MSAA_8, AA_MSAA_16, AA_MSAA_32 };
	static const int	sAntiAliasingSamples[];
	void				setAntiAliasing( int aAntiAliasing );
	int					getAntiAliasing() const { return mAntiAliasing; }

	virtual void	startDraw();
	virtual void	finishDraw();
	virtual void	defaultResize();
	virtual void	makeCurrentContext();
	virtual Surface	copyWindowSurface( const Area &area );
	
 protected:
	RendererGl( const RendererGl &renderer );

	int			mAntiAliasing;
#if defined( CINDER_MAC )
	AppImplCocoaRendererGl		*mImpl;
#elif defined( CINDER_COCOA_TOUCH )
	AppImplCocoaTouchRendererGl	*mImpl;
#elif defined( CINDER_MSW )
	class AppImplMswRendererGl	*mImpl;
	HWND						mWnd;
	friend class				AppImplMswRendererGl;
#endif
};

typedef std::shared_ptr<class Renderer2d>	Renderer2dRef;
#if defined( CINDER_COCOA )
class Renderer2d : public Renderer {
  public:
  	Renderer2d();
	
	static Renderer2dRef	create() { return Renderer2dRef( new Renderer2d() ); }
	virtual RendererRef		clone() const { return Renderer2dRef( new Renderer2d( *this ) ); }

	#if defined( CINDER_COCOA_TOUCH )
		virtual void setup( App *aApp, const Area &frame, UIView *cinderView, RendererRef sharedRenderer );
	#else
		~Renderer2d();
		virtual void setup( App *aApp, CGRect frame, NSView *cinderView, RendererRef sharedRenderer, bool retinaEnabled );
	#endif

	virtual CGContextRef			getCgContext();

	virtual void startDraw();
	virtual void finishDraw();
	virtual void defaultResize();
	virtual void makeCurrentContext();	
	virtual void setFrameSize( int width, int height );
	virtual Surface	copyWindowSurface( const Area &area );
	
  protected:
	Renderer2d( const Renderer2d &renderer );

#if defined( CINDER_MAC )
	AppImplCocoaRendererQuartz		*mImpl;
#else
	AppImplCocoaTouchRendererQuartz	*mImpl;
#endif
	CGContextRef					mCGContext;
};

#elif defined( CINDER_MSW )

class Renderer2d : public Renderer {
 public:
	Renderer2d( bool doubleBuffer = true ); 
 
	static Renderer2dRef	create( bool doubleBuffer = true ) { return Renderer2dRef( new Renderer2d( doubleBuffer ) ); }
	virtual RendererRef		clone() const { return Renderer2dRef( new Renderer2d( *this ) ); }
	
	virtual void setup( App *aApp, HWND wnd, HDC dc, RendererRef sharedRenderer );
	virtual void kill();
	
	virtual HWND	getHwnd() { return mWnd; }
	virtual HDC		getDc();

	virtual void	prepareToggleFullScreen();
	virtual void	finishToggleFullScreen();

	virtual void startDraw() override;
	virtual void finishDraw() override;
	virtual void defaultResize() override;
	virtual Surface	copyWindowSurface( const Area &area );
	
 protected:
	Renderer2d( const Renderer2d &renderer );
 
	class AppImplMswRendererGdi	*mImpl;

	bool			mDoubleBuffer;
	HWND			mWnd;
	HDC				mDC;
};

#endif

} } // namespace cinder::app