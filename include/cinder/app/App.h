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
#include "cinder/app/Renderer.h"
#include "cinder/Vector.h"
#include "cinder/app/MouseEvent.h"
#include "cinder/app/KeyEvent.h"
#include "cinder/app/FileDropEvent.h"
#include "cinder/Stream.h"
#include "cinder/Display.h"
#include "cinder/DataSource.h"
#include "cinder/Timer.h"
#if defined( CINDER_COCOA )
	#if defined( CINDER_COCOA_TOUCH )
		#if defined( __OBJC__ )
			#import <UIKit/UIKit.h>
			#import <CoreFoundation/CoreFoundation.h>
		#endif
	#else
		#include <ApplicationServices/ApplicationServices.h>
	#endif
	#if defined __OBJC__
		@class CinderView;
		@class AppImplCocoaRendererQuartz;
		@class AppImplCocoaRendererGl;
	#else
		class AppImplCocoaRendererQuartz;
		class AppImplCocoaRendererGl;
	#endif
//	class CinderView;
#elif defined( CINDER_MSW )
	#include "cinder/msw/OutputDebugStringStream.h"
#endif

#include <vector>

namespace cinder { namespace app { 

class App {
 public:
	class Settings {
	  public:
	    // whether or not the app should terminate prior to launching
		bool	isPrepared() const { return !mShouldQuit; };

		//! width and height of the window when applicable
		void	setWindowSize( int aWindowSizeX, int aWindowSizeY );

		//! The maximum frameRate the update/draw loop will execute at, specified in frames per second. Default value is 30 FPS
		void	setFrameRate( float aFrameRate );

		//! a value of true allows screensavers or the system's power management to hide the app. Default value is \c false.
		void	enablePowerManagement( bool aPowerManagement = true );

		//! is the application set to run at fullscreen
		bool	isFullScreen() const { return mFullScreen; }
		//! width of the application's window specified in pixels
		int		getWindowWidth() const { return mWindowSizeX; }
		//! height of the application's window specified in pixels
		int		getWindowHeight() const { return mWindowSizeY; }
		//! width and height of the application's window specified in pixels
		Vec2i	getWindowSize() const { return Vec2i( mWindowSizeX, mWindowSizeY ); }
		//! the size of the application's window specified in pixels. \return cinder::Area( 0, 0, width in pixels, height in pixels )
		Area	getWindowBounds() const { return Area( 0, 0, mWindowSizeX, mWindowSizeY ); }
		//! the title of the app reflected in ways particular to the app type and platform (such as its Window or menu)
		void	setTitle( const std::string &title ) { mTitle = title; }

		//! maximum frameRate of the application specified in frames per second
		float	getFrameRate() const { return mFrameRate; }
		//! are users allowed to resize the window
		bool	isResizable() const { return mResizable; }
		//! is power management enabled, allowing screensavers and the system's power management to hide the application
		bool	getPowerManagement() const { return mPowerManagement; }

	  protected:
		Settings();
		virtual ~Settings() {}	  
	  
		bool			mShouldQuit; // defaults to false, facilitates early termination
		int				mWindowSizeX, mWindowSizeY; // default: 640x480
		bool			mFullScreen; // window covers screen. default: false
		float			mFrameRate;
		bool			mResizable; // window is Resizable. default: true
		bool			mPowerManagement; // allow screensavers or power management to hide app. default: false
		std::string		mTitle;
	};


 public:
	// interface
	App();
	virtual ~App();

	//! Override to perform any application setup after the Renderer has been initialized.
	virtual void	setup() {}
	//! Override to perform any application cleanup before exiting.
	virtual void	shutdown() {}

	//! Override to perform any once-per-loop computation.
	virtual void	update() {}
	//! Override to perform any rendering once-per-loop or in response to OS-prompted requests for refreshes.
	virtual void	draw() {}
	
