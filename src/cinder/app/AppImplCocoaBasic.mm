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

#include "cinder/app/AppImplCocoaBasic.h"
#include "cinder/app/Renderer.h"
#include "cinder/app/Window.h"
#import "cinder/cocoa/CinderCocoa.h"

#import <OpenGL/OpenGL.h>

// This seems to be missing for unknown reasons
@interface NSApplication(MissingFunction)
	- (void)setAppleMenu:(NSMenu *)menu;
@end 

// CinderWindow - necessary to enable a borderless window to receive keyboard events
@interface CinderWindow : NSWindow {
}
- (BOOL)canBecomeMainWindow;
- (BOOL)canBecomeKeyWindow;
@end
@implementation CinderWindow
- (BOOL)canBecomeMainWindow { return YES; }
- (BOOL)canBecomeKeyWindow { return YES; }
@end

@implementation AppImplCocoaBasic

@synthesize windows = mWindows;

- (id)init:(cinder::app::AppBasic*)aApp
{	
	self = [super init];
	
	NSMenu *mainMenu = [[NSMenu alloc] init];
	[NSApp setMainMenu:mainMenu];
	
	self.windows = [NSMutableArray array];
	
	const std::string& applicationName = aApp->getSettings().getTitle();
	[self setApplicationMenu:[NSString stringWithUTF8String: applicationName.c_str()]];
	
	[NSApp setDelegate:self];
	
	mApp = aApp;
	mApp->privateSetImpl__( self );
	mNeedsUpdate = YES;
	
    return self;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	// build our list of requested formats; an empty list implies we should make the default window format
	std::vector<cinder::app::Window::Format> formats( mApp->getSettings().getWindowFormats() );
	if( formats.empty() )
		formats.push_back( mApp->getSettings().getDefaultWindowFormat() );

	// create all the requested windows
	for( const auto &format : formats ) {
		WindowImplBasicCocoa *winImpl = [ WindowImplBasicCocoa instantiate:format withAppImpl:self withRetina:mApp->getSettings().isHighDensityDisplayEnabled() ];
		[mWindows addObject:winImpl];
		if( format.isFullScreen() )
			[winImpl setFullScreen:YES options:&format.getFullScreenOptions()];
	}
	
	mFrameRate = mApp->getSettings().getFrameRate();
	mFrameRateEnabled = mApp->getSettings().isFrameRateEnabled();

	mApp->getRenderer()->makeCurrentContext();
	mApp->privateSetup__();
	
	// give all windows initial resizes
	for( WindowImplBasicCocoa* winIt in mWindows ) {
		[winIt->mCinderView makeCurrentContext];
		[self setActiveWindow:winIt];
		winIt->mWindowRef->emitResize();
	}	
	
	// make the first window the active window
	[self setActiveWindow:[mWindows objectAtIndex:0]];
	[self startAnimationTimer];
}

- (void)startAnimationTimer
{
	if( mAnimationTimer && [mAnimationTimer isValid] )
		[mAnimationTimer invalidate];
	
	float interval = ( mFrameRateEnabled ) ? 1.0f / mFrameRate : 0.001;
	mAnimationTimer = [NSTimer	 timerWithTimeInterval:interval
												target:self
											  selector:@selector(timerFired:)
											  userInfo:nil
											   repeats:YES];
	[[NSRunLoop currentRunLoop] addTimer:mAnimationTimer forMode:NSDefaultRunLoopMode];
	[[NSRunLoop currentRunLoop] addTimer:mAnimationTimer forMode:NSEventTrackingRunLoopMode];
}

- (void)timerFired:(NSTimer *)t
{
	// note: this would not work if the frame rate were set to something absurdly low
	if( ! mApp->isPowerManagementEnabled() ) {
		static double lastSystemActivity = 0;
		double curTime = cinder::app::getElapsedSeconds();
		if( curTime - lastSystemActivity >= 30 ) { // every thirty seconds call this to prevent sleep
			::UpdateSystemActivity( OverallAct );
			lastSystemActivity = curTime;
		}
	}

	// issue update() event
	mApp->privateUpdate__();

	// mark all windows as ready to draw; this really only matters the first time, to ensure the first update() fires before draw()
	for( WindowImplBasicCocoa* winIt in mWindows ) {
		[winIt->mCinderView setReadyToDraw:YES];
	}
	
	// walk all windows and draw them
	for( WindowImplBasicCocoa* winIt in mWindows ) {
		[winIt->mCinderView draw];
	}
}

