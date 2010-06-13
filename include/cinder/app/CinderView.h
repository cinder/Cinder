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

#include "cinder/Cinder.h"
#include "cinder/app/App.h"

#import <AppKit/NSView.h>
#import <Foundation/Foundation.h>
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_5
	#import <AppKit/NSTouch.h> 
	#include "cinder/app/TouchEvent.h"
#endif

#include <map>

#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_5
@protocol CinderViewMultiTouchDelegate
- (void)touchesBegan:(ci::app::TouchEvent*)event;
- (void)touchesMoved:(ci::app::TouchEvent*)event;
- (void)touchesEnded:(ci::app::TouchEvent*)event;
- (void)touchesEnded:(ci::app::TouchEvent*)event;
- (void)setActiveTouches:(std::vector<ci::app::TouchEvent::Touch>*)touches;
@end
#endif

@interface CinderView : NSView
{
@public
	cinder::app::App	*app;
	BOOL				stayFullScreen;
	BOOL				appSetupCalled;
	BOOL				receivesEvents;

#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_5	
	NSMutableDictionary					*mTouchIdMap;
	std::map<uint32_t,ci::Vec2f>		mTouchPrevPointMap;
	id<CinderViewMultiTouchDelegate>	mMultiTouchDelegate;
#endif
}

@property (readwrite) BOOL appSetupCalled;
@property (readwrite) BOOL receivesEvents;

- (id)initWithFrame:(NSRect)frame app:(cinder::app::App*)aApp;
- (id)initFullScreenWithApp:(cinder::app::App*)aApp;
- (void)setupRenderer:(NSRect)frame;

- (void)draw;

- (void)setup:(cinder::app::App *)aApp;
- (void)setApp:(cinder::app::App *)aApp;

- (void)applicationWillResignActive:(NSNotification *)aNotification;

#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_5
- (void)setMultiTouchDelegate:(id<CinderViewMultiTouchDelegate>)multiTouchDelegate;
- (uint32_t)addTouchToMap:(NSTouch *)touch withPoint:(ci::Vec2f)point;
- (void)removeTouchFromMap:(NSTouch *)touch;
- (std::pair<uint32_t,ci::Vec2f>)updateTouch:(NSTouch *)touch withPoint:(ci::Vec2f)point;
- (void)updateActiveTouches:(NSEvent *)event;
#endif

@end
