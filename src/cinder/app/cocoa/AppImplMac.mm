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

#include "cinder/app/cocoa/AppImplMac.h"
#include "cinder/app/Renderer.h"
#include "cinder/app/Window.h"
#include "cinder/app/cocoa/PlatformCocoa.h"
#include "cinder/Log.h"
#import "cinder/cocoa/CinderCocoa.h"

#include <memory>

#import <OpenGL/OpenGL.h>

using namespace cinder;
using namespace cinder::app;

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

// private properties
@interface AppImplMac()
@property(nonatomic) IOPMAssertionID idleSleepAssertionID;
@property(nonatomic) IOPMAssertionID displaySleepAssertionID;
@end

@implementation AppImplMac

@synthesize windows = mWindows;

@synthesize idleSleepAssertionID = mIdleSleepAssertionID;
@synthesize displaySleepAssertionID = mDisplaySleepAssertionID;

- (AppImplMac *)init:(AppMac *)app settings:(const AppMac::Settings &)settings
{	
	self = [super init];

	// This needs to be called before creating any windows, as it internally constructs the shared NSApplication
	[[NSApplication sharedApplication] setDelegate:self];

	NSMenu *mainMenu = [[NSMenu alloc] init];
	[NSApp setMainMenu:mainMenu];
	[mainMenu release];
	
	self.windows = [NSMutableArray array];
	
	const std::string& applicationName = settings.getTitle();
	[self setApplicationMenu:[NSString stringWithUTF8String: applicationName.c_str()]];
	
	mApp = app;
	mNeedsUpdate = YES;
	mQuitOnLastWindowClosed = settings.isQuitOnLastWindowCloseEnabled(); // TODO: consider storing this in AppBase. it is also needed by AppMsw's impl

	// build our list of requested formats; an empty list implies we should make the default window format
	std::vector<Window::Format> formats( settings.getWindowFormats() );
	if( formats.empty() )
		formats.push_back( settings.getDefaultWindowFormat() );

	// create all the requested windows
	for( const auto &format : formats ) {
		WindowImplBasicCocoa *winImpl = [WindowImplBasicCocoa instantiate:format withAppImpl:self];
		[mWindows addObject:winImpl];
		if( format.isFullScreen() )
			[winImpl setFullScreen:YES options:&format.getFullScreenOptions()];
	}
	
	mFrameRate = settings.getFrameRate();
	mFrameRateEnabled = settings.isFrameRateEnabled();

	// lastly, ensure the first window is the currently active context
	[((WindowImplBasicCocoa *)[mWindows firstObject])->mCinderView makeCurrentContext];

	return self;
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
	mApp->getRenderer()->makeCurrentContext();

	// update positions for all windows, their frames might not have been correct when created.
	for( WindowImplBasicCocoa* winIt in mWindows ) {
		[winIt updatePosRelativeToPrimaryDisplay];
	}

	mApp->privateSetup__();
	
	// call initial window resize signals
	for( WindowImplBasicCocoa* winIt in mWindows ) {
		[winIt->mCinderView makeCurrentContext];
		[self setActiveWindow:winIt];
		winIt->mWindowRef->emitResize();
	}
	
	// when available, make the first window the active window
	[self setActiveWindow:[mWindows firstObject]];
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
	if( ! ((PlatformCocoa*)Platform::get())->isInsideModalLoop() ) {
		// issue update() event
		mApp->privateUpdate__();

		// if quit() was called from update(), we don't want to issue another draw()
		if( mApp->getQuitRequested() )
			return;

		// mark all windows as ready to draw; this really only matters the first time, to ensure the first update() fires before draw()
		for( WindowImplBasicCocoa* winIt in mWindows ) {
			[winIt->mCinderView setReadyToDraw:YES];
		}
		
		// walk all windows and draw them
		for( WindowImplBasicCocoa* winIt in mWindows ) {
			[winIt->mCinderView draw];
		}
	}
}

