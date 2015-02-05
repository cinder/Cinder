//
//  AppDelegate.m
//  TestApp
//
//  Created by Andrew Bell on 7/2/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/Cinder.h"
#include "cinder/app/RendererGl.h"
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
#if defined( USE_RENDERER2D )
	auto defaultRenderer = Renderer2d::create();
#else
	auto defaultRenderer = RendererGl::create();
#endif

	AppCocoaView::Settings settings;
	AppCocoaView::initialize( &settings, defaultRenderer );
	MyCinderApp::prepareSettings( &settings );

	mApp = new MyCinderApp;
	mApp->setupCinderView( cinderView );
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
