/*
 Copyright (c) 2015, The Barbarian Group
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

#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/CinderViewCocoaTouch.h"
#include "cinder/cocoa/CinderCocoaTouch.h"
#include "cinder/Log.h"

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#include <list>

@class AppImplCocoaTouch;
@class WindowImplCocoaTouch;

//! We use a separate object for the app delegate, so that AppImplCocoaTouch can be
//! completely initialized before UIApplicationMain is called.
@interface AppDelegateImpl : NSObject <UIApplicationDelegate> {
  @public
	cinder::app::AppCocoaTouch*		mApp;
	AppImplCocoaTouch*				mAppImpl;
}

@end // AppDelegateImpl


@interface AppImplCocoaTouch : NSObject {
  @public
	ci::app::AppCocoaTouch*				mApp;
	std::list<WindowImplCocoaTouch*>	mWindows;
	WindowImplCocoaTouch*				mActiveWindow;
	CADisplayLink*						mDisplayLink;

	BOOL								mSetupHasFired;
	BOOL								mUpdateHasFired;
	BOOL								mAnimating;
	NSInteger 							mAnimationFrameInterval;
	BOOL								mProximityStateIsClose;
	BOOL								mIsUnplugged;
	float								mBatteryLevel;

	std::string							mKeyboardString;		// copy is kept so changes are visible from textShouldChange callback
}

- (AppImplCocoaTouch *)init:(cinder::app::AppCocoaTouch *)app settings:(const cinder::app::AppCocoaTouch::Settings &)settings;
- (ci::app::RendererRef)findSharedRenderer:(ci::app::RendererRef)match;
- (WindowImplCocoaTouch *)getDeviceWindow;
- (ci::app::WindowRef)createWindow:(ci::app::Window::Format)format;
- (void)setActiveWindow:(WindowImplCocoaTouch *)win;
- (void)updatePowerManagement;
- (void)setFrameRate:(float)frameRate;
- (void)showKeyboard:(const ci::app::AppCocoaTouch::KeyboardOptions &)options;
- (bool)isKeyboardVisible;
- (void)hideKeyboard;
- (const std::string &)getKeyboardString;
- (void)setKeyboardString:(const std::string &)keyboardString;
- (UITextView *)getKeyboardTextView;
- (void)showStatusBar:(UIStatusBarAnimation)anim;
- (void)hideStatusBar:(UIStatusBarAnimation)anim;
- (void)displayLinkDraw:(id)sender;
- (void)proximityStateChange:(NSNotificationCenter *)notification;
- (void)batteryStateChange:(NSNotificationCenter *)notification;
- (void)batteryLevelChange:(NSNotificationCenter *)notification;
- (void)startAnimation;
- (void)stopAnimation;
- (ci::app::InterfaceOrientation)convertInterfaceOrientation:(UIInterfaceOrientation)orientation;

@end // AppImplCocoaTouch


@interface WindowImplCocoaTouch : UIViewController<WindowImplCocoa, CinderViewCocoaTouchDelegate, UIKeyInput, UITextViewDelegate> {
  @public
	AppImplCocoaTouch*			mAppImpl;
	UIWindow*					mUiWindow;
	UIViewController*			mRootViewController;
	CinderViewCocoaTouch*		mCinderView;
	UITextView*					mKeyboardTextView;

	ci::app::WindowRef			mWindowRef;
	ci::DisplayRef				mDisplay;

	ci::ivec2					mSize, mPos;
	BOOL						mResizeHasFired;
	BOOL						mHidden;
	BOOL						mKeyboardVisible;
	BOOL						mKeyboardClosesOnReturn;
}

- (void)loadView;
- (WindowImplCocoaTouch *)initWithFormat:(const ci::app::Window::Format &)format withAppImpl:(AppImplCocoaTouch *)appImpl sharedRenderer:(ci::app::RendererRef)sharedRenderer;
- (void)finishLoad;

// virtual keyboard management
- (void)showKeyboard:(const ci::app::AppCocoaTouch::KeyboardOptions &)options;
- (void)hideKeyboard;
- (bool)isKeyboardVisible;
- (void)setKeyboardString:(const std::string *)keyboardString;
- (void)keyboardWillShow:(NSNotification *)notification;
- (void)keyboardWillHide:(NSNotification *)notification;

// UIKeyInput Protocol Methods
- (BOOL)canBecomeFirstResponder;
- (void)insertText:(NSString *)text;
- (void)deleteBackward;

// WindowImplCocoa Protocol Methods
- (BOOL)isFullScreen;
- (void)setFullScreen:(BOOL)fullScreen options:(ci::app::FullScreenOptions *)options;
- (ci::ivec2)getSize;
- (void)setSize:(ci::ivec2)size;
- (ci::ivec2)getPos;
- (void)setPos:(ci::ivec2)pos;
- (float)getContentScale;
- (void)close;
- (NSString *)getTitle;
- (BOOL)isBorderless;
- (void)setBorderless:(BOOL)borderless;
- (BOOL)isAlwaysOnTop;
- (void)setAlwaysOnTop:(BOOL)alwaysOnTop;
- (ci::DisplayRef)getDisplay;
- (ci::app::RendererRef)getRenderer;
- (void *)getNative;
- (const std::vector<ci::app::TouchEvent::Touch> &)getActiveTouches;

// CinderViewCocoaTouchDelegate Protocol methods
- (void)draw;
- (void)mouseDown:(ci::app::MouseEvent *)event;
- (void)mouseDrag:(ci::app::MouseEvent *)event;
- (void)mouseUp:(ci::app::MouseEvent *)event;
- (void)keyDown:(ci::app::KeyEvent *)event;

@property (nonatomic, readonly) UITextView *keyboardTextView; // lazy-loaded in getter

@end // WindowImplCocoaTouch
