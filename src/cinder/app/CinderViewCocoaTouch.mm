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


#include "cinder/app/CinderViewCocoaTouch.h"
#include "cinder/cocoa/CinderCocoa.h"

#import <QuartzCore/QuartzCore.h>

@implementation CinderViewCocoaTouch

// Set in initWithFrame based on the renderer
static Boolean sIsEaglLayer;

+ (Class)layerClass
{
	if( sIsEaglLayer )
		return [CAEAGLLayer class];
	else
		return [CALayer class];
}

- (id)initWithFrame:(CGRect)frame app:(ci::app::AppCocoaTouch*)app renderer:(ci::app::RendererRef)renderer sharedRenderer:(ci::app::RendererRef)sharedRenderer contentScale:(float)contentScale
{
	// This needs to get setup immediately as +layerClass will be called when the view is initialized
	sIsEaglLayer = renderer->isEaglLayer();
	
	if( (self = [super initWithFrame:frame]) ) {
		mApp = app;
		mRenderer = renderer;

		renderer->setup( mApp, ci::cocoa::CgRectToArea( frame ), self, sharedRenderer );
		
		self.multipleTouchEnabled = mApp->getSettings().isMultiTouchEnabled();
	}

	mDelegate = nil;
	
    return self;
}

- (void)setDelegate:(id<CinderViewCocoaTouchDelegate>)delegate
{
	mDelegate = delegate;
}

- (ci::app::RendererRef)getRenderer
{
	return mRenderer;
}

- (void)layoutSubviews
{
	[super layoutSubviews];
	
	CGRect bounds = [self bounds];
	if( ! mApp->getSettings().isHighDensityDisplayEnabled() )
		self.layer.contentsScale = 1.0f;
	mRenderer->setFrameSize( bounds.size.width, bounds.size.height );

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
	// const float contentScale = [self contentScaleFactor];

	mActiveTouches.clear();
	for( const auto &touch : mTouchIdMap ) {
		CGPoint pt = [touch.first locationInView:self];
		CGPoint prevPt = [touch.first previousLocationInView:self];
		mActiveTouches.push_back( ci::app::TouchEvent::Touch( ci::Vec2f( pt.x, pt.y ), ci::Vec2f( prevPt.x, prevPt.y ), touch.second, [touch.first timestamp], touch.first ) );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Event handlers
- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	if( mApp->getSettings().isMultiTouchEnabled() ) {
		std::vector<ci::app::TouchEvent::Touch> touchList;
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];
			CGPoint prevPt = [touch previousLocationInView:self];
			touchList.push_back( ci::app::TouchEvent::Touch( ci::Vec2f( pt.x, pt.y ), ci::Vec2f( prevPt.x, prevPt.y ), [self addTouchToMap:touch], [touch timestamp], touch ) );
		}
		[self updateActiveTouches];
		if( ! touchList.empty() ) {
			ci::app::TouchEvent touchEvent( [mDelegate getWindowRef], touchList );
			[mDelegate touchesBegan:&touchEvent];
		}
	}
	else {
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];		
			int mods = 0;
			mods |= cinder::app::MouseEvent::LEFT_DOWN;
			cinder::app::MouseEvent mouseEvent( [mDelegate getWindowRef], cinder::app::MouseEvent::LEFT_DOWN, pt.x, pt.y, mods, 0.0f, 0 );
			[mDelegate mouseDown:&mouseEvent];
		}
	}
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	// const float contentScale = [self contentScaleFactor];

	if( mApp->getSettings().isMultiTouchEnabled() ) {
		std::vector<ci::app::TouchEvent::Touch> touchList;
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];
			CGPoint prevPt = [touch previousLocationInView:self];			
			touchList.push_back( ci::app::TouchEvent::Touch( ci::Vec2f( pt.x, pt.y ), ci::Vec2f( prevPt.x, prevPt.y ), [self findTouchInMap:touch], [touch timestamp], touch ) );
		}
		[self updateActiveTouches];
		if( ! touchList.empty() ) {
			ci::app::TouchEvent touchEvent( [mDelegate getWindowRef], touchList );
			[mDelegate touchesMoved:&touchEvent];
		}
	}
	else {
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];
			int mods = 0;
			mods |= cinder::app::MouseEvent::LEFT_DOWN;
			cinder::app::MouseEvent mouseEvent( [mDelegate getWindowRef], cinder::app::MouseEvent::LEFT_DOWN, pt.x, pt.y, mods, 0.0f, 0 );
			[mDelegate mouseDrag:&mouseEvent];
		}
	}
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	// const float contentScale = [self contentScaleFactor];

	if( mApp->getSettings().isMultiTouchEnabled() ) {
		std::vector<ci::app::TouchEvent::Touch> touchList;
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];
			CGPoint prevPt = [touch previousLocationInView:self];
			touchList.push_back( ci::app::TouchEvent::Touch( ci::Vec2f( pt.x, pt.y ), ci::Vec2f( prevPt.x, prevPt.y ), [self findTouchInMap:touch], [touch timestamp], touch ) );
			[self removeTouchFromMap:touch];
		}
		[self updateActiveTouches];
		if( ! touchList.empty() ) {
			ci::app::TouchEvent touchEvent( [mDelegate getWindowRef], touchList );
			[mDelegate touchesEnded:&touchEvent];
		}
	}
	else {
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];		
			int mods = 0;
			mods |= cinder::app::MouseEvent::LEFT_DOWN;
			cinder::app::MouseEvent mouseEvent( [mDelegate getWindowRef], cinder::app::MouseEvent::LEFT_DOWN, pt.x, pt.y, mods, 0.0f, 0 );
			[mDelegate mouseUp:&mouseEvent];
		}
	}
}

- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
	[self touchesEnded:touches withEvent:event];
}

- (const std::vector<cinder::app::TouchEvent::Touch>&)getActiveTouches
{
	return mActiveTouches;
}

@end
