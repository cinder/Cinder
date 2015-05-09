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

#include "cinder/gl/platform.h" // necessary to give glload the jump on Cocoa.h

#import <Cocoa/Cocoa.h>
#import <ScreenSaver/ScreenSaver.h>

#include "cinder/app/AppScreenSaver.h"
#import "cinder/app/cocoa/CinderViewMac.h"
#import "cinder/app/Window.h"

#include <string>

@class WindowImplCocoaScreenSaver;

@interface AppImplMacScreenSaver : NSObject {
  @public
	cinder::app::AppScreenSaver					*mApp;
	std::list<WindowImplCocoaScreenSaver*>		mWindows;
	WindowImplCocoaScreenSaver					*mActiveWindow;
	float										mFrameRate;

	BOOL										mSetupCalled;
}

- (AppImplMacScreenSaver*)init;
- (void)addWindow:(WindowImplCocoaScreenSaver*)windowImpl;
- (cinder::app::RendererRef)findSharedRenderer:(cinder::app::RendererRef)sharedRenderer;
- (BOOL)isPreview;

- (size_t)getNumWindows;
- (cinder::app::WindowRef)getWindowIndex:(size_t)index;
- (cinder::app::WindowRef)getWindow;
- (void)setActiveWindowToMain;
- (void)setActiveWindow:(WindowImplCocoaScreenSaver*)activeWindow;

- (void)animateOneFrame:(WindowImplCocoaScreenSaver*)callee;

- (float)getFrameRate;
- (void)setFrameRate:(float)frameRate;
- (cinder::fs::path)getAppPath;
- (void)removeCinderView:(WindowImplCocoaScreenSaver*)win;
- (void)finalCleanup;

@end

@interface WindowImplCocoaScreenSaver : ScreenSaverView<WindowImplCocoa,CinderViewDelegate> {
  @public
	CinderViewMac						*mCinderView;
	cinder::app::WindowRef				mWindowRef;
	cinder::DisplayRef					mDisplay;

	BOOL								mPreview;	
	BOOL								mResizeCalled;
	BOOL								mIsMainView;
	BOOL								mHasDrawnSinceLastUpdate;
}

// ScreenSaverView methods
- (void)drawRect:(NSRect)rect;
- (void)instantiateView:(NSRect)rect;

// WindowImplCocoa methods
- (BOOL)isFullScreen;
- (void)setFullScreen:(BOOL)fullScreen options:(const cinder::app::FullScreenOptions *)options;
- (cinder::ivec2)getSize;
- (void)setSize:(cinder::ivec2)size;
- (cinder::ivec2)getPos;
- (void)setPos:(cinder::ivec2)pos;
- (float)getContentScale;
- (void)close;
- (std::string)getTitle;
- (void)setTitle:(std::string)title;
- (BOOL)isBorderless;
- (void)setBorderless:(BOOL)borderless;
- (BOOL)isAlwaysOnTop;
- (void)setAlwaysOnTop:(BOOL)alwaysOnTop;
- (cinder::DisplayRef)getDisplay;
- (cinder::app::RendererRef)getRenderer;
- (const std::vector<cinder::app::TouchEvent::Touch>&)getActiveTouches;
- (void*)getNative;
- (void)hide;
- (void)show;

// CinderViewDelegate methods
- (void)draw;
- (cinder::app::WindowRef)getWindowRef;

@end
