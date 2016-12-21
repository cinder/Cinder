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

#include "cinder/app/AppBase.h"
#include "cinder/cocoa/CinderCocoaTouch.h"
#include "cinder/app/Window.h"
#include "cinder/app/TouchEvent.h"

#ifdef __OBJC__
	@class AppImplCocoaTouch;
	@class UITextView;
#else
	class AppImplCocoaTouch;
	class UITextView;
#endif

namespace cinder { namespace app {

struct AppCocoaTouchState;

class AppCocoaTouch : public AppBase {
  public:
	class Settings : public AppBase::Settings {
	  public:
		//! Customizes some AppBase::Settings, enabling power management, high density display, and multitouch by default.
		Settings();

		//! Determines whether the system status bar is visible initially. Default is \c false.
		void		setStatusBarEnabled( bool enable = true ) { mStatusBarEnabled = enable; }
		//! Returns whether the system status bar is visible initially. Default is \c false.
		bool		isStatusBarEnabled() const { return mStatusBarEnabled; }
		
	  private:
		bool		mStatusBarEnabled;
	};

	typedef std::function<void ( Settings *settings )>	SettingsFn;

	AppCocoaTouch();
	virtual ~AppCocoaTouch();

	signals::Signal<void()>&	getSignalDidEnterBackground() { return mSignalDidEnterBackground; }
	void						emitDidEnterBackground();
	signals::Signal<void()>&	getSignalWillEnterForeground() { return mSignalWillEnterForeground; }
	void						emitWillEnterForeground();
	signals::Signal<void()>&	getSignalMemoryWarning() { return mSignalMemoryWarning; }
	void 						emitMemoryWarning();

	//! Returns the signal emitted when an orientation change may occur, allowing the user to specify which orientations are permitted (any connection can forbid a given orientation).  The connected std::function must return an \t InterfaceOrientation bitmask. 
	EventSignalSupportedOrientations&	getSignalSupportedOrientations() { return mSignalSupportedOrientations; }
	//! Emits a signal to ask the user what orientations are supported.
	uint32_t							emitSupportedOrientations();
	//! Returns the signal emitted when the interface is about to rotate to a new orientation. At this time, the Window's bounds and orientation have already been updated.
	signals::Signal<void()>&			getSignalWillRotate() { return mSignalWillRotate; }
	//! Emits the signal to notify the user that the orientation will change.
	void								emitWillRotate();
	//! Returns the signal emitted when the interface is finished rotating to a new orientation.
	signals::Signal<void()>&			getSignalDidRotate() { return mSignalDidRotate; }
	//! Emits the signal to notify the user that the orientation did change.
	void								emitDidRotate();
	//! Returns the signal emitted when the virtual keyboard is about to animate on screen.
	signals::Signal<void()>&			getSignalKeyboardWillShow() { return mSignalKeyboardWillShow; }
	//! Emits the signal used to notify when the virtual keyboard is about to animate on screen.
	void								emitKeyboardWillShow();
	//! Returns the signal emitted when the virtual keyboard is about to animate off screen.
	signals::Signal<void()>&			getSignalKeyboardWillHide() { return mSignalKeyboardWillHide; }
	//! Emits the signal used to notify when the virtual keyboard is about to animate off screen.
	void								emitKeyboardWillHide();

	WindowRef 		createWindow( const Window::Format &format = Window::Format() ) override;

	WindowRef		getWindow() const override;
	WindowRef		getForegroundWindow() const override { return getWindow(); }
	size_t			getNumWindows() const override;
	app::WindowRef	getWindowIndex( size_t index = 0 ) const override;

	//! Returns the  \t InterfaceOrientation for the device \t Window.
	InterfaceOrientation	getOrientation() const;
	//! Returns the current \t InterfaceOrientation for the active \t Window.
	InterfaceOrientation	getWindowOrientation() const;

	//! no-op, no cursor on this platform
	void	hideCursor() override {}
	//! no-op, no cursor on this platform
	void	showCursor() override {}
	//! no-op, cannot disable framerate on this platform
	void	disableFrameRate() override {}
	//! always false, cannot disable framerate on this platform
	bool	isFrameRateEnabled() const override { return false; }

	//! Enables the device's proximity sensor, which can return whether the device is close to the user or not. Use in conjunction with proximityIsClose() or getSignalProximitySensor()
	void enableProximitySensor();
	//! Disables the device's proximity sensor.
	void disableProximitySensor();
	//! Returns whether the device is close to the user or not. Must call \a enableProximitySensor() before using.
	bool proximityIsClose() const;
	//! Signal emitted when the device becomes close (\c true) or far (\c false). Must call \a enableProximitySensor() before using.
	signals::Signal<void( bool )>&	getSignalProximitySensor()	{ return mSignalProximitySensor; }
	void							emitSignalProximitySensor( bool isClose );

	//! Enables the device's battery monitor. Use in conjunction with \a getBatteryLevel(), \a isUnplugged() and \a getSignalBatteryState()
	void 	enableBatteryMonitoring();
	//! Disables the device's battery monitor.
	void 	disableBatteryMonitoring();
	//! Returns the device's battery level, in the range of [0,1];
	float 	getBatteryLevel() const;
	//! Returns \c true when the device is not plugged into power
	bool 	isUnplugged() const;
	//! Signal emitted when the device is unplugged (\c true) or plugged in (\c false). Use in place of polling \a isUnplugged().
	signals::Signal<void( bool )>&	getSignalBatteryState()	{ return mSignalBatteryState; }
	void							emitSignalBatteryState( bool isUnplugged );

	//! When disabled, the device will not sleep even after the idling threshold.
	void enablePowerManagement( bool powerManagement = true ) override;

	enum KeyboardType { DEFAULT, NUMERICAL, URL };

