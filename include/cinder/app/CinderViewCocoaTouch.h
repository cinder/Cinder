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
#import <QuartzCore/QuartzCore.h>

#include "cinder/gl/gl.h"
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"

#include <map>

@interface CinderViewCocoaTouch : UIView
{    
  @private
	BOOL animating;
	NSInteger animationFrameInterval;

	id							displayLink;
	ci::app::AppCocoaTouch		*mApp;
	ci::app::Renderer			*mRenderer;
	std::map<UITouch*,uint32_t>	mTouchIdMap;
	ci::Vec3d					mAcceleration;
	BOOL						appSetupCalled;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;
@property (readwrite) BOOL appSetupCalled;

- (id)initWithFrame:(CGRect)frame app:(cinder::app::AppCocoaTouch*)app renderer:(cinder::app::Renderer*)renderer;
- (void)drawRect:(CGRect)rect;
- (void)startAnimation;
- (void)layoutSubviews;
- (void)drawView:(id)sender;
- (void)stopAnimation;

- (uint32_t)addTouchToMap:(UITouch*)touch;
- (void)removeTouchFromMap:(UITouch*)touch;
- (uint32_t)findTouchInMap:(UITouch*)touch;
- (void)updateActiveTouches;
- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event;

@end
