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

#import "cinder/app/cocoa/RendererImpl2dMacQuartz.h"


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

- (BOOL)isFlipped
{
	return YES;
}

@end

/////////////////////////////////////////////////////////////////////////////////////////

@implementation RendererImpl2dMacQuartz

- (id)initWithFrame:(NSRect)frame cinderView:(NSView *)cinderView
{
	self = [super init];
	
	view = [[CocoaRendererQuartzView alloc] initWithFrame:frame];
	currentRef = nil;
	[cinderView addSubview:view];
	
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
	
	// We need to recreate the CGContextRef if this method isn't called between startDraw and finishDraw
	CGContextRef tempRef = currentRef;
	if( ! tempRef )
		tempRef = (CGContextRef)[currentGraphicsContext graphicsPort];

	CGContextFlush( tempRef );
	NSRect rect = NSMakeRect( area.x1, area.y1, area.getWidth(), area.getHeight() );
	NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithFocusedViewRect:rect];
	[view unlockFocus];

	// Caller assumes ownership
	return rep;
}

- (void)startDraw
{
	if( currentRef )
		return;

	currentGraphicsContext = [[view window] graphicsContext];
	[currentGraphicsContext saveGraphicsState];
	currentRef = (CGContextRef)[currentGraphicsContext graphicsPort];
	CGContextRetain( currentRef );

	// set the clipping rectangle to be the parent (CinderView)'s bounds
	CGRect boundsRect = NSRectToCGRect([[view superview] bounds]);
	CGContextClipToRect( currentRef, boundsRect );
	
	// undo any previously transformations, so that we start with identity CTM
	CGAffineTransform originalCtm = ::CGContextGetCTM( currentRef );
	CGAffineTransform originalCtmInv = ::CGAffineTransformInvert( originalCtm );
	::CGContextConcatCTM( currentRef, originalCtmInv );	
}

- (void)finishDraw
{
	CGContextFlush( currentRef );
	CGContextRelease( currentRef );
	[currentGraphicsContext restoreGraphicsState];
	currentRef = nil;
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

@end
