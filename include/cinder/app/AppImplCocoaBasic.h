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

#include "cinder/gl/gl.h" // necessary to give GLee the jump on Cocoa.h

#import <Cocoa/Cocoa.h>

#import "cinder/app/AppBasic.h"
#import "cinder/app/CinderView.h"
#import "cinder/Stream.h"
#import "cinder/Display.h"

#include "cinder/app/TouchEvent.h"

#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_5
@interface AppImplCocoaBasic : NSApplication<NSWindowDelegate,CinderViewMultiTouchDelegate> {
#else
@interface AppImplCocoaBasic : NSApplication {
#endif
	NSWindow				*win;
	CinderView				*cinderView;
	
	NSTimer					*animationTimer;
	class cinder::app::AppBasic	*app;
	
	BOOL					mNeedsUpdate;
	
	cinder::Display			*mDisplay;
	int						mWindowWidth, mWindowHeight;
	BOOL					mFullScreen;
	float					mFrameRate;
}

- (id)init:(cinder::app::AppBasic*)aApp;
- (void)setApplicationMenu: (NSString*) applicationName;
- (void)startAnimationTimer;
- (void)applicationWillTerminate:(NSNotification *)notification;
- (void)quit;

- (void)createWindow;

- (int)getWindowWidth;
- (void)setWindowWidth:(int)windowWidth;
- (int)getWindowHeight;
- (void)setWindowHeight:(int)windowHeight;
- (void)setWindowSizeWithWidth:(int)w height:(int)h;
- (void)handleResizeWithWidth:(int)w height:(int)h;
- (float)getFrameRate;
- (void)setFrameRate:(float)aFrameRate;
- (bool)isFullScreen;
- (void)enterFullScreen;
- (void)exitFullScreen;
- (std::string)getAppPath;
- (CGPoint)mouseLocation;
- (void)quit;
- (cinder::Display*)getDisplay;
- (void)setDisplay:(cinder::Display*)aDisplay;

#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_5
// multiTouch delegate methods
- (void)touchesBegan:(ci::app::TouchEvent*)event;
- (void)touchesMoved:(ci::app::TouchEvent*)event;
- (void)touchesEnded:(ci::app::TouchEvent*)event;
- (void)touchesEnded:(ci::app::TouchEvent*)event;
- (void)setActiveTouches:(std::vector<ci::app::TouchEvent::Touch>*)touches;
#endif // MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_5

@end
