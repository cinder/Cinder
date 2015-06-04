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

#include "cinder/app/cocoa/AppCocoaTouch.h"
#include "cinder/app/cocoa/CinderViewCocoaTouch.h"
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
	cinder::app::AppCocoaTouch*			mApp;
	std::list<WindowImplCocoaTouch*>	mWindows;
	WindowImplCocoaTouch*				mActiveWindow;
	CADisplayLink*						mDisplayLink;

	bool								mSetupHasFired;
	bool								mUpdateHasFired;
	bool								mAnimating;
	NSInteger 							mAnimationFrameInterval;
	bool								mProximityStateIsClose;
	bool								mIsUnplugged;
	bool								mStatusBarShouldHide;
	float								mBatteryLevel;

	std::string							mKeyboardString;		// copy is kept so changes are visible from textShouldChange callback
}

- (AppImplCocoaTouch *)init:(cinder::app::AppCocoaTouch *)app settings:(const cinder::app::AppCocoaTouch::Settings &)settings;
- (cinder::app::RendererRef)findSharedRenderer:(cinder::app::RendererRef)match;
- (WindowImplCocoaTouch *)getDeviceWindow;
- (cinder::app::WindowRef)createWindow:(cinder::app::Window::Format)format;
- (void)setActiveWindow:(WindowImplCocoaTouch *)win;
- (void)updatePowerManagement;
- (void)setFrameRate:(float)frameRate;
- (void)showKeyboard:(const cinder::app::AppCocoaTouch::KeyboardOptions &)options;
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
- (cinder::app::InterfaceOrientation)convertInterfaceOrientation:(UIInterfaceOrientation)orientation;

@end // AppImplCocoaTouch


@interface WindowImplCocoaTouch : UIViewController<WindowImplCocoa, CinderViewCocoaTouchDelegate, UIKeyInput, UITextViewDelegate> {
  @public
	AppImplCocoaTouch*			mAppImpl;
	UIWindow*					mUiWindow;
	UIViewController*			mRootViewController;
	CinderViewCocoaTouch*		mCinderView;
	UITextView*					mKeyboardTextView;

	cinder::app::WindowRef		mWindowRef;
	cinder::DisplayRef			mDisplay;

	cinder::ivec2				mSize, mPos;
	bool						mResizeHasFired;
	bool						mHidden;
	bool						mKeyboardVisible;
	bool						mKeyboardClosesOnReturn;
}

- (void)loadView;
- (WindowImplCocoaTouch *)initWithFormat:(const cinder::app::Window::Format &)format withAppImpl:(AppImplCocoaTouch *)appImpl sharedRenderer:(cinder::app::RendererRef)sharedRenderer;
- (void)finishLoad;

// virtual keyboard management
- (void)showKeyboard:(const cinder::app::AppCocoaTouch::KeyboardOptions &)options;
- (void)hideKeyboard;
- (bool)isKeyboardVisible;
- (void)setKeyboardString:(const std::string *)keyboardString;
- (void)keyboardWillShow:(NSNotification *)notification;
- (void)keyboardWillHide:(NSNotification *)notification;

// UIKeyInput Protocol Methods
- (bool)canBecomeFirstResponder;
- (void)insertText:(NSString *)text;
- (void)deleteBackward;

// WindowImplCocoa Protocol Methods
- (bool)isFullScreen;
- (void)setFullScreen:(bool)fullScreen options:(cinder::app::FullScreenOptions *)options;
- (cinder::ivec2)getSize;
- (void)setSize:(cinder::ivec2)size;
- (cinder::ivec2)getPos;
- (void)setPos:(cinder::ivec2)pos;
- (float)getContentScale;
- (void)close;
- (NSString *)getTitle;
- (bool)isBorderless;
- (void)setBorderless:(bool)borderless;
- (bool)isAlwaysOnTop;
- (void)setAlwaysOnTop:(bool)alwaysOnTop;
- (cinder::DisplayRef)getDisplay;
- (cinder::app::RendererRef)getRenderer;
- (void *)getNative;
- (const std::vector<cinder::app::TouchEvent::Touch> &)getActiveTouches;

// CinderViewCocoaTouchDelegate Protocol methods
- (void)draw;
- (void)mouseDown:(cinder::app::MouseEvent *)event;
- (void)mouseDrag:(cinder::app::MouseEvent *)event;
- (void)mouseUp:(cinder::app::MouseEvent *)event;
- (void)keyDown:(cinder::app::KeyEvent *)event;

@property (nonatomic, readonly) UITextView *keyboardTextView; // lazy-loaded in getter

@end // WindowImplCocoaTouch
