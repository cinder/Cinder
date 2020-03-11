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
#include "cinder/Signals.h"
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
	@class NSString;
	#if defined( CINDER_COCOA_TOUCH )
		@class UIViewController;
	#endif

	@protocol WindowImplCocoa
		@required
		- (bool)isFullScreen;
		- (void)setFullScreen:(bool)fullScreen options:(const cinder::app::FullScreenOptions *)options;
		- (cinder::ivec2)getSize;
		- (void)setSize:(cinder::ivec2)size;
		- (cinder::ivec2)getPos;
		- (void)setPos:(cinder::ivec2)pos;
		- (float)getContentScale;
		- (void)close;
		- (NSString *)getTitle;
		- (void)setTitle:(NSString *)title;
		- (bool)isBorderless;
		- (void)setBorderless:(bool)borderless;
		- (bool)isAlwaysOnTop;
		- (void)setAlwaysOnTop:(bool)alwaysOnTop;
		- (void)hide;
		- (void)show;
		- (bool)isHidden;
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
#elif defined( CINDER_UWP )
	namespace cinder { namespace app {
		class WindowImplWinRt;
	} } // namespace cinder::app
#elif defined( CINDER_MSW_DESKTOP )
	namespace cinder { namespace app {
		class WindowImplMsw;
	} } // namespace cinder::app
#elif defined( CINDER_ANDROID )
	namespace cinder { namespace app {
  	class WindowImplAndroid;
	} } // namespace cinder::app
#elif defined( CINDER_LINUX )
	namespace cinder { namespace app {
		class WindowImplLinux;
	} } // namespace cinder::app
#endif

namespace cinder { namespace app {

typedef	 signals::Signal<void( MouseEvent & ),		CollectorEvent<MouseEvent> >		EventSignalMouse;
typedef	 signals::Signal<void( TouchEvent & ),		CollectorEvent<TouchEvent> >		EventSignalTouch;
typedef	 signals::Signal<void( KeyEvent & ),		CollectorEvent<KeyEvent> >			EventSignalKey;
typedef	 signals::Signal<void( FileDropEvent & ),	CollectorEvent<FileDropEvent> >		EventSignalFileDrop;
typedef	 signals::Signal<void()>														EventSignalWindow;

//! Thrown when an operation is performed on a WindowRef which refers to an invalid Window
class CI_API ExcInvalidWindow : public cinder::Exception {
	virtual const char * what() const throw() { return "Invalid Window"; }
};

//! Options passed when entering fullscreen.
struct CI_API FullScreenOptions {
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

class CI_API Window : public std::enable_shared_from_this<Window> {
  public:
	// Parameters for a Window, which are used to create the physical window by the App
	struct Format {
		Format( RendererRef renderer = RendererRef(), DisplayRef display = DisplayRef(), bool fullScreen = false, ivec2 size = ivec2( 640, 480 ), ivec2 pos = ivec2() )
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
		ivec2		getSize() const { return mSize; }
		//! Sets the size in points at which the Window will be created. Default is 640 x 480.
		void		setSize( const ivec2 &size ) { mSize = size; }
		//! Sets the size in points at which the Window will be created. Default is 640 x 480.
		void		setSize( int32_t width, int32_t height ) { mSize = ivec2( width, height ); }
		//! Sets the size in points at which the Window will be created. Default is 640 x 480.
		Format&		size( const ivec2 &s ) { mSize = s; return *this; }
		//! Sets the size in points at which the Window will be created. Default is 640 x 480.
		Format&		size( int32_t width, int32_t height ) { mSize = ivec2( width, height ); return *this; }

