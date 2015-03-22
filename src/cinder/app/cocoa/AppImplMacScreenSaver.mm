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

#import "cinder/app/cocoa/AppImplMacScreenSaver.h"
#include "cinder/app/cocoa/PlatformCocoa.h"
#include "cinder/CinderAssert.h"

#import <Foundation/NSThread.h>

static AppImplMacScreenSaver *sAppImplInstance = nil;
static std::unique_ptr<cinder::app::AppScreenSaver::Settings> sSettings;
static void initSettings();
static AppImplMacScreenSaver* getAppImpl();
static bool sFirstView = true; // records whether a call is the first to initView; reset to true at stop animation

@implementation WindowImplCocoaScreenSaver

//////////////////////////////////////////////////////////////////////////////////////
// ScreenSaverView methods
- (void)instantiateView:(NSRect)rect
{
	mResizeCalled = NO;
	mHasDrawnSinceLastUpdate = YES; // in order to force an update later
	
	initSettings();
	
	bool blankingWindow = sSettings->isSecondaryDisplayBlankingEnabled() && ( ! mIsMainView );

	if( ! blankingWindow ) {
		cinder::app::RendererRef renderer = sSettings->getDefaultRenderer()->clone();
		// we only want to look for a shared renderer if we're not the first window
		cinder::app::RendererRef sharedRenderer;
		if( sAppImplInstance )
			sharedRenderer = [sAppImplInstance findSharedRenderer:renderer];
		mCinderView = [[CinderViewMac alloc] initWithFrame:rect renderer:renderer sharedRenderer:sharedRenderer
										appReceivesEvents:NO highDensityDisplay:sSettings->isHighDensityDisplayEnabled() enableMultiTouch:NO];
		[mCinderView setDelegate:self];

		[self setAutoresizesSubviews:YES];
		[self addSubview:mCinderView];

		[mCinderView release]; // addSubview incs the retainCount and the parent assumes ownership
		
		[self setAnimationTimeInterval:1 / sSettings->getFrameRate()];
	}
	else {
		// a blanking window is just a black window with no CinderView
		mCinderView = nil;
		[self setAnimationTimeInterval:-1];
	}
	
	// now that we've initialized the window, for impl instantiation
	getAppImpl();
}

- (id)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
	self = [super initWithFrame:frame isPreview:isPreview];
	
	mPreview = isPreview;
	mCinderView = nil;

    return self;
}

- (void)setFrameSize:(NSSize)newSize
{
	if( ! sAppImplInstance )
		return;

	[super setFrameSize:newSize];
	if( mCinderView )
		[mCinderView setFrameSize:newSize];
}

- (void)drawRect:(NSRect)rect
{
	if( ! sAppImplInstance ) {
		[super drawRect:rect]; // draws black by default
		return;
	}

	if( ( ! mCinderView ) || ( ! mCinderView.readyToDraw ) ) {
		[super drawRect:rect]; // draws black by default
		return;
	}
}

- (void)animateOneFrame
{

	if( ! sAppImplInstance ) // ignore until we have an instance
		return;

	[getAppImpl() animateOneFrame:self];
}

- (BOOL)isOpaque
{
	return YES;
}

- (void)startAnimation
{
	NSRect newFrame = [self frame];
	newFrame.origin = NSZeroPoint;
	mIsMainView = sFirstView;
	sFirstView = false;
	[self instantiateView:newFrame];
	
	mWindowRef = cinder::app::Window::privateCreate__( self, getAppImpl()->mApp );
	// this needs to be called after instantiateView
	[getAppImpl() addWindow:self];

	[super startAnimation];
}

- (void)stopAnimation
{
	[super stopAnimation];
	[getAppImpl() removeCinderView:self];
	sFirstView = true;
}

- (BOOL)hasConfigureSheet
{
	initSettings();
	
    return sSettings->getProvidesMacConfigDialog();
}

- (NSWindow*)configureSheet
{
	initSettings();

	if( sSettings->getProvidesMacConfigDialog() )
		return static_cast<NSWindow*>( getAppImpl()->mApp->createMacConfigDialog() );
	else
	    return nil;
}

//////////////////////////////////////////////////////////////////////////////////////
// WindowImplCocoa methods
- (BOOL)isFullScreen
{
	return YES;
}

- (void)setFullScreen:(BOOL)fullScreen options:(const cinder::app::FullScreenOptions *)options
{ // NO-OP
}

