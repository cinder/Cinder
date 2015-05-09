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
#include "cinder/Display.h"
#include "cinder/app/Renderer.h"
#include "cinder/Vector.h"
#include "cinder/Function.h"
#include "cinder/Rect.h"
#include "cinder/app/MouseEvent.h"
#include "cinder/app/TouchEvent.h"
#include "cinder/app/KeyEvent.h"
#include "cinder/app/FileDropEvent.h"
#include "cinder/Exception.h"


namespace cinder { namespace app {

class Window;
struct FullScreenOptions;
typedef std::shared_ptr<Window>		WindowRef;

} } // namespace cinder::app

#if defined( CINDER_COCOA ) && defined( __OBJC__ )
	#import <Foundation/Foundation.h>
	#if defined( CINDER_COCOA_TOUCH )
		@class UIViewController;
	#endif

	@protocol WindowImplCocoa
		@required
		- (BOOL)isFullScreen;
		- (void)setFullScreen:(BOOL)fullScreen options:(const cinder::app::FullScreenOptions *)options;
		- (cinder::Vec2i)getSize;
		- (void)setSize:(cinder::Vec2i)size;
		- (cinder::Vec2i)getPos;
		- (void)setPos:(cinder::Vec2i)pos;
		- (float)getContentScale;
		- (void)close;
		- (NSString *)getTitle;
		- (void)setTitle:(NSString *)title;
		- (BOOL)isBorderless;
		- (void)setBorderless:(BOOL)borderless;
		- (BOOL)isAlwaysOnTop;
		- (void)setAlwaysOnTop:(BOOL)alwaysOnTop;
		- (void)hide;
		- (void)show;
		- (BOOL)isHidden;
		- (cinder::DisplayRef)getDisplay;
		- (cinder::app::RendererRef)getRenderer;
		- (const std::vector<cinder::app::TouchEvent::Touch>&)getActiveTouches;
		- (void*)getNative;
	#if defined( CINDER_COCOA_TOUCH )
		- (UIViewController *)getNativeViewController;
	#endif
	@end
#elif defined( CINDER_COCOA )
	class WindowImplCocoa;
	#if defined( CINDER_COCOA_TOUCH )
		class UIViewController;
	#endif
#elif defined( CINDER_WINRT )
	namespace cinder { namespace app {
		class WindowImplWinRT;
	} } // namespace cinder::app
#elif defined( CINDER_MSW )
	namespace cinder { namespace app {
		class WindowImplMsw;
	} } // namespace cinder::app
#endif

namespace cinder { namespace app {

typedef	 signals::signal<void(MouseEvent&),EventCombiner<MouseEvent> >		EventSignalMouse;
typedef	 signals::signal<void(TouchEvent&),EventCombiner<TouchEvent> >		EventSignalTouch;
typedef	 signals::signal<void(KeyEvent&),EventCombiner<KeyEvent> >			EventSignalKey;
typedef	 signals::signal<void(FileDropEvent&),EventCombiner<FileDropEvent> > EventSignalFileDrop;
typedef	 signals::signal<void()>											EventSignalWindow;

//! Thrown when an operation is performed on a WindowRef which refers to an invalid Window
class ExcInvalidWindow : public cinder::Exception {
	virtual const char * what() const throw() { return "Invalid Window"; }
};

//! Options passed when entering fullscreen.
struct FullScreenOptions {
	FullScreenOptions() : mKioskMode( true ), mSecondaryDisplayBlanking( false ), mExclusive( false )
	{}

	//! (OS X only) Sets the fullscreen mode to 'kiosk', which means don't use the 10.7 way of animating to fullscreen. Default is \c true.
	FullScreenOptions&	kioskMode( bool enable = true )					{ mKioskMode = enable; return *this; }
	//! Sets whether secondary displays should be blanked (made black). Default is \c false.
	FullScreenOptions&	secondaryDisplayBlanking( bool enable = true )	{ mSecondaryDisplayBlanking = enable; return *this; }
	//! (OS X only) Sets whether the \t Window related to these options is the only accessible window. Default is \c false.
	FullScreenOptions&	exclusive( bool enable = true )					{ mExclusive = enable; return *this; }
	//! Sets the display which will be used in fullscreen mode. Defaults to the \t Window's current \t Display. Ignored on Mac OS X outside of kiosk mode.
	FullScreenOptions&	display( DisplayRef display )					{ mDisplay = display; return *this; }