		//! Returns the position in points measured relative to the system's primary display's upper-left corner at which the Window will be created. Default is centered on the display.
		ivec2		getPos() const { return mPos; }
		//! Sets the position in points measured relative to the system's primary display's upper-left corner at which the Window will be created. Default is centered on the display.
		void		setPos( const ivec2 &pos ) { mPos = pos; mPosSpecified = true; }
		//! Sets the position in points measured relative to the system's primary display's upper-left corner at which the Window will be created. Default is centered on the display.
		void		setPos( int32_t x, int32_t y ) { mPos = ivec2( x, y ); mPosSpecified = true; }
		//! Sets the position in points measured relative to the system's primary display's upper-left corner at which the Window will be created. Default is centered on the display.
		Format&		pos( const ivec2 &pos ) { mPos = pos; mPosSpecified = true; return *this; }
		//! Sets the position in points measured relative to the system's primary display's upper-left corner at which the Window will be created. Default is centered on the display.
		Format&		pos( int32_t x, int32_t y ) { mPos = ivec2( x, y ); mPosSpecified = true; return *this; }
		//! Returns whether a non-default position has been requested for the Window.
		bool		isPosSpecified() const { return mPosSpecified; }
		//! Unspecifies a non-default position for the window, effectively requestion the default position.
		void		setPosUnspecified() { mPosSpecified = false; }

		//! Returns the Renderer which will be instantiated for the Window. Defaults to an instance of the App's default renderer (specified in the app-instantiation macro).
		RendererRef	getRenderer() const { return mRenderer; }
		//! Sets the Renderer which will be instantiated for the Window.
		void		setRenderer( const RendererRef &renderer ) { mRenderer = renderer; }
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
		void		setTitleUnspecified()		{ mTitleSpecified = false; }


	  private:
		RendererRef				mRenderer;
		bool					mFullScreen;
		FullScreenOptions		mFullScreenOptions;
		DisplayRef				mDisplay;
		ivec2					mSize, mPos;
		bool					mPosSpecified;
		bool					mResizable, mBorderless, mAlwaysOnTop, mFullScreenButtonEnabled;
		std::string				mTitle;
		bool					mTitleSpecified;

#if defined( CINDER_COCOA_TOUCH )
		__unsafe_unretained UIViewController *mRootViewController;
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
	virtual ivec2	getSize() const;
	//! Sets the size of the Window to ( \a width, \a height ) measured in points
	void	setSize( int32_t width, int32_t height ) { setSize( ivec2( width, height ) ); }
	//! Sets the size of the Window to \a size measured in points
	void	setSize( const ivec2 &size );
	//! Gets the position of the Window's upper-left corner measured in points, relative to the primary display's upper-left corner.
	ivec2	getPos() const;
	//! Sets the position of the Window's upper-left corner relative to the primary display's upper-left corner to (\a x, \a y) measured in points.
	void	setPos( int32_t x, int32_t y ) const { setPos( ivec2( x, y ) ); }
	//! Sets the position of the Window's upper-left corner relative to the primary display's upper-left to \a pos measured in points.
	void	setPos( const ivec2 &pos ) const;
	//! Returns the center of the Window in its own coordinate system measured in points
	vec2	getCenter() const { return vec2( getWidth() / 2.0f, getHeight() / 2.0f ); }
	//! Sets the position and size of the Window so that it spans all connected displays
	void	spanAllDisplays();
	//! Returns the mouse position, in window coordinates measured in points. Can be negative or larger than window size if the mouse is outside of this window.
	ivec2	getMousePos() const;
	//! Returns the multiplier (typically 2 on high-density (Retina) displays, 1 otherwise) mapping points to pixels
	float	getContentScale() const;
	//! Returns a scalar mapped from points to pixels by multiplying by getContentScale()
	int32_t	toPixels( int32_t s ) const { return static_cast<int32_t>( s * getContentScale() ); }
	//! Returns a scalar mapped from points to pixels by multiplying by getContentScale()
	float	toPixels( float s ) const { return s * getContentScale(); }
	//! Returns a vec2 mapped from points to pixels by multiplying by getContentScale()
	vec2	toPixels( vec2 s ) const { return s * getContentScale(); }
	//! Returns a ivec2 mapped from points to pixels by multiplying by getContentScale()
	ivec2	toPixels( ivec2 s ) const { return ivec2( (int32_t)(s.x * getContentScale()), (int32_t)(s.y * getContentScale()) ); }	
	//! Returns an Area mapped from points to pixels by multiplying by getContentScale()
	Area	toPixels( const Area &a ) const { const float s = getContentScale(); return Area( (int32_t)(a.x1 * s), (int32_t)(a.y1 * s), (int32_t)(a.x2 * s), (int32_t)(a.y2 * s) ); }
	//! Returns a Rectf mapped from points to pixels by multiplying by getContentScale()
	Rectf	toPixels( const Rectf &a ) const { return a * getContentScale(); }
	//! Returns a scalar mapped from pixels to points by dividing by getContentScale()
	float	toPoints( float s ) const { return s / getContentScale(); }
	//! Returns a scalar mapped from points to pixels by multiplying by getContentScale()
	int32_t	toPoints( int32_t s ) const { return static_cast<int32_t>( s / getContentScale() ); }
	//! Returns a vec2 mapped from pixels to points by dividing by getContentScale()
	vec2	toPoints( vec2 s ) const { return s / getContentScale(); }
	//! Returns a ivec2 mapped from pixels to points by dividing by getContentScale()
	ivec2	toPoints( ivec2 s ) const { return ivec2( (int32_t)(s.x / getContentScale()), (int32_t)(s.y / getContentScale()) ); }	
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
#elif defined( CINDER_UWP )
	::Platform::Agile<Windows::UI::Core::CoreWindow> getNativeCoreWindow();
#endif
#if defined( CINDER_MSW_DESKTOP )
	//! Returns the Window's HDC on MSW. Suitable for GDI+ calls with Renderer2d.
	HDC				getDc() const { return getRenderer()->getDc(); }
#endif

