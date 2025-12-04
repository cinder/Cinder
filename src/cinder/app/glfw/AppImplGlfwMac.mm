/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.

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

#include "cinder/app/glfw/AppImplGlfw.h"
#include "cinder/app/glfw/AppGlfw.h"
#include "cinder/app/glfw/WindowImplGlfw.h"
#include "cinder/app/TouchEvent.h"
#include "glfw/glfw3.h"

#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>

#include <map>

// Forward declaration for GLFW native access function
extern "C" NSWindow* glfwGetCocoaWindow(GLFWwindow* window);

// Custom NSApplicationDelegate wrapper for macOS-specific functionality
@interface CinderGlfwAppDelegate : NSObject <NSApplicationDelegate>
@property (nonatomic, assign) id<NSApplicationDelegate> glfwDelegate;
@property (nonatomic, assign) cinder::app::AppGlfw *app;
@property (nonatomic, assign) bool *quitOnLastWindowClosed;
@end

@implementation CinderGlfwAppDelegate

- (void)applicationDidBecomeActive:(NSNotification *)notification
{
	self.app->emitDidBecomeActive();
	// Also forward to GLFW's delegate if it implements this
	if( [self.glfwDelegate respondsToSelector:@selector(applicationDidBecomeActive:)] ) {
		[self.glfwDelegate applicationDidBecomeActive:notification];
	}
}

- (void)applicationWillResignActive:(NSNotification *)notification
{
	self.app->emitWillResignActive();
	// Also forward to GLFW's delegate if it implements this
	if( [self.glfwDelegate respondsToSelector:@selector(applicationWillResignActive:)] ) {
		[self.glfwDelegate applicationWillResignActive:notification];
	}
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)application
{
	return *self.quitOnLastWindowClosed;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	// First, ask the Cinder app if it wants to quit
	bool shouldQuit = self.app->privateEmitShouldQuit();
	if( shouldQuit ) {
		// Forward to GLFW's delegate to proceed with quit
		if( [self.glfwDelegate respondsToSelector:@selector(applicationShouldTerminate:)] ) {
			return [self.glfwDelegate applicationShouldTerminate:sender];
		}
		return NSTerminateNow;
	}
	else {
		// App wants to cancel the quit
		return NSTerminateCancel;
	}
}

// Automatic message forwarding for all other NSApplicationDelegate methods
- (NSMethodSignature *)methodSignatureForSelector:(SEL)selector
{
	return [self.glfwDelegate methodSignatureForSelector:selector];
}

- (void)forwardInvocation:(NSInvocation *)invocation
{
	[invocation invokeWithTarget:self.glfwDelegate];
}

- (BOOL)respondsToSelector:(SEL)selector
{
	// Respond to selectors we implement or that GLFW's delegate implements
	if( [super respondsToSelector:selector] ) {
		return YES;
	}
	return [self.glfwDelegate respondsToSelector:selector];
}

@end

// NSView category to add touch event handling to GLFW's content view
@interface NSView (CinderTouchEvents)
@property (nonatomic, strong) NSMutableDictionary *cinderTouchIdMap;
@property (nonatomic, assign) cinder::app::WindowImplGlfw *cinderWindowImpl;
@end

@implementation NSView (CinderTouchEvents)

static char kTouchIdMapKey;
static char kWindowImplKey;
static char kHasOriginalTouchesBeganKey;
static char kHasOriginalTouchesMovedKey;
static char kHasOriginalTouchesEndedKey;
static char kHasOriginalTouchesCancelledKey;

static inline bool classHasOriginalSelector( Class cls, void *key )
{
	NSNumber *flag = objc_getAssociatedObject( cls, key );
	return flag ? [flag boolValue] : false;
}

static inline void setClassHasOriginalSelector( Class cls, void *key, bool value )
{
	if( value )
		objc_setAssociatedObject( cls, key, @YES, OBJC_ASSOCIATION_RETAIN_NONATOMIC );
	else
		objc_setAssociatedObject( cls, key, nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC );
}

- (NSMutableDictionary *)cinderTouchIdMap
{
	return objc_getAssociatedObject(self, &kTouchIdMapKey);
}

