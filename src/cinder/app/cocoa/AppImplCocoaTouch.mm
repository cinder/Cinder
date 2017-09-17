/*
 Copyright (c) 2015, The Barbarian Group
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

#import "cinder/app/cocoa/AppImplCocoaTouch.h"
#include "cinder/app/cocoa/PlatformCocoa.h"

using namespace cinder;
using namespace cinder::app;

// ----------------------------------------------------------------------------------------------------
// MARK: - AppDelegateImpl
// ----------------------------------------------------------------------------------------------------

@implementation AppDelegateImpl

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	mApp = AppCocoaTouch::get();
	mAppImpl = mApp->privateGetImpl();

	[UIApplication sharedApplication].statusBarHidden = mAppImpl->mStatusBarShouldHide;

	for( auto &windowImpl : mAppImpl->mWindows )
		[windowImpl finishLoad];

	mApp->privateSetup__();
	mAppImpl->mSetupHasFired = YES;

	return YES;
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	mApp->emitDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	mApp->emitWillEnterForeground();
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	[mAppImpl stopAnimation];
	mApp->emitWillResignActive();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	[mAppImpl startAnimation];
	mApp->emitDidBecomeActive();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	mApp->emitCleanup();
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
	mApp->emitMemoryWarning();
}

@end // AppDelegateImpl

// ----------------------------------------------------------------------------------------------------
// MARK: - AppImplCocoaTouch
// ----------------------------------------------------------------------------------------------------

@implementation AppImplCocoaTouch

- (AppImplCocoaTouch *)init:(AppCocoaTouch *)app settings:(const AppCocoaTouch::Settings &)settings
{
	self = [super init];

	mApp = app;
	mAnimationFrameInterval = 1;
	mAnimating = NO;
	mUpdateHasFired = NO;
	mSetupHasFired = NO;
	mProximityStateIsClose = NO;
	mIsUnplugged = NO;
	mStatusBarShouldHide = ! settings.isStatusBarEnabled();
	mBatteryLevel = -1.0f;

	NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
	[center addObserver:self selector:@selector(screenDidConnect:) name:UIScreenDidConnectNotification object:nil];
	[center addObserver:self selector:@selector(screenDidDisconnect:) name:UIScreenDidDisconnectNotification object:nil];
	[center addObserver:self selector:@selector(screenModeDidChange:) name:	UIScreenModeDidChangeNotification object:nil];

	mAnimationFrameInterval = std::max<float>( 1.0f, floor( 60.0f / settings.getFrameRate() + 0.5f ) );

	// build our list of requested formats; an empty list implies we should make the default window format
	std::vector<Window::Format> formats( settings.getWindowFormats() );
	if( formats.empty() )
		formats.push_back( settings.getDefaultWindowFormat() );

	for( auto format : formats ) {
		if( ! format.getRenderer() )
			format.setRenderer( mApp->getDefaultRenderer()->clone() );

		RendererRef sharedRenderer = [self findSharedRenderer:format.getRenderer()];
		mWindows.push_back( [[WindowImplCocoaTouch alloc] initWithFormat:format withAppImpl:self sharedRenderer:sharedRenderer] );
	}

	[self setActiveWindow:mWindows.front()];

	// lastly, ensure the first window is the currently active context
	[mWindows.front()->mCinderView makeCurrentContext];

	return self;
}

- (void)displayLinkDraw:(id)sender
{
	// issue initial resizes if that's necessary (only once)
	for( auto &win : mWindows ) {
		if( ! win->mResizeHasFired )
			[win resize];
	}

	mApp->privateUpdate__();
	mUpdateHasFired = YES;

	for( auto &win : mWindows ) {
		[win->mCinderView drawView];
	}
}

- (void)proximityStateChange:(NSNotificationCenter *)notification
{
	mProximityStateIsClose = ([[UIDevice currentDevice] proximityState] == YES);
	mApp->emitSignalProximitySensor( mProximityStateIsClose );
}

- (void)batteryStateChange:(NSNotificationCenter *)notification
{
	bool unplugged = [UIDevice currentDevice].batteryState == UIDeviceBatteryStateUnplugged;
	if( unplugged != mIsUnplugged ) {
		mIsUnplugged = unplugged;
		mApp->emitSignalBatteryState( mIsUnplugged );
	}
}

- (void)batteryLevelChange:(NSNotificationCenter *)notification
{
	mBatteryLevel = [UIDevice currentDevice].batteryLevel;
}

- (void)startAnimation
{
	if( ! mAnimating ) {
		mDisplayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(displayLinkDraw:)];
		[mDisplayLink setFrameInterval:mAnimationFrameInterval];
		[mDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];

		mAnimating = TRUE;
	}
}

- (void)stopAnimation
{
	if( mAnimating ) {
		[mDisplayLink invalidate];
		mDisplayLink = nil;

		mAnimating = FALSE;
	}
}

- (void)screenDidConnect:(NSNotification *)notification
{
	UIScreen *screen = (UIScreen *)[notification object];
	DisplayRef connected = app::PlatformCocoa::get()->findDisplayFromUiScreen( screen );
	if( ! connected ) {
		app::PlatformCocoa::get()->addDisplay( DisplayRef( new DisplayCocoaTouch( screen ) ) );
	}
}

- (void)screenDidDisconnect:(NSNotification *)notification
{
	DisplayRef disconnected = app::PlatformCocoa::get()->findDisplayFromUiScreen( (UIScreen *)[notification object] );
	if( disconnected ) {
		app::PlatformCocoa::get()->removeDisplay( disconnected );
	}
}

- (void)screenModeDidChange:(NSNotification *)notification
{
	NSLog(@"A screen got disconnected: %@", [notification object]);
	//	cinder::Display::markDisplaysDirty();
}

- (void)updatePowerManagement
{
	if( ! mApp->isPowerManagementEnabled() ) {
		[UIApplication sharedApplication].idleTimerDisabled = NO; // setting to NO -> YES seems to be necessary rather than just direct to YES
		[UIApplication sharedApplication].idleTimerDisabled = YES;
	}
	else
		[UIApplication sharedApplication].idleTimerDisabled = NO;
}

- (void)dealloc
{
	for( auto &win : mWindows )
		[win release];

	[super dealloc];
}

// Returns a pointer to a Renderer of the same type if any existing Windows have one of the same type
- (RendererRef)findSharedRenderer:(RendererRef)match
{
	for( auto &win : mWindows ) {
		RendererRef renderer = [win->mCinderView getRenderer];
		if( typeid(renderer) == typeid(match) )
			return renderer;
	}

	return RendererRef();
}

- (WindowImplCocoaTouch*)getDeviceWindow
{
	for( auto &win : mWindows ) {
		if ( [win getDisplay] == cinder::Display::getMainDisplay() )
			return win;
	}
	
	return nil;
}

- (WindowRef)createWindow:(Window::Format)format
{
	if( ! format.getRenderer() )
		format.setRenderer( mApp->getDefaultRenderer()->clone() );

	RendererRef sharedRenderer = [self findSharedRenderer:format.getRenderer()];
	mWindows.push_back( [[WindowImplCocoaTouch alloc] initWithFormat:format withAppImpl:self sharedRenderer:sharedRenderer] );
	
	[mWindows.back() finishLoad];
	
	return mWindows.back()->mWindowRef;
}

- (void)setActiveWindow:(WindowImplCocoaTouch*)win
{
	mActiveWindow = win;
}

- (void)setFrameRate:(float)frameRate
{
	mAnimationFrameInterval = std::max<float>( 1.0f, floor( 60.0f / frameRate + 0.5f ) );
	if( mDisplayLink )
		[mDisplayLink setFrameInterval:mAnimationFrameInterval];
}

- (void)showKeyboard:(const AppCocoaTouch::KeyboardOptions &)options
{
	if( ! mWindows.empty() )
		[mWindows.front() showKeyboard:options];
}

- (void)hideKeyboard
{
	if( ! mWindows.empty() )
		[mWindows.front() hideKeyboard];
}

- (bool)isKeyboardVisible
{
	if( ! mWindows.empty() )
		[mWindows.front() hideKeyboard];

	return false;
}

- (const std::string &)getKeyboardString
{
	return mKeyboardString;
}

- (void)setKeyboardString:(const std::string &)keyboardString
{
	mKeyboardString = keyboardString;
	if( ! mWindows.empty() )
		[mWindows.front() setKeyboardString:&keyboardString];
}

- (UITextView *)getKeyboardTextView
{
	if( ! mWindows.empty() )
		return mWindows.front().keyboardTextView;

	return NULL;
}

- (void)showStatusBar:(UIStatusBarAnimation)anim
{
	if( [UIApplication sharedApplication].statusBarHidden != NO ) {
		[[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation:anim];
		UIViewController *viewController = mWindows.front();
		[viewController.view setFrame:[viewController.view bounds]];
	}
}

- (void)hideStatusBar:(UIStatusBarAnimation)anim
{
	if( [UIApplication sharedApplication].statusBarHidden != YES ) {
		[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:anim];
		UIViewController *viewController = mWindows.front();
		[viewController.view setFrame:[viewController.view bounds]];
	}
}

- (InterfaceOrientation)convertInterfaceOrientation:(UIInterfaceOrientation)orientation
{
	switch( orientation ) {
		case		UIInterfaceOrientationPortrait:				return InterfaceOrientation::Portrait;
		case		UIInterfaceOrientationPortraitUpsideDown:	return InterfaceOrientation::PortraitUpsideDown;
		case		UIInterfaceOrientationLandscapeLeft:		return InterfaceOrientation::LandscapeLeft;
		case		UIInterfaceOrientationLandscapeRight:		return InterfaceOrientation::LandscapeRight;
		default:												return InterfaceOrientation::Unknown;
	}
}

@end // AppImplCocoaTouch

// ----------------------------------------------------------------------------------------------------
// MARK: - WindowImplCocoaTouch
// ----------------------------------------------------------------------------------------------------

@implementation WindowImplCocoaTouch;

@synthesize keyboardTextView = mKeyboardTextView;

- (WindowImplCocoaTouch *)initWithFormat:(const Window::Format &)format withAppImpl:(AppImplCocoaTouch *)appImpl sharedRenderer:(RendererRef)sharedRenderer
{
	self = [super initWithNibName:nil bundle:nil];

	self.wantsFullScreenLayout = YES;

	mAppImpl = appImpl;
	mResizeHasFired = NO;
	mKeyboardVisible = NO;
	mUiWindow = NULL; // this will be set in finishLoad
	mRootViewController = format.getRootViewController() ? format.getRootViewController() : self;

	mDisplay = format.getDisplay();
	if( ! mDisplay ) // a NULL display implies the main display
		mDisplay = cinder::Display::getMainDisplay();

	cinder::Area screenBounds = mDisplay->getBounds();
	CGRect screenBoundsCgRect;
	screenBoundsCgRect.origin.x = 0;
	screenBoundsCgRect.origin.y = 0;
	screenBoundsCgRect.size.width = screenBounds.getWidth();
	screenBoundsCgRect.size.height = screenBounds.getHeight();

	mCinderView = [[CinderViewCocoaTouch alloc] initWithFrame:screenBoundsCgRect app:mAppImpl->mApp renderer:format.getRenderer() sharedRenderer:sharedRenderer];
	[mCinderView setDelegate:self];
	mSize = cinder::ivec2( screenBoundsCgRect.size.width, screenBoundsCgRect.size.height );
	mPos = cinder::ivec2( 0, 0 );
	mWindowRef = Window::privateCreate__( self, mAppImpl->mApp );

	return self;
}

- (void)finishLoad
{
	UIScreen *screen = std::dynamic_pointer_cast<cinder::DisplayCocoaTouch>( mDisplay )->getUiScreen();
	mUiWindow = [[UIWindow alloc] initWithFrame:[screen bounds]];
	mUiWindow.screen = screen;
	mUiWindow.rootViewController = mRootViewController;

	// this needs to be last
	if( mDisplay == cinder::Display::getMainDisplay() )
		[mUiWindow makeKeyAndVisible];
	else
		mUiWindow.hidden = NO;
}

- (void)dealloc
{
	if( mKeyboardTextView ) {
		[[NSNotificationCenter defaultCenter] removeObserver:self name:UIKeyboardWillShowNotification object:nil];
		[[NSNotificationCenter defaultCenter] removeObserver:self name:UIKeyboardWillHideNotification object:nil];
		[mKeyboardTextView release];
	}

	[mUiWindow release];
	[super dealloc];
}

- (void)loadView
{
	[super loadView];
	self.view = mCinderView;
}

- (BOOL)prefersStatusBarHidden
{
	return mAppImpl->mStatusBarShouldHide;
}

// pre iOS 6
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
	// Only rotate after setup. On secondary monitors we don't support any rotation
	if( ! mAppImpl->mSetupHasFired || mDisplay != cinder::Display::getMainDisplay() ) {
		return ( toInterfaceOrientation == UIInterfaceOrientationPortrait );
	}

	InterfaceOrientation orientation = [mAppImpl convertInterfaceOrientation:toInterfaceOrientation];
	uint32_t supportedOrientations = mAppImpl->mApp->emitSupportedOrientations();

	return ( ( supportedOrientations & orientation ) != 0 );
}

// iOS 6+
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 60000
- (NSUInteger)supportedInterfaceOrientations
{
	// Only rotate after setup. On secondary monitors we don't support any rotation
	if( ! mAppImpl->mSetupHasFired || mDisplay != cinder::Display::getMainDisplay() ) {
		return UIInterfaceOrientationMaskAll;
	}

	uint32_t supportedOrientations = mAppImpl->mApp->emitSupportedOrientations();
	NSUInteger result = 0;
	if( supportedOrientations & InterfaceOrientation::Portrait )
		result |= UIInterfaceOrientationMaskPortrait;
	if( supportedOrientations & InterfaceOrientation::PortraitUpsideDown )
		result |= UIInterfaceOrientationMaskPortraitUpsideDown;
	if( supportedOrientations & InterfaceOrientation::LandscapeLeft )
		result |= UIInterfaceOrientationMaskLandscapeLeft;
	if( supportedOrientations & InterfaceOrientation::LandscapeRight )
		result |= UIInterfaceOrientationMaskLandscapeRight;

	return result;
}
#endif

- (void)willAnimateRotationToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation duration:(NSTimeInterval)duration
{
	mAppImpl->mApp->emitWillRotate();
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
	mAppImpl->mApp->emitDidRotate();
}

///////////////////////////////////////////////////////////////////////////////////////
// Keyboard Management

- (void)showKeyboard:(const AppCocoaTouch::KeyboardOptions &)options
{
	if( mKeyboardVisible )
		[self.keyboardTextView resignFirstResponder];

	using namespace cinder::app;

	switch ( options.getType() ) {
		case AppCocoaTouch::KeyboardType::NUMERICAL:	self.keyboardTextView.keyboardType = UIKeyboardTypeDecimalPad; break;
		case AppCocoaTouch::KeyboardType::URL:			self.keyboardTextView.keyboardType = UIKeyboardTypeURL; break;
		default:										self.keyboardTextView.keyboardType = UIKeyboardTypeDefault;
	}

	[self setKeyboardString:&options.getInitialString()];

	mKeyboardVisible = YES;
	mKeyboardClosesOnReturn = options.getCloseOnReturn();

	[self.keyboardTextView becomeFirstResponder];
}

- (void)hideKeyboard
{
	if( ! mKeyboardVisible )
		return;

	mKeyboardVisible = NO;
	[self.keyboardTextView resignFirstResponder];
}

- (bool)isKeyboardVisible
{
	return mKeyboardVisible;
}

- (void)setKeyboardString:(const std::string *)keyboardString
{
	self.keyboardTextView.text = [NSString stringWithCString:keyboardString->c_str() encoding:NSUTF8StringEncoding];
}

- (void)keyboardWillShow:(NSNotification *)notification
{
	mAppImpl->mApp->emitKeyboardWillShow();
}

- (void)keyboardWillHide:(NSNotification *)notification
{
	// this can be triggered from keyboard itself, which skips hideKeyboard, so update bool state
	mKeyboardVisible = NO;
	mAppImpl->mApp->emitKeyboardWillHide();
}

// lazy loader for text field
- (UITextView *)keyboardTextView
{
	if( ! mKeyboardTextView ) {
		mKeyboardTextView = [[UITextView alloc] initWithFrame:CGRectZero];
		mKeyboardTextView.hidden = YES;
		mKeyboardTextView.delegate = self;

		mKeyboardTextView.autocapitalizationType = UITextAutocapitalizationTypeNone;
		mKeyboardTextView.autocorrectionType = UITextAutocorrectionTypeNo;

		[mCinderView addSubview:mKeyboardTextView];

		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillShow:) name:UIKeyboardWillShowNotification object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillHide:) name:UIKeyboardWillHideNotification object:nil];
	}

	return mKeyboardTextView;
}

///////////////////////////////////////////////////////////////////////////////////////
// UIKeyInput Protocol Methods

- (bool)canBecomeFirstResponder
{
	return NO;
}

- (void)insertText:(NSString *)text
{
	if( mKeyboardClosesOnReturn && [text isEqualToString:@"\n"] ) {
		KeyEvent keyEvent( mWindowRef, KeyEvent::KEY_RETURN, '\r', '\r', 0, 0 );
		[self keyDown:&keyEvent];
		[self hideKeyboard];
		return;
	}

	NSUInteger n = [text length];
	for( NSUInteger i = 0; i < n; i++ ) {
		unichar c = [text characterAtIndex:i];

		// For now, use ASCII key codes on iOS, which is already mapped out in KeyEvent's enum.
		int keyCode = ( c < 127 ? c : KeyEvent::KEY_UNKNOWN );
		KeyEvent keyEvent( mWindowRef, keyCode, c, c, 0, 0 );
		[self keyDown:&keyEvent];
	}
}

- (void)deleteBackward
{
	KeyEvent keyEvent( mWindowRef, KeyEvent::KEY_BACKSPACE, '\b', '\b', 0, 0 );
	[self keyDown:&keyEvent];
}

- (BOOL)hasText
{
	return YES;
}

///////////////////////////////////////////////////////////////////////////////////////
// UITextViewDelegate Protocol Methods

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text
{
	NSMutableString *currentString = [[textView.text mutableCopy] autorelease];
	// if we are getting a backspace, the length of the range can't be trusted to map to the length of a composed character (such as emoji)
	if( [text length] == 0 ) {
		// guard against backspace when textView's text is empty
		if( [currentString length] != 0 ) {
			NSRange delRange = [currentString rangeOfComposedCharacterSequenceAtIndex:range.location];
			[currentString deleteCharactersInRange:delRange];
		}
	}
	else if( mKeyboardClosesOnReturn && [text isEqualToString:@"\n"] ) {
		KeyEvent keyEvent( mWindowRef, KeyEvent::KEY_RETURN, '\r', '\r', 0, 0 );
		[self keyDown:&keyEvent];
		[self hideKeyboard];
		return NO;
	}
	else
		[currentString replaceCharactersInRange:range withString:text];

	//make a copy so getKeyboardString() is up to date in the KeyEvent callbacks
	const char *utf8KeyboardChar = [currentString UTF8String];
	if( utf8KeyboardChar )
		mAppImpl->mKeyboardString = std::string( utf8KeyboardChar );

	if( [text length] == 0 ) {
		KeyEvent keyEvent( mWindowRef, KeyEvent::KEY_BACKSPACE, '\b', '\b', 0, 0 );
		[self keyDown:&keyEvent];
	}
	else {
		for( int i = 0; i < [text length]; i++) {
			unichar c = [text characterAtIndex:i];

			// For now, use ASCII key codes on iOS, which is already mapped out in KeyEvent's enum.
			int keyCode = ( c < 127 ? c : KeyEvent::KEY_UNKNOWN );
			KeyEvent keyEvent( mWindowRef, keyCode, c, c, 0, 0 );
			[self keyDown:&keyEvent];
		}
	}

	return YES;
}

///////////////////////////////////////////////////////////////////////////////////////
// WindowImplCocoa Protocol Methods

- (bool)isFullScreen
{
	return YES;
}

- (void)setFullScreen:(bool)fullScreen options:(FullScreenOptions *)options
{ // NO-OP
}

- (ivec2)getSize;
{
	return mSize;
}

- (void)setSize:(ivec2)size
{ // NO-OP
}

- (ivec2)getPos;
{
	return mPos;
}

- (void)setPos:(ivec2)pos;
{ // NO-OP
}

- (float)getContentScale
{
	return [mCinderView contentScaleFactor];
}

- (void)close;
{ // NO-OP
}

- (NSString *)getTitle
{
	return self.title;
}

- (bool)isBorderless
{
	return YES;
}

- (void)setBorderless:(bool)borderless
{ // NO-OP
}

- (bool)isAlwaysOnTop
{
	return YES;
}

- (void)setAlwaysOnTop:(bool)alwaysOnTop
{ // NO-OP
}

- (void)hide
{ // NO-OP
}

- (void)show
{ // NO-OP
}

- (bool)isHidden
{
	return NO;
}

- (DisplayRef)getDisplay;
{
	return mDisplay;
}

- (RendererRef)getRenderer;
{
	return [mCinderView getRenderer];
}

- (void*)getNative
{
	return mCinderView;
}

- (UIViewController *)getNativeViewController
{
	return self;
}

- (const std::vector<TouchEvent::Touch> &)getActiveTouches
{
	return [mCinderView getActiveTouches];
}


// CinderViewCocoaTouchDelegate methods
- (void)draw
{
	[mAppImpl setActiveWindow:self];
	if( mAppImpl->mUpdateHasFired )
		mWindowRef->emitDraw();
}

- (void)resize
{
	[mAppImpl setActiveWindow:self];

	mSize.x = [mCinderView.layer bounds].size.width; // * mCinderView.contentScaleFactor;
	mSize.y = [mCinderView.layer bounds].size.height; // * mCinderView.contentScaleFactor;

	if( mAppImpl->mSetupHasFired ) {
		mWindowRef->emitResize();
		mResizeHasFired = YES;
	}
}

- (void)mouseDown:(MouseEvent *)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitMouseDown( event );
}

- (void)mouseDrag:(MouseEvent *)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitMouseDrag( event );
}

- (void)mouseUp:(MouseEvent *)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitMouseUp( event );
}

- (void)touchesBegan:(TouchEvent *)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitTouchesBegan( event );
}

- (void)touchesMoved:(TouchEvent *)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitTouchesMoved( event );
}

- (void)touchesEnded:(TouchEvent *)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitTouchesEnded( event );
}

- (WindowRef)getWindowRef
{
	return mWindowRef;
}

- (void)keyDown:(KeyEvent *)event
{
	[mAppImpl setActiveWindow:self];
	event->setWindow( mWindowRef );
	mWindowRef->emitKeyDown( event );
	event->setHandled( false );
	mWindowRef->emitKeyUp( event );
}

@end // WindowImplCocoaTouch
