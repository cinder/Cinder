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

#import "cinder/app/AppImplCocoaRendererQuartz.h"


// This is only here so that we can override rightMouseDown
@interface CocoaRendererQuartzView : NSView
{}
@end

@implementation CocoaRendererQuartzView
// For whatever reasons, rightMouseDown does not get propagated along the normal responder chain
- (void)rightMouseDown:(NSEvent *)theEvent
{
	[[self superview] rightMouseDown:theEvent];
}

@end

/////////////////////////////////////////////////////////////////////////////////////////

@implementation AppImplCocoaRendererQuartz

- (id)initWithFrame:(NSRect)frame cinderView:(NSView*)aCinderView app:(cinder::app::App*)aApp
{
	self = [super init];
	
	view = [[CocoaRendererQuartzView alloc] initWithFrame:frame];
	app = aApp;
	currentRef = nil;
	[aCinderView addSubview:view];
	
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (NSView*)view
{
	return view;
}

- (NSBitmapImageRep*)getContents:(cinder::Area)area
{
	[view lockFocus];
	CGContextFlush( currentRef );
	NSRect rect = NSMakeRect( area.x1, area.y1, area.getWidth(), area.getHeight() );
	NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithFocusedViewRect:rect];
	[view unlockFocus];
	return rep;
}

- (void)makeCurrentContext
{
	currentGraphicsContext = [NSGraphicsContext currentContext];
	currentRef = (CGContextRef)[currentGraphicsContext graphicsPort];
	CGContextRetain( currentRef );
	NSRect bounds = [view frame];
}

- (void)flushBuffer
{
	CGContextRelease( currentRef );
}

- (void)setFrameSize:(CGSize)newSize
{
}

- (void)defaultResize
{
}

- (CGContextRef)getCGContextRef
{
	return currentRef;
}

- (BOOL)isFlipped
{
	return YES;
}

@end