	//! Returns the display on which the fullscreen should occur. A NULL value implies the default, which is the \t Window's current \t Display.
	DisplayRef			getDisplay()									const { return mDisplay; }
	//! Returns whether kiosk mode is enabled.
	bool				isKioskModeEnabled()							const { return mKioskMode; }
	//! Returns whether blanking of secondary displays in enabled. Default is \c false.
	bool				isSecondaryDisplayBlankingEnabled()				const { return mSecondaryDisplayBlanking; }
	//! Returns whether the \t Window related to these options is the only accessible window.
	bool				isExclusive()									const { return mExclusive; }

  private:
	DisplayRef	mDisplay;
	bool		mKioskMode, mSecondaryDisplayBlanking, mExclusive;
};

class Window : public std::enable_shared_from_this<Window> {
  public:
	// Parameters for a Window, which are used to create the physical window by the App
	struct Format {
		Format( RendererRef renderer = RendererRef(), DisplayRef display = Display::getMainDisplay(), bool fullScreen = false, Vec2i size = Vec2i( 640, 480 ), Vec2i pos = Vec2i::zero() )
			: mRenderer( renderer ), mFullScreen( fullScreen ), mDisplay( display ), mSize( size ), mPos( pos ), mPosSpecified( false ),
			mResizable( true ), mBorderless( false ), mAlwaysOnTop( false ), mFullScreenButtonEnabled( false ),
			mTitleSpecified( false ), mTitle( "" )
#if defined( CINDER_COCOA_TOUCH )
			, mRootViewController( NULL )
#endif
		{
			mFullScreenOptions.kioskMode( true );
		}

		//! Returns the Display the Window will be created on. Defaults to the primary display.
		DisplayRef	getDisplay() const { return mDisplay; }
		//! Sets the Display the Window will be created on. Defaults to the primary display.
		void		setDisplay( DisplayRef display ) { mDisplay = display; }
		//! Sets the Display the Window will be created on. Defaults to the primary display.
		Format&		display( DisplayRef displayRef ) { mDisplay = displayRef; return *this; }
		//! Returns whether the Window will be created full-screen. Default is \c false.
		bool		isFullScreen() const { return mFullScreen; }
		//! Returns the \a options associated with fullscreen at startup.
		const FullScreenOptions& getFullScreenOptions() const { return mFullScreenOptions; }
		//! Sets whether the Window will be created full-screen with FullScreenOptions \a options. Default is \c false. If \t true, FullScreenOptions mode defaults to kiosk.
		void		setFullScreen( bool fullScreen = true, const FullScreenOptions &options = FullScreenOptions() ) { mFullScreen = fullScreen; mFullScreenOptions = options; }
		//! Sets whether the Window will be created full-screen. Default is \c false.
		Format&		fullScreen( bool fs = true ) { mFullScreen = fs; return *this; }
		//! Returns the size in points at which the Window will be created. Default is 640 x 480.
		Vec2i		getSize() const { return mSize; }
		//! Sets the size in points at which the Window will be created. Default is 640 x 480.
		void		setSize( const Vec2i &size ) { mSize = size; }
		//! Sets the size in points at which the Window will be created. Default is 640 x 480.
		void		setSize( int32_t width, int32_t height ) { mSize = Vec2i( width, height ); }
		//! Sets the size in points at which the Window will be created. Default is 640 x 480.
		Format&		size( const Vec2i &s ) { mSize = s; return *this; }
		//! Sets the size in points at which the Window will be created. Default is 640 x 480.
		Format&		size( int32_t width, int32_t height ) { mSize = Vec2i( width, height ); return *this; }

