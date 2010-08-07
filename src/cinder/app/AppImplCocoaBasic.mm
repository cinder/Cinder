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

#include "cinder/app/AppImplCocoaBasic.h"
#include "cinder/app/Renderer.h"
#import <OpenGL/OpenGL.h>

// This seems to be missing for unknown reasons
@interface NSApplication(MissingFunction)
	- (void)setAppleMenu:(NSMenu *)menu;
@end 

@implementation AppImplCocoaBasic

- (id)init:(cinder::app::AppBasic*)aApp 
{	
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	
	[NSApplication sharedApplication];
	
	[NSApp setMainMenu:[[NSMenu alloc] init]];
	
	const std::string& applicationName = aApp->getSettings().getTitle();
	[self setApplicationMenu: [NSString stringWithUTF8String: applicationName.c_str()]];
	
	[NSApp setDelegate:self];
	
	app = aApp;
	app->privateSetImpl__( self );
	cinderView = nil;
	win = nil;
	mNeedsUpdate = YES;

    [NSApp run];

	[pool release];
	
    return 0;
}

- (void)dealloc
{
	[super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	mDisplay = app->getSettings().getDisplay();
	if( mDisplay == 0 )
		mDisplay = cinder::Display::getMainDisplay().get();
	
	mFrameRate = app->getSettings().getFrameRate();
	[self createWindow];
	if( app->getSettings().isFullScreen() )
		[self enterFullScreen];
	app->getRenderer()->makeCurrentContext();
	app->privateSetup__();
	[cinderView setAppSetupCalled:YES];
	app->privateResize__( ci::app::ResizeEvent( ci::Vec2i( app->getWindowWidth(), app->getWindowHeight() ) ) );
}

- (void)startAnimationTimer
{
	if( ( animationTimer == nil ) || ( ! [animationTimer isValid] ) ) {
		float interval = 1.0f / mFrameRate;
		animationTimer = [NSTimer	 timerWithTimeInterval:interval
													target:self
												  selector:@selector(timerFired:)
												  userInfo:nil
												   repeats:YES];
		[[NSRunLoop currentRunLoop] addTimer:animationTimer forMode:NSDefaultRunLoopMode];
		[[NSRunLoop currentRunLoop] addTimer:animationTimer forMode:NSEventTrackingRunLoopMode];
	}	
	app->getRenderer()->makeCurrentContext();
}

- (void)timerFired:(NSTimer *)t
{
	app->privateUpdate__();
	[cinderView draw];	
}

- (void)createWindow
{
	int offsetX = ( mDisplay->getWidth() - app->getSettings().getWindowWidth() ) / 2;
	int offsetY = ( mDisplay->getHeight() - app->getSettings().getWindowHeight() ) / 2;	
	NSRect winRect = NSMakeRect( offsetX, offsetY, app->getSettings().getWindowWidth(), app->getSettings().getWindowHeight() );
	unsigned int myStyleMask = NSTitledWindowMask;
	
	if( app->getSettings().isResizable() ) {
		myStyleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask| NSResizableWindowMask;
	}
	win = [[NSWindow alloc] initWithContentRect:winRect
									  styleMask:myStyleMask
										backing:NSBackingStoreBuffered
										  defer:NO
										 screen:mDisplay->getNSScreen()];

	if( cinderView == nil ) {
		cinderView = [[CinderView alloc] initWithFrame:NSMakeRect( 0, 0, app->getSettings().getWindowWidth(), app->getSettings().getWindowHeight() ) app:app];
		[cinderView retain];
	}

	[win setDelegate:self];	
	[win setContentView:cinderView];

	mWindowWidth = static_cast<int>( winRect.size.width );
	mWindowHeight = static_cast<int>( winRect.size.height );
		
	[self startAnimationTimer];
	[win makeKeyAndOrderFront:nil];
	[win setInitialFirstResponder:cinderView];
	[win setAcceptsMouseMovedEvents:YES];
	// we need to get told about it when the window changes screens so we can update the display link
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(windowChangedScreen:) name:NSWindowDidMoveNotification object:nil];
	[cinderView setNeedsDisplay:YES];
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_5	
	if( app->getSettings().isMultiTouchEnabled() )
		[cinderView setMultiTouchDelegate:self];
#endif
}

- (void)destroyWindow
{
	[animationTimer invalidate];
	if( win )
		[win release];
	win = nil;
}

- (bool)isFullScreen
{
	return mFullScreen;
}

- (void)exitFullScreen
{
	[cinderView exitFullScreenModeWithOptions:nil];
	[win becomeKeyWindow];
	[win makeFirstResponder:cinderView];
	mFullScreen = NO;	
}

- (void)enterFullScreen
{
	NSDictionary *options = nil;
	if( ! app->getSettings().isSecondaryDisplayBlankingEnabled() )
		options = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:NO] forKey:NSFullScreenModeAllScreens];
	[cinderView enterFullScreenMode:[NSScreen mainScreen] withOptions:options];
	
	NSRect bounds = [cinderView bounds];
	mWindowWidth = static_cast<int>( bounds.size.width );
	mWindowHeight = static_cast<int>( bounds.size.height );
	mFullScreen = YES;
}

