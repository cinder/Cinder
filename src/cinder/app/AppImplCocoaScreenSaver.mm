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

/* 	OK - this is complicated. We get (potentially) multiple initWithFrame: calls; one per screen.
	In order to accommodate this, we need to instantiate a singleton AppImpl in initWithFrame:.
	The next call we'll receive will be an animateOneFrame:, which we pass on up to the appImpl.
	It in turn will call App::setup() if it hasn't been called yet. It also ignores all animateOneFrame:
	calls that aren't for the main window. It calls update() once, and then draw() once per Window.
*/

#import "cinder/app/AppImplCocoaScreenSaver.h"

#import <Foundation/NSThread.h>

static AppImplCocoaScreenSaver *sAppImplInstance = nil;
AppImplCocoaScreenSaver* getAppImpl( BOOL *isMainWindow );


/* Yet another mess. How do we fire our app's shutdown() method and delete it?
   The class below fires inconsistently when previewing the screensaver, but consistently when it
   is "live". However the WindowCloseNotification fires consistently in preview and *not* when we're live.
   One of the two should fire and allow us to [sAppImplInstance finalCleanup].
   For my own records, I have attempted to use viewDidUnload, NSThreadWillExitNotification and
   NSApplicationWillTerminate, but all are inconsistent.
   Why can't we do this with [ScreenSaverView stopAnimation]? When ending the preview, we have only stopped
   animations, but we'll shortly thereafter be started again as a preview, so we can't know whether a given
   [stopAnimation:] is the last one we'll receive or not.
*/
class ShutdownStatic {
  public:
  	~ShutdownStatic() {
		if( sAppImplInstance ) {
			[sAppImplInstance finalCleanup];
		}
	}
};
static ShutdownStatic shutdownStatic;

@implementation WindowImplCocoaScreenSaver

//////////////////////////////////////////////////////////////////////////////////////
// ScreenSaverView methods
- (id)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
	self = [super initWithFrame:frame isPreview:isPreview];
	// This returns a pointer to the app, instantiating it if we are the first ScreenSaverView.
	BOOL isMainWindow;
	mAppImpl = getAppImpl( &isMainWindow );

	NSRect newFrame = frame;
	// Slam the origin values
	newFrame.origin = NSZeroPoint;

	mIsMainView = isMainWindow;
	mResizeCalled = NO;
	mReadyToBeDestroyed = NO;
	mHasDrawnSinceLastUpdate = YES; // in order to force an update later
	// we may get another initFrame call later with a new preview value. But whatever the most recent isPreview value is is the correct one
	mPreview = isPreview;
	BOOL blankingWindow = mAppImpl->mApp->getSettings().isSecondaryDisplayBlankingEnabled() && ( ! isMainWindow );

	if( ! blankingWindow ) {
		cinder::app::RendererRef renderer = mAppImpl->mApp->getDefaultRenderer()->clone();
		cinder::app::RendererRef sharedRenderer = mAppImpl->mApp->findSharedRenderer( renderer );
		mCinderView = [[CinderView alloc] initWithFrame:newFrame app:mAppImpl->mApp renderer:renderer sharedRenderer:sharedRenderer];
		[mCinderView setDelegate:self];
		mWindowRef = cinder::app::Window::privateCreate__( self, mAppImpl->mApp );

		[self setAutoresizesSubviews:YES];
		[self addSubview:mCinderView];

		[mCinderView release]; // addSubview incs the retainCount and the parent assumes ownership
		
		[self setAnimationTimeInterval:1 / [mAppImpl getFrameRate]];
	}
	else {
		// a blanking window is just a black window with no CinderView
		mCinderView = nil;
		[self setAnimationTimeInterval:-1];
	}

	[mAppImpl addWindow:self];
	if( mIsMainView )
		[mAppImpl setActiveWindow:self];

    return self;
}

-(void)windowClosing:(NSNotification*)notification {
	if( sAppImplInstance && ( [self isPreview] == 1 ) ) {
		[sAppImplInstance finalCleanup];
	}
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
	if( ! sAppImplInstance )
		return;

	if( ( ! mCinderView ) || ( ! mCinderView.readyToDraw ) ) {
		[super drawRect:rect]; // draws black by default
		return;
	}
}

- (void)animateOneFrame
{
	if( ! sAppImplInstance ) // ignore until we have an instance
		return;

	[mAppImpl animateOneFrame:self];
}

- (BOOL)isOpaque
{
	return YES;
}