	EventSignalMouse&	getSignalMouseDown() { return mSignalMouseDown; }
	void				emitMouseDown( MouseEvent *event );

	EventSignalMouse&	getSignalMouseDrag() { return mSignalMouseDrag; }
	void				emitMouseDrag( MouseEvent *event );

	EventSignalMouse&	getSignalMouseUp() { return mSignalMouseUp; }
	void				emitMouseUp( MouseEvent *event );

	EventSignalMouse&	getSignalMouseMove() { return mSignalMouseMove; }
	void				emitMouseMove( MouseEvent *event );

	EventSignalMouse&	getSignalMouseWheel() { return mSignalMouseWheel; }
	void				emitMouseWheel( MouseEvent *event );

	EventSignalTouch&	getSignalTouchesBegan() { return mSignalTouchesBegan; }
	void				emitTouchesBegan( TouchEvent *event );

	EventSignalTouch&	getSignalTouchesMoved() { return mSignalTouchesMoved; }
	void				emitTouchesMoved( TouchEvent *event );

	EventSignalTouch&	getSignalTouchesEnded() { return mSignalTouchesEnded; }
	void				emitTouchesEnded( TouchEvent *event );

	//! Returns a std::vector of all active touches
	const std::vector<TouchEvent::Touch>&	getActiveTouches() const;

	EventSignalKey&		getSignalKeyDown() { return mSignalKeyDown; }
	void				emitKeyDown( KeyEvent *event );

	EventSignalKey&		getSignalKeyUp() { return mSignalKeyUp; }
	void				emitKeyUp( KeyEvent *event );

	EventSignalWindow&	getSignalDraw() { return mSignalDraw; }
	//! Fires the 'draw' signal. Note in general this should not be called directly as it doesn't perform all necessary setup.
	void				emitDraw();

	//! Returns the signal which is emitted after the draw signal and app's draw() virtual method
	EventSignalWindow&	getSignalPostDraw() { return mSignalPostDraw; }

	EventSignalWindow&	getSignalMove() { return mSignalMove; }
	void				emitMove();

	EventSignalWindow&	getSignalResize() { return mSignalResize; }
	void 				emitResize();

