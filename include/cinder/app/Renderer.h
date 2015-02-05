/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.
 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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
#include "cinder/Exception.h"
#include "cinder/MatrixStack.h"

#if defined( CINDER_MSW )
	struct HWND__;
	typedef HWND__* DX_WINDOW_TYPE;
#elif defined( CINDER_WINRT )
	#include <agile.h>
	typedef Platform::Agile<Windows::UI::Core::CoreWindow>	DX_WINDOW_TYPE;
	#undef min
	#undef max
#endif

#include "cinder/Surface.h"
#include "cinder/Display.h"

#if defined( CINDER_MAC )
	#include <CoreGraphics/CGGeometry.h>
	#if defined __OBJC__
		@class AppImplCocoaRendererQuartz;
		@class NSView;
	#else
		class AppImplCocoaRendererQuartz;
		class NSView;
	#endif
	typedef struct _CGLContextObject       *CGLContextObj;
	typedef struct _CGLPixelFormatObject   *CGLPixelFormatObj;
	typedef struct CGContext				*CGContextRef;
#elif defined( CINDER_COCOA_TOUCH )
	#if defined __OBJC__
		typedef struct CGContext * CGContextRef;
		@class AppImplCocoaTouchRendererQuartz;
		@class UIView;
	#else
		typedef struct CGContext * CGContextRef;
		class AppImplCocoaTouchRendererQuartz;
		class UIView;
	#endif
#endif


namespace cinder { namespace app {

class Window;
typedef std::shared_ptr<Window>		WindowRef;

class App;

typedef std::shared_ptr<class Renderer>		RendererRef;
class Renderer {
 public:
	enum RendererType {
		RENDERER_GL,
		RENDERER_DX
	};

	virtual ~Renderer() {}
	
	virtual RendererRef	clone() const = 0;
	virtual RendererType getRendererType() const { return RENDERER_GL; }
	
#if defined( CINDER_COCOA )
	#if defined( CINDER_MAC )
		virtual void	setup( App *aApp, CGRect frame, NSView *cinderView, RendererRef sharedRenderer, bool retinaEnabled ) = 0;
		virtual CGContextRef			getCgContext() { throw; } // the default behavior is failure
		virtual CGLContextObj			getCglContext() { throw; } // the default behavior is failure
		virtual CGLPixelFormatObj		getCglPixelFormat() { throw; } // the default behavior is failure
	#elif defined( CINDER_COCOA_TOUCH )
		virtual void		setup( App *aApp, const Area &frame, UIView *cinderView, RendererRef sharedRenderer ) = 0;
		virtual bool		isEaglLayer() const { return false; }
	#endif

	virtual void	setFrameSize( int width, int height ) {}		

#elif defined( CINDER_MSW )
	virtual void setup( App *aApp, HWND wnd, HDC dc, RendererRef sharedRenderer ) = 0;

	virtual void prepareToggleFullScreen() {}
	virtual void finishToggleFullScreen() {}
	virtual void kill() {}

	virtual HWND				getHwnd() = 0;
	virtual HDC					getDc() { return NULL; }
#elif defined( CINDER_WINRT)
	virtual void setup( App *aApp, DX_WINDOW_TYPE wnd) = 0;

	virtual void prepareToggleFullScreen() {}
	virtual void finishToggleFullScreen() {}
	virtual void kill() {}
#endif

	virtual Surface8u		copyWindowSurface( const Area &area ) = 0;

	virtual void startDraw() {}
	virtual void finishDraw() {}
	virtual void makeCurrentContext() {}
	virtual void swapBuffers() {}
	virtual void defaultResize() {}

 protected:
 	Renderer() : mApp( 0 ) {}
	Renderer( const Renderer &renderer );

	App			*mApp;
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

	void			startDraw() override;
	void			finishDraw() override;
	void			defaultResize() override;
	void			makeCurrentContext() override;
	void			setFrameSize( int width, int height ) override;
	Surface8u		copyWindowSurface( const Area &area ) override;
	
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
	
	void setup( App *aApp, HWND wnd, HDC dc, RendererRef sharedRenderer );
	void kill();
	
	HWND	getHwnd() { return mWnd; }
	HDC		getDc();

	void			prepareToggleFullScreen();
	void			finishToggleFullScreen();

	void			startDraw() override;
	void			finishDraw() override;
	void			defaultResize() override;
	Surface8u		copyWindowSurface( const Area &area ) override;
	
 protected:
	Renderer2d( const Renderer2d &renderer );
 
	class AppImplMswRendererGdi	*mImpl;

	bool			mDoubleBuffer;
	HWND			mWnd;
	HDC				mDC;
};

#endif

class ExcRenderer : public Exception {
  public:
	ExcRenderer() {}
	ExcRenderer( const std::string &description )
		: Exception( description )
	{}
};

class ExcRendererAllocation : public ExcRenderer {
  public:
	ExcRendererAllocation() {}
	ExcRendererAllocation( const std::string &description )
		: ExcRenderer( description )
	{}
};

} } // namespace cinder::app