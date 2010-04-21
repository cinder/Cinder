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

#import "cinder/app/CinderView.h"
#include "cinder/app/Renderer.h"
#import <Cocoa/Cocoa.h>

@implementation CinderView
@synthesize appSetupCalled;
@synthesize receivesEvents;

- (id)initWithFrame:(NSRect)frame
{
	self = [super initWithFrame:frame];

	app = NULL;
	appSetupCalled = NO;
	receivesEvents = YES;
	
	return self;
}

- (id)initWithFrame:(NSRect)frame app:(cinder::app::App*)aApp
{
	self = [super initWithFrame:frame];
	
	app = aApp;
	appSetupCalled = NO;
	receivesEvents = app->receivesEvents();
	
	[self setupRenderer:frame];
	
	return self;
}

- (id)initFullScreenWithApp:(cinder::app::App*)aApp
{
	self = [super initWithFrame:NSMakeRect(0,0,1,1)];

	app = aApp;
	receivesEvents = app->receivesEvents();

	[self setupRenderer:NSMakeRect(0,0,1,1)];

	return self;
}

- (void)setupRenderer:(NSRect)frame
{
	app->getRenderer()->setup( app, NSRectToCGRect( frame ), self );

	// register for drop events
	if( receivesEvents )
		[self registerForDraggedTypes:[NSArray arrayWithObject:NSFilenamesPboardType]];
}

- (void)draw
{
	if( ! appSetupCalled )
		return;

	[self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)rect
{
	[super drawRect:rect];

	if( ! appSetupCalled )
		return;

	app->getRenderer()->startDraw();
	app->privateDraw__();
	app->getRenderer()->finishDraw();
}

- (void)setFrameSize:(NSSize)newSize
{	
	[super setFrameSize:newSize];
	app->getRenderer()->setFrameSize( newSize.width, newSize.height );
	
	NSRect bounds = [self bounds];
	bounds.size = [self convertSize:bounds.size toView:nil];

	if( appSetupCalled ) { 
		app->privateResize__((int) bounds.size.width, (int)bounds.size.height );
		[self draw];
	}
}

- (BOOL)isFlipped
{
	return YES;
}

/////////////////////////////////////////////////////////////////////////////////
// Event Handling
- (BOOL)acceptsFirstResponder
{
	return receivesEvents;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)theEvent
{
	return receivesEvents;
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
	char c		= [[theEvent charactersIgnoringModifiers] characterAtIndex:0];
	int code	= [theEvent keyCode];
	int mods	= [self prepKeyEventModifiers:theEvent];
	
	cinder::app::KeyEvent k = cinder::app::KeyEvent (cinder::app::KeyEvent::translateNativeKeyCode( code ), (char)c, mods, code);	
	app->privateKeyDown__( k );
}

- (void)keyUp:(NSEvent*)theEvent
{
	char c		= [[theEvent charactersIgnoringModifiers] characterAtIndex:0];
	int code	= [theEvent keyCode];
	int mods	= [self prepKeyEventModifiers:theEvent];
	
	app->privateKeyUp__( cinder::app::KeyEvent (cinder::app::KeyEvent::translateNativeKeyCode( code ), (char)c, mods, code) );
}

- (void)mouseDown:(NSEvent*)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int y					= app->getWindowHeight() - curPoint.y;
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::LEFT_DOWN;
 	app->privateMouseDown__( cinder::app::MouseEvent( cinder::app::MouseEvent::LEFT_DOWN, curPoint.x, y, mods, 0.0f, [theEvent modifierFlags] ) );
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int y					= app->getWindowHeight() - curPoint.y;
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::RIGHT_DOWN;
 	app->privateMouseDown__( cinder::app::MouseEvent( cinder::app::MouseEvent::RIGHT_DOWN, curPoint.x, y, mods, 0.0f, [theEvent modifierFlags] ) );
}

