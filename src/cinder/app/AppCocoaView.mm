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

#import <Cocoa/Cocoa.h>

#include "cinder/app/AppCocoaView.h"
#include "cinder/app/CinderView.h"

@interface WindowImplCocoaView : NSObject<CinderViewDelegate,WindowImplCocoa> {
  @public
	AppImplCocoaView					*mAppImpl;
	CinderView							*mCinderView;
	cinder::app::WindowRef				mWindowRef;
	cinder::DisplayRef					mDisplay;
	cinder::Vec2i						mSize, mPos;
	BOOL								mBorderless, mAlwaysOnTop, mIsHidden;
}

- (WindowImplCocoaView*)init:(CinderView*)cinderView format:(cinder::app::Window::Format)winFormat appImpl:(AppImplCocoaView*)appImpl;
- (void)dealloc;
- (BOOL)isFullScreen;
- (void)setFullScreen:(BOOL)fullScreen options:(const cinder::app::FullScreenOptions *)options;
- (cinder::Vec2i)getSize;
- (void)setSize:(cinder::Vec2i)size;
- (cinder::Vec2i)getPos;
- (void)setPos:(cinder::Vec2i)pos;
- (float)getContentScale;
- (void)close;
- (NSString *)getTitle;
- (void)setTitle:(NSString *)title;
- (BOOL)isBorderless;
- (void)setBorderless:(BOOL)borderless;
- (BOOL)isAlwaysOnTop;
- (void)setAlwaysOnTop:(BOOL)alwaysOnTop;
- (cinder::DisplayRef)getDisplay;
- (cinder::app::RendererRef)getRenderer;
- (void*)getNative;

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

@end

@interface AppImplCocoaView : NSObject {
  @public
	NSTimer								*mAnimationTimer;
	cinder::app::AppCocoaView			*mApp;
	cinder::app::RendererRef			mDefaultRenderer;
	std::list<WindowImplCocoaView*>		mWindows;
	WindowImplCocoaView					*mActiveWindow;
	
	BOOL								mNeedsUpdate;
	BOOL								mFrameRateEnabled;
	float								mFrameRate;
}

- (AppImplCocoaView*)init:(cinder::app::AppCocoaView*)app defaultRenderer:(cinder::app::RendererRef)defaultRenderer;
- (WindowImplCocoaView*)setupCinderView:(CinderView*)cinderView renderer:(cinder::app::RendererRef)renderer;
- (void)startAnimationTimer;
- (void)applicationWillTerminate:(NSNotification *)notification;

- (float)getFrameRate;
- (void)setFrameRate:(float)aFrameRate;
- (void)disableFrameRate;
- (bool)isFrameRateEnabled;

- (cinder::app::RendererRef)findSharedRenderer:(cinder::app::RendererRef)match;
- (void)setActiveWindow:(WindowImplCocoaView*)activeWindow;
- (cinder::app::WindowRef)getWindow;
- (size_t)getNumWindows;
- (cinder::app::WindowRef)getWindowIndex:(size_t)index;
@end


@implementation WindowImplCocoaView

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

- (const std::vector<cinder::app::TouchEvent::Touch>&)getActiveTouches
{
	return [mCinderView getActiveTouches];
}

- (BOOL)isFullScreen
{
	return [mCinderView isFullScreen];
}

- (void)setFullScreen:(BOOL)fullScreen options:(const cinder::app::FullScreenOptions *)options
{
	[mCinderView setFullScreen:fullScreen options:options];
}

- (cinder::Vec2i)getSize
{
	return mSize;
}

- (void)setSize:(cinder::Vec2i)size
{
	mSize = size;
	NSSize newSize;
	newSize.width = mSize.x;
	newSize.height = mSize.y;
	[mCinderView setFrameSize:newSize];
}

- (NSString *)getTitle
{
	return [[mCinderView window] title];
}

- (void)setTitle:(NSString *)title
{
	[[mCinderView window] setTitle:title];
}