- (cinder::app::WindowRef)createWindow:(cinder::app::Window::Format)format
{
	WindowImplBasicCocoa *winImpl = [ WindowImplBasicCocoa instantiate:format withAppImpl:self withRetina:mApp->getSettings().isHighDensityDisplayEnabled() ];
	[mWindows addObject:winImpl];
	if( format.isFullScreen() )
		[winImpl setFullScreen:YES options:&format.getFullScreenOptions()];

	// pass the window its first resize
	[winImpl->mCinderView makeCurrentContext];
	[self setActiveWindow:winImpl];
	winImpl->mWindowRef->emitResize();

	// mark the window as readyToDraw, now that we've resized it
	[winImpl->mCinderView setReadyToDraw:YES];
		
	return winImpl->mWindowRef;
}

// Returns a pointer to a Renderer of the same type if any existing Windows have one of the same type
- (cinder::app::RendererRef)findSharedRenderer:(cinder::app::RendererRef)match
{
	for( WindowImplBasicCocoa* winIt in mWindows ) {
		cinder::app::RendererRef renderer = [winIt->mCinderView getRenderer];
		if( typeid(renderer) == typeid(match) )
			return renderer;
	}
	
	return cinder::app::RendererRef();
}

- (cinder::app::WindowRef)getWindow
{
	if( ! mActiveWindow )
		throw cinder::app::ExcInvalidWindow();
	else
		return mActiveWindow->mWindowRef;
}

- (cinder::app::WindowRef)getForegroundWindow
{
	NSWindow *mainWin = [NSApp mainWindow];
	WindowImplBasicCocoa *winImpl = [self findWindowImpl:mainWin];
	if( winImpl )
		return winImpl->mWindowRef;
	else
		return cinder::app::WindowRef();
}

- (size_t)getNumWindows
{
	return [mWindows count];
}

- (cinder::app::WindowRef)getWindowIndex:(size_t)index
{
	if( index >= [mWindows count] )
		throw cinder::app::ExcInvalidWindow();
	
	WindowImplBasicCocoa *winImpl = [mWindows objectAtIndex:index];
	return winImpl->mWindowRef;
}

- (void)setActiveWindow:(WindowImplBasicCocoa*)win
{
	mActiveWindow = win;
}

- (WindowImplBasicCocoa*)findWindowImpl:(NSWindow*)window
{
	for( WindowImplBasicCocoa* winIt in mWindows ) {
		if( winIt->mWin == window )
			return winIt;
	}

	return nil;
}

- (void)releaseWindow:(WindowImplBasicCocoa*)windowImpl
{
	if( mActiveWindow == windowImpl ) {
		if( [mWindows count] == 1 ) // we're about to release the last window; set the active window to be NULL
			mActiveWindow = nil;
		else
			mActiveWindow = [mWindows objectAtIndex:0];
	}

	windowImpl->mWindowRef->setInvalid();
	windowImpl->mWindowRef.reset();
	windowImpl->mWin = nil;
	[mWindows removeObject:windowImpl];
}

- (std::string)getAppPath
{
	NSString *resultPath = [[NSBundle mainBundle] bundlePath];
	std::string result;
	result = [resultPath cStringUsingEncoding:NSUTF8StringEncoding];
	return result;
}

// This is all necessary because we don't use NIBs in Cinder apps
// and we have to generate our menu programmatically
- (void)setApplicationMenu: (NSString*) applicationName
{
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
	// we need to close all existing windows
	while( [mWindows count] > 0 ) {
		// this counts on windowWillCloseNotification: firing and in turn calling releaseWindow
		[[mWindows objectAtIndex:0] close];
	}

	mApp->emitShutdown();
	delete mApp;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
	return mApp->getSettings().isQuitOnLastWindowCloseEnabled();
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)theApplication
{
	bool shouldQuit = mApp->privateShouldQuit();
	return ( shouldQuit ) ? NSTerminateNow : NSTerminateCancel;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)theEvent
{
	return YES;
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
//TODO
//	if( cinderView )
//		[cinderView rightMouseDown:theEvent];
}

