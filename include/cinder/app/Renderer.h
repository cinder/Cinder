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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/gl/gl.h"  // necessary to give GLee the jump on Cocoa.h
#include "cinder/Surface.h"
#include "cinder/Display.h"

#if defined( CINDER_MAC )
	#if defined __OBJC__
		@class AppImplCocoaRendererQuartz;
		@class AppImplCocoaRendererGl;
		@class NSView;
	#else
		class AppImplCocoaRendererQuartz;
		class AppImplCocoaRendererGl;
		class NSView;
	#endif
#elif defined( CINDER_COCOA_TOUCH )
	#if defined __OBJC__
		typedef struct CGContext * CGContextRef;
		@class AppImplCocoaTouchRendererGl;
		@class AppImplCocoaTouchRendererQuartz;
		@class UIView;
	#else
		typedef struct CGContext * CGContextRef;
		class AppImplCocoaTouchRendererGl;
		class AppImplCocoaTouchRendererQuartz;
		class UIView;
	#endif
#endif


namespace cinder { namespace app {

class Renderer {
 public:
	virtual ~Renderer() {};
#if defined( CINDER_COCOA )
	#if defined( CINDER_MAC )
		virtual void	setup( class App *aApp, CGRect frame, NSView *cinderView ) = 0;	
		virtual CGContextRef			getCgContext() { throw; } // the default behavior is failure
		virtual CGLContextObj			getCglContext() { throw; } // the default behavior is failure
		virtual CGLPixelFormatObj		getCglPixelFormat() { throw; } // the default behavior is failure
	#elif defined( CINDER_COCOA_TOUCH )
		virtual void	setup( class App *aApp, const Area &frame, UIView *cinderView ) = 0;
		virtual bool	isEaglLayer() const { return false; }
	#endif

	virtual void	setFrameSize( int width, int height ) {}		

	virtual void					makeCurrentContext() = 0;
#elif defined( CINDER_MSW )
	virtual void setup( class App *aApp, HWND wnd, HDC dc ) = 0;

	virtual void prepareToggleFullScreen() {}
	virtual void finishToggleFullScreen() {}
	virtual void kill() {}

	virtual HWND				getHwnd() = 0;
	virtual HDC					getDc() { throw; } // the default behavior is failure
#endif

	virtual Surface	copyWindowSurface( const Area &area ) = 0;

	virtual void startDraw() {}
	virtual void finishDraw() {}		
	virtual void defaultResize() {}

 protected:
	class App			*mApp;
};

class RendererGl : public Renderer {
 public:
	RendererGl();
	RendererGl( int aAntiAliasing );
	~RendererGl();
 
#if defined( CINDER_COCOA )
	#if defined( CINDER_MAC )
		virtual void setup( App *aApp, CGRect frame, NSView *cinderView );
		virtual CGLContextObj			getCglContext();
		virtual CGLPixelFormatObj		getCglPixelFormat();
	#elif defined( CINDER_COCOA_TOUCH )
		virtual void setup( App *aApp, const Area &frame, UIView *cinderView );
		virtual bool	isEaglLayer() const { return true; }
	#endif
	virtual void	setFrameSize( int width, int height );			
	virtual void	makeCurrentContext();
#elif defined( CINDER_MSW )
	virtual void	setup( App *aApp, HWND wnd, HDC dc );
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
	virtual Surface	copyWindowSurface( const Area &area );
	
 protected:
	int			mAntiAliasing;
#if defined( CINDER_MAC )
	AppImplCocoaRendererGl		*mImpl;
#elif defined( CINDER_COCOA_TOUCH )
	AppImplCocoaTouchRendererGl		*mImpl;
#elif defined( CINDER_MSW )
	class AppImplMswRendererGl	*mImpl;
	HWND						mWnd;
#endif
};

#if defined( CINDER_COCOA )
class Renderer2d : public Renderer {
 public:
	#if defined( CINDER_COCOA_TOUCH )
		virtual void setup( App *aApp, const Area &frame, UIView *cinderView );
	#else
		~Renderer2d();
		virtual void setup( App *aApp, CGRect frame, NSView *cinderView );
	#endif

	virtual CGContextRef			getCgContext();

	virtual void startDraw();
	virtual void finishDraw();
	virtual void setFrameSize( int width, int height );
	virtual void defaultResize();
	virtual void makeCurrentContext();
	virtual Surface	copyWindowSurface( const Area &area );
 protected:
#if defined( CINDER_MAC )
	AppImplCocoaRendererQuartz		*mImpl;
#else
	AppImplCocoaTouchRendererQuartz		*mImpl;
#endif
	CGContextRef					mCGContext;
};

#elif defined( CINDER_MSW )

class Renderer2d : public Renderer {
 public:
	Renderer2d( bool doubleBuffer = true );
	
	virtual void setup( App *aApp, HWND wnd, HDC dc );
	virtual void kill();
	
	virtual HWND	getHwnd() { return mWnd; }
	virtual HDC		getDc();

	virtual void	prepareToggleFullScreen();
	virtual void	finishToggleFullScreen();

	virtual void startDraw();
	virtual void finishDraw();
	virtual void defaultResize();
	virtual Surface	copyWindowSurface( const Area &area );
	
 protected:
	class AppImplMswRendererGdi	*mImpl;

	bool			mDoubleBuffer;
	HWND			mWnd;
	HDC				mDC;
};

#endif

} } // namespace cinder::app