	EventSignalWindow&	getSignalDisplayChange() { return mSignalDisplayChange; }
	void				emitDisplayChange();

	//! Returns the Signal emitted whenever a Window is closing. The WindowRef parameter is still valid at this point but its renderer is not.
	EventSignalWindow&	getSignalClose() { return mSignalClose; }
	//! Fires the 'close' signal.
	void				emitClose();

	EventSignalFileDrop&	getSignalFileDrop() { return mSignalFileDrop; }
	void					emitFileDrop( FileDropEvent *event );

	//! Returns the window-specific data associated with this Window.
	template<typename T>
	T*			getUserData() { return static_cast<T*>( mUserData.get() ); }
	//! Sets the window-specific data associated with this Window. The variable is \c deleted upon destruction of the Window.
	template<typename T>
	void		setUserData( T *userData ) { mUserData = std::shared_ptr<void>( std::shared_ptr<T>( userData ) ); }
	
	//! Returns whether this Window is valid. \c false means it should no longer be used (neither read nor write)
	bool	isValid() const { return mValid; }
	void	setInvalid() { mValid = false; }
	
	//! \cond
	// This should not be called except by App implementations
#if defined( CINDER_COCOA ) && defined( __OBJC__ )
	static WindowRef		privateCreate__( id<WindowImplCocoa> impl, AppBase *app )
#elif defined( CINDER_MSW_DESKTOP )
	static WindowRef		privateCreate__( WindowImplMsw *impl, AppBase *app )
#elif defined( CINDER_UWP )
	static WindowRef		privateCreate__( WindowImplWinRt *impl, AppBase *app )
#elif defined( CINDER_ANDROID )
  static WindowRef    privateCreate__( WindowImplAndroid *impl, AppBase *app )
#elif defined( CINDER_LINUX )
  static WindowRef    privateCreate__( WindowImplLinux *impl, AppBase *app )
#else
	static WindowRef		privateCreate__( WindowImplCocoa *impl, AppBase *app )
#endif
	{
		WindowRef result( new Window );
		result->setImpl( impl );
		result->setApp( app );
		
		return result;
	}
	//! \endcond

	AppBase*			getApp() const { return mApp; }
	
  protected:
	Window() : mValid( true ), mImpl( 0 ) {}
  
	void	testValid() const {
		if( ! mValid )
			throw ExcInvalidWindow();
	}

	void		setApp( AppBase *app ) { mApp = app; }	
	void		applyCurrentContext();

#if defined( CINDER_COCOA )
  #if defined( __OBJC__ )
	void		setImpl( id<WindowImplCocoa> impl ) { mImpl = impl; }
  #else
	void		setImpl( WindowImplCocoa *impl ) { mImpl = impl; }
  #endif
#elif defined( CINDER_MSW_DESKTOP )
	void		setImpl( WindowImplMsw *impl ) { mImpl = impl; }
#elif defined( CINDER_UWP )
	void		setImpl( WindowImplWinRt *impl ) { mImpl = impl; }
#elif defined( CINDER_ANDROID )
  void    setImpl( WindowImplAndroid *impl ) { mImpl = impl; }    
#elif defined( CINDER_LINUX )
  void    setImpl( WindowImplLinux *impl ) { mImpl = impl; }    
#endif

	AppBase							*mApp;
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
#elif defined( CINDER_MSW_DESKTOP )
	WindowImplMsw		*mImpl;
#elif defined( CINDER_UWP )
	WindowImplWinRt 	*mImpl;
#elif defined( CINDER_ANDROID )
	WindowImplAndroid	*mImpl;
#elif defined( CINDER_LINUX )
	WindowImplLinux		*mImpl;
#endif
 
private:
#if defined( CINDER_ANDROID )
	friend class AppImplAndroid;
	WindowImplAndroid   *getImpl() { return mImpl; }
#elif defined( CINDER_LINUX )
	friend class AppImplLinux;
	WindowImplLinux     *getImpl() { return mImpl; }
#endif    
};

} } // namespace cinder::app
