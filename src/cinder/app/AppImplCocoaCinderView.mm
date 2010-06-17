/*
 Copyright (c) 2009, The Barbarian Group
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

#include "cinder/app/AppImplCocoaCinderView.h"
#import <OpenGL/OpenGL.h>

@implementation AppImplCocoaCinderView

- (id)init:(cinder::app::AppCinderView *)aApp
{	
	self = [super init];
	app = aApp;
	cinderView = nil;
	
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (void)launchApplication:(CinderView *)aCinderView
{
	cinderView = aCinderView;
	[cinderView setup:app];

	mFrameRate = app->getSettings().getFrameRate();

	app->setup();
	[cinderView setAppSetupCalled:YES];
	app->privateResize__( ci::app::ResizeEvent( ci::Vec2i( app->getWindowWidth(), app->getWindowHeight() ) ) );
	[self startAnimationTimer];
}

- (void)startAnimationTimer
{
	if( ( animationTimer == nil ) || ( ! [animationTimer isValid] ) ) {
		animationTimer = [NSTimer	 timerWithTimeInterval:1.0f / mFrameRate
													target:self
												  selector:@selector(timerFired:)
												  userInfo:nil
												   repeats:YES];
		[[NSRunLoop currentRunLoop] addTimer:animationTimer forMode:NSDefaultRunLoopMode];
		[[NSRunLoop currentRunLoop] addTimer:animationTimer forMode:NSEventTrackingRunLoopMode];
	}
}

- (void)timerFired:(NSTimer *)t
{
	app->update();
	[cinderView draw];
}

- (bool)isFullScreen
{
	return false;
}

- (void)exitFullScreen
{
	[cinderView exitFullScreenModeWithOptions:nil];
	[[cinderView window] becomeKeyWindow];
	[[cinderView window] makeFirstResponder:cinderView];
}

- (void)enterFullScreen
{	
	[cinderView enterFullScreenMode:[NSScreen mainScreen] withOptions:nil];
	NSRect bounds = [cinderView bounds];
	mWindowWidth = bounds.size.width;
	mWindowHeight = bounds.size.height;
}

- (std::string)getAppPath
{
	NSString *resultPath = [[NSBundle mainBundle] bundlePath];
	std::string result;
	result = [resultPath cStringUsingEncoding:NSMacOSRomanStringEncoding];
	return result;
}

- (double)getElapsedSeconds
{
	CFAbsoluteTime currentTime = CFAbsoluteTimeGetCurrent();
	return ( currentTime - mStartTime );
}

- (void)quit
{
	[NSApp terminate:self];
}

- (int)getWindowWidth
{
	return mWindowWidth;
}

- (void)setWindowWidth:(int)windowWidth
{
}

- (int)getWindowHeight
{
	return mWindowHeight;
}

- (void)setWindowHeight:(int)aWindowHeight
{
}

- (void)setWindowSizeWithWidth:(int)w height:(int)h
{
}

- (void)reflectWindowSizeWithWidth:(int)w height:(int)h
{
	mWindowWidth = w;
	mWindowHeight = h;
}

- (void)defaultResize
{
	[cinderView defaultResize];
}

- (CGPoint)mouseLocation
{
	NSPoint winRel = [[cinderView window] mouseLocationOutsideOfEventStream];
	NSRect bounds = [cinderView bounds];
	return NSPointToCGPoint( NSMakePoint( winRel.x - bounds.origin.x, winRel.y - bounds.origin.y ) );
}

- (float)getFrameRate
{
	return mFrameRate;
}

- (void)setFrameRate:(float)frameRate
{
	// TODO: Kill current timer and replace with a new one reflecting this
}

@end