- (std::string)getAppPath
{
	NSString *resultPath = [[NSBundle mainBundle] bundlePath];
	std::string result;
	result = [resultPath cStringUsingEncoding:NSUTF8StringEncoding];
	return result;
}

// application menu stolen from SDLMian.m
- (void)setApplicationMenu: (NSString*) applicationName
{
    /* warning: this code is very odd */
    NSMenu *appleMenu;
    NSMenuItem *menuItem;
    NSString *title;
    appleMenu = [[NSMenu alloc] initWithTitle:@""];
    
    /* Add menu items */
    title = [@"About " stringByAppendingString:applicationName];
    [appleMenu addItemWithTitle:title action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
	
    [appleMenu addItem:[NSMenuItem separatorItem]];
	
    title = [@"Hide " stringByAppendingString:applicationName];
    [appleMenu addItemWithTitle:title action:@selector(hide:) keyEquivalent:@"h"];
	
    menuItem = (NSMenuItem *)[appleMenu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"];
    [menuItem setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];
	
    [appleMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];
	
    [appleMenu addItem:[NSMenuItem separatorItem]];
	
    title = [@"Quit " stringByAppendingString:applicationName];
    [appleMenu addItemWithTitle:title action:@selector(quit) keyEquivalent:@"q"];
    
    /* Put menu into the menubar */
    menuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
    [menuItem setSubmenu:appleMenu];
    [[NSApp mainMenu] addItem:menuItem];
	
    /* Tell the application object that this is now the application menu */
    [NSApp setAppleMenu:appleMenu];
	
    /* Finally give up our references to the objects */
    [appleMenu release];
    [menuItem release];
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	app->privateShutdown__();
	delete app;	
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
	return YES;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)theEvent
{
	return YES;
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
	if( cinderView )
		[cinderView rightMouseDown:theEvent];
}

- (void)quit
{
	[NSApp terminate:self];
}

- (void)setDisplay:(cinder::Display*)aDisplay
{
	mDisplay = aDisplay;
}

- (cinder::Display*)getDisplay
{
	return mDisplay;
}

- (int)getWindowWidth
{
	return mWindowWidth;
}

- (void)setWindowWidth:(int)windowWidth
{
	NSSize newSize;
	newSize.width = windowWidth;
	newSize.height = app->getWindowHeight();
	[win setContentSize:newSize];
	mWindowWidth = app->getWindowWidth();	
}

- (int)getWindowHeight
{
	return mWindowHeight;
}

- (void)setWindowHeight:(int)windowHeight
{
	NSSize newSize;
	newSize.width = app->getWindowWidth();
	newSize.height = windowHeight;
	[win setContentSize:newSize];
	mWindowHeight = app->getWindowHeight();
}

- (void)setWindowSizeWithWidth:(int)w height:(int)h
{
	NSSize newSize;
	newSize.width = w;
	newSize.height = h;
	[win setContentSize:newSize];
	mWindowWidth = w;
	mWindowHeight = h;
}

- (void)handleResizeWithWidth:(int)w height:(int)h
{
	mWindowWidth = w;
	mWindowHeight = h;
}

- (void)windowChangedScreen:(NSNotification*)inNotification
{
    // If the video moves to a different screen, synchronize to the timing of that screen.
	NSWindow *window = [inNotification object]; 
	CGDirectDisplayID displayID = (CGDirectDisplayID)[[[[window screen] deviceDescription] objectForKey:@"NSScreenNumber"] intValue];

	if( displayID != mDisplay->getCGDirectDisplayID() ) {
		mDisplay = cinder::Display::findFromCGDirectDisplayID( displayID ).get();
	}
}

- (CGPoint)mouseLocation
{
	NSPoint winRel = [win mouseLocationOutsideOfEventStream];
	NSRect bounds = [cinderView bounds];
	return NSPointToCGPoint( NSMakePoint( winRel.x - bounds.origin.x, winRel.y - bounds.origin.y ) );
}

- (float)getFrameRate
{
	return mFrameRate;
}

- (void)setFrameRate:(float)aFrameRate
{
	// TODO: Kill current timer and replace with a new one reflecting this
//	return aFrameRate;
}

- (void)windowDidResignKey:(NSNotification*)aNotification
{
	[cinderView applicationWillResignActive:aNotification];
}

// multiTouch delegate methods
- (void)touchesBegan:(ci::app::TouchEvent*)event
{
	app->privateTouchesBegan__( *event );
}

- (void)touchesMoved:(ci::app::TouchEvent*)event
{
	app->privateTouchesMoved__( *event );
}

- (void)touchesEnded:(ci::app::TouchEvent*)event
{
	app->privateTouchesEnded__( *event );
}

- (void)setActiveTouches:(std::vector<ci::app::TouchEvent::Touch>*)touches
{
	app->privateSetActiveTouches__( *touches );
}

- (void)touchesEndedWithEvent:(NSEvent *)event
{
}

- (void)touchesCancelledWithEvent:(NSEvent *)event
{
}

@end