- (BOOL)isBorderless
{
	return [[mCinderView window] styleMask] == NSBorderlessWindowMask;
}

- (void)setBorderless:(BOOL)borderless
{ // NO-OP
}

- (BOOL)isAlwaysOnTop
{
	return [[mCinderView window] level] == NSNormalWindowLevel;
}

- (void)setAlwaysOnTop:(BOOL)alwaysOnTop
{
	[[mCinderView window] setLevel:(alwaysOnTop)?NSScreenSaverWindowLevel:NSNormalWindowLevel];
}

- (cinder::DisplayRef)getDisplay
{
	NSWindow *win = [mCinderView window];
	if( win ) {
		NSScreen *screen = [win screen];
		if( screen )
			return cinder::Display::findFromNsScreen( screen );
	}
	
	return cinder::DisplayRef();
}

- (cinder::Vec2i)getPos
{
	return mPos;
}

- (void)setPos:(cinder::Vec2i)pos
{
	mPos = pos;
	NSRect frame = [mCinderView frame];
	frame.origin.x = mPos.x;
	frame.origin.y = mPos.y;
	[mCinderView setFrame:frame];
}

- (float)getContentScale
{
	return [[mCinderView window] backingScaleFactor];
}

- (void)close
{
	[[mCinderView window] close];
}

- (void)resize
{
	[mAppImpl setActiveWindow:self];

	NSRect frame = [mCinderView frame];
	mSize = cinder::Vec2i( frame.size.width, frame.size.height );
	mPos = cinder::Vec2i( frame.origin.x, frame.origin.y );
	
	mWindowRef->emitResize();
}

- (void)hide
{
	[mCinderView setHidden:YES];
}

- (void)show
{
	[mCinderView setHidden:NO];
}

- (BOOL)isHidden
{
	return [mCinderView isHidden];
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

- (WindowImplCocoaView*)init:(CinderView*)cinderView format:(cinder::app::Window::Format)winFormat appImpl:(AppImplCocoaView*)appImpl
{
	self = [super init];

	mCinderView = cinderView;
	mAppImpl = appImpl;
	mWindowRef = cinder::app::Window::privateCreate__( self, mAppImpl->mApp );

	NSRect contentRect = [mCinderView frame];
	mSize.x = (int)contentRect.size.width;
	mSize.y = (int)contentRect.size.height;
	mPos = cinder::Vec2i( contentRect.origin.x, contentRect.origin.y );
	
	// for some renderers, ok really just GL, we want an existing renderer so we can steal its context to share with. If this comes back with NULL that's fine - we're first
	cinder::app::RendererRef sharedRenderer = [appImpl findSharedRenderer:winFormat.getRenderer()];

	[mCinderView setupRendererWithFrame:contentRect renderer:winFormat.getRenderer() sharedRenderer:sharedRenderer];

	[mCinderView setDelegate:self];
	[mCinderView setNeedsDisplay:YES];

	return self;
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[super dealloc];
}

@end

@implementation AppImplCocoaView

- (AppImplCocoaView*)init:(cinder::app::AppCocoaView*)app defaultRenderer:(cinder::app::RendererRef)defaultRenderer
{
	self = [super init];
	
	mApp = app;
	mDefaultRenderer = defaultRenderer;
	mAnimationTimer = nil;
	mFrameRateEnabled = mApp->getSettings().isFrameRateEnabled();
	mFrameRate = mApp->getSettings().getFrameRate();
	
	// register for notification of application termination
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(applicationWillTerminate:) name:NSApplicationWillTerminateNotification object:nil];
	
	return self;
}

// Returns a pointer to a Renderer of the same type if any existing Windows have one of the same type
- (cinder::app::RendererRef)findSharedRenderer:(cinder::app::RendererRef)match
{
	for( auto &win : mWindows ) {
		cinder::app::RendererRef renderer = [win->mCinderView getRenderer];
		if( typeid(renderer) == typeid(match) )
			return renderer;
	}
	
	return cinder::app::RendererRef();
}