- (void)setCinderTouchIdMap:(NSMutableDictionary *)map
{
	objc_setAssociatedObject(self, &kTouchIdMapKey, map, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

- (cinder::app::WindowImplGlfw *)cinderWindowImpl
{
	NSValue *value = objc_getAssociatedObject(self, &kWindowImplKey);
	return value ? static_cast<cinder::app::WindowImplGlfw *>( [value pointerValue] ) : nullptr;
}

- (void)setCinderWindowImpl:(cinder::app::WindowImplGlfw *)impl
{
	NSValue *value = impl ? [NSValue valueWithPointer:impl] : nil;
	objc_setAssociatedObject(self, &kWindowImplKey, value, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

- (uint32_t)cinderAddTouchToMap:(NSTouch *)touch withPoint:(cinder::vec2)point prevPointMap:(std::map<uint32_t,cinder::vec2>&)prevPointMap
{
	uint32_t candidateId = 0;
	NSArray *currentValues = [self.cinderTouchIdMap allValues];
	bool found = true;
	while( found ) {
		candidateId++;
		if( [currentValues indexOfObjectIdenticalTo:[NSNumber numberWithInt:candidateId]] == NSNotFound )
			found = false;
	}

	[self.cinderTouchIdMap setObject:[NSNumber numberWithInt:candidateId] forKey:[touch identity]];
	prevPointMap[candidateId] = point;
	return candidateId;
}

- (void)cinderRemoveTouchFromMap:(NSTouch *)touch prevPointMap:(std::map<uint32_t,cinder::vec2>&)prevPointMap
{
	NSNumber *num = [self.cinderTouchIdMap objectForKey:[touch identity]];
	if( num ) {
		uint32_t curId = [num unsignedIntValue];
		[self.cinderTouchIdMap removeObjectForKey:[touch identity]];
		prevPointMap.erase( curId );
	}
}

- (std::pair<uint32_t,cinder::vec2>)cinderUpdateTouch:(NSTouch *)touch withPoint:(cinder::vec2)point prevPointMap:(std::map<uint32_t,cinder::vec2>&)prevPointMap
{
	NSNumber *num = [self.cinderTouchIdMap objectForKey:[touch identity]];
	if( num ) {
		uint32_t curId = [num unsignedIntValue];
		cinder::vec2 prevPt = prevPointMap[curId];
		prevPointMap[curId] = point;
		return std::make_pair( curId, prevPt );
	}
	else {
		// sometimes we will get a move event for a touch we have no record of
		return std::make_pair( [self cinderAddTouchToMap:touch withPoint:point prevPointMap:prevPointMap], point );
	}
}

- (void)cinder_touchesBeganWithEvent:(NSEvent *)event
{
	if( ! self.cinderWindowImpl )
		return;

	static std::map<uint32_t,cinder::vec2> sPrevPointMap;
	static std::vector<cinder::app::TouchEvent::Touch> sActiveTouches;

	std::vector<cinder::app::TouchEvent::Touch> touchList;
	NSSet *touches = [event touchesMatchingPhase:NSTouchPhaseBegan inView:self];
	float width = [self frame].size.width;
	float height = [self frame].size.height;
	double eventTime = [event timestamp];
	for( NSTouch *touch in touches ) {
		NSPoint rawPt = [touch normalizedPosition];
		cinder::vec2 pt( rawPt.x * width, height - rawPt.y * height );
		touchList.push_back( cinder::app::TouchEvent::Touch( pt, pt, [self cinderAddTouchToMap:touch withPoint:pt prevPointMap:sPrevPointMap], eventTime, touch ) );
	}

	// Update active touches
	sActiveTouches.clear();
	touches = [event touchesMatchingPhase:NSTouchPhaseTouching inView:self];
	for( NSTouch *touch in touches ) {
		NSPoint rawPt = [touch normalizedPosition];
		cinder::vec2 pt( rawPt.x * width, height - rawPt.y * height );
		std::pair<uint32_t,cinder::vec2> prev = [self cinderUpdateTouch:touch withPoint:pt prevPointMap:sPrevPointMap];
		sActiveTouches.push_back( cinder::app::TouchEvent::Touch( pt, prev.second, prev.first, eventTime, touch ) );
	}

	if( ! touchList.empty() ) {
		cinder::app::TouchEvent touchEvent( self.cinderWindowImpl->getWindow(), touchList );
		self.cinderWindowImpl->getWindow()->emitTouchesBegan( &touchEvent );
	}
}

- (void)cinder_touchesMovedWithEvent:(NSEvent *)event
{
	if( ! self.cinderWindowImpl )
		return;

	static std::map<uint32_t,cinder::vec2> sPrevPointMap;
	static std::vector<cinder::app::TouchEvent::Touch> sActiveTouches;

	std::vector<cinder::app::TouchEvent::Touch> touchList;
	NSSet *touches = [event touchesMatchingPhase:NSTouchPhaseMoved inView:self];
	float width = [self frame].size.width;
	float height = [self frame].size.height;
	double eventTime = [event timestamp];
	for( NSTouch *touch in touches ) {
		NSPoint rawPt = [touch normalizedPosition];
		cinder::vec2 pt( rawPt.x * width, height - rawPt.y * height );
		std::pair<uint32_t,cinder::vec2> prev = [self cinderUpdateTouch:touch withPoint:pt prevPointMap:sPrevPointMap];
		touchList.push_back( cinder::app::TouchEvent::Touch( pt, prev.second, prev.first, eventTime, touch ) );
	}

	// Update active touches
	sActiveTouches.clear();
	touches = [event touchesMatchingPhase:NSTouchPhaseTouching inView:self];
	for( NSTouch *touch in touches ) {
		NSPoint rawPt = [touch normalizedPosition];
		cinder::vec2 pt( rawPt.x * width, height - rawPt.y * height );
		std::pair<uint32_t,cinder::vec2> prev = [self cinderUpdateTouch:touch withPoint:pt prevPointMap:sPrevPointMap];
		sActiveTouches.push_back( cinder::app::TouchEvent::Touch( pt, prev.second, prev.first, eventTime, touch ) );
	}

	if( !touchList.empty() ) {
		cinder::app::TouchEvent touchEvent( self.cinderWindowImpl->getWindow(), touchList );
		self.cinderWindowImpl->getWindow()->emitTouchesMoved( &touchEvent );
	}
}

- (void)cinder_touchesEndedWithEvent:(NSEvent *)event
{
	if( ! self.cinderWindowImpl )
		return;

	static std::map<uint32_t,cinder::vec2> sPrevPointMap;
	static std::vector<cinder::app::TouchEvent::Touch> sActiveTouches;

	std::vector<cinder::app::TouchEvent::Touch> touchList;
	NSSet *touches = [event touchesMatchingPhase:NSTouchPhaseEnded inView:self];
	float width = [self frame].size.width;
	float height = [self frame].size.height;
	double eventTime = [event timestamp];
	for( NSTouch *touch in touches ) {
		NSPoint rawPt = [touch normalizedPosition];
		cinder::vec2 pt( rawPt.x * width, height - rawPt.y * height );
		std::pair<uint32_t,cinder::vec2> prev = [self cinderUpdateTouch:touch withPoint:pt prevPointMap:sPrevPointMap];
		touchList.push_back( cinder::app::TouchEvent::Touch( pt, prev.second, prev.first, eventTime, touch ) );
		[self cinderRemoveTouchFromMap:touch prevPointMap:sPrevPointMap];
	}

	// Update active touches
	sActiveTouches.clear();
	touches = [event touchesMatchingPhase:NSTouchPhaseTouching inView:self];
	for( NSTouch *touch in touches ) {
		NSPoint rawPt = [touch normalizedPosition];
		cinder::vec2 pt( rawPt.x * width, height - rawPt.y * height );
		std::pair<uint32_t,cinder::vec2> prev = [self cinderUpdateTouch:touch withPoint:pt prevPointMap:sPrevPointMap];
		sActiveTouches.push_back( cinder::app::TouchEvent::Touch( pt, prev.second, prev.first, eventTime, touch ) );
	}

	if( !touchList.empty() ) {
		cinder::app::TouchEvent touchEvent( self.cinderWindowImpl->getWindow(), touchList );
		self.cinderWindowImpl->getWindow()->emitTouchesEnded( &touchEvent );
	}
}

- (void)cinder_touchesCancelledWithEvent:(NSEvent *)event
{
	if( ! self.cinderWindowImpl )
		return;

	static std::map<uint32_t,cinder::vec2> sPrevPointMap;
	static std::vector<cinder::app::TouchEvent::Touch> sActiveTouches;

	std::vector<cinder::app::TouchEvent::Touch> touchList;
	NSSet *touches = [event touchesMatchingPhase:NSTouchPhaseCancelled inView:self];
	float width = [self frame].size.width;
	float height = [self frame].size.height;
	double eventTime = [event timestamp];
	for( NSTouch *touch in touches ) {
		NSPoint rawPt = [touch normalizedPosition];
		cinder::vec2 pt( rawPt.x * width, height - rawPt.y * height );
		std::pair<uint32_t,cinder::vec2> prev = [self cinderUpdateTouch:touch withPoint:pt prevPointMap:sPrevPointMap];
		touchList.push_back( cinder::app::TouchEvent::Touch( pt, prev.second, prev.first, eventTime, touch ) );
		[self cinderRemoveTouchFromMap:touch prevPointMap:sPrevPointMap];
	}

	// Update active touches (should be empty after cancel)
	sActiveTouches.clear();

	if( !touchList.empty() ) {
		cinder::app::TouchEvent touchEvent( self.cinderWindowImpl->getWindow(), touchList );
		self.cinderWindowImpl->getWindow()->emitTouchesEnded( &touchEvent );
	}
}

@end

namespace cinder { namespace app {

// C++ interface to setup the macOS delegate
void setupMacOSDelegate( AppGlfw *app, bool *quitOnLastWindowClosed )
{
	NSApplication *nsApp = [NSApplication sharedApplication];
	id<NSApplicationDelegate> glfwDelegate = [nsApp delegate];

	// Wrap GLFW's delegate with our custom delegate
	CinderGlfwAppDelegate *cinderDelegate = [[CinderGlfwAppDelegate alloc] init];
	cinderDelegate.glfwDelegate = glfwDelegate;
	cinderDelegate.app = app;
	cinderDelegate.quitOnLastWindowClosed = quitOnLastWindowClosed;

	[nsApp setDelegate:cinderDelegate];
}

// Enable multitouch on a GLFW window
void enableMultiTouchForWindow( void *nativeWindow, WindowImplGlfw *windowImpl )
{
	GLFWwindow* glfwWindow = static_cast<GLFWwindow*>( nativeWindow );
	NSWindow *nsWindow = glfwGetCocoaWindow( glfwWindow );
	NSView *contentView = [nsWindow contentView];

	// Set up touch tracking on the content view
	[contentView setAcceptsTouchEvents:YES];
	[contentView setWantsRestingTouches:YES];

	// Initialize touch ID map
	contentView.cinderTouchIdMap = [[NSMutableDictionary alloc] initWithCapacity:10];
	contentView.cinderWindowImpl = windowImpl;

	// Swizzle touch event methods
	Class viewClass = [contentView class];
	static NSMutableSet<Class> *sSwizzledViewClasses = nil;
	static dispatch_once_t onceToken;
	dispatch_once( &onceToken, ^{
		sSwizzledViewClasses = [[NSMutableSet alloc] init];
	} );

	if( ! [sSwizzledViewClasses containsObject:viewClass] ) {
		auto swizzleSelector = ^( SEL originalSel, SEL swizzledSel, void *flagKey ) {
			Method originalMethod = class_getInstanceMethod( viewClass, originalSel );
			Method swizzledMethod = class_getInstanceMethod( viewClass, swizzledSel );
			if( ! swizzledMethod )
				return;

			bool hasOriginal = ( originalMethod != nullptr );
			if( hasOriginal ) {
				method_exchangeImplementations( originalMethod, swizzledMethod );
			}
			else {
				class_addMethod( viewClass, originalSel, method_getImplementation( swizzledMethod ), method_getTypeEncoding( swizzledMethod ) );
			}

			setClassHasOriginalSelector( viewClass, flagKey, hasOriginal );
		};

		swizzleSelector( @selector(touchesBeganWithEvent:), @selector(cinder_touchesBeganWithEvent:), &kHasOriginalTouchesBeganKey );
		swizzleSelector( @selector(touchesMovedWithEvent:), @selector(cinder_touchesMovedWithEvent:), &kHasOriginalTouchesMovedKey );
		swizzleSelector( @selector(touchesEndedWithEvent:), @selector(cinder_touchesEndedWithEvent:), &kHasOriginalTouchesEndedKey );
		swizzleSelector( @selector(touchesCancelledWithEvent:), @selector(cinder_touchesCancelledWithEvent:), &kHasOriginalTouchesCancelledKey );

		[sSwizzledViewClasses addObject:viewClass];
	}
}

}} // namespace cinder::app

// Observer object for window resize/move notifications
// Note: Objective-C declarations must be at global scope
@interface CinderResizeObserver : NSObject
@property (nonatomic, assign) cinder::app::WindowImplGlfw *windowImpl;
@end

@implementation CinderResizeObserver

- (void)windowWillStartLiveResize:(NSNotification *)notification
{
	if( self.windowImpl ) {
		self.windowImpl->setResizing( true );
	}
}

- (void)windowDidEndLiveResize:(NSNotification *)notification
{
	if( self.windowImpl ) {
		self.windowImpl->setResizing( false );
		self.windowImpl->handlePostResize();
	}
}

@end

// Key for storing resize observer as associated object
static char kResizeObserverKey;

namespace cinder { namespace app {

// Enable resize tracking on a GLFW window
void enableResizeTrackingForWindow( void *nativeWindow, WindowImplGlfw *windowImpl )
{
	GLFWwindow* glfwWindow = static_cast<GLFWwindow*>( nativeWindow );
	NSWindow *nsWindow = glfwGetCocoaWindow( glfwWindow );
	if( ! nsWindow )
		return;

	// Create observer object
	CinderResizeObserver *observer = [[CinderResizeObserver alloc] init];
	observer.windowImpl = windowImpl;

	// Register for resize notifications
	[[NSNotificationCenter defaultCenter] addObserver:observer
											 selector:@selector(windowWillStartLiveResize:)
												 name:NSWindowWillStartLiveResizeNotification
											   object:nsWindow];

	[[NSNotificationCenter defaultCenter] addObserver:observer
											 selector:@selector(windowDidEndLiveResize:)
												 name:NSWindowDidEndLiveResizeNotification
											   object:nsWindow];

	// Store the observer as an associated object so it persists with the window
	objc_setAssociatedObject( nsWindow, &kResizeObserverKey, observer, OBJC_ASSOCIATION_RETAIN_NONATOMIC );
}

// Get current global modifier key state (works even when window doesn't have focus)
unsigned int getGlobalModifierState()
{
	NSUInteger cocoaMods = [NSEvent modifierFlags];
	unsigned int modifiers = 0;

	if( cocoaMods & NSEventModifierFlagShift )
		modifiers |= 0x0008;  // MouseEvent::SHIFT_DOWN
	if( cocoaMods & NSEventModifierFlagOption )
		modifiers |= 0x0010;  // MouseEvent::ALT_DOWN
	if( cocoaMods & NSEventModifierFlagControl )
		modifiers |= 0x0020;  // MouseEvent::CTRL_DOWN
	if( cocoaMods & NSEventModifierFlagCommand )
		modifiers |= 0x0040;  // MouseEvent::META_DOWN

	return modifiers;
}

// Disable resize tracking on a GLFW window
void disableResizeTrackingForWindow( void *nativeWindow )
{
	GLFWwindow* glfwWindow = static_cast<GLFWwindow*>( nativeWindow );
	NSWindow *nsWindow = glfwGetCocoaWindow( glfwWindow );
	if( ! nsWindow )
		return;

	CinderResizeObserver *observer = objc_getAssociatedObject( nsWindow, &kResizeObserverKey );
	if( observer ) {
		// Remove from notification center
		[[NSNotificationCenter defaultCenter] removeObserver:observer];
		// Clear the windowImpl pointer to prevent dangling pointer access
		observer.windowImpl = nullptr;
		// Remove the associated object
		objc_setAssociatedObject( nsWindow, &kResizeObserverKey, nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC );
	}
}

}} // namespace cinder::app
