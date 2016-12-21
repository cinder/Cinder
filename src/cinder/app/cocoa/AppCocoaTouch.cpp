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

#include "cinder/app/cocoa/AppCocoaTouch.h"
#include "cinder/app/cocoa/AppImplCocoaTouch.h"
#include "cinder/Log.h"

namespace cinder { namespace app {

AppCocoaTouch*	AppCocoaTouch::sInstance = nullptr;

AppCocoaTouch::Settings::Settings()
	: AppBase::Settings(), mStatusBarEnabled( false )
{
	mPowerManagementEnabled = true;
	mHighDensityDisplayEnabled = true;
	mMultiTouchEnabled = true;
}

AppCocoaTouch::AppCocoaTouch()
	: AppBase()
{
	AppCocoaTouch::sInstance = this;

	auto settings = dynamic_cast<Settings *>( sSettingsFromMain );
	CI_ASSERT( settings );

	mImpl = [[AppImplCocoaTouch alloc] init:this settings:*settings];

	enablePowerManagement( settings->isPowerManagementEnabled() );
}

AppCocoaTouch::~AppCocoaTouch()
{
	[mImpl release];
	mImpl = nil;
}

void AppCocoaTouch::launch()
{
	const auto &args = getCommandLineArgs();
	int argc = (int)args.size();

	char* argv[argc];
	for( int i = 0; i < argc; i++ )
		argv[i] = const_cast<char *>( args[i].c_str() );

	::UIApplicationMain( argc, argv, nil, ::NSStringFromClass( [AppDelegateImpl class] ) );
}

WindowRef AppCocoaTouch::createWindow( const Window::Format &format )
{
	return [mImpl createWindow:format];
}

WindowRef AppCocoaTouch::getWindow() const
{
	if( ! mImpl->mActiveWindow )
		throw cinder::app::ExcInvalidWindow();
	else
		return mImpl->mActiveWindow->mWindowRef;
}

size_t AppCocoaTouch::getNumWindows() const
{
	return mImpl->mWindows.size();
}

WindowRef AppCocoaTouch::getWindowIndex( size_t index ) const
{
	if( index >= getNumWindows() )
		throw cinder::app::ExcInvalidWindow();

	auto iter = mImpl->mWindows.begin();
	std::advance( iter, index );
	return (*iter)->mWindowRef;
}

InterfaceOrientation AppCocoaTouch::getOrientation() const
{
	UIInterfaceOrientation orientation = [[mImpl getDeviceWindow] interfaceOrientation];
	return [mImpl convertInterfaceOrientation:orientation];
}

InterfaceOrientation AppCocoaTouch::getWindowOrientation() const
{
	UIInterfaceOrientation orientation = [mImpl->mActiveWindow interfaceOrientation];
	return [mImpl convertInterfaceOrientation:orientation];
}

void AppCocoaTouch::enableProximitySensor()
{
	[UIDevice currentDevice].proximityMonitoringEnabled = YES;
	[[NSNotificationCenter defaultCenter] addObserver:mImpl selector:@selector(proximityStateChange:)
		name:UIDeviceProximityStateDidChangeNotification object:nil];
}

void AppCocoaTouch::disableProximitySensor()
{
	[UIDevice currentDevice].proximityMonitoringEnabled = NO;
}

bool AppCocoaTouch::proximityIsClose() const
{
	return mImpl->mProximityStateIsClose;
}

void AppCocoaTouch::enableBatteryMonitoring()
{
	[UIDevice currentDevice].batteryMonitoringEnabled = YES;
	mImpl->mBatteryLevel = [UIDevice currentDevice].batteryLevel;
	mImpl->mIsUnplugged = [UIDevice currentDevice].batteryState == UIDeviceBatteryStateUnplugged;
	[[NSNotificationCenter defaultCenter] addObserver:mImpl selector:@selector(batteryLevelChange:) 
		name:UIDeviceBatteryLevelDidChangeNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:mImpl selector:@selector(batteryStateChange:) 
		name:UIDeviceBatteryStateDidChangeNotification object:nil];
}

void AppCocoaTouch::disableBatteryMonitoring()
{
	[UIDevice currentDevice].batteryMonitoringEnabled = NO;
}

float AppCocoaTouch::getBatteryLevel() const
{
	return mImpl->mBatteryLevel;
}

bool AppCocoaTouch::isUnplugged() const
{
	return mImpl->mIsUnplugged;
}

void AppCocoaTouch::showKeyboard( const KeyboardOptions &options )
{
	[mImpl showKeyboard:options];
}

bool AppCocoaTouch::isKeyboardVisible() const
{
	return [mImpl isKeyboardVisible];
}

void AppCocoaTouch::hideKeyboard()
{
	[mImpl hideKeyboard];
}

std::string	AppCocoaTouch::getKeyboardString() const
{
	return [mImpl getKeyboardString];
}

void AppCocoaTouch::setKeyboardString( const std::string &keyboardString )
{
	[mImpl setKeyboardString:keyboardString];
}

::UITextView* AppCocoaTouch::getKeyboardTextView() const
{
	return [mImpl getKeyboardTextView];
}

void AppCocoaTouch::showStatusBar( AppCocoaTouch::StatusBarAnimation animation )
{
	if( animation == StatusBarAnimation::FADE )
		[mImpl showStatusBar:UIStatusBarAnimationFade];
	else if( animation == StatusBarAnimation::SLIDE )
		[mImpl showStatusBar:UIStatusBarAnimationSlide];
	else
		[mImpl showStatusBar:UIStatusBarAnimationNone];
}

bool AppCocoaTouch::isStatusBarVisible() const
{
	return [UIApplication sharedApplication].statusBarHidden == NO;
}

void AppCocoaTouch::hideStatusBar( AppCocoaTouch::StatusBarAnimation animation )
{
	if( animation == StatusBarAnimation::FADE )
		[mImpl hideStatusBar:UIStatusBarAnimationFade];
	else if( animation == StatusBarAnimation::SLIDE )
		[mImpl hideStatusBar:UIStatusBarAnimationSlide];
	else
		[mImpl hideStatusBar:UIStatusBarAnimationNone];
}

//! Returns the maximum frame-rate the App will attempt to maintain.
float AppCocoaTouch::getFrameRate() const
{
	return 60.0f / mImpl->mAnimationFrameInterval;
}

//! Sets the maximum frame-rate the App will attempt to maintain.
void AppCocoaTouch::setFrameRate( float frameRate )
{
	[mImpl setFrameRate:frameRate];
}

bool AppCocoaTouch::isFullScreen() const
{
	return true;
}

void AppCocoaTouch::setFullScreen( bool fullScreen, const FullScreenOptions &options )
{
	// NO-OP
}

void AppCocoaTouch::quit()
{
	CI_LOG_W( "quit() has no effect on iOS" );
	return; // no effect on iOS
}

void AppCocoaTouch::enablePowerManagement( bool powerManagement )
{
	mPowerManagement = powerManagement;
	[mImpl updatePowerManagement];
}

void AppCocoaTouch::emitDidEnterBackground()
{
	mSignalDidEnterBackground.emit();
}

void AppCocoaTouch::emitWillEnterForeground()
{
	mSignalWillEnterForeground.emit();
}

void AppCocoaTouch::emitMemoryWarning()
{
	mSignalMemoryWarning.emit();
}

uint32_t AppCocoaTouch::emitSupportedOrientations()
{
	return mSignalSupportedOrientations.emit();
}

void AppCocoaTouch::emitWillRotate()
{
	mSignalWillRotate.emit();
}

void AppCocoaTouch::emitDidRotate()
{
	mSignalDidRotate.emit();
}

void AppCocoaTouch::emitKeyboardWillShow()
{
	mSignalKeyboardWillShow.emit();
}

void AppCocoaTouch::emitKeyboardWillHide()
{
	mSignalKeyboardWillHide.emit();
}

void AppCocoaTouch::emitSignalProximitySensor( bool isClose )
{
	mSignalProximitySensor.emit( isClose );
}

void AppCocoaTouch::emitSignalBatteryState( bool isUnplugged )
{
	mSignalBatteryState.emit( isUnplugged );
}

std::ostream& operator<<( std::ostream &lhs, const InterfaceOrientation &rhs )
{
	switch( rhs ) {
		case InterfaceOrientation::Portrait:			lhs << "Portrait";				break;
		case InterfaceOrientation::PortraitUpsideDown:	lhs << "PortraitUpsideDown";	break;
		case InterfaceOrientation::LandscapeLeft:		lhs << "LandscapeLeft";			break;
		case InterfaceOrientation::LandscapeRight:		lhs << "LandscapeRight";		break;
		default: lhs << "Error";
	}
	return lhs;
}

float getOrientationDegrees( InterfaceOrientation orientation )
{
	switch( orientation ) {
		case InterfaceOrientation::Portrait:			return 0.0f;
		case InterfaceOrientation::PortraitUpsideDown:	return 180.0f;
		case InterfaceOrientation::LandscapeLeft:		return 90.0f;
		case InterfaceOrientation::LandscapeRight:		return 270.0f;
		default: return 0.0f;
	}
}

} } // namespace cinder::app
