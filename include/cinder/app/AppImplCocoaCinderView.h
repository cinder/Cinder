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

#import "cinder/app/AppCinderView.h"
#import "cinder/app/AppImplCocoaRenderer.h"
#import "cinder/app/CinderView.h"
#import "cinder/Stream.h"

@interface AppImplCocoaCinderView : NSObject {
	CinderView				*cinderView;
	
	NSTimer							*animationTimer;
	class cinder::app::AppCinderView	*app;
	
	BOOL					isCurrentlyFullScreen;
	CFAbsoluteTime			mStartTime;
	BOOL					mUpdated;
	
	int						mWindowWidth, mWindowHeight;
	float					mFrameRate;
}

- (id)init:(cinder::app::AppCinderView *)aApp;
- (void)launchApplication:(CinderView *)aCinderView;
- (void)startAnimationTimer;
- (void)quit;

- (int)getWindowWidth;
- (void)setWindowWidth:(int)windowWidth;
- (int)getWindowHeight;
- (void)setWindowHeight:(int)windowHeight;
- (void)setWindowSizeWithWidth:(int)w height:(int)h;
- (void)reflectWindowSizeWithWidth:(int)w height:(int)h;
- (void)defaultResize;
- (CGPoint)mouseLocation;
- (float)getFrameRate;
- (void)setFrameRate:(float)frameRate;
- (bool)isFullScreen;
- (void)enterFullScreen;
- (void)exitFullScreen;
- (std::string)getAppPath;
- (double)getElapsedSeconds;

@end