	//! Override to receive mouse-down events.
	virtual void	mouseDown( MouseEvent event ) {}
	//! Override to receive mouse-up events.
	virtual void	mouseUp( MouseEvent event ) {}	
	//! Override to receive mouse-wheel events.
	virtual void	mouseWheel( MouseEvent event ) {}
	//! Override to receive mouse-move events.
	virtual void	mouseMove( MouseEvent event ) {}
	//! Override to receive mouse-drag events.
	virtual void	mouseDrag( MouseEvent event ) {}	
	//! Override to receive key-down events.
	virtual void	keyDown( KeyEvent event ) {}
	//! Override to receive key-up events.
	virtual void	keyUp( KeyEvent event ) {}
	//! Override to receive window resize events.
	virtual void	resize( int width, int height ) {}
	//! Override to receive file-drop events.	
	virtual void	fileDrop( FileDropEvent event ) {}
	
	//! Quits the application gracefully
	virtual void	quit() = 0;

	class Listener {
	 public:
		virtual bool	mouseDown( MouseEvent event ) { return false; }
		virtual bool	mouseUp( MouseEvent event ) { return false; }
		virtual bool	mouseWheel( MouseEvent event ) { return false; }
		virtual bool	mouseMove( MouseEvent event ) { return false; }
		virtual bool	mouseDrag( MouseEvent event ) { return false; }
		virtual bool	keyDown( KeyEvent event ) { return false; }
		virtual bool	keyUp( KeyEvent event ) { return false; }
		virtual bool	resize( int width, int height ) { return false; }
		virtual bool	fileDrop( FileDropEvent event ) { return false; }
	};

	//! Adds a Listener to the App's event listeners. The app <tt>delete</tt>s \a listener upon its own destruction unless it is removed via removeListener
	void		addListener( Listener *listener );
	//! Removes a listener from the App's event listeners. Does not <tt>delete</tt> \a listener.
	void		removeListener( Listener *listener ); 

	// Accessors
	virtual const Settings&	getSettings() const { return getSettings(); }
	Renderer*				getRenderer() const { return mRenderer.get(); }
	
	//! Returns the width of the App's window measured in pixels, or the screen when in full-screen mode.	
	virtual int			getWindowWidth() const = 0;
	//! Sets the width of the App's window measured in pixels. Ignored in full-screen mode.	
	virtual void		setWindowWidth( int windowWidth ) = 0;
	//! Returns the height of the App's window measured in pixels, or the screen when in full-screen mode.	
	virtual int			getWindowHeight() const = 0;
	//! Sets the height of the App's window measured in pixels. Ignored in full-screen mode.	
	virtual void		setWindowHeight( int windowHeight ) = 0;
	//! Sets the size of the App's window. Ignored in full-screen mode.
	virtual void		setWindowSize( int windowWidth, int windowHeight ) = 0;
	//! Sets the size of the App's window. Ignored in full-screen mode.
	void				setWindowSize( const Vec2i &size ) { setWindowSize( size.x, size.y ); }
	//! Returns the center of the App's window or the screen in full-screen mode.
	/** Equivalent to \code Vec2f( getWindowWidth() * 0.5, getWindowHeight() * 0.5 ) \endcode **/	
	Vec2f				getWindowCenter() const { return Vec2f( (float)getWindowWidth(), (float)getWindowHeight() ) * 0.5f; }
	//! Returns the size of the App's window or the screen in full-screen mode
	Vec2i				getWindowSize() const { return Vec2i( getWindowWidth(), getWindowHeight() ); }
	//! Returns the aspect ratio of the App's window or the screen in full-screen mode
	float				getWindowAspectRatio() const { return getWindowWidth() / (float)getWindowHeight(); }
	//! Returns the bounding area of the App's window or the screen in full-screen mode.
	/** Equivalent to \code Area( 0, 0, getWindowWidth(), getWindowHeight() ); \endcode **/	
	Area				getWindowBounds() const { return Area( 0, 0, getWindowWidth(), getWindowHeight() ); }
	//! Returns the maximum frame-rate the App will attempt to maintain.
	virtual float		getFrameRate() const = 0;
	//! Sets the maximum frame-rate the App will attempt to maintain.
	virtual void		setFrameRate( float aFrameRate ) = 0;
	//! Returns the average frame-rate attained by the App as measured in frames-per-second
	float				getAverageFps() const { return mAverageFps; }
	//! Returns the sampling rate in seconds for measuring the average frame-per-second as returned by getAverageFps()
	double				getFpsSampleInterval() const { return mFpsSampleInterval; }
	//! Sets the sampling rate in seconds for measuring the average frame-per-second as returned by getAverageFps()
	void				setFpsSampleInterval( double sampleInterval ) { mFpsSampleInterval = sampleInterval; }	