- (app::WindowRef)createWindow:(const Window::Format &)format
{
	WindowImplBasicCocoa *winImpl = [WindowImplBasicCocoa instantiate:format withAppImpl:self];
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
- (RendererRef)findSharedRenderer:(RendererRef)match
{
	for( WindowImplBasicCocoa* winIt in mWindows ) {
		RendererRef renderer = [winIt->mCinderView getRenderer];
		if( typeid(renderer) == typeid(match) )
			return renderer;
	}
	
	return RendererRef();
}

- (app::WindowRef)getWindow
{
	if( ! mActiveWindow )
		throw ExcInvalidWindow();
	else
		return mActiveWindow->mWindowRef;
}

- (app::WindowRef)getForegroundWindow
{
	NSWindow *mainWin = [NSApp mainWindow];
	WindowImplBasicCocoa *winImpl = [self findWindowImpl:mainWin];
	if( winImpl )
		return winImpl->mWindowRef;
	else
		return app::WindowRef();
}

- (size_t)getNumWindows
{
	return [mWindows count];
}

- (app::WindowRef)getWindowIndex:(size_t)index
{
	if( index >= [mWindows count] )
		throw ExcInvalidWindow();
	
	WindowImplBasicCocoa *winImpl = [mWindows objectAtIndex:index];
	return winImpl->mWindowRef;
}

- (void)setActiveWindow:(WindowImplBasicCocoa *)win
{
	mActiveWindow = win;
}

- (WindowImplBasicCocoa *)findWindowImpl:(NSWindow *)window
{
	for( WindowImplBasicCocoa* winIt in mWindows ) {
		if( winIt->mWin == window )
			return winIt;
	}

	return nil;
}

- (void)releaseWindow:(WindowImplBasicCocoa *)windowImpl
{
	if( mActiveWindow == windowImpl ) {
		if( [mWindows count] == 1 ) // we're about to release the last window; set the active window to be NULL
			mActiveWindow = nil;
		else
			mActiveWindow = [mWindows firstObject];
	}

	windowImpl->mWindowRef->setInvalid();
	windowImpl->mWindowRef.reset();
	windowImpl->mWin = nil;
	[mWindows removeObject:windowImpl];
}

// This is all necessary because we don't use NIBs in Cinder apps
// and we have to generate our menu programmatically
- (void)setApplicationMenu:(NSString *)applicationName
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
		[[mWindows lastObject] close];
	}

	mApp->emitCleanup();
	delete mApp;
}

- (void)applicationDidBecomeActive:(NSNotification *)notification
{
	// Due to bug #960: https://github.com/cinder/Cinder/issues/960 We need to force the background window
	// to be actually in the background when we're fullscreen. Was true of 10.9 and 10.10
	if( app::AppBase::get() && app::getWindow() && app::getWindow()->isFullScreen() )
		[[[NSApplication sharedApplication] mainWindow] orderBack:nil];

	mApp->emitDidBecomeActive();
}

- (void)applicationWillResignActive:(NSNotification *)notification
{
	mApp->emitWillResignActive();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)application
{
	return mQuitOnLastWindowClosed;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)application
{
	bool shouldQuit = mApp->privateEmitShouldQuit();
	return ( shouldQuit ) ? NSTerminateNow : NSTerminateCancel;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)event
{
	return YES;
}

- (void)rightMouseDown:(NSEvent *)event
{
//TODO
//	if( cinderView )
//		[cinderView rightMouseDown:event];
}

- (void)quit
{
	// in certain scenarios self seems to have be deallocated inside terminate:
	// so we call this here and then pass nil to terminate: instead
	if( ! mApp->privateEmitShouldQuit() )
		return;

	mApp->setQuitRequested();

	[NSApp stop:nil];

	// we need to post a dummy event to force the runloop to cycle once more
	// otherwise the app won't actually terminate until the mouse is moved or similar
	NSEvent* event = [NSEvent otherEventWithType: NSApplicationDefined location: NSMakePoint(0,0)
							modifierFlags: 0 timestamp: 0.0 windowNumber: 0 context: nil subtype: 0 data1: 0 data2: 0];
	[NSApp postEvent:event atStart:YES];
}

