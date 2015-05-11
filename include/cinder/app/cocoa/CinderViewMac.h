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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBase.h"

#import <AppKit/NSView.h>
#import <Foundation/Foundation.h>
#import <AppKit/NSTouch.h> 
#include "cinder/app/TouchEvent.h"

#include <map>

@protocol CinderViewDelegate
@required
- (void)resize;
- (void)draw;
- (void)mouseDown:(cinder::app::MouseEvent*)event;
- (void)mouseDrag:(cinder::app::MouseEvent*)event;
- (void)mouseUp:(cinder::app::MouseEvent*)event;
- (void)mouseMove:(cinder::app::MouseEvent*)event;
- (void)mouseWheel:(cinder::app::MouseEvent*)event;
- (void)keyDown:(cinder::app::KeyEvent*)event;
- (void)keyUp:(cinder::app::KeyEvent*)event;
- (void)touchesBegan:(cinder::app::TouchEvent*)event;
- (void)touchesMoved:(cinder::app::TouchEvent*)event;
- (void)touchesEnded:(cinder::app::TouchEvent*)event;
- (void)fileDrop:(cinder::app::FileDropEvent*)event;
- (cinder::app::WindowRef)getWindowRef;
@end

@interface CinderViewMac : NSView {
  @private
	BOOL						mFullScreen;
	BOOL						mFullScreenModeKiosk;
	BOOL						mReadyToDraw; // don't allow draw until setup() and resize() have been called
	BOOL						mReceivesEvents;
	BOOL						mHighDensityDisplayEnabled;
	BOOL						mMultiTouchEnabled;
	cinder::app::RendererRef	mRenderer;
	
	float						mContentScaleFactor;

	NSMutableDictionary					*mTouchIdMap;
	std::map<uint32_t,cinder::vec2>	mTouchPrevPointMap;
	std::vector<cinder::app::TouchEvent::Touch> mActiveTouches;
	id<CinderViewDelegate>				mDelegate;
}

@property (readwrite) BOOL readyToDraw;
@property (readwrite) BOOL receivesEvents;

- (CinderViewMac *)initWithFrame:(NSRect)frame renderer:(cinder::app::RendererRef)renderer sharedRenderer:(cinder::app::RendererRef)sharedRenderer
			appReceivesEvents:(BOOL)appReceivesEvents highDensityDisplay:(BOOL)highDensityDisplay enableMultiTouch:(BOOL)enableMultiTouch;
- (void)setupRendererWithFrame:(NSRect)frame renderer:(cinder::app::RendererRef)renderer sharedRenderer:(cinder::app::RendererRef)sharedRenderer;

- (void)setDelegate:(id<CinderViewDelegate>)delegate;

- (BOOL)isFullScreen;
- (void)setFullScreen:(BOOL)fullScreen options:(const cinder::app::FullScreenOptions *)options;

- (void)draw;
- (void)makeCurrentContext;

- (void)applicationWillResignActive:(NSNotification *)aNotification;

- (cinder::app::RendererRef)getRenderer;

- (uint32_t)addTouchToMap:(NSTouch *)touch withPoint:(cinder::vec2)point;
- (void)removeTouchFromMap:(NSTouch *)touch;
- (std::pair<uint32_t,cinder::vec2>)updateTouch:(NSTouch *)touch withPoint:(cinder::vec2)point;
- (void)updateActiveTouches:(NSEvent *)event;
- (const std::vector<cinder::app::TouchEvent::Touch>&)getActiveTouches;

- (float)contentScaleFactor;


@end