	//! Returns whether the App is in full-screen mode or not.
	virtual bool		isFullScreen() const = 0;
	//! Sets whether the active App is in full-screen mode based on \a fullScreen
	virtual void		setFullScreen( bool aFullScreen ) = 0;

	//! Returns the number of seconds which have elapsed since application launch
	double				getElapsedSeconds() const { return mTimer.getSeconds(); }
	//! Returns the number of animation frames which have elapsed since application launch
	uint32_t			getElapsedFrames() const { return mFrameCount; }
	
	// utilities
	static DataSourceRef		loadResource( const std::string &macPath, int mswID, const std::string &mswType );
#if defined( CINDER_COCOA )
	static DataSourcePathRef	loadResource( const std::string &macPath );
	std::string					getResourcePath( const std::string &rsrcRelativePath );
#else
	static DataSourceBufferRef	loadResource( int mswID, const std::string &mswType );
#endif
	
	//! Returns the path to the application on disk
	virtual std::string			getAppPath() = 0;
	//! Presents the user with a file-open dialog and returns the selected file path.
	/** The dialog optionally begins at the path \a initialPath and can be limited to allow selection of files ending in the extensions enumerated in \a extensions.
		If the active app is in full-screen mode it will temporarily switch to windowed-mode to present the dialog.
		\return the selected file path or an empty string if the user cancelled. **/
	std::string		getOpenFilePath( const std::string &initialPath = "", std::vector<std::string> extensions = std::vector<std::string>() );
	//! Presents the user with a folder-open dialog and returns the selected folder.
	std::string		getOpenFolderPath(const std::string &initialPath="");
	//! Presents the user with a file-save dialog and returns the selected file path.
	/** The dialog optionally begins at the path \a initialPath and can be limited to allow selection of files ending in the extensions enumerated in \a extensions.
		If the active app is in full-screen mode it will temporarily switch to windowed-mode to present the dialog.
		\return the selected file path or an empty string if the user cancelled. **/
	std::string		getSaveFilePath( const std::string &initialPath = "", std::vector<std::string> extensions = std::vector<std::string>() );

	//! Returns a reference to an output console, which is an alias to std::cout on the mac, and a wrapper around OutputDebugString on MSW
	std::ostream&	console();

	/** \return a copy of the window's contents as a Surface8u **/
	Surface	copyWindowSurface();
	/** \return a copy of the Area \a area from the window's contents as a Surface8u **/
	Surface	copyWindowSurface( const Area &area );
	//! Restores the current rendering context to be the App's window or the screen in full-screen mode. Generally this is only necessary if the app has displayed a dialog box or some other external window.
	void	restoreWindowContext();

	
	// DO NOT CALL - should be private but aren't for esoteric reasons
	//! \cond
	// Internal handlers - these are called into by AppImpl's. If you are calling one of these, you have likely strayed far off the path.
	void	privateMouseDown__( const MouseEvent &event );
	void	privateMouseUp__( const MouseEvent &event );
	void	privateMouseWheel__( const MouseEvent &event );
	void	privateMouseMove__( const MouseEvent &event );
	void	privateMouseDrag__( const MouseEvent &event );
	void	privateKeyDown__( const KeyEvent &event );
	void	privateKeyUp__( const KeyEvent &event );
	void	privateFileDrop__( const FileDropEvent &event );

