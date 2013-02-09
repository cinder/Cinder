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

#import "cinder/app/CinderView.h"
#include "cinder/app/Renderer.h"
#include "cinder/app/TouchEvent.h"
#include "cinder/cocoa/CinderCocoa.h"
#import <Cocoa/Cocoa.h>

@implementation CinderView
@synthesize readyToDraw = mReadyToDraw;
@synthesize receivesEvents = mReceivesEvents;

- (id)initWithFrame:(NSRect)frame
{
	self = [super initWithFrame:frame];

	mApp = NULL;
	mReadyToDraw = NO;
	mReceivesEvents = YES;
	mFullScreen = NO;

	mTouchIdMap = nil;
	mDelegate = nil;
	
	return self;
}

- (id)initWithFrame:(NSRect)frame app:(cinder::app::App*)aApp renderer:(cinder::app::RendererRef)aRenderer sharedRenderer:(cinder::app::RendererRef)sharedRenderer
{
	self = [super initWithFrame:frame];
	
	mApp = aApp;
	mRenderer = aRenderer;
	mReadyToDraw = NO;
	mReceivesEvents = mApp->receivesEvents();
	mFullScreen = NO;

	mTouchIdMap = nil;
	mDelegate = nil;

	[self setupRendererWithFrame:frame renderer:aRenderer sharedRenderer:sharedRenderer];
	
	return self;
}

- (void)setupRendererWithFrame:(NSRect)frame renderer:(cinder::app::RendererRef)renderer sharedRenderer:(cinder::app::RendererRef)sharedRenderer
{
	mRenderer = renderer;
	mRenderer->setup( mApp, NSRectToCGRect( frame ), self, sharedRenderer, mApp->getSettings().isHighDensityDisplayEnabled() );
	
	if( mApp->getSettings().isHighDensityDisplayEnabled() ) {
		mContentScaleFactor = self.window.backingScaleFactor;
	}
	else {
		mContentScaleFactor = 1.0f;
	}

	// register for drop events
	if( mReceivesEvents )
		[self registerForDraggedTypes:[NSArray arrayWithObject:NSFilenamesPboardType]];

	// register for touch events
	if( mDelegate ) {
		[self setAcceptsTouchEvents:YES];
		[self setWantsRestingTouches:YES];
		if( ! mTouchIdMap )
			mTouchIdMap = [[NSMutableDictionary alloc] initWithCapacity:10];
	}
}

- (float)contentScaleFactor
{
	return mContentScaleFactor;
}

- (void)viewDidChangeBackingProperties
{
	// note: mApp may not be set at init time, which is when this is called in some cases
	if( mApp && mApp->getSettings().isHighDensityDisplayEnabled() ) {
		mContentScaleFactor = self.window.backingScaleFactor;
		mRenderer->defaultResize();
	}
	else {
		mContentScaleFactor = 1.0f;
	}
}

- (BOOL)isFullScreen
{
	return mFullScreen;
}

- (void)setFullScreen:(BOOL)fullScreen withSecondaryBlanking:(BOOL)secondaryBlanking onNsScreen:(NSScreen*)screen
{
	if( fullScreen == mFullScreen )
		return;

	if( fullScreen ) {
		NSDictionary *options = nil;
		if( ! secondaryBlanking )
			options = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:NO] forKey:NSFullScreenModeAllScreens];
		[self enterFullScreenMode:screen withOptions:options];
		mFullScreen = true;
	}
	else {
		[self exitFullScreenModeWithOptions:nil];
		[[self window] becomeKeyWindow];
		[[self window] makeFirstResponder:self];
		mFullScreen = false;	
	}
}

- (void)draw
{
	if( ! mReadyToDraw )
		return;

	[self display];
}

- (void)makeCurrentContext
{
	mRenderer->startDraw();
}

- (void)drawRect:(NSRect)rect
{
	[super drawRect:rect];

	if( ! mReadyToDraw )
		return;

	mRenderer->startDraw();
	[mDelegate draw];
	mRenderer->finishDraw();
}

- (void)setFrameSize:(NSSize)newSize
{
	[super setFrameSize:newSize];

	if( mReadyToDraw ) {
		mRenderer->setFrameSize( newSize.width, newSize.height );
		mRenderer->makeCurrentContext();
		mRenderer->defaultResize();
		[mDelegate resize];
		[self draw];
	}
}

- (BOOL)isFlipped
{
	return YES;
}

- (BOOL)isOpaque
{
	return NO;
}

- (void)dealloc
{
	[super dealloc];
}