		//! Returns the position in points measured relative to the system's primary display's upper-left corner at which the Window will be created. Default is centered on the display.
		Vec2i		getPos() const { return mPos; }
		//! Sets the position in points measured relative to the system's primary display's upper-left corner at which the Window will be created. Default is centered on the display.
		void		setPos( const Vec2i &pos ) { mPos = pos; mPosSpecified = true; }
		//! Sets the position in points measured relative to the system's primary display's upper-left corner at which the Window will be created. Default is centered on the display.
		void		setPos( int32_t x, int32_t y ) { mPos = Vec2i( x, y ); mPosSpecified = true; }
		//! Sets the position in points measured relative to the system's primary display's upper-left corner at which the Window will be created. Default is centered on the display.
		Format&		pos( const Vec2i &pos ) { mPos = pos; mPosSpecified = true; return *this; }
		//! Sets the position in points measured relative to the system's primary display's upper-left corner at which the Window will be created. Default is centered on the display.
		Format&		pos( int32_t x, int32_t y ) { mPos = Vec2i( x, y ); mPosSpecified = true; return *this; }
		//! Returns whether a non-default position has been requested for the Window.
		bool		isPosSpecified() const { return mPosSpecified; }
		//! Unspecifies a non-default position for the window, effectively requestion the default position.
		void		unspecifyPos() { mPosSpecified = false; }

		//! Returns the Renderer which will be instantiated for the Window. Defaults to an instance of the App's default renderer (specified in the app-instantiation macro).
		RendererRef	getRenderer() const { return mRenderer; }
		//! Sets the Renderer which will be instantiated for the Window.
		void		setRenderer( RendererRef renderer ) { mRenderer = renderer; }
		//! Sets the Renderer which will be instantiated for the Window.
		Format&		renderer( RendererRef r ) { mRenderer = r; return *this; }

#if defined( CINDER_COCOA_TOUCH )
		//! Returns the root UIViewController for the associated UIWindow on iOS
		UIViewController*	getRootViewController() const { return mRootViewController;}
		//! Sets the root UIViewController for the associated UIWindow on iOS. Use this to enable a view heirarchy that contains native components at the root and later add Cinder's UIView / UIViewController
		void				setRootViewController( UIViewController *v ) { mRootViewController = v; }
		//! Sets the root UIViewController for the associated UIWindow on iOS. Use this to enable a view heirarchy that contains native components at the root and later add Cinder's UIView / UIViewController
		Format&				rootViewController( UIViewController *v ) { mRootViewController = v; return *this; }
#endif

		//! Returns whether the Window created will be resizable. Defaults to \c true.
		bool		isResizable() const { return mResizable; }
		//! Sets whether the Window created will be resizable. Defaults to \c true.
		void		setResizable( bool resizable = true ) { mResizable = resizable; }
		//! Sets whether the Window created will be resizable. Defaults to \c true.
		Format&		resizable( bool res = true ) { mResizable = res; return *this; }
		//! Returns whether the Window created will have no border. Defaults to \c false.
		bool		isBorderless() const { return mBorderless; }
		//! Sets whether the Window created will have no border. Defaults to \c false.
		void		setBorderless( bool borderless = true ) { mBorderless = borderless; }
		//! Sets whether the Window created will have no border. Defaults to \c false.
		Format&		borderless( bool border = true ) { mBorderless = border; return *this; }
		//! Returns whether the Window created will always be above all other windows, including other applications' windows. Defaults to \c false.
		bool		isAlwaysOnTop() const { return mAlwaysOnTop; }
		//! Sets whether the Window created will always be above all other windows, including other applications' windows. Defaults to \c false.
		void		setAlwaysOnTop( bool alwaysOnTop = true ) { mAlwaysOnTop = alwaysOnTop; }
		//! Sets whether the Window created will always be above all other windows, including other applications' windows. Defaults to \c false.
		Format&		alwaysOnTop( bool top = true ) { mAlwaysOnTop = top; return *this; }
		//! On Mac OS X enables the native full screen toggle button. Defaults to \c false.
		void		enableFullScreenButton( bool enabled = true ) { mFullScreenButtonEnabled = enabled; }
		//! On Mac OS X enables the native full screen toggle button. Defaults to \c false.
		Format&		fullScreenButton( bool enabled = true ) { mFullScreenButtonEnabled = enabled; return *this; }
		//! On Mac OS X returns whether the native full screen toggle button is displayed. Defaults to \c false.
		bool		isFullScreenButtonEnabled() const { return mFullScreenButtonEnabled; }