- (void)setPowerManagementEnabled:(BOOL)enable
{
	if( enable == [self isPowerManagementEnabled] )
		return;

	if( ! enable ) { // do not allow sleep or screensavers
		CFStringRef reasonForActivity = CFSTR( "Cinder Application Execution" );
		IOReturn status = ::IOPMAssertionCreateWithName( kIOPMAssertPreventUserIdleSystemSleep, kIOPMAssertionLevelOn, reasonForActivity, &mIdleSleepAssertionID );
		if( status != kIOReturnSuccess )
			CI_LOG_E( "failed to create power management assertion to prevent idle system sleep" );

		status = ::IOPMAssertionCreateWithName( kIOPMAssertPreventUserIdleDisplaySleep, kIOPMAssertionLevelOn, reasonForActivity, &mDisplaySleepAssertionID );
		if( status != kIOReturnSuccess )
			CI_LOG_E( "failed to create power management assertion to prevent idle display sleep" );
	}
	else {
		IOReturn status = ::IOPMAssertionRelease( self.idleSleepAssertionID );
		if( status != kIOReturnSuccess )
			CI_LOG_E( "failed to release and deactivate power management assertion that prevents idle system sleep" );
		self.idleSleepAssertionID = 0;

		status = ::IOPMAssertionRelease( self.displaySleepAssertionID );
		if( status != kIOReturnSuccess )
			CI_LOG_E( "failed to release and deactivate power management assertion that prevents idle display sleep" );
		self.displaySleepAssertionID = 0;
	}
}

- (BOOL)isPowerManagementEnabled
{
	return self.idleSleepAssertionID == 0 && self.displaySleepAssertionID == 0;
}

- (float)getFrameRate
{
	return mFrameRate;
}

- (void)setFrameRate:(float)frameRate
{
	mFrameRate = frameRate;
	mFrameRateEnabled = YES;
	[self startAnimationTimer];
}

- (void)disableFrameRate
{
	mFrameRateEnabled = NO;
	[self startAnimationTimer];
}

- (bool)isFrameRateEnabled
{
	return mFrameRateEnabled;
}

- (void)windowDidResignKey:(NSNotification *)notification
{
//TODO	[cinderView applicationWillResignActive:notification];
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
	if( mTitle )
		[mTitle release];

	[super dealloc];
}

- (BOOL)isFullScreen
{
	return [mCinderView isFullScreen];
}

- (void)setFullScreen:(BOOL)fullScreen options:(const FullScreenOptions *)options;
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

- (cinder::ivec2)getSize
{
	return mSize;
}

- (void)setSize:(cinder::ivec2)size
{
	// this compensates for the Mac wanting to resize from the lower-left corner
	ivec2 sizeDelta = size - mSize;
	NSRect r = [mWin frame];
	r.size.width += sizeDelta.x;
	r.size.height += sizeDelta.y;
	r.origin.y -= sizeDelta.y;
	[mWin setFrame:r display:YES];

	mSize.x = (int)mCinderView.frame.size.width;
	mSize.y = (int)mCinderView.frame.size.height;
}

- (cinder::ivec2)getPos
{
	return mPos;
}

- (float)getContentScale
{
	return [mCinderView contentScaleFactor];
}

- (void)setPos:(cinder::ivec2)pos
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

- (void)updatePosRelativeToPrimaryDisplay
{
	NSRect frame = [mWin frame];
	NSRect content = [mWin contentRectForFrameRect:frame];
	mPos = ivec2( content.origin.x, cinder::Display::getMainDisplay()->getHeight() - frame.origin.y - content.size.height );
}

- (void)close
{
	[mWin close];
}

- (NSString *)getTitle
{
	return mTitle;
}

