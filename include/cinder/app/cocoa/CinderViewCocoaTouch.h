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

#pragma once

#import <UIKit/UIKit.h>

#include "cinder/app/cocoa/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"

#include <map>

@protocol CinderViewCocoaTouchDelegate
@required
- (void)resize;
- (void)draw;
- (void)mouseDown:(cinder::app::MouseEvent*)event;
- (void)mouseDrag:(cinder::app::MouseEvent*)event;
- (void)mouseUp:(cinder::app::MouseEvent*)event;
- (void)touchesBegan:(cinder::app::TouchEvent*)event;
- (void)touchesMoved:(cinder::app::TouchEvent*)event;
- (void)touchesEnded:(cinder::app::TouchEvent*)event;
- (cinder::app::WindowRef)getWindowRef;
@end


@interface CinderViewCocoaTouch : UIView
{
	cinder::app::AppCocoaTouch					*mApp;
	id<CinderViewCocoaTouchDelegate>			mDelegate;

	cinder::app::RendererRef					mRenderer;
	std::map<UITouch*,uint32_t>					mTouchIdMap;
	std::vector<cinder::app::TouchEvent::Touch> mActiveTouches;
}

- (CinderViewCocoaTouch *)initWithFrame:(CGRect)frame app:(cinder::app::AppCocoaTouch *)app renderer:(cinder::app::RendererRef)renderer sharedRenderer:(cinder::app::RendererRef)sharedRenderer;
- (void)setDelegate:(id<CinderViewCocoaTouchDelegate>)delegate;
- (cinder::app::RendererRef)getRenderer;

- (void)drawView;
- (void)makeCurrentContext;

- (uint32_t)addTouchToMap:(UITouch *)touch;
- (void)removeTouchFromMap:(UITouch *)touch;
- (uint32_t)findTouchInMap:(UITouch *)touch;
- (void)updateActiveTouches;
- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent *)event;
- (const std::vector<cinder::app::TouchEvent::Touch> &)getActiveTouches;

@end