		//! Returns the title of the Window as a UTF-8 string.
		std::string getTitle() const { return mTitle; }
		//! Sets the title of the Window as a UTF-8 string.
		void		setTitle( const std::string &title ) { mTitle = title; mTitleSpecified = true; }
		//! Sets the title of the Window as a UTF-8 string.
		Format&		title( const std::string &t ) { setTitle( t ); return *this; }
		//! Returns whether a non-default title has been requested for the Window.
		bool		isTitleSpecified() const { return mTitleSpecified; }
		//! Unspecifies a non-default title for the window, effectively requestion the default title.
		void		unspecifyTitle() { mTitleSpecified = false; }


	  private:
		RendererRef				mRenderer;
		bool					mFullScreen;
		FullScreenOptions		mFullScreenOptions;
		DisplayRef				mDisplay;
		Vec2i					mSize, mPos;
		bool					mPosSpecified;
		bool					mResizable, mBorderless, mAlwaysOnTop, mFullScreenButtonEnabled;
		std::string				mTitle;
		bool					mTitleSpecified;

#if defined( CINDER_COCOA_TOUCH )
		UIViewController *mRootViewController;
#endif
	};

	//! Returns whether the Window is full-screen or not
	bool	isFullScreen() const;
	//! Sets the Window to be full-screen or not
	void	setFullScreen( bool fullScreen = true, const FullScreenOptions& options = FullScreenOptions() );
	//! Returns the width of the Window in points
	int32_t	getWidth() const { return getSize().x; }
	//! Returns the height of the Window in points
	int32_t	getHeight() const { return getSize().y; }
	//! Returns the Window aspect ratio, which is its width / height
	float	getAspectRatio() const { return getSize().x / (float)getSize().y; }
	//! Returns the bounding Area of the Window in points: [0,0]-(width,height)
	Area	getBounds() const { return Area( 0, 0, getSize().x, getSize().y ); }
	//! Gets the size of the Window measured in points
	virtual Vec2i	getSize() const;
	//! Sets the size of the Window to ( \a width, \a height ) measured in points
	void	setSize( int32_t width, int32_t height ) { setSize( Vec2i( width, height ) ); }
	//! Sets the size of the Window to \a size measured in points
	void	setSize( const Vec2i &size );
	//! Gets the position of the Window's upper-left corner measured in points, relative to the primary display's upper-left corner.
	Vec2i	getPos() const;
	//! Sets the position of the Window's upper-left corner relative to the primary display's upper-left corner to (\a x, \a y) measured in points.
	void	setPos( int32_t x, int32_t y ) const { setPos( Vec2i( x, y ) ); }
	//! Sets the position of the Window's upper-left corner relative to the primary display's upper-left to \a pos measured in points.
	void	setPos( const Vec2i &pos ) const;
	//! Returns the center of the Window in its own coordinate system measured in points
	Vec2f	getCenter() const { return Vec2f( getWidth() / 2.0f, getHeight() / 2.0f ); }
	//! Sets the position and size of the Window so that it spans all connected displays
	void	spanAllDisplays();
	