- (void)otherMouseDown:(NSEvent *)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int y					= app->getWindowHeight() - curPoint.y;
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::MIDDLE_DOWN;
 	app->privateMouseDown__( cinder::app::MouseEvent( cinder::app::MouseEvent::MIDDLE_DOWN, curPoint.x, y, mods, 0.0f, [theEvent modifierFlags] ) );
}

- (void)mouseUp:(NSEvent*)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int y					= app->getWindowHeight() - curPoint.y;
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::LEFT_DOWN;
	app->privateMouseUp__( cinder::app::MouseEvent( cinder::app::MouseEvent::LEFT_DOWN, curPoint.x, y, mods, 0.0f, [theEvent modifierFlags] ) );
}

- (void)rightMouseUp:(NSEvent*)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int y					= app->getWindowHeight() - curPoint.y;
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::RIGHT_DOWN;
	app->privateMouseUp__( cinder::app::MouseEvent( cinder::app::MouseEvent::RIGHT_DOWN, curPoint.x, y, mods, 0.0f, [theEvent modifierFlags] ) );	
}

- (void)otherMouseUp:(NSEvent*)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int y					= app->getWindowHeight() - curPoint.y;
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::MIDDLE_DOWN;
	app->privateMouseUp__( cinder::app::MouseEvent( cinder::app::MouseEvent::MIDDLE_DOWN, curPoint.x, y, mods, 0.0f, [theEvent modifierFlags] ) );
}

- (void)mouseMoved:(NSEvent*)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int y					= app->getWindowHeight() - curPoint.y;
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	app->privateMouseMove__( cinder::app::MouseEvent( 0, curPoint.x, y, mods, 0.0f, [theEvent modifierFlags] ) );
}

- (void)rightMouseDragged:(NSEvent*)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int y					= app->getWindowHeight() - curPoint.y;
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::RIGHT_DOWN;
	app->privateMouseDrag__( cinder::app::MouseEvent( cinder::app::MouseEvent::RIGHT_DOWN, curPoint.x, y, mods, 0.0f, [theEvent modifierFlags] ) );
}

- (void)otherMouseDragged:(NSEvent*)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int y					= app->getWindowHeight() - curPoint.y;
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::MIDDLE_DOWN;
	app->privateMouseDrag__( cinder::app::MouseEvent( cinder::app::MouseEvent::MIDDLE_DOWN, curPoint.x, y, mods, 0.0f, [theEvent modifierFlags] ) );
}

- (void)mouseDragged:(NSEvent*)theEvent
{
	NSPoint curPoint		= [theEvent locationInWindow];
	int y					= app->getWindowHeight() - curPoint.y;
	int mods				= [self prepMouseEventModifiers:theEvent];
	
	mods |= cinder::app::MouseEvent::LEFT_DOWN;
	app->privateMouseDrag__( cinder::app::MouseEvent( cinder::app::MouseEvent::LEFT_DOWN, curPoint.x, y, mods, 0.0f, [theEvent modifierFlags] ) );
}

- (void)scrollWheel:(NSEvent*)theEvent
{
	float wheelDelta		= [theEvent deltaX] + [theEvent deltaY];
	NSPoint curPoint		= [theEvent locationInWindow];
	int y					= app->getWindowHeight() - curPoint.y;
	int mods				= [self prepMouseEventModifiers:theEvent];
	app->privateMouseWheel__( cinder::app::MouseEvent( 0, curPoint.x, y, mods, wheelDelta / 4.0f, [theEvent modifierFlags] ) );	
}

- (void)setup:(cinder::app::App *)aApp
{
	app = aApp;
	[self setupRenderer:[self bounds]];
//	[self addSubview:app->getRenderer()->getView()];
}

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
		std::vector<std::string> paths;
		for( int i = 0; i < numberOfFiles; ++i )
			paths.push_back( std::string( [[files objectAtIndex:i] UTF8String] ) );
		NSPoint loc = [sender draggingLocation];
		app->privateFileDrop__( cinder::app::FileDropEvent( (int)loc.x, app->getWindowHeight() - (int)loc.y, paths ) );
	}

    return YES;
}

- (void)setApp:(cinder::app::App *)aApp
{
	app = aApp;
}


@end