	virtual void	privateSetup__();
	virtual void	privateResize__( int width, int height );	
	virtual void	privateUpdate__();
	virtual void	privateDraw__();
	virtual void	privateShutdown__();
	//! \endcond

#if defined( CINDER_MSW )
	// Not all Windows target types receive paint events, and the AppImplMswRenderer* needs to know that.
	virtual bool        getsWindowsPaintEvents() = 0;
#endif

	virtual bool		receivesEvents() const { return true; }

	//! Returns a pointer to the active App
	static App*			get() { return sInstance; }

  protected:
	//! \cond
	// These are called by application instantation macros and are only used in the launch process
	static void		prepareLaunch();
	static void		executeLaunch( App *app, class Renderer *renderer, const char *title, int argc, char * const argv[] );
	static void		cleanupLaunch();
	
	virtual void	launch( const char *title, int argc, char * const argv[] ) = 0;
	//! \endcond

  private:
  
#if defined( CINDER_MSW )
	friend class AppImplMsw;
	shared_ptr<cinder::msw::dostream>	mOutputStream;
#else
	static void				*sAutoReleasePool;
#endif

	Timer					mTimer;
	uint32_t				mFrameCount;
	float					mAverageFps;
	uint32_t				mFpsLastSampleFrame;
	double					mFpsLastSampleTime;
	double					mFpsSampleInterval;

	shared_ptr<Renderer>	mRenderer;
	std::vector<Listener*>	mListeners;
	
