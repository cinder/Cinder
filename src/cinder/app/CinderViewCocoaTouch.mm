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

@implementation CinderViewCocoaTouch

@synthesize animating;
@synthesize appSetupCalled;
@dynamic animationFrameInterval;

// Set in initWithFrame based on the renderer
static Boolean sIsEaglLayer;

+ (Class) layerClass
{
	if( sIsEaglLayer )
		return [CAEAGLLayer class];
	else
		return [CALayer class];
}

- (id)initWithFrame:(CGRect)frame app:(ci::app::AppCocoaTouch*)app renderer:(ci::app::Renderer*)renderer
{
	// This needs to get setup immediately as +layerClass will be called when the view is initialized
	sIsEaglLayer = renderer->isEaglLayer();
	
	if( (self = [super initWithFrame:frame]) ) {
		animating = FALSE;
		appSetupCalled = FALSE;
		animationFrameInterval = 1;
		displayLink = nil;
		mApp = app;
		mRenderer = renderer;
		if( [[UIScreen mainScreen] respondsToSelector:@selector(scale:)] &&
				[self respondsToSelector:@selector(setContentScaleFactor:)] )
			[self setContentScaleFactor:[[UIScreen mainScreen] scale]];

		renderer->setup( mApp, ci::cocoa::CgRectToArea( frame ), self );
		
		self.multipleTouchEnabled = mApp->getSettings().isMultiTouchEnabled();
	}
	
    return self;
}

- (void) layoutSubviews
{
	[super layoutSubviews];
	
	CGRect bounds = [self bounds];
	mRenderer->setFrameSize( bounds.size.width, bounds.size.height );
	if( appSetupCalled ) {
		mApp->privateResize__( ci::app::ResizeEvent( ci::Vec2i( bounds.size.width, bounds.size.height ) ) );
		[self drawView:nil];	
	}
}

- (void)drawRect:(CGRect)rect
{
	mRenderer->startDraw();
	mApp->privateUpdate__();
	mApp->privateDraw__();
	mRenderer->finishDraw();
}

- (void)drawView:(id)sender
{
	if( sIsEaglLayer ) {
		mRenderer->startDraw();
		mApp->privateUpdate__();
		mApp->privateDraw__();
		mRenderer->finishDraw();
	}
	else
		[self performSelectorOnMainThread:@selector(setNeedsDisplay) withObject:self waitUntilDone:NO];
}

