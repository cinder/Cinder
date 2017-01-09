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


#include "cinder/app/cocoa/CinderViewCocoaTouch.h"
#include "cinder/cocoa/CinderCocoa.h"

#import <QuartzCore/QuartzCore.h>

using namespace cinder;
using namespace cinder::app;

@implementation CinderViewCocoaTouch

// Set in initWithFrame based on the renderer
static bool sIsEaglLayer;

+ (Class)layerClass
{
	if( sIsEaglLayer )
		return [CAEAGLLayer class];
	else
		return [CALayer class];
}

- (CinderViewCocoaTouch *)initWithFrame:(CGRect)frame app:(AppCocoaTouch *)app renderer:(RendererRef)renderer sharedRenderer:(RendererRef)sharedRenderer
{
	// This needs to get setup immediately as +layerClass will be called when the view is initialized
	sIsEaglLayer = renderer->isEaglLayer();
	
	if( (self = [super initWithFrame:frame]) ) {
		mApp = app;
		mRenderer = renderer;

		renderer->setup( cocoa::CgRectToArea( frame ), self, sharedRenderer );
		
		self.multipleTouchEnabled = mApp->isMultiTouchEnabled();
	}

	mDelegate = nil;
	
    return self;
}

- (void)setDelegate:(id<CinderViewCocoaTouchDelegate>)delegate
{
	mDelegate = delegate;
}

- (RendererRef)getRenderer
{
	return mRenderer;
}

- (void)layoutSubviews
{
	[super layoutSubviews];
	
	if( ! mApp->isHighDensityDisplayEnabled() )
		self.layer.contentsScale = 1.0f;

	mRenderer->setFrameSize( self.bounds.size.width, self.bounds.size.height );

	[mDelegate resize];
}

- (void)drawRect:(CGRect)rect
{
	mRenderer->startDraw();
	[mDelegate draw];
	mRenderer->finishDraw();
}

- (void)drawView
{
	if( sIsEaglLayer ) {
		mRenderer->startDraw();
		[mDelegate draw];
		mRenderer->finishDraw();
	}
	else
		[self performSelectorOnMainThread:@selector(setNeedsDisplay) withObject:self waitUntilDone:NO];
}

- (void)makeCurrentContext
{
	mRenderer->startDraw();
}

- (uint32_t)addTouchToMap:(UITouch*)touch
{
	uint32_t candidateId = 0;
	bool found = true;
	while( found ) {
		candidateId++;
		found = false;
		for( const auto &touchId : mTouchIdMap ) {
			if( touchId.second == candidateId ) {
				found = true;
				break;
			}
		}
	}
	
	mTouchIdMap.insert( std::make_pair( touch, candidateId ) );
	
	return candidateId;
}

- (void)removeTouchFromMap:(UITouch*)touch
{
	auto found( mTouchIdMap.find( touch ) );
	if( found == mTouchIdMap.end() )
		;//std::cout << "Couldn' find touch in map?" << std::endl;
	else
		mTouchIdMap.erase( found );
}

- (uint32_t)findTouchInMap:(UITouch*)touch
{
	const auto found( mTouchIdMap.find( touch ) );
	if( found == mTouchIdMap.end() ) {
		;//std::cout << "Couldn' find touch in map?" << std::endl;
		return 0;
	}
	else
		return found->second;
}

- (void)updateActiveTouches
{
	mActiveTouches.clear();
	for( const auto &touch : mTouchIdMap ) {
		CGPoint pt = [touch.first locationInView:self];
		CGPoint prevPt = [touch.first previousLocationInView:self];
		mActiveTouches.push_back( TouchEvent::Touch( vec2( pt.x, pt.y ), vec2( prevPt.x, prevPt.y ), touch.second, [touch.first timestamp], touch.first ) );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Event handlers

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	if( mApp->isMultiTouchEnabled() ) {
		std::vector<TouchEvent::Touch> touchList;
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];
			CGPoint prevPt = [touch previousLocationInView:self];
			touchList.push_back( TouchEvent::Touch( vec2( pt.x, pt.y ), vec2( prevPt.x, prevPt.y ), [self addTouchToMap:touch], [touch timestamp], touch ) );
		}
		[self updateActiveTouches];
		if( ! touchList.empty() ) {
			TouchEvent touchEvent( [mDelegate getWindowRef], touchList, event );
			[mDelegate touchesBegan:&touchEvent];
		}
	}
	else {
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];		
			int mods = 0;
			mods |= MouseEvent::LEFT_DOWN;
			MouseEvent mouseEvent( [mDelegate getWindowRef], MouseEvent::LEFT_DOWN, pt.x, pt.y, mods, 0.0f, 0 );
			[mDelegate mouseDown:&mouseEvent];
		}
	}
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	if( mApp->isMultiTouchEnabled() ) {
		std::vector<TouchEvent::Touch> touchList;
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];
			CGPoint prevPt = [touch previousLocationInView:self];			
			touchList.push_back( TouchEvent::Touch( vec2( pt.x, pt.y ), vec2( prevPt.x, prevPt.y ), [self findTouchInMap:touch], [touch timestamp], touch ) );
		}
		[self updateActiveTouches];
		if( ! touchList.empty() ) {
			TouchEvent touchEvent( [mDelegate getWindowRef], touchList, event );
			[mDelegate touchesMoved:&touchEvent];
		}
	}
	else {
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];
			int mods = 0;
			mods |= MouseEvent::LEFT_DOWN;
			MouseEvent mouseEvent( [mDelegate getWindowRef], MouseEvent::LEFT_DOWN, pt.x, pt.y, mods, 0.0f, 0 );
			[mDelegate mouseDrag:&mouseEvent];
		}
	}
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	if( mApp->isMultiTouchEnabled() ) {
		std::vector<TouchEvent::Touch> touchList;
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];
			CGPoint prevPt = [touch previousLocationInView:self];
			touchList.push_back( TouchEvent::Touch( vec2( pt.x, pt.y ), vec2( prevPt.x, prevPt.y ), [self findTouchInMap:touch], [touch timestamp], touch ) );
			[self removeTouchFromMap:touch];
		}
		[self updateActiveTouches];
		if( ! touchList.empty() ) {
			TouchEvent touchEvent( [mDelegate getWindowRef], touchList, event );
			[mDelegate touchesEnded:&touchEvent];
		}
	}
	else {
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];		
			int mods = 0;
			mods |= MouseEvent::LEFT_DOWN;
			MouseEvent mouseEvent( [mDelegate getWindowRef], MouseEvent::LEFT_DOWN, pt.x, pt.y, mods, 0.0f, 0 );
			[mDelegate mouseUp:&mouseEvent];
		}
	}
}

- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
	[self touchesEnded:touches withEvent:event];
}

- (const std::vector<TouchEvent::Touch>&)getActiveTouches
{
	return mActiveTouches;
}

@end