/////////////////////////////////////////////////////////////////////////////////
// Event Handling
- (BOOL)acceptsFirstResponder
{
	return mReceivesEvents;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)theEvent
{
	return mReceivesEvents;
}

- (int)prepMouseEventModifiers:(NSEvent *)evt
{	
	unsigned int result = 0;
	if( [evt modifierFlags] & NSControlKeyMask ) result |= cinder::app::MouseEvent::CTRL_DOWN;
	if( [evt modifierFlags] & NSShiftKeyMask ) result |= cinder::app::MouseEvent::SHIFT_DOWN;
	if( [evt modifierFlags] & NSAlternateKeyMask ) result |= cinder::app::MouseEvent::ALT_DOWN;
	if( [evt modifierFlags] & NSCommandKeyMask ) result |= cinder::app::MouseEvent::META_DOWN;
	
	return result;
}

- (int)prepKeyEventModifiers:(NSEvent *)evt
{
	unsigned int result = 0;
	
	if( [evt modifierFlags] & NSShiftKeyMask ) result |= cinder::app::KeyEvent::SHIFT_DOWN;
	if( [evt modifierFlags] & NSAlternateKeyMask ) result |= cinder::app::KeyEvent::ALT_DOWN;
	if( [evt modifierFlags] & NSCommandKeyMask ) result |= cinder::app::KeyEvent::META_DOWN;
	if( [evt modifierFlags] & NSControlKeyMask ) result |= cinder::app::KeyEvent::CTRL_DOWN;
	
	return result;
}

- (void)keyDown:(NSEvent*)theEvent
{
	NSString *chars = [theEvent characters];
	uint32_t c32	= ([chars length] > 0 ) ? [chars characterAtIndex:0] : 0;
	NSString *charsNoMods	= [theEvent charactersIgnoringModifiers];
	char c					= ([charsNoMods length] > 0 ) ? [charsNoMods characterAtIndex:0] : 0;
	
	int code	= [theEvent keyCode];
	int mods	= [self prepKeyEventModifiers:theEvent];
	
	cinder::app::KeyEvent keyEvent( [mDelegate getWindowRef], cinder::app::KeyEvent::translateNativeKeyCode( code ), c32,
									c, mods, code);	
	[mDelegate keyDown:&keyEvent];
}

- (void)keyUp:(NSEvent*)theEvent
{
	NSString *chars = [theEvent characters];
	uint32_t c32	= ([chars length] > 0 ) ? [chars characterAtIndex:0] : 0;
	NSString *charsNoMods	= [theEvent charactersIgnoringModifiers];
	char c					= ([charsNoMods length] > 0 ) ? [charsNoMods characterAtIndex:0] : 0;

	int code	= [theEvent keyCode];
	int mods	= [self prepKeyEventModifiers:theEvent];
	
	cinder::app::KeyEvent keyEvent( [mDelegate getWindowRef], cinder::app::KeyEvent::translateNativeKeyCode( code ),
									c32, c, mods, code );
	[mDelegate keyUp:&keyEvent];
}

// TODO: this isn't implemented yet
- (void)flagsChanged:(NSEvent*)theEvent
{
/*	char c		= 0;
	int code	= [theEvent keyCode];
	int mods	= [self prepKeyEventModifiers:theEvent];

	NSLog( @"%@", theEvent );	
	cinder::app::KeyEvent keyEvent = cinder::app::KeyEvent (cinder::app::KeyEvent::translateNativeKeyCode( code ), (char)c, mods, code);	
	[mDelegate keyDown:&keyEvent];*/
}

- (void)mouseDown:(NSEvent*)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int x					= (curPoint.x - [self frame].origin.x);
	int y					= ([self frame].size.height - ( curPoint.y - [self frame].origin.y ));
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::LEFT_DOWN;
	cinder::app::MouseEvent mouseEvent( [mDelegate getWindowRef], cinder::app::MouseEvent::LEFT_DOWN, x, y, mods, 0.0f, [theEvent modifierFlags] );
	[mDelegate mouseDown:&mouseEvent];
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int x					= (curPoint.x - [self frame].origin.x);
	int y					= ([self frame].size.height - ( curPoint.y - [self frame].origin.y ));
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::RIGHT_DOWN;
	cinder::app::MouseEvent mouseEvent( [mDelegate getWindowRef], cinder::app::MouseEvent::RIGHT_DOWN, x, y, mods, 0.0f, [theEvent modifierFlags] );
	[mDelegate mouseDown:&mouseEvent];
}