- (WindowImplCocoaView*)setupCinderView:(CinderView*)cinderView renderer:(cinder::app::RendererRef)renderer
{
	cinder::app::Window::Format format( renderer );
	//WindowImplCocoaView*)init:(cinder::app::Window::Format)winFormat withAppImpl:(AppImplCocoaView*)appImpl;
	mWindows.push_back( [[WindowImplCocoaView alloc] init:cinderView format:format appImpl:self] );
	return mWindows.back();
}

- (size_t)getNumWindows
{
	return mWindows.size();
}

- (cinder::app::WindowRef)getWindowIndex:(size_t)index
{
	if( index >= mWindows.size() )
		return cinder::app::WindowRef();
	
	std::list<WindowImplCocoaView*>::iterator iter = mWindows.begin();
	std::advance( iter, index );
	return (*iter)->mWindowRef;
}

- (cinder::app::WindowRef)getWindow
{
	return mActiveWindow->mWindowRef;
}

- (void)setActiveWindow:(WindowImplCocoaView*)activeWindow
{
	mActiveWindow = activeWindow;
}

- (void)startAnimationTimer
{
	if( mAnimationTimer && [mAnimationTimer isValid] )
		[mAnimationTimer invalidate];
	
	float interval = ( mFrameRateEnabled ) ? 1.0f / mFrameRate : 0.001f;
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
	
	// all live windows are ready to draw now that we've fired an update
	for( auto &win : mWindows ) {
		[win->mCinderView setReadyToDraw:YES];
	}	
	
	// walk all windows and draw them
	for( auto &win : mWindows ) {
		[self setActiveWindow:win];
		[win->mCinderView draw];
	}
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	mApp->emitShutdown();
	delete mApp;
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

@end

namespace cinder { namespace app {

AppCocoaView::AppCocoaView()
	: App()
{
}

AppCocoaView::Settings::Settings()
	: App::Settings()
{
}

void AppCocoaView::prepareLaunch( RendererRef defaultRenderer )
{
	App::sInstance = this;
	prepareSettings( &mSettings );
	mImpl = [[AppImplCocoaView alloc] init:this defaultRenderer:defaultRenderer];
}

void AppCocoaView::setupCinderView( CinderView *cinderView, cinder::app::RendererRef renderer )
{
	[cinderView setApp:this];
	[mImpl setupCinderView:cinderView renderer:renderer];
}

void AppCocoaView::launch( const char *title, int argc, char * const argv[] )
{
	if( ! mImpl->mWindows.empty() )
		[mImpl setActiveWindow:*(mImpl->mWindows.begin())];
	else
		[mImpl setActiveWindow:nil];
	privateSetup__();

	// issue initial resizes
	for( auto &win : mImpl->mWindows ) {
		[win->mCinderView makeCurrentContext];
		[mImpl setActiveWindow:win];
		win->mWindowRef->emitResize();
	}	
	
	[mImpl startAnimationTimer];
}

void AppCocoaView::quit()
{
	[NSApp terminate:mImpl];
}

float AppCocoaView::getFrameRate() const
{
	return [mImpl getFrameRate];
}

void AppCocoaView::setFrameRate( float frameRate )
{
	[mImpl setFrameRate:frameRate];
}

void AppCocoaView::disableFrameRate()
{
	[mImpl disableFrameRate];
}

bool AppCocoaView::isFrameRateEnabled() const
{
	return [mImpl isFrameRateEnabled];
}

fs::path AppCocoaView::getAppPath() const
{
	NSString *resultPath = [[NSBundle mainBundle] bundlePath];
	std::string result;
	result = [resultPath cStringUsingEncoding:NSUTF8StringEncoding];
	return result;
}

size_t AppCocoaView::getNumWindows() const
{
	return [mImpl getNumWindows];
}

WindowRef AppCocoaView::getWindow() const
{
	return [mImpl getWindow];
}

WindowRef AppCocoaView::getWindowIndex( size_t index ) const
{
	return [mImpl getWindowIndex:index];
}

} } // namespace cinder::app