- (void)startAnimation
{
	if( ! sAppImplInstance )
		return;

	// see note on ShutdownStatic
	NSWindow *parentWindow = [self window];
	if( parentWindow ) {
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(windowClosing:) name:NSWindowWillCloseNotification object:parentWindow];
	}


	[super startAnimation];
	bool found = false;
	for( std::list<WindowImplCocoaScreenSaver*>::const_iterator winIt = sAppImplInstance->mWindows.begin(); winIt != sAppImplInstance->mWindows.end(); ++winIt )
		if( (*winIt)->mIsMainView )
			found = true;
	
	if( ! found ) {
		mIsMainView = YES;
	}
}

- (void)stopAnimation
{
	if( ! sAppImplInstance )
		return;

	[super stopAnimation];

	mIsMainView = NO;
}

- (void)viewWillMoveToWindow:(NSWindow*)win
{
	[super viewWillMoveToWindow:win];
	if( win == nil ) {
		mIsMainView = NO;
		// have we already been here?
		mReadyToBeDestroyed = YES;
	}
}

- (BOOL)hasConfigureSheet
{
	if( ! sAppImplInstance )
		return NO;

    return mAppImpl->mApp->getSettings().getProvidesMacConfigDialog();
}

- (NSWindow*)configureSheet
{
	if( ! sAppImplInstance )
		return nil;

	if( mAppImpl->mApp->getSettings().getProvidesMacConfigDialog() )
		return static_cast<NSWindow*>( mAppImpl->mApp->createMacConfigDialog() );
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

- (cinder::Vec2i)getSize
{
	NSRect frame = [self frame];
	return cinder::Vec2i( frame.size.width, frame.size.height );
}

- (void)setSize:(cinder::Vec2i)size
{ // NO-OP
}

- (cinder::Vec2i)getPos
{
	return cinder::Vec2i::zero();
}

- (float)getContentScale
{
	return [[mCinderView window] backingScaleFactor];
}

- (void)setPos:(cinder::Vec2i)pos
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
		mDisplay = cinder::Display::findFromNsScreen( [[self window] screen] );
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
	[mAppImpl setActiveWindow:self];
	mWindowRef->emitResize();
}

- (void)draw
{
	if( [self isAnimating] ) {
		[mAppImpl setActiveWindow:self];
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

-(void)dealloc
{
	[super dealloc];
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

@end


AppImplCocoaScreenSaver* getAppImpl( BOOL *isMainWindow )
{
	BOOL firstCall = NO;
	if( ! sAppImplInstance ) {
		sAppImplInstance = [[AppImplCocoaScreenSaver alloc] init];
		sAppImplInstance->mApp = ScreenSaverFactoryMethod( (void*)sAppImplInstance );
		firstCall = YES;
		sAppImplInstance->mFrameRate = sAppImplInstance->mApp->getSettings().getFrameRate();
	}
	
	*isMainWindow = YES;
	for( const auto &win : sAppImplInstance->mWindows ) {
		if( win->mIsMainView )
			*isMainWindow = NO;
	}
	return sAppImplInstance;
}

@implementation AppImplCocoaScreenSaver


- (AppImplCocoaScreenSaver*)init
{
	self = [super init];
	
	mApp = NULL;
	mSetupCalled = NO;
	
	return self;
}

- (void)addWindow:(WindowImplCocoaScreenSaver*)windowImpl
{
	// we also clean up any "old" windows. We do this here so that context sharing can have inherited their contexts
	for( auto winIt = sAppImplInstance->mWindows.begin(); winIt != sAppImplInstance->mWindows.end(); ) {
		if( (*winIt)->mReadyToBeDestroyed ) {
			[self setActiveWindow:*winIt];
			(*winIt)->mWindowRef->emitClose();
			[(*winIt) release];
			winIt = sAppImplInstance->mWindows.erase( winIt );
		}
		else
			++winIt;
	}

	[windowImpl retain];
	mWindows.push_back( windowImpl );
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
		if( [win isAnimating]  && ( ! win->mHasDrawnSinceLastUpdate ) ) {
			allWindowsDrawn = NO;
			break;
		}
	}

	if( allWindowsDrawn ) {
		[self setActiveWindow:callee];
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

- (void)finalCleanup
{
	if( sAppImplInstance ) {
		// emit closes on all our windows
		for( auto &win : mWindows ) {
			[self setActiveWindow:win];
			win->mWindowRef->emitClose();
			[win release];
		}
		mApp->emitShutdown();	
		delete sAppImplInstance->mApp;
		[sAppImplInstance release];
		sAppImplInstance = nil;
	}
}

@end