	//! Returns the multiplier (typically 2 on high-density (Retina) displays, 1 otherwise) mapping points to pixels
	float	getContentScale() const;
	//! Returns a scalar mapped from points to pixels by multiplying by getContentScale()
	float	toPixels( float s ) const { return s * getContentScale(); }
	//! Returns a Vec2f mapped from points to pixels by multiplying by getContentScale()
	Vec2f	toPixels( Vec2f s ) const { return s * getContentScale(); }
	//! Returns a Vec2i mapped from points to pixels by multiplying by getContentScale()
	Vec2i	toPixels( Vec2i s ) const { return Vec2i( (int32_t)(s.x * getContentScale()), (int32_t)(s.y * getContentScale()) ); }	
	//! Returns an Area mapped from points to pixels by multiplying by getContentScale()
	Area	toPixels( const Area &a ) const { const float s = getContentScale(); return Area( (int32_t)(a.x1 * s), (int32_t)(a.y1 * s), (int32_t)(a.x2 * s), (int32_t)(a.y2 * s) ); }
	//! Returns a Rectf mapped from points to pixels by multiplying by getContentScale()
	Rectf	toPixels( const Rectf &a ) const { return a * getContentScale(); }
	//! Returns a scalar mapped from pixels to points by dividing by getContentScale()
	float	toPoints( float s ) const { return s / getContentScale(); }
	//! Returns a Vec2f mapped from pixels to points by dividing by getContentScale()
	Vec2f	toPoints( Vec2f s ) const { return s / getContentScale(); }
	//! Returns a Vec2i mapped from pixels to points by dividing by getContentScale()
	Vec2i	toPoints( Vec2i s ) const { return Vec2i( (int32_t)(s.x / getContentScale()), (int32_t)(s.y / getContentScale()) ); }	
	//! Returns an Area mapped from pixels to points by dividing by getContentScale()
	Area	toPoints( const Area &a ) const { const float s = 1.0f / getContentScale(); return Area( (int32_t)(a.x1 * s), (int32_t)(a.y1 * s), (int32_t)(a.x2 * s), (int32_t)(a.y2 * s) ); }
	//! Returns a Rectf mapped from pixels to points by dividing by getContentScale()
	Rectf	toPoints( const Rectf &a ) const { return a / getContentScale(); }
	
	//! Returns the Window's title as a UTF-8 string.
	std::string		getTitle() const;
	//! Sets the Window's title as a UTF-8 string.
	void			setTitle( const std::string &title );
	
	//! Returns whether the window has a border (chrome/frame)
	bool	isBorderless() const;
	//! Sets whether the window has a border (chrome/frame)
	void	setBorderless( bool borderless = true );
	//! Returns whether the window always remains above all other windows
	bool	isAlwaysOnTop() const;
	//! Sets whether the window always remains above all other windows
	void	setAlwaysOnTop( bool alwaysOnTop = true );

	//! Hides the window but does not destroy it
	void	hide();
	//! Shows a window that was hidden with \a hide()
	void	show();
	//! Returns whether the window has been hidden with \a hide()
	bool	isHidden() const;

	//! Closes and destroys the Window
	void	close();

	//! Returns the Display on which the Window resides
	DisplayRef		getDisplay() const;
	//! Returns the Renderer associated with the Window
	RendererRef		getRenderer() const;
	//! Returns the associated NSView on Mac OS X, UIView on iOS, or HWND on MSW
	void*			getNative() const;
#if defined( CINDER_COCOA_TOUCH )
	//! Returns the UIViewController instance that manages the assoicated UIView on iOS
	UIViewController* getNativeViewController();
#elif defined( CINDER_WINRT )
	DX_WINDOW_TYPE getNativeCoreWindow();
#endif
#if defined( CINDER_MSW )
	//! Returns the Window's HDC on MSW. Suitable for GDI+ calls with Renderer2d.
	HDC				getDc() const { return getRenderer()->getDc(); }
#endif

	EventSignalMouse&	getSignalMouseDown() { return mSignalMouseDown; }
	void				emitMouseDown( MouseEvent *event );
	template<typename T, typename Y>
	signals::connection	connectMouseDown( T fn, Y *inst ) { return getSignalMouseDown().connect( std::bind( fn, inst, std::placeholders::_1 ) ); }

	EventSignalMouse&	getSignalMouseDrag() { return mSignalMouseDrag; }
	void				emitMouseDrag( MouseEvent *event );
	template<typename T, typename Y>
	signals::connection	connectMouseDrag( T fn, Y *inst ) { return getSignalMouseDrag().connect( std::bind( fn, inst, std::placeholders::_1 ) ); }

	EventSignalMouse&	getSignalMouseUp() { return mSignalMouseUp; }
	void				emitMouseUp( MouseEvent *event );
	template<typename T, typename Y>
	signals::connection	connectMouseUp( T fn, Y *inst ) { return getSignalMouseUp().connect( std::bind( fn, inst, std::placeholders::_1 ) ); }