- (void) startAnimation
{
	if( ! animating ) {
		displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
		[displayLink setFrameInterval:animationFrameInterval];
		[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		
		animating = TRUE;
	}
}

- (void)stopAnimation
{
	if( animating ) {
		[displayLink invalidate];
		displayLink = nil;
		
		animating = FALSE;
	}
}

- (NSInteger) animationFrameInterval
{
	return animationFrameInterval;
}

- (void) setAnimationFrameInterval:(NSInteger)frameInterval
{
	if ( frameInterval >= 1 ) {
		animationFrameInterval = frameInterval;
		
		if( animating ) {
			[self stopAnimation];
			[self startAnimation];
		}
	}
}

- (void)dealloc
{
    [super dealloc];
}

- (uint32_t)addTouchToMap:(UITouch*)touch
{
	uint32_t candidateId = 0;
	bool found = true;
	while( found ) {
		candidateId++;
		found = false;
		for( std::map<UITouch*,uint32_t>::const_iterator mapIt = mTouchIdMap.begin(); mapIt != mTouchIdMap.end(); ++mapIt ) {
			if( mapIt->second == candidateId ) {
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
	std::map<UITouch*,uint32_t>::iterator found( mTouchIdMap.find( touch ) );
	if( found == mTouchIdMap.end() )
		;//std::cout << "Couldn' find touch in map?" << std::endl;
	else
		mTouchIdMap.erase( found );
}

- (uint32_t)findTouchInMap:(UITouch*)touch
{
	std::map<UITouch*,uint32_t>::const_iterator found( mTouchIdMap.find( touch ) );
	if( found == mTouchIdMap.end() ) {
		;//std::cout << "Couldn' find touch in map?" << std::endl;
		return 0;
	}
	else
		return found->second;
}

- (void)updateActiveTouches
{
	static float contentScale = [self respondsToSelector:NSSelectorFromString(@"contentScaleFactor")] ? self.contentScaleFactor : 1;

	std::vector<ci::app::TouchEvent::Touch> activeTouches;
	for( std::map<UITouch*,uint32_t>::const_iterator touchIt = mTouchIdMap.begin(); touchIt != mTouchIdMap.end(); ++touchIt ) {
		CGPoint pt = [touchIt->first locationInView:self];
		CGPoint prevPt = [touchIt->first previousLocationInView:self];
		activeTouches.push_back( ci::app::TouchEvent::Touch( ci::Vec2f( pt.x, pt.y ) * contentScale, ci::Vec2f( prevPt.x, prevPt.y ) * contentScale, touchIt->second, [touchIt->first timestamp], touchIt->first ) );
	}
	mApp->privateSetActiveTouches__( activeTouches );
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Event handlers
- (void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	static float contentScale = [self respondsToSelector:NSSelectorFromString(@"contentScaleFactor")] ? self.contentScaleFactor : 1;
	
	if( mApp->getSettings().isMultiTouchEnabled() ) {
		std::vector<ci::app::TouchEvent::Touch> touchList;
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];
			CGPoint prevPt = [touch previousLocationInView:self];
			touchList.push_back( ci::app::TouchEvent::Touch( ci::Vec2f( pt.x, pt.y ) * contentScale, ci::Vec2f( prevPt.x, prevPt.y ) * contentScale, [self addTouchToMap:touch], [touch timestamp], touch ) );
		}
		[self updateActiveTouches];
		if( ! touchList.empty() )
			mApp->privateTouchesBegan__( ci::app::TouchEvent( touchList ) );
	}
	else {
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];		
			int mods = 0;
			mods |= cinder::app::MouseEvent::LEFT_DOWN;
			mApp->privateMouseDown__( cinder::app::MouseEvent( cinder::app::MouseEvent::LEFT_DOWN, pt.x * contentScale, pt.y * contentScale, mods, 0.0f, 0 ) );
		}
	}
}

- (void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	static float contentScale = [self respondsToSelector:NSSelectorFromString(@"contentScaleFactor")] ? self.contentScaleFactor : 1;

	if( mApp->getSettings().isMultiTouchEnabled() ) {
		std::vector<ci::app::TouchEvent::Touch> touchList;
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];
			CGPoint prevPt = [touch previousLocationInView:self];			
			touchList.push_back( ci::app::TouchEvent::Touch( ci::Vec2f( pt.x, pt.y ) * contentScale, ci::Vec2f( prevPt.x, prevPt.y ) * contentScale, [self findTouchInMap:touch], [touch timestamp], touch ) );
		}
		[self updateActiveTouches];
		if( ! touchList.empty() )
			mApp->privateTouchesMoved__( ci::app::TouchEvent( touchList ) );
	}
	else {
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];		
			int mods = 0;
			mods |= cinder::app::MouseEvent::LEFT_DOWN;
			mApp->privateMouseDrag__( cinder::app::MouseEvent( cinder::app::MouseEvent::LEFT_DOWN, pt.x * contentScale, pt.y * contentScale, mods, 0.0f, 0 ) );
		}
	}
}

- (void) touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	static float contentScale = [self respondsToSelector:NSSelectorFromString(@"contentScaleFactor")] ? self.contentScaleFactor : 1;

	if( mApp->getSettings().isMultiTouchEnabled() ) {
		std::vector<ci::app::TouchEvent::Touch> touchList;
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];
			CGPoint prevPt = [touch previousLocationInView:self];
			touchList.push_back( ci::app::TouchEvent::Touch( ci::Vec2f( pt.x, pt.y ) * contentScale, ci::Vec2f( prevPt.x, prevPt.y ) * contentScale, [self findTouchInMap:touch], [touch timestamp], touch ) );
			[self removeTouchFromMap:touch];
		}
		[self updateActiveTouches];
		if( ! touchList.empty() )
			mApp->privateTouchesEnded__( ci::app::TouchEvent( touchList ) );
	}
	else {
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self];		
			int mods = 0;
			mods |= cinder::app::MouseEvent::LEFT_DOWN;
			mApp->privateMouseUp__( cinder::app::MouseEvent( cinder::app::MouseEvent::LEFT_DOWN, pt.x * contentScale, pt.y * contentScale, mods, 0.0f, 0 ) );
		}
	}
}

- (void) touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
	[self touchesEnded:touches withEvent:event];
}


@end