- (void)otherMouseDown:(NSEvent *)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int x					= (curPoint.x - [self frame].origin.x);
	int y					= ([self frame].size.height - ( curPoint.y - [self frame].origin.y ));
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::MIDDLE_DOWN;
 	cinder::app::MouseEvent mouseEvent( [mDelegate getWindowRef], cinder::app::MouseEvent::MIDDLE_DOWN, x, y, mods, 0.0f, [theEvent modifierFlags] );
	[mDelegate mouseDown:&mouseEvent];
}

- (void)mouseUp:(NSEvent*)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int x					= (curPoint.x - [self frame].origin.x);
	int y					= ([self frame].size.height - ( curPoint.y - [self frame].origin.y ));
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::LEFT_DOWN;
	cinder::app::MouseEvent mouseEvent( [mDelegate getWindowRef], cinder::app::MouseEvent::LEFT_DOWN, x, y, mods, 0.0f, [theEvent modifierFlags] );
	[mDelegate mouseUp:&mouseEvent];
}

- (void)rightMouseUp:(NSEvent*)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int x					= (curPoint.x - [self frame].origin.x);
	int y					= ([self frame].size.height - ( curPoint.y - [self frame].origin.y ));
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::RIGHT_DOWN;
	cinder::app::MouseEvent mouseEvent( [mDelegate getWindowRef], cinder::app::MouseEvent::RIGHT_DOWN, x, y, mods, 0.0f, [theEvent modifierFlags] );
	[mDelegate mouseUp:&mouseEvent];
}

- (void)otherMouseUp:(NSEvent*)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int x					= (curPoint.x - [self frame].origin.x);
	int y					= ([self frame].size.height - ( curPoint.y - [self frame].origin.y ));
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::MIDDLE_DOWN;
	cinder::app::MouseEvent mouseEvent( [mDelegate getWindowRef], cinder::app::MouseEvent::MIDDLE_DOWN, x, y, mods, 0.0f, [theEvent modifierFlags] );
	[mDelegate mouseUp:&mouseEvent];
}

- (void)mouseMoved:(NSEvent*)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int x					= (curPoint.x - [self frame].origin.x);
	int y					= ([self frame].size.height - ( curPoint.y - [self frame].origin.y ));
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	cinder::app::MouseEvent mouseEvent( [mDelegate getWindowRef], 0, x, y, mods, 0.0f, [theEvent modifierFlags] );
	[mDelegate mouseMove:&mouseEvent];
}

- (void)rightMouseDragged:(NSEvent*)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int x					= (curPoint.x - [self frame].origin.x);
	int y					= ([self frame].size.height - ( curPoint.y - [self frame].origin.y ));
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::RIGHT_DOWN;
	cinder::app::MouseEvent mouseEvent( [mDelegate getWindowRef], cinder::app::MouseEvent::RIGHT_DOWN, x, y, mods, 0.0f, [theEvent modifierFlags] );
	[mDelegate mouseDrag:&mouseEvent];
}

- (void)otherMouseDragged:(NSEvent*)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int x					= (curPoint.x - [self frame].origin.x);
	int y					= ([self frame].size.height - ( curPoint.y - [self frame].origin.y ));
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::MIDDLE_DOWN;
	cinder::app::MouseEvent mouseEvent( [mDelegate getWindowRef], cinder::app::MouseEvent::MIDDLE_DOWN, x, y, mods, 0.0f, [theEvent modifierFlags] );
	[mDelegate mouseDrag:&mouseEvent];
}

- (void)mouseDragged:(NSEvent*)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int x					= (curPoint.x - [self frame].origin.x);
	int y					= ([self frame].size.height - ( curPoint.y - [self frame].origin.y ));
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::LEFT_DOWN;
	cinder::app::MouseEvent mouseEvent( [mDelegate getWindowRef], cinder::app::MouseEvent::LEFT_DOWN, x, y, mods, 0.0f, [theEvent modifierFlags] );
	[mDelegate mouseDrag:&mouseEvent];
}

- (void)scrollWheel:(NSEvent*)theEvent
{
	float wheelDelta		= [theEvent deltaX] + [theEvent deltaY];
	NSPoint curPoint		= [theEvent locationInWindow];
	int x					= (curPoint.x - [self frame].origin.x);
	int y					= ([self frame].size.height - ( curPoint.y - [self frame].origin.y ));
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	cinder::app::MouseEvent mouseEvent( [mDelegate getWindowRef], 0, x, y, mods, wheelDelta / 4.0f, [theEvent modifierFlags] );
	[mDelegate mouseWheel:&mouseEvent];
}