	EventSignalMouse&	getSignalMouseMove() { return mSignalMouseMove; }
	void				emitMouseMove( MouseEvent *event );
	template<typename T, typename Y>
	signals::connection	connectMouseMove( T fn, Y *inst ) { return getSignalMouseMove().connect( std::bind( fn, inst, std::placeholders::_1 ) ); }

	EventSignalMouse&	getSignalMouseWheel() { return mSignalMouseWheel; }
	void				emitMouseWheel( MouseEvent *event );
	template<typename T, typename Y>
	signals::connection	connectMouseWheel( T fn, Y *inst ) { return getSignalMouseWheel().connect( std::bind( fn, inst, std::placeholders::_1 ) ); }

	EventSignalTouch&	getSignalTouchesBegan() { return mSignalTouchesBegan; }
	void				emitTouchesBegan( TouchEvent *event );
	template<typename T, typename Y>
	signals::connection	connectTouchesBegan( T fn, Y *inst ) { return getSignalTouchesBegan().connect( std::bind( fn, inst, std::placeholders::_1 ) ); }

	EventSignalTouch&	getSignalTouchesMoved() { return mSignalTouchesMoved; }
	void				emitTouchesMoved( TouchEvent *event );
	template<typename T, typename Y>
	signals::connection	connectTouchesMoved( T fn, Y *inst ) { return getSignalTouchesMoved().connect( std::bind( fn, inst, std::placeholders::_1 ) ); }

	EventSignalTouch&	getSignalTouchesEnded() { return mSignalTouchesEnded; }
	void				emitTouchesEnded( TouchEvent *event );
	template<typename T, typename Y>
	signals::connection	connectTouchesEnded( T fn, Y *inst ) { return getSignalTouchesEnded().connect( std::bind( fn, inst, std::placeholders::_1 ) ); }

	//! Returns a std::vector of all active touches
	const std::vector<TouchEvent::Touch>&	getActiveTouches() const;

	EventSignalKey&		getSignalKeyDown() { return mSignalKeyDown; }
	void				emitKeyDown( KeyEvent *event );
	template<typename T, typename Y>
	signals::connection	connectKeyDown( T fn, Y *inst ) { return getSignalKeyDown().connect( std::bind( fn, inst, std::placeholders::_1 ) ); }

	EventSignalKey&		getSignalKeyUp() { return mSignalKeyUp; }
	void				emitKeyUp( KeyEvent *event );
	template<typename T, typename Y>
	signals::connection	connectKeyUp( T fn, Y *inst ) { return getSignalKeyUp().connect( std::bind( fn, inst, std::placeholders::_1 ) ); }
	
	EventSignalWindow&	getSignalDraw() { return mSignalDraw; }
	//! Fires the 'draw' signal. Note in general this should not be called directly as it doesn't perform all necessary setup.
	void				emitDraw();
	template<typename T, typename Y>
	signals::connection	connectDraw( T fn, Y *inst ) { return getSignalDraw().connect( std::bind( fn, inst ) ); }

	//! Returns the signal which is emitted after the draw signal and app's draw() virtual method
	EventSignalWindow&	getSignalPostDraw() { return mSignalPostDraw; }
	template<typename T, typename Y>
	signals::connection	connectPostDraw( T fn, Y *inst ) { return getSignalPostDraw().connect( std::bind( fn, inst ) ); }

	EventSignalWindow&	getSignalMove() { return mSignalMove; }
	void				emitMove();
	template<typename T, typename Y>
	signals::connection	connectMove( T fn, Y *inst ) { return getSignalMove().connect( std::bind( fn, inst ) ); }

	EventSignalWindow&	getSignalResize() { return mSignalResize; }
	void 				emitResize();
	template<typename T, typename Y>
	signals::connection	connectResize( T fn, Y *inst ) { return getSignalResize().connect( std::bind( fn, inst ) ); }

	EventSignalWindow&	getSignalDisplayChange() { return mSignalDisplayChange; }
	void				emitDisplayChange();
	template<typename T, typename Y>
	signals::connection	connectDisplayChange( T fn, Y *inst ) { return getSignalDisplayChange().connect( std::bind( fn, inst ) ); }