- (cinder::ivec2)getSize
{
	NSRect frame = [self frame];
	return cinder::ivec2( frame.size.width, frame.size.height );
}

- (void)setSize:(cinder::ivec2)size
{ // NO-OP
}

- (cinder::ivec2)getPos
{
	return cinder::ivec2( 0 );
}

- (float)getContentScale
{
	return [[mCinderView window] backingScaleFactor];
}

- (void)setPos:(cinder::ivec2)pos
{ // NO-OP
}

- (void)close
{ // NO-OP
}

- (std::string)getTitle
{
	return std::string();
}

- (void)setTitle:(std::string)title
{ // NO-OP
}

- (BOOL)isBorderless
{
	return YES;
}

- (void)setBorderless:(BOOL)borderless
{ // NO-OP
}

- (BOOL)isAlwaysOnTop
{
	return YES;
}

- (void)setAlwaysOnTop:(BOOL)alwaysOnTop
{ // NO-OP
}

- (cinder::DisplayRef)getDisplay
{
	if( ! mDisplay )
		mDisplay = cinder::app::PlatformCocoa::get()->findFromNsScreen( [[self window] screen] );
	
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
	return self;
}

- (void)hide
{ // NO-OP
}

- (void)show
{ // NO-OP
}

//////////////////////////////////////////////////////////////////////////////////////
// CinderViewDelegate methods
- (void)resize
{
	[getAppImpl() setActiveWindow:self];
	mWindowRef->emitResize();
}

- (void)draw
{
	if( [self isAnimating] ) {
		[getAppImpl() setActiveWindow:self];
		mWindowRef->emitDraw();
	}
}

- (cinder::app::WindowRef)getWindowRef
{
	return mWindowRef;
}

- (void)mouseDown:(cinder::app::MouseEvent*)event
{ // NO-OP
}

- (void)mouseDrag:(cinder::app::MouseEvent*)event
{ // NO-OP
}

- (void)mouseUp:(cinder::app::MouseEvent*)event
{ // NO-OP
}

- (void)mouseMove:(cinder::app::MouseEvent*)event
{ // NO-OP
}

- (void)mouseWheel:(cinder::app::MouseEvent*)event
{ // NO-OP
}

- (void)keyDown:(cinder::app::KeyEvent*)event
{ // NO-OP
}

- (void)keyUp:(cinder::app::KeyEvent*)event
{ // NO-OP
}

- (void)touchesBegan:(cinder::app::TouchEvent*)event
{ // NO-OP
}

- (void)touchesMoved:(cinder::app::TouchEvent*)event
{ // NO-OP
}

- (void)touchesEnded:(cinder::app::TouchEvent*)event
{ // NO-OP
}

- (const std::vector<cinder::app::TouchEvent::Touch>&)getActiveTouches
{
	if( mCinderView )
		return [mCinderView getActiveTouches];
	else { // this should never actually be called
		static std::vector<cinder::app::TouchEvent::Touch> emptyTouches;
		return emptyTouches;
	}
}

- (void)fileDrop:(cinder::app::FileDropEvent*)event
{ // NO-OP
}

@end

static void initSettings()
{
	if( ! sSettings ) {
		sSettings = std::unique_ptr<cinder::app::AppScreenSaver::Settings>( new cinder::app::AppScreenSaver::Settings() );
		ScreenSaverSettingsMethod( sSettings.get() );
	}
}

static AppImplMacScreenSaver* getAppImpl()
{
	if( ! sAppImplInstance ) {
		initSettings();
		
		sAppImplInstance = [[AppImplMacScreenSaver alloc] init];
		sAppImplInstance->mApp = ScreenSaverFactoryMethod( sAppImplInstance, sSettings.get() );
		sAppImplInstance->mFrameRate = sSettings->getFrameRate();
		sAppImplInstance->mSetupCalled = NO;
	}
	
	return sAppImplInstance;
}

//////////////////////////////////////////////////////////////////////////////////////
// AppImplMacScreenSaver

@implementation AppImplMacScreenSaver

- (AppImplMacScreenSaver*)init
{
	self = [super init];
	
	mApp = NULL;
	
	return self;
}

- (void)addWindow:(WindowImplCocoaScreenSaver*)windowImpl
{
	[windowImpl retain];
	mWindows.push_back( windowImpl );	
}