/*- (void)setup:(cinder::app::App *)aApp
{
	app = aApp;
	[self setupRenderer:[self bounds]];
//	[self addSubview:app->getRenderer()->getView()];
}*/

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
	return NSDragOperationCopy;
}

- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender
{
	return YES;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
    NSPasteboard *pboard = [sender draggingPasteboard];
	
    if( [[pboard types] containsObject:NSFilenamesPboardType] ) {
        NSArray *files = [pboard propertyListForType:NSFilenamesPboardType];
        int numberOfFiles = [files count];
		std::vector<cinder::fs::path> paths;
		for( int i = 0; i < numberOfFiles; ++i )
			paths.push_back( cinder::fs::path( [[files objectAtIndex:i] UTF8String] ) );
		NSPoint curPoint = [sender draggingLocation];
		int x = curPoint.x - [self frame].origin.x;
		int y = [self frame].size.height - ( curPoint.y - [self frame].origin.y );
		cinder::app::FileDropEvent fileDropEvent( [mDelegate getWindowRef], x, y, paths );
		[mDelegate fileDrop:&fileDropEvent];
	}

    return YES;
}

- (void)applicationWillResignActive:(NSNotification *)aNotification
{
   	std::vector<cinder::app::TouchEvent::Touch> touchList;
	double eventTime = mApp->getElapsedSeconds();
	for( const auto &prevPt : mTouchPrevPointMap ) {
		touchList.push_back( cinder::app::TouchEvent::Touch( prevPt.second, prevPt.second, prevPt.first, eventTime, nil ) );
	}

	if( ! touchList.empty() ) {
		cinder::app::TouchEvent touchEvent( [mDelegate getWindowRef], touchList );
		[mDelegate touchesEnded:&touchEvent];
	}

	mTouchPrevPointMap.clear();
	[mTouchIdMap removeAllObjects];
}

