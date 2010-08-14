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

#import "cinder/app/AppImplCocoaScreenSaver.h"

using std::cout;

@implementation AppImplCocoaScreenSaver

- (id)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
	self = [super initWithFrame:frame isPreview:isPreview];

	// Our pointer to a screensaver factory will provide us with a new instance of the actual ScreenSaver class
	app = ScreenSaverFactoryMethod( (void*)self );
	app->privateSetImpl__( self );
	NSRect newFrame = frame;
	// Slam the origin values
	newFrame.origin.x = 0.0;
	newFrame.origin.y = 0.0;

	cinderView = [[CinderView alloc] initWithFrame:newFrame app:app];
	[self setAutoresizesSubviews:YES];
	[self addSubview:cinderView];

	[cinderView release]; // addSubview incs the retainCount and the parent assumes ownership

	app->privateSetup__();
	cinderView.appSetupCalled = YES;

	app->privateResize__( ci::app::ResizeEvent( ci::Vec2i( app->getWindowWidth(), app->getWindowHeight() ) ) );

	[self setAnimationTimeInterval:1 / 30.0f];//app->getFrameRate()];
	
    return self;
}

- (void)setFrameSize:(NSSize)newSize
{
	[super setFrameSize:newSize];
	[cinderView setFrameSize:newSize]; 
}

- (void)dealloc
{
	if( app ) {
		app->privateShutdown__();
		delete app;
		app = 0;
	}
	[super dealloc];
}

- (void)drawRect:(NSRect)rect
{
	if( ! app ) {
		return;
	}

	[super drawRect:rect];
	[cinderView drawRect:rect];
}

- (void)animateOneFrame
{
	if( ! app ) {
		return;
	}
	
	app->privateUpdate__();
	[self setNeedsDisplay:YES];
}

- (int)getWindowWidth
{
	NSRect r = [self frame];
	return r.size.width;
}

- (int)getWindowHeight
{
	NSRect r = [self frame];
	return r.size.height;
}

- (void)startAnimation
{
	[super startAnimation];
}

- (void)stopAnimation
{
	[super stopAnimation];
}

- (void)setFrameRate:(float)aFrameRate
{
	::NSTimeInterval interval = 1.0 / aFrameRate; 
	[self setAnimationTimeInterval:interval];
}

- (float)getFrameRate
{
	return (float)[self animationTimeInterval];
}

- (std::string)getAppPath
{
	return [[[NSBundle bundleForClass:[self class]] bundlePath] UTF8String];
}

@end
