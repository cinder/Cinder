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

#include "cinder/gl/gl.h" // necessary to give GLee the jump on Cocoa.h

#import <Cocoa/Cocoa.h>

#import "cinder/app/AppBasic.h"
#import "cinder/app/CinderView.h"
#import "cinder/Stream.h"
#import "cinder/Display.h"

#include "cinder/app/TouchEvent.h"

#include <list>

@class CinderWindow; // inherits from NSWindow
@class WindowImplBasicCocoa;

@interface AppImplCocoaBasic : NSObject <NSApplicationDelegate,NSWindowDelegate> {
  @public
	NSTimer							*mAnimationTimer;
	class cinder::app::AppBasic		*mApp;
	
	BOOL							mNeedsUpdate;
	BOOL							mFrameRateEnabled;
	float							mFrameRate;
	
	NSMutableArray					*mWindows;
	WindowImplBasicCocoa			*mActiveWindow;
}

@property(retain, nonatomic) NSMutableArray *windows;

- (id)init:(cinder::app::AppBasic*)aApp;
- (void)setApplicationMenu: (NSString*) applicationName;
- (void)startAnimationTimer;
- (void)applicationWillTerminate:(NSNotification *)notification;
- (void)applicationDidBecomeActive:(NSNotification *)notification;
- (void)applicationWillResignActive:(NSNotification *)notification;
- (void)quit;

- (cinder::app::WindowRef)createWindow:(cinder::app::Window::Format)format;

- (float)getFrameRate;
- (void)setFrameRate:(float)aFrameRate;
- (void)disableFrameRate;
- (bool)isFrameRateEnabled;
- (std::string)getAppPath;
- (void)quit;

- (cinder::app::RendererRef)findSharedRenderer:(cinder::app::RendererRef)match;
- (cinder::app::WindowRef)getWindow;
- (cinder::app::WindowRef)getForegroundWindow;
- (size_t)getNumWindows;
- (cinder::app::WindowRef)getWindowIndex:(size_t)index;
- (void)setActiveWindow:(WindowImplBasicCocoa*)win;
- (WindowImplBasicCocoa*)findWindowImpl:(NSWindow*)window;

@end

///////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface WindowImplBasicCocoa : NSObject<NSWindowDelegate,CinderViewDelegate,WindowImplCocoa> {
  @public
	AppImplCocoaBasic					*mAppImpl;
	NSWindow							*mWin;
	CinderView							*mCinderView;
	cinder::app::WindowRef				mWindowRef;
	cinder::DisplayRef					mDisplay;
	cinder::Vec2i						mSize, mPos;
	BOOL								mResizable, mBorderless, mAlwaysOnTop, mHidden;
}


// WindowImplCocoa methods
- (void)dealloc;
- (BOOL)isFullScreen;
- (void)setFullScreen:(BOOL)fullScreen options:(const cinder::app::FullScreenOptions *)options;
- (cinder::Vec2i)getSize;
- (void)setSize:(cinder::Vec2i)size;
- (cinder::Vec2i)getPos;
- (float)getContentScale;
- (void)setPos:(cinder::Vec2i)pos;
- (float)getContentScale;
- (void)close;
- (NSString *)getTitle;
- (void)setTitle:(NSString *)title;
- (BOOL)isBorderless;
- (void)setBorderless:(BOOL)borderless;
- (bool)isAlwaysOnTop;
- (void)setAlwaysOnTop:(bool)alwaysOnTop;
- (void)hide;
- (void)show;
- (BOOL)isHidden;
- (cinder::DisplayRef)getDisplay;
- (cinder::app::RendererRef)getRenderer;
- (void*)getNative;

- (void)windowMovedNotification:(NSNotification*)inNotification;
- (void)windowWillCloseNotification:(NSNotification*)inNotification;

// CinderViewDelegate methods
- (void)resize;
- (void)draw;
- (void)mouseDown:(cinder::app::MouseEvent*)event;
- (void)mouseDrag:(cinder::app::MouseEvent*)event;
- (void)mouseUp:(cinder::app::MouseEvent*)event;
- (void)mouseMove:(cinder::app::MouseEvent*)event;
- (void)mouseWheel:(cinder::app::MouseEvent*)event;
- (void)keyDown:(cinder::app::KeyEvent*)event;
- (void)keyUp:(cinder::app::KeyEvent*)event;
- (void)touchesBegan:(cinder::app::TouchEvent*)event;
- (void)touchesMoved:(cinder::app::TouchEvent*)event;
- (void)touchesEnded:(cinder::app::TouchEvent*)event;
- (const std::vector<cinder::app::TouchEvent::Touch>&)getActiveTouches;
- (void)fileDrop:(cinder::app::FileDropEvent*)event;
- (cinder::app::WindowRef)getWindowRef;

+ (WindowImplBasicCocoa*)instantiate:(cinder::app::Window::Format)winFormat withAppImpl:(AppImplCocoaBasic*)appImpl withRetina:(BOOL)retinaEnabled;

@end