- (cinder::app::RendererRef)findSharedRenderer:(cinder::app::RendererRef)sharedRenderer
{
	if( ! sharedRenderer )
		return cinder::app::RendererRef();
	
	for( const auto &win : mWindows ) {
		auto ren = win->mWindowRef->getRenderer();
		if( ren && ( typeid(*ren) == typeid(*sharedRenderer) ) )
			return ren;
	}

	return cinder::app::RendererRef();
}

- (BOOL)isPreview
{
	return mActiveWindow->mPreview;
}

- (size_t)getNumWindows
{
	return mWindows.size();
}

- (cinder::app::WindowRef)getWindowIndex:(size_t)index
{
	if( index >= mWindows.size() )
		throw cinder::app::ExcInvalidWindow();
	
	std::list<WindowImplCocoaScreenSaver*>::iterator iter = mWindows.begin();
	std::advance( iter, index );
	return (*iter)->mWindowRef;
}

- (cinder::app::WindowRef)getWindow
{
	if( ! mActiveWindow )
		throw cinder::app::ExcInvalidWindow();
	else
		return mActiveWindow->mWindowRef;
}

- (void)setActiveWindowToMain
{
	for( auto &win : mWindows ) {
		if( win->mIsMainView ) {
			[self setActiveWindow:win];
			break;
		}
	}
}

- (void)setActiveWindow:(WindowImplCocoaScreenSaver*)activeWindow
{
	mActiveWindow = activeWindow;
	[mActiveWindow->mCinderView makeCurrentContext];
}

- (float)getFrameRate
{
	return mFrameRate;
}

- (void)setFrameRate:(float)frameRate
{
	mFrameRate = frameRate;
	if( mFrameRate <= 0 ) // sub-zero framerate is no good
		mFrameRate = 0.0001f;
	float interval = 1.0f / mFrameRate;

	for( auto &win : mWindows )
		[win setAnimationTimeInterval:interval];
}

- (cinder::fs::path)getAppPath
{
	return [[[NSBundle bundleForClass:[self class]] bundlePath] UTF8String];
}

- (void)animateOneFrame:(WindowImplCocoaScreenSaver*)callee
{
	if( ! mSetupCalled ) {
		// fire setup for the first time
		[self setActiveWindow:callee];
		mApp->privateSetup__();
		mSetupCalled = YES;
	}

	// mark all our CinderViews as ready to draw after calling resize where necessary; this only has any effect the first time
	for( auto &win : mWindows ) {
		if( win->mCinderView && ( ! win->mResizeCalled ) ) {
			[self setActiveWindow:win];
			win->mWindowRef->emitResize();
			win->mResizeCalled = YES;
		}
	
		if( win->mCinderView )
			win->mCinderView.readyToDraw = [win isAnimating];
	}

	// determine if all of our windows have drawn since the last update, and update if so
	BOOL allWindowsDrawn = YES;
	for( auto &win : mWindows ) {
		if( win->mCinderView && [win isAnimating] && ( ! win->mHasDrawnSinceLastUpdate ) ) {
			allWindowsDrawn = NO;
			break;
		}
	}

	if( allWindowsDrawn ) {
		[getAppImpl() setActiveWindowToMain];
		mApp->privateUpdate__();
		for( auto &win : mWindows )
			win->mHasDrawnSinceLastUpdate = NO;
	}
	
	if( ! callee->mHasDrawnSinceLastUpdate ) {
		if( callee->mCinderView )
			[callee setNeedsDisplay:YES];
		callee->mHasDrawnSinceLastUpdate = YES;
	}
}

- (void)removeCinderView:(WindowImplCocoaScreenSaver*)win
{
	// issue a close signal against this window
	[self setActiveWindow:win];
	win->mWindowRef->emitClose();

	// release its Cinder View
	[win->mCinderView removeFromSuperview];
	win->mCinderView = nil;
	
	// if no Windows have CinderViews anymore that means we need to finalCleanup
	BOOL foundACinderView = NO;
	for( auto &win : mWindows ) {
		if( win->mCinderView != nil )
			foundACinderView = YES;
	}
	
	if( ! foundACinderView ) {
		[self finalCleanup];
		return;
	}
	else { // if we're not going away, let's make sure we have a main view
		for( auto &win : mWindows ) {
			if( win->mCinderView != nil )
				win->mIsMainView = YES;
		}
	}
}

- (void)finalCleanup
{
	if( sAppImplInstance ) {
		mApp->emitCleanup();
		delete sAppImplInstance->mApp;
		[sAppImplInstance release];
		sAppImplInstance = nil;
	}
}

@end