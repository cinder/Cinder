//
//  AppDelegate.m
//  TestApp
//
//  Created by Andrew Bell on 7/2/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/Cinder.h"
#import "AppDelegate.h"

@implementation AppDelegate

@synthesize window;

using namespace cinder::app;

- (void)dealloc
{
    [super dealloc];
}
	
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	mApp = new MyCinderApp;
	mApp->prepareLaunch();
#if defined( USE_RENDERER2D )
	mApp->setupCinderView( cinderView, Renderer2d::create() );
#else
	mApp->setupCinderView( cinderView, RendererGl::create() );
#endif
	mApp->launch();
	
	colorWell.color = [NSColor colorWithCalibratedRed:mApp->mColor.r green:mApp->mColor.g blue:mApp->mColor.b alpha:1.0f]; 
}

- (IBAction)subdivisionSliderChanged:(id)sender
{
	mApp->mRadius = [sender intValue];
}

- (IBAction)colorChanged:(id)sender
{
	NSColor *color = [sender color];
	mApp->mColor = ci::Colorf( [color redComponent], [color greenComponent], [color blueComponent] );
}

@end