- (void)setTitle:(NSString *)title
{
	if( mTitle )
		[mTitle release];

	mTitle = [title copy]; // title is cached because sometimes we need to restore it after changing window border styles
	[mWin setTitle:title];
}

- (BOOL)isBorderless
{
	return mBorderless;
}

- (void)setBorderless:(BOOL)borderless
{
	if( mBorderless == borderless )
		return;

	mBorderless = borderless;

	NSUInteger styleMask;
	if( mBorderless )
		styleMask = ( mResizable ) ? ( NSBorderlessWindowMask | NSResizableWindowMask ) : NSBorderlessWindowMask;
	else if( mResizable )
		styleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;
	else
		styleMask = NSTitledWindowMask;

	[mWin setStyleMask:styleMask];
	[mWin makeFirstResponder:mCinderView];
	[mWin makeKeyWindow];
	[mWin makeMainWindow];
	[mWin setHasShadow:( ! mBorderless )];

	// kludge: the titlebar buttons don't want to re-appear after coming back from borderless mode unless we resize the window.
	if( ! mBorderless && mResizable ) {
		ivec2 currentSize = mSize;
		[self setSize:currentSize + ivec2( 0, 1 )];
		[self setSize:currentSize];

		// restore title, which also seems to disappear after coming back from borderless
		if( mTitle )
			[mWin setTitle:mTitle];
	}
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

- (RendererRef)getRenderer
{
	if( mCinderView )
		return [mCinderView getRenderer];
	else
		return RendererRef();
}

- (void *)getNative
{
	return mCinderView;
}

- (void)windowDidBecomeMainNotification:(NSNotification *)notification
{
	mWindowRef->getRenderer()->makeCurrentContext( true );
}

- (void)windowMovedNotification:(NSNotification *)notification
{
	[self updatePosRelativeToPrimaryDisplay];
	[mAppImpl setActiveWindow:self];

	NSWindow *window = [notification object];
	NSScreen *screen = [window screen]; // This appears to be NULL in some scenarios
	if( screen ) {
		NSDictionary *dict = [screen deviceDescription];
		CGDirectDisplayID displayID = (CGDirectDisplayID)[[dict objectForKey:@"NSScreenNumber"] intValue];
		if( displayID != (std::dynamic_pointer_cast<cinder::DisplayMac>( mDisplay )->getCgDirectDisplayId()) ) {
			auto newDisplay = cinder::app::PlatformCocoa::get()->findFromCgDirectDisplayId( displayID );
			if( newDisplay ) {
				mDisplay = newDisplay;
				if( ! ((PlatformCocoa*)Platform::get())->isInsideModalLoop() ) {
					mWindowRef->emitDisplayChange();
				}
			}
		}
	}
	
	if( ! ((PlatformCocoa*)Platform::get())->isInsideModalLoop() ) {
		mWindowRef->emitMove();
	}
}

- (void)windowWillCloseNotification:(NSNotification *)notification
{
	// if this is the last window and we're set to terminate on last window, invalidate the timer
	if( [mAppImpl getNumWindows] == 1 && mAppImpl->mQuitOnLastWindowClosed ) {
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
	mSize = cinder::ivec2( nsSize.width, nsSize.height );

	ivec2 prevPos = mPos;
	[self updatePosRelativeToPrimaryDisplay];

	if( ! ((PlatformCocoa*)Platform::get())->isInsideModalLoop() ) {
		[mAppImpl setActiveWindow:self];
		mWindowRef->emitResize();
		
		// If the resize happened from top left, also signal that the Window moved.
		if( prevPos != mPos )
			mWindowRef->emitMove();
	}
}

- (void)draw
{
	if( ! ((PlatformCocoa*)Platform::get())->isInsideModalLoop() ) {
		[mAppImpl setActiveWindow:self];
		mWindowRef->emitDraw();
	}
}

- (void)mouseDown:(MouseEvent *)event
{
	if( ! ((PlatformCocoa*)Platform::get())->isInsideModalLoop() ) {
		[mAppImpl setActiveWindow:self];
		event->setWindow( mWindowRef );
		mWindowRef->emitMouseDown( event );
	}
}

- (void)mouseDrag:(MouseEvent *)event
{
	if( ! ((PlatformCocoa*)Platform::get())->isInsideModalLoop() ) {
		[mAppImpl setActiveWindow:self];
		event->setWindow( mWindowRef );
		mWindowRef->emitMouseDrag( event );
	}
}

- (void)mouseUp:(MouseEvent *)event
{
	if( ! ((PlatformCocoa*)Platform::get())->isInsideModalLoop() ) {
		[mAppImpl setActiveWindow:self];
		event->setWindow( mWindowRef );
		mWindowRef->emitMouseUp( event );
	}
}

- (void)mouseMove:(MouseEvent *)event
{
	if( ! ((PlatformCocoa*)Platform::get())->isInsideModalLoop() ) {
		[mAppImpl setActiveWindow:self];
		event->setWindow( mWindowRef );
		mWindowRef->emitMouseMove( event );
	}
}

- (void)mouseWheel:(MouseEvent *)event
{
	if( ! ((PlatformCocoa*)Platform::get())->isInsideModalLoop() ) {
		[mAppImpl setActiveWindow:self];
		event->setWindow( mWindowRef );
		mWindowRef->emitMouseWheel( event );
	}
}

- (void)keyDown:(KeyEvent *)event
{
	if( ! ((PlatformCocoa*)Platform::get())->isInsideModalLoop() ) {
		[mAppImpl setActiveWindow:self];
		event->setWindow( mWindowRef );
		mWindowRef->emitKeyDown( event );
	}
}

- (void)keyUp:(KeyEvent *)event
{
	if( ! ((PlatformCocoa*)Platform::get())->isInsideModalLoop() ) {
		[mAppImpl setActiveWindow:self];
		event->setWindow( mWindowRef );
		mWindowRef->emitKeyUp( event );
	}
}

- (void)touchesBegan:(TouchEvent *)event
{
	if( ! ((PlatformCocoa*)Platform::get())->isInsideModalLoop() ) {
		[mAppImpl setActiveWindow:self];
		event->setWindow( mWindowRef );
		mWindowRef->emitTouchesBegan( event );
	}
}

- (void)touchesMoved:(TouchEvent *)event
{
	if( ! ((PlatformCocoa*)Platform::get())->isInsideModalLoop() ) {
		[mAppImpl setActiveWindow:self];
		event->setWindow( mWindowRef );
		mWindowRef->emitTouchesMoved( event );
	}
}

- (void)touchesEnded:(TouchEvent *)event
{
	if( ! ((PlatformCocoa*)Platform::get())->isInsideModalLoop() ) {
		[mAppImpl setActiveWindow:self];
		event->setWindow( mWindowRef );
		mWindowRef->emitTouchesEnded( event );
	}
}

- (const std::vector<TouchEvent::Touch> &)getActiveTouches
{
	return [mCinderView getActiveTouches];
}

- (void)fileDrop:(FileDropEvent *)event
{
	if( ! ((PlatformCocoa*)Platform::get())->isInsideModalLoop() ) {
		[mAppImpl setActiveWindow:self];
		event->setWindow( mWindowRef );
		mWindowRef->emitFileDrop( event );
	}
}

- (app::WindowRef)getWindowRef
{
	return mWindowRef;
}

+ (WindowImplBasicCocoa *)instantiate:(Window::Format)winFormat withAppImpl:(AppImplMac *)appImpl
{
	WindowImplBasicCocoa *winImpl = [[WindowImplBasicCocoa alloc] init];

	winImpl->mAppImpl = appImpl;
	winImpl->mWindowRef = app::Window::privateCreate__( winImpl, winImpl->mAppImpl->mApp );
	winImpl->mDisplay = winFormat.getDisplay();
	winImpl->mHidden = NO;
	winImpl->mResizable = winFormat.isResizable();
	winImpl->mBorderless = winFormat.isBorderless();
	winImpl->mAlwaysOnTop = winFormat.isAlwaysOnTop();

	if( ! winImpl->mDisplay )
		winImpl->mDisplay = Display::getMainDisplay();

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
		styleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;
	else
		styleMask = NSTitledWindowMask;

	winImpl->mWin = [[CinderWindow alloc] initWithContentRect:winRect
													styleMask:styleMask
													  backing:NSBackingStoreBuffered
														defer:NO
													   screen:std::dynamic_pointer_cast<cinder::DisplayMac>( winImpl->mDisplay )->getNsScreen()];

	NSRect contentRect = [winImpl->mWin contentRectForFrameRect:[winImpl->mWin frame]];
	winImpl->mSize.x = (int)contentRect.size.width;
	winImpl->mSize.y = (int)contentRect.size.height;
	[winImpl updatePosRelativeToPrimaryDisplay];

	[winImpl->mWin setLevel:( winImpl->mAlwaysOnTop ? NSScreenSaverWindowLevel : NSNormalWindowLevel )];

	if( ! winFormat.getTitle().empty() )
		[winImpl setTitle:[NSString stringWithUTF8String:winFormat.getTitle().c_str()]];

	if( winFormat.isFullScreenButtonEnabled() )
		[winImpl->mWin setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];

	if( ! winFormat.getRenderer() )
		winFormat.setRenderer( appImpl->mApp->getDefaultRenderer()->clone() );

	// for some renderers, ok really just GL, we want an existing renderer so we can steal its context to share with. If this comes back with NULL that's fine - we're first
	app::RendererRef sharedRenderer = [appImpl findSharedRenderer:winFormat.getRenderer()];
	
	app::RendererRef renderer = winFormat.getRenderer();
	NSRect viewFrame = NSMakeRect( 0, 0, winImpl->mSize.x, winImpl->mSize.y );
	winImpl->mCinderView = [[CinderViewMac alloc] initWithFrame:viewFrame renderer:renderer sharedRenderer:sharedRenderer
															appReceivesEvents:YES
															highDensityDisplay:appImpl->mApp->isHighDensityDisplayEnabled()
															enableMultiTouch:appImpl->mApp->isMultiTouchEnabled()];

	[winImpl->mWin setDelegate:self];
	// add CinderView as subview of window's content view to avoid NSWindow warning: https://github.com/cinder/Cinder/issues/584
	[winImpl->mWin.contentView addSubview:winImpl->mCinderView];
	winImpl->mCinderView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

	[winImpl->mWin makeKeyAndOrderFront:nil];
	// after showing the window, the size may have changed (see NSWindow::constrainFrameRect) so we need to update our internal variable
	winImpl->mSize.x = (int)winImpl->mCinderView.frame.size.width;
	winImpl->mSize.y = (int)winImpl->mCinderView.frame.size.height;
	[winImpl->mWin setInitialFirstResponder:winImpl->mCinderView];
	[winImpl->mWin setAcceptsMouseMovedEvents:YES];
	[winImpl->mWin setOpaque:YES];
	[[NSNotificationCenter defaultCenter] addObserver:winImpl selector:@selector(windowDidBecomeMainNotification:) name:NSWindowDidBecomeMainNotification object:winImpl->mWin];
	[[NSNotificationCenter defaultCenter] addObserver:winImpl selector:@selector(windowMovedNotification:) name:NSWindowDidMoveNotification object:winImpl->mWin];
	[[NSNotificationCenter defaultCenter] addObserver:winImpl selector:@selector(windowWillCloseNotification:) name:NSWindowWillCloseNotification object:winImpl->mWin];
	[winImpl->mCinderView setNeedsDisplay:YES];
	[winImpl->mCinderView setDelegate:winImpl];

	// make this window the active window
	appImpl->mActiveWindow = winImpl;

	return [winImpl autorelease];
}

@end