- (cinder::app::RendererRef)getRenderer
{
	return mRenderer;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// MultiTouch

- (void)setDelegate:(id<CinderViewDelegate>)delegate
{
	mDelegate = delegate;
	if( delegate ) {
		[self setAcceptsTouchEvents:YES];
		if( ! mTouchIdMap )
			mTouchIdMap = [[NSMutableDictionary alloc] initWithCapacity:10];
	}
	else {
		[self setAcceptsTouchEvents:NO];
	}
}

- (uint32_t)addTouchToMap:(NSTouch *)touch withPoint:(cinder::Vec2f)point
{
	uint32_t candidateId = 0;
	NSArray *currentValues = [mTouchIdMap allValues];
	bool found = true;
	while( found ) {
		candidateId++;
		if( [currentValues indexOfObjectIdenticalTo:[NSNumber numberWithInt:candidateId]] == NSNotFound )
			found = false;
	}
	
	[mTouchIdMap setObject:[NSNumber numberWithInt:candidateId] forKey:[touch identity]];
	mTouchPrevPointMap[candidateId] = point;
	return candidateId;
}

- (void)removeTouchFromMap:(NSTouch *)touch
{
	NSNumber *num = [mTouchIdMap objectForKey:[touch identity]];
	uint32_t curId = [num unsignedIntValue];
	[mTouchIdMap removeObjectForKey:[touch identity]];
	mTouchPrevPointMap.erase( curId );
}

- (std::pair<uint32_t,cinder::Vec2f>)updateTouch:(NSTouch *)touch withPoint:(cinder::Vec2f)point
{
	uint32_t curId = 0;
	NSNumber *num = [mTouchIdMap objectForKey:[touch identity]];
	if( num ) {
		curId = [num unsignedIntValue];
		cinder::Vec2f prevPt = mTouchPrevPointMap[curId];
		mTouchPrevPointMap[curId] = point;
		return std::make_pair( curId, prevPt );		
	}
	else {
		// sometimes we will get a move event for a touch we have no record of
		// this can happen when the app resigns and the user never ends the touch
		return std::make_pair( [self addTouchToMap:touch withPoint:point], point );
	}	
}

- (void)updateActiveTouches:(NSEvent *)event
{
	mActiveTouches.clear();
	NSSet *touches = [event touchesMatchingPhase:NSTouchPhaseTouching inView:self];
	float width = [self frame].size.width;
	float height = [self frame].size.height;
	double eventTime = [event timestamp];
	for( NSTouch *touch in touches ) {
		NSPoint rawPt = [touch normalizedPosition];
		cinder::Vec2f pt( rawPt.x * width, height - rawPt.y * height );
		std::pair<uint32_t,cinder::Vec2f> prev = [self updateTouch:touch withPoint:pt];
		mActiveTouches.push_back( cinder::app::TouchEvent::Touch( pt, prev.second, prev.first, eventTime, touch ) );
	}
}

- (const std::vector<cinder::app::TouchEvent::Touch>&)getActiveTouches
{
	return mActiveTouches;
}

- (void)touchesBeganWithEvent:(NSEvent *)event
{
	std::vector<cinder::app::TouchEvent::Touch> touchList;
	NSSet *touches = [event touchesMatchingPhase:NSTouchPhaseBegan inView:self];
	float width = [self frame].size.width;
	float height = [self frame].size.height;
	double eventTime = [event timestamp];
	for( NSTouch *touch in touches ) {
		NSPoint rawPt = [touch normalizedPosition];
		cinder::Vec2f pt( rawPt.x * width, height - rawPt.y * height );
		touchList.push_back( cinder::app::TouchEvent::Touch( pt, pt, [self addTouchToMap:touch withPoint:pt], eventTime, touch ) );
	}
	[self updateActiveTouches:event];
	if( mDelegate && ( ! touchList.empty() ) ) {
		cinder::app::TouchEvent touchEvent( [mDelegate getWindowRef], touchList );
		[mDelegate touchesBegan:&touchEvent];
	}
}

- (void)touchesMovedWithEvent:(NSEvent *)event
{
   	std::vector<cinder::app::TouchEvent::Touch> touchList;
	NSSet *touches = [event touchesMatchingPhase:NSTouchPhaseMoved inView:self];
	float width = [self frame].size.width;
	float height = [self frame].size.height;
	double eventTime = [event timestamp];
	for( NSTouch *touch in touches ) {
		NSPoint rawPt = [touch normalizedPosition];
		cinder::Vec2f pt( rawPt.x * width, height - rawPt.y * height );
		std::pair<uint32_t,cinder::Vec2f> prev = [self updateTouch:touch withPoint:pt];
		touchList.push_back( cinder::app::TouchEvent::Touch( pt, prev.second, prev.first, eventTime, touch ) );
	}
	[self updateActiveTouches:event];
	if( mDelegate && ( ! touchList.empty() ) ) {
		cinder::app::TouchEvent touchEvent( [mDelegate getWindowRef], touchList );
		[mDelegate touchesMoved:&touchEvent];
	}
}

- (void)touchesEndedWithEvent:(NSEvent *)event
{
   	std::vector<cinder::app::TouchEvent::Touch> touchList;
	NSSet *touches = [event touchesMatchingPhase:NSTouchPhaseEnded inView:self];
	float width = [self frame].size.width;
	float height = [self frame].size.height;
	double eventTime = [event timestamp];
	for( NSTouch *touch in touches ) {
		NSPoint rawPt = [touch normalizedPosition];
		cinder::Vec2f pt( rawPt.x * width, height - rawPt.y * height );
		std::pair<uint32_t,cinder::Vec2f> prev = [self updateTouch:touch withPoint:pt];
		touchList.push_back( cinder::app::TouchEvent::Touch( pt, prev.second, prev.first, eventTime, touch ) );
		[self removeTouchFromMap:touch];
	}

	[self updateActiveTouches:event];
	if( mDelegate && ( ! touchList.empty() ) ) {
		cinder::app::TouchEvent touchEvent( [mDelegate getWindowRef], touchList );
		[mDelegate touchesEnded:&touchEvent];
	}
}
 
- (void)touchesCancelledWithEvent:(NSEvent *)event
{
   	std::vector<cinder::app::TouchEvent::Touch> touchList;
	NSSet *touches = [event touchesMatchingPhase:NSTouchPhaseCancelled inView:self];
	float width = [self frame].size.width;
	float height = [self frame].size.height;
	double eventTime = [event timestamp];
	for( NSTouch *touch in touches ) {
		NSPoint rawPt = [touch normalizedPosition];
		cinder::Vec2f pt( rawPt.x * width, height - rawPt.y * height );
		std::pair<uint32_t,cinder::Vec2f> prev = [self updateTouch:touch withPoint:pt];
		touchList.push_back( cinder::app::TouchEvent::Touch( pt, prev.second, prev.first, eventTime, touch ) );
		[self removeTouchFromMap:touch];
	}

	[self updateActiveTouches:event];
	if( mDelegate && ( ! touchList.empty() ) ) {
		cinder::app::TouchEvent touchEvent( [mDelegate getWindowRef], touchList );
		[mDelegate touchesEnded:&touchEvent];
	}
}

- (void)setApp:(cinder::app::App *)app
{
	mApp = app;
}

@end