	//! Optional params passed to showKeyboard();
	struct KeyboardOptions {
		KeyboardOptions() : mType( KeyboardType::DEFAULT ), mCloseOnReturn( true ) {}

		//! Sets the initial value of the keyboard string. Default is an empty string.
		KeyboardOptions& initialString( const std::string &str )	{ mInitialString = str; return *this; }
		//! Sets the keyboard layout type to \type. Default is \a KeyboardType::DEFAULT.
		KeyboardOptions& type( KeyboardType type )					{ mType = type; return *this; }
		//! Sets whether the keyboard closes (hides) when the return key is entered. Default is \c true. \note if false, a carriage return (\r) will be appended to the keyboard string. In either case, a KeyEvent will still fire.
		KeyboardOptions& closeOnReturn( bool enable )				{ mCloseOnReturn = enable; return *this; }

		//! Returns the keyboard's initial string.
		const std::string&	getInitialString() const				{ return mInitialString; }
		//! Returns the keyboard layout type.
		KeyboardType		getType() const							{ return mType; }
		//! Returns whether the keyboard closes (hides) when the return key is entered.
		bool				getCloseOnReturn() const				{ return mCloseOnReturn; }

	  private:
		std::string		mInitialString;
		KeyboardType	mType;
		bool			mCloseOnReturn;
	};

	//! Shows the iOS virtual keyboard with KeyboardOptions \a options. This method can be called with new options while the keyboard is visible.
	void 		showKeyboard( const KeyboardOptions &options = KeyboardOptions() );
	//! Returns whether the iOS keyboard is visible
	bool		isKeyboardVisible() const;
	//! Hides the default iOS keyboard
	void		hideKeyboard();
	//! Returns the current text recorded since the most recent call to \a showKeyboard().
	std::string	getKeyboardString() const;
	//! Sets the current text string that the keyboard is manipulating, overwriting any previous content.
	void setKeyboardString( const std::string &keyboardString );
	//! Retuens a pointer to the native UITextView, which can be used to customize the virtual keyboard.
	::UITextView	*getKeyboardTextView() const;

	typedef enum StatusBarAnimation { NONE, FADE, SLIDE } StatusBarAnimation;
	//! Shows the system status bar
	void 	showStatusBar( StatusBarAnimation animation = StatusBarAnimation::NONE );
	//! Returns whether the system status bar is visible
	bool	isStatusBarVisible() const;
	//! Hides the system status bar
	void	hideStatusBar( StatusBarAnimation animation = StatusBarAnimation::NONE );
	
	float	getFrameRate() const override;
	void	setFrameRate( float frameRate ) override;
	//! Overidden to always return \a true on mobile
	bool	isFullScreen() const override;
	//! Overidden to disable, mobile is always full screen.
	void	setFullScreen( bool fullScreen, const FullScreenOptions &options = FullScreenOptions() ) override;

	//! Returns an invalid position since the device doesn't have a mouse.
	ivec2 getMousePos() const override	{ return ivec2( -1 ); }

	//! No-op on iOS
	void quit() override;

	//! Returns a pointer to the current global AppCocoaTouch
	static AppCocoaTouch*	get() { return sInstance; }

	// DO NOT CALL - should be private but aren't for esoteric reasons
	//! \cond
	// Internal handlers - these are called into by AppImpl's. If you are calling one of these, you have likely strayed far off the path.
	AppImplCocoaTouch* privateGetImpl()	{ return mImpl; }

	// Called during application instanciation via CINDER_APP_COCOA_TOUCH macro
	template<typename AppT>
	static void main( const RendererRef &defaultRenderer, const char *title, int argc, char * const argv[], const SettingsFn &settingsFn = SettingsFn() );
	//! \endcond

  protected:
	void	launch() override;

  private:
	friend void		setupCocoaTouchWindow( AppCocoaTouch *app );
		
	static AppCocoaTouch	*sInstance;
	AppImplCocoaTouch		*mImpl;

	signals::Signal<void()>		mSignalDidEnterBackground, mSignalWillEnterForeground, mSignalMemoryWarning;

	signals::Signal<void(bool)>		mSignalProximitySensor, mSignalBatteryState;

	EventSignalSupportedOrientations		mSignalSupportedOrientations;
	signals::Signal<void()>					mSignalWillRotate, mSignalDidRotate;
	signals::Signal<void(void)>				mSignalKeyboardWillShow, mSignalKeyboardWillHide;
};

//! Stream \t InterfacefaceOrientation enum to std::ostream
extern	std::ostream& operator<<( std::ostream &lhs, const InterfaceOrientation &rhs );
//! returns the degrees rotation from Portrait for the provided \a orientation
float	getOrientationDegrees( InterfaceOrientation orientation );

template<typename AppT>
void AppCocoaTouch::main( const RendererRef &defaultRenderer, const char *title, int argc, char * const argv[], const SettingsFn &settingsFn )
{
	AppBase::prepareLaunch();

	Settings settings;
	AppBase::initialize( &settings, defaultRenderer, title, argc, argv );

	if( settingsFn )
		settingsFn( &settings );

	if( settings.getShouldQuit() )
		return;

	AppCocoaTouch *app = static_cast<AppCocoaTouch *>( new AppT );
	app->executeLaunch();

	AppBase::cleanupLaunch();
}

#define CINDER_APP_COCOA_TOUCH( APP, RENDERER, ... )									\
int main( int argc, char * const argv[] )												\
{																						\
	cinder::app::RendererRef renderer( new RENDERER );									\
	cinder::app::AppCocoaTouch::main<APP>( renderer, #APP, argc, argv, ##__VA_ARGS__ );	\
	return 0;																			\
}

} } // namespace cinder::app