- (void)quit
{
	// in certain scenarios self seems to have be deallocated inside terminate:
	// so we call this here and then pass nil to terminate: instead
	if( ! mApp->privateShouldQuit() )
		return;

	[NSApp terminate:nil];
}

- (float)getFrameRate
{
	return mFrameRate;
}

- (void)setFrameRate:(float)aFrameRate
{
    mFrameRate = aFrameRate;
	mFrameRateEnabled = YES;
    [mAnimationTimer invalidate];
    [self startAnimationTimer];
}

- (void)disableFrameRate
{
	mFrameRateEnabled = NO;
    [mAnimationTimer invalidate];
	[self startAnimationTimer];
}

- (bool)isFrameRateEnabled
{
	return mFrameRateEnabled;
}

- (void)windowDidResignKey:(NSNotification*)aNotification
{
//TODO	[cinderView applicationWillResignActive:aNotification];
}

- (void)touchesEndedWithEvent:(NSEvent *)event
{
}

- (void)touchesCancelledWithEvent:(NSEvent *)event
{
}

@end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WindowImplBasicCocoa
@implementation WindowImplBasicCocoa

- (void)dealloc
{
	[mCinderView release];
	[super dealloc];
}

- (BOOL)isFullScreen
{
	return [mCinderView isFullScreen];
}

- (void)setFullScreen:(BOOL)fullScreen options:(const cinder::app::FullScreenOptions *)options;
{
	if( fullScreen == [mCinderView isFullScreen] )
		return;

	[mCinderView setFullScreen:fullScreen options:options];

	if( fullScreen ) {
		// ???: necessary? won't this be set in resize?
		NSRect bounds = [mCinderView bounds];
		mSize.x = static_cast<int>( bounds.size.width );
		mSize.y = static_cast<int>( bounds.size.height );	
	}
	else {
		[mWin becomeKeyWindow];
		[mWin makeFirstResponder:mCinderView];
	}
}

- (cinder::Vec2i)getSize
{
	return mSize;
}

- (void)setSize:(cinder::Vec2i)size
{
	// this compensates for the Mac wanting to resize from the lower-left corner
	ci::Vec2i sizeDelta = size - mSize;
	NSRect r = [mWin frame];
	r.size.width += sizeDelta.x;
	r.size.height += sizeDelta.y;
	r.origin.y -= sizeDelta.y;
	[mWin setFrame:r display:YES];

	mSize = size;
}

- (cinder::Vec2i)getPos
{
	return mPos;
}

- (float)getContentScale
{
	return [mCinderView contentScaleFactor];
}

- (void)setPos:(cinder::Vec2i)pos
{
	NSPoint p;
	p.x = pos.x;
	p.y = cinder::Display::getMainDisplay()->getHeight() - pos.y;
	mPos = pos;
	NSRect currentContentRect = [mWin contentRectForFrameRect:[mWin frame]];
	NSRect targetContentRect = NSMakeRect( p.x, p.y - currentContentRect.size.height, currentContentRect.size.width, currentContentRect.size.height);
	NSRect targetFrameRect = [mWin frameRectForContentRect:targetContentRect];
	[mWin setFrameOrigin:targetFrameRect.origin];
}

- (void)close
{
	[mWin close];
}

- (NSString *)getTitle
{
	return [mWin title];
}

- (void)setTitle:(NSString *)title
{
	[mWin setTitle:title];
}

- (BOOL)isBorderless
{
	return mBorderless;
}

- (void)setBorderless:(BOOL)borderless
{
	mBorderless = borderless;

	unsigned int styleMask;
	if( mBorderless )
		styleMask = ( mResizable ) ? ( NSBorderlessWindowMask | NSResizableWindowMask ) : NSBorderlessWindowMask;
	else if( mResizable )
		styleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask| NSResizableWindowMask;
	else
		styleMask = NSTitledWindowMask;
	[mWin setStyleMask:styleMask];
	[mWin makeFirstResponder:mCinderView];
	[mWin makeKeyWindow];
	[mWin makeMainWindow];
	[mWin setHasShadow:(! mBorderless)];
}

- (bool)isAlwaysOnTop
{
	return mAlwaysOnTop;
}