	//! Returns the Signal emitted whenever a Window is closing. The WindowRef parameter is still valid at this point but its renderer is not.
	EventSignalWindow&	getSignalClose() { return mSignalClose; }
	//! Fires the 'close' signal.
	void				emitClose();
	template<typename T, typename Y>
	signals::connection	connectClose( T fn, Y *inst ) { return getSignalClose().connect( std::bind( fn, inst ) ); }

	EventSignalFileDrop&	getSignalFileDrop() { return mSignalFileDrop; }
	void					emitFileDrop( FileDropEvent *event );
	template<typename T, typename Y>
	signals::connection		connectFileDrop( T fn, Y *inst ) { return getSignalFileDrop().connect( std::bind( fn, inst, std::placeholders::_1 ) ); }
	
	//! Returns the window-specific data associated with this Window.
	template<typename T>
	T*			getUserData() { return static_cast<T*>( mUserData.get() ); }
	//! Sets the window-specific data associated with this Window. The variable is \c deleted upon destruction of the Window.
	template<typename T>
	void		setUserData( T *userData ) { mUserData = std::shared_ptr<void>( std::shared_ptr<T>( userData ) ); }
	
	//! Returns whether this Window is valid. \c false means it should no longer be used (neither read nor write)
	bool	isValid() const { return ! mValid; }
	void	setInvalid() { mValid = false; }
	
	//! \cond
	// This should not be called except by App implementations
#if defined( CINDER_COCOA ) && defined( __OBJC__ )
	static WindowRef		privateCreate__( id<WindowImplCocoa> impl, App *app )
#elif defined( CINDER_MSW )
	static WindowRef		privateCreate__( WindowImplMsw *impl, App *app )
#elif defined( CINDER_WINRT )
	static WindowRef		privateCreate__( WindowImplWinRT *impl, App *app )
#else
	static WindowRef		privateCreate__( WindowImplCocoa *impl, App *app )
#endif
	{
		WindowRef result( new Window );
		result->setImpl( impl );
		result->setApp( app );
		
		return result;
	}
	//! \endcond

	App*			getApp() const { return mApp; }
	
  protected:
	Window() : mValid( true ), mImpl( 0 ) {}
  
	void	testValid() const {
		if( ! mValid )
			throw ExcInvalidWindow();
	}

	void		setApp( App *app ) { mApp = app; }	

#if defined( CINDER_COCOA )
  #if defined( __OBJC__ )
	void		setImpl( id<WindowImplCocoa> impl ) { mImpl = impl; }
  #else
	void		setImpl( WindowImplCocoa *impl ) { mImpl = impl; }
  #endif
#elif defined( CINDER_MSW )
	void		setImpl( WindowImplMsw *impl ) { mImpl = impl; }
#elif defined( CINDER_WINRT )
	void		setImpl( WindowImplWinRT *impl ) { mImpl = impl; }
#endif

	App							*mApp;
	bool						mValid;
	std::shared_ptr<void>		mUserData;
	
	EventSignalMouse		mSignalMouseDown, mSignalMouseDrag, mSignalMouseUp, mSignalMouseWheel, mSignalMouseMove;
	EventSignalTouch		mSignalTouchesBegan, mSignalTouchesMoved, mSignalTouchesEnded;
	EventSignalKey			mSignalKeyDown, mSignalKeyUp;
	EventSignalWindow		mSignalDraw, mSignalPostDraw, mSignalMove, mSignalResize, mSignalDisplayChange, mSignalClose;
	EventSignalFileDrop		mSignalFileDrop;
	
#if defined( CINDER_COCOA )
  #if defined( __OBJC__ )
	id<WindowImplCocoa>		mImpl;
  #else
    WindowImplCocoa			*mImpl; // necessary to trick c++ translation units
  #endif
#elif defined( CINDER_MSW )
	WindowImplMsw		*mImpl;
#elif defined( CINDER_WINRT )
	WindowImplWinRT *mImpl;
#endif
};

} } // namespace cinder::app