	static App*		sInstance;
};

/** @name App Free Functions
	Convenience methods which mirror App member-functions and apply to the active application
**/
//@{
//! Returns the width of the active App's window measured in pixels, or the screen when in full-screen mode.
inline int	getWindowWidth() { return App::get()->getWindowWidth(); }
//! Sets the width of the active App's window measured in pixels. Ignored in full-screen mode.
inline void	setWindowWidth( int windowWidth ) { App::get()->setWindowWidth( windowWidth ); }
//! Returns the height of the active App's window measured in pixels, or the screen when in full-screen mode.
inline int	getWindowHeight() { return App::get()->getWindowHeight(); }
//! Sets the height of the active App's window measured in pixels. Ignored in full-screen mode.
inline void	setWindowHeight( int windowHeight ) { App::get()->setWindowHeight( windowHeight ); }
//! Sets the size of the active App's window. Ignored in full-screen mode.
inline void		setWindowSize( int windowWidth, int windowHeight ) { App::get()->setWindowSize( windowWidth, windowHeight ); }
//! Returns the center of the active App's window or the screen in full-screen mode.
/** Equivalent to \code Vec2f( getWindowWidth() * 0.5, getWindowHeight() * 0.5 ) \endcode **/
inline Vec2f	getWindowCenter() { return App::get()->getWindowCenter(); }
//! Returns the size of the active App's window or the screen in full-screen mode
inline Vec2i	getWindowSize() { return App::get()->getWindowSize(); }
//! Returns the aspect ratio of the active App's window or the screen in full-screen mode
inline float	getWindowAspectRatio() { return App::get()->getWindowAspectRatio(); }
//! Returns the bounding area of the active App's window or the screen in full-screen mode.
/** Equivalent to \code Area( 0, 0, getWindowWidth(), getWindowHeight() ); \endcode **/
inline Area		getWindowBounds() { return App::get()->getWindowBounds(); }
//! Returns the maximum frame-rate the active App will attempt to maintain.
inline float	getFrameRate() { return App::get()->getFrameRate(); }
//! Sets the maximum frame-rate the active App will attempt to maintain.
inline void		setFrameRate( float frameRate ) { App::get()->setFrameRate( frameRate ); }
//! Returns whether the active App is in full-screen mode or not.
inline bool		isFullScreen() { return App::get()->isFullScreen(); }
//! Sets whether the active App is in full-screen mode based on \a fullScreen
inline void		setFullScreen( bool fullScreen = true ) { App::get()->setFullScreen( fullScreen ); }

//! Returns the number seconds which have elapsed since the active App launched.
inline double	getElapsedSeconds() { return App::get()->getElapsedSeconds(); }
//! Returns the number of animation frames which have elapsed since the active App launched.
inline uint32_t	getElapsedFrames() { return App::get()->getElapsedFrames(); }

inline DataSourceRef			loadResource( const std::string &macPath, int mswID, const std::string &mswType ) { return App::get()->loadResource( macPath, mswID, mswType ); }
#if defined( CINDER_COCOA )
	inline DataSourcePathRef	loadResource( const std::string &macPath ) { return App::get()->loadResource( macPath ); }
	inline std::string			getResourcePath( const std::string &rsrcRelativePath ) { return App::get()->getResourcePath( rsrcRelativePath ); }
#else
	inline DataSourceBufferRef	loadResource( int mswID, const std::string &mswType ) { return App::get()->loadResource( mswID, mswType ); }
#endif

//! Returns the path to the active App on disk
inline std::string		getAppPath() { return App::get()->getAppPath(); }
//! Presents the user with a file-open dialog and returns the selected file path.
/** The dialog optionally begins at the path \a initialPath and can be limited to allow selection of files ending in the extensions enumerated in \a extensions.
	If the active app is in full-screen mode it will temporarily switch to windowed-mode to present the dialog.
	\return the selected file path or an empty string if the user cancelled. **/
inline std::string		getOpenFilePath( const std::string &initialPath = "", std::vector<std::string> extensions = std::vector<std::string>() ) { return App::get()->getOpenFilePath( initialPath, extensions ); }
//! Presents the user with a file-save dialog and returns the selected file path.
/** The dialog optionally begins at the path \a initialPath and can be limited to allow selection of files ending in the extensions enumerated in \a extensions.
	If the active app is in full-screen mode it will temporarily switch to windowed-mode to present the dialog.
	\return the selected file path or an empty string if the user cancelled. **/
inline std::string		getSaveFilePath( const std::string &initialPath = "", std::vector<std::string> extensions = std::vector<std::string>() ) { return App::get()->getSaveFilePath( initialPath, extensions ); }

//! Returns a reference to an output console, which is an alias to std::cout on the mac, and a wrapper around OutputDebugString on MSW
/** On Mac OS X all output is echoed either to the Debugger Console in XCode or the system console
	On Windows output is echoed using OutputDebugString, echoed to the Output window of the debugger or to a stream viewable with Dbgview
	\code console() << "This line will be echoed" << std::endl; \endcode **/
inline std::ostream&	console() { return App::get()->console(); }

//! Returns a copy of the window's contents as a Surface8u
inline Surface	copyWindowSurface() { return App::get()->copyWindowSurface(); }
//! Returns a copy of the Area \a area from the window's contents as a Surface8u
inline Surface	copyWindowSurface( const Area &area ) { return App::get()->copyWindowSurface( area ); }
//! Restores the current rendering context to be the App's window or the screen in full-screen mode. Generally this is only necessary if the app has display a dialog box or some other external window.
inline void		restoreWindowContext() { return App::get()->restoreWindowContext(); }

#if defined( CINDER_COCOA )
//! Returns a CGContextRef for drawing to the Window using CoreGraphics under Cocoa & Cocoa Touch. Assumes your App uses a Renderer2d
inline ::CGContextRef	createWindowCgContext() { return ((Renderer2d*)(App::get()->getRenderer()))->getCgContext(); }
#endif

//@}

} } // namespace cinder::app