- (void)setAlwaysOnTop:(bool)alwaysOnTop
{
	if( mAlwaysOnTop != alwaysOnTop ) {
		mAlwaysOnTop = alwaysOnTop;
		[mWin setLevel:(mAlwaysOnTop)?NSScreenSaverWindowLevel:NSNormalWindowLevel];
	}
}

- (void)hide
{
	if( ! mHidden ) {
		[mWin orderOut:self];
		mHidden = YES;
	}	
}

- (void)show
{
	if( mHidden ) {
		[mWin makeKeyAndOrderFront:self];
		mHidden = NO;
	}
}

- (BOOL)isHidden
{
	return mHidden;
}

- (cinder::DisplayRef)getDisplay
{
	return mDisplay;
}

- (cinder::app::RendererRef)getRenderer
{
	if( mCinderView )
		return [mCinderView getRenderer];
	else
		return cinder::app::RendererRef();
}

- (void*)getNative
{
	return mCinderView;
}

- (void)windowMovedNotification:(NSNotification*)inNotification
{
	NSWindow *window = [inNotification object];
	CGDirectDisplayID displayID = (CGDirectDisplayID)[[[[window screen] deviceDescription] objectForKey:@"NSScreenNumber"] intValue];

	NSRect frame = [mWin frame];
	NSRect content = [mWin contentRectForFrameRect:frame];
	mPos = ci::Vec2i( content.origin.x, cinder::Display::getMainDisplay()->getHeight() - frame.origin.y - content.size.height );

	[mAppImpl setActiveWindow:self];

	if( displayID != mDisplay->getCgDirectDisplayId() ) {
		mDisplay = cinder::Display::findFromCgDirectDisplayId( displayID );
		mWindowRef->emitDisplayChange();
	}
	
	mWindowRef->emitMove();
}

- (void)windowWillCloseNotification:(NSNotification*)inNotification
{
	// if this is the last window and we're set to terminate on last window, invalidate the timer
	if( [mAppImpl getNumWindows] == 1 && mAppImpl->mApp->getSettings().isQuitOnLastWindowCloseEnabled() ) {
		[mAppImpl->mAnimationTimer invalidate];
		mAppImpl->mAnimationTimer = nil;
	}

	[mAppImpl setActiveWindow:self];
	// emit the signal before we start destroying stuff
	mWindowRef->emitClose();
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[mAppImpl releaseWindow:self];
}

// CinderViewDelegate Methods
- (void)resize
{
	NSSize nsSize = [mCinderView frame].size;
	mSize = cinder::Vec2i( nsSize.width, nsSize.height );
	
	[mAppImpl setActiveWindow:self];
	
	mWindowRef->emitResize();
}

- (void)draw
{
	[mAppImpl setActiveWindow:self];
	mWindowRef->emitDraw();
}

- (void)mouseDown:(cinder::app::MouseEvent*)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitMouseDown( event );
}

- (void)mouseDrag:(cinder::app::MouseEvent*)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitMouseDrag( event );
}

- (void)mouseUp:(cinder::app::MouseEvent*)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitMouseUp( event );
}

- (void)mouseMove:(cinder::app::MouseEvent*)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitMouseMove( event );
}

- (void)mouseWheel:(cinder::app::MouseEvent*)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitMouseWheel( event );
}

- (void)keyDown:(cinder::app::KeyEvent*)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitKeyDown( event );
}

- (void)keyUp:(cinder::app::KeyEvent*)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitKeyUp( event );
}

- (void)touchesBegan:(cinder::app::TouchEvent*)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitTouchesBegan( event );
}

- (void)touchesMoved:(cinder::app::TouchEvent*)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitTouchesMoved( event );
}

- (void)touchesEnded:(cinder::app::TouchEvent*)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitTouchesEnded( event );
}

- (const std::vector<cinder::app::TouchEvent::Touch>&)getActiveTouches
{
	return [mCinderView getActiveTouches];
}

- (void)fileDrop:(cinder::app::FileDropEvent*)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitFileDrop( event );
}

- (cinder::app::WindowRef)getWindowRef
{
	return mWindowRef;
}

+ (WindowImplBasicCocoa*)instantiate:(cinder::app::Window::Format)winFormat withAppImpl:(AppImplCocoaBasic*)appImpl withRetina:(BOOL)retinaEnabled;
{
	WindowImplBasicCocoa *winImpl = [[WindowImplBasicCocoa alloc] init];

	winImpl->mAppImpl = appImpl;
	winImpl->mWindowRef = cinder::app::Window::privateCreate__( winImpl, winImpl->mAppImpl->mApp );
	winImpl->mDisplay = winFormat.getDisplay();
	winImpl->mHidden = NO;
	winImpl->mResizable = winFormat.isResizable();
	winImpl->mBorderless = winFormat.isBorderless();
	winImpl->mAlwaysOnTop = winFormat.isAlwaysOnTop();
	int offsetX, offsetY;
	if( ! winFormat.isPosSpecified() ) {
		offsetX = ( winImpl->mDisplay->getWidth() - winFormat.getSize().x ) / 2;
		offsetY = ( winImpl->mDisplay->getHeight() - winFormat.getSize().y ) / 2;
	}
	else {
		offsetX = winFormat.getPos().x;
		offsetY = cinder::Display::getMainDisplay()->getHeight() - winFormat.getPos().y - winFormat.getSize().y;
	}
	NSRect winRect = NSMakeRect( offsetX, offsetY, winFormat.getSize().x, winFormat.getSize().y );
	unsigned int styleMask;
	
	if( winImpl->mBorderless )
		styleMask = ( winImpl->mResizable ) ? ( NSBorderlessWindowMask | NSResizableWindowMask ) : ( NSBorderlessWindowMask );
	else if( winImpl->mResizable )
		styleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask| NSResizableWindowMask;
	else
		styleMask = NSTitledWindowMask;
	winImpl->mWin = [[CinderWindow alloc] initWithContentRect:winRect
									  styleMask:styleMask
										backing:NSBackingStoreBuffered
										  defer:NO
										 screen:winImpl->mDisplay->getNsScreen()];

	NSRect contentRect = [winImpl->mWin contentRectForFrameRect:[winImpl->mWin frame]];
	winImpl->mSize.x = (int)contentRect.size.width;
	winImpl->mSize.y = (int)contentRect.size.height;
	winImpl->mPos = ci::Vec2i( contentRect.origin.x, cinder::Display::getMainDisplay()->getHeight() - [winImpl->mWin frame].origin.y - contentRect.size.height );

	[winImpl->mWin setLevel:(winImpl->mAlwaysOnTop)?NSScreenSaverWindowLevel:NSNormalWindowLevel];

	// title
	if( ! winFormat.getTitle().empty() )
		[winImpl->mWin setTitle:[NSString stringWithUTF8String:winFormat.getTitle().c_str()]];

	if( winFormat.isFullScreenButtonEnabled() )
		[winImpl->mWin setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
	
	if( ! winFormat.getRenderer() )
		winFormat.setRenderer( appImpl->mApp->getDefaultRenderer()->clone() );
	// for some renderers, ok really just GL, we want an existing renderer so we can steal its context to share with. If this comes back with NULL that's fine - we're first
	cinder::app::RendererRef sharedRenderer = [appImpl findSharedRenderer:winFormat.getRenderer()];
	
	cinder::app::RendererRef renderer = winFormat.getRenderer();
	winImpl->mCinderView = [[CinderView alloc] initWithFrame:NSMakeRect( 0, 0, winImpl->mSize.x, winImpl->mSize.y ) app:winImpl->mAppImpl->mApp renderer:renderer sharedRenderer:sharedRenderer];

	[winImpl->mWin setDelegate:self];	
	[winImpl->mWin setContentView:winImpl->mCinderView];

	[winImpl->mWin makeKeyAndOrderFront:nil];
	[winImpl->mWin setInitialFirstResponder:winImpl->mCinderView];
	[winImpl->mWin setAcceptsMouseMovedEvents:YES];
	[winImpl->mWin setOpaque:YES];
	[[NSNotificationCenter defaultCenter] addObserver:winImpl selector:@selector(windowMovedNotification:) name:NSWindowDidMoveNotification object:winImpl->mWin];
	[[NSNotificationCenter defaultCenter] addObserver:winImpl selector:@selector(windowWillCloseNotification:) name:NSWindowWillCloseNotification object:winImpl->mWin];
	[winImpl->mCinderView setNeedsDisplay:YES];
	[winImpl->mCinderView setDelegate:winImpl];

	// make this window the active window
	appImpl->mActiveWindow = winImpl;

	return [winImpl autorelease];
}

@end
