//
//  AppController.mm
//  CocoaFlintRough
//
//  Created by andrewfb on 2/21/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "AppController.h"
#include "flint/app/Renderer.h"
#include "flint/app/AppFlintView.h"
#include "flint/Color.h"
#include "SimpleApp.h"

@implementation AppController

- (id)init
{
	self = [super init];
	app = new SimpleApp();

	return self;
}

- (void)awakeFromNib
{
	app->launchApplication( flintView, new fli::app::RendererGL );
	
	[subdivisionSlider setIntValue:app->getSubdivision()];
	
	NSColor *color = [NSColor colorWithDeviceRed:app->getColor().r green:app->getColor().g blue:app->getColor().b alpha:1.0f]; 
	[colorWell setColor:color];
}

- (IBAction)subdivisionSliderChanged:(id)sender
{
	int val = [subdivisionSlider intValue];
	
	app->setSubdivision( val );
}

- (IBAction)colorChanged:(id)sender
{
	NSColor *color = [colorWell color];
	
	fli::Colorf fliColor( [color redComponent], [color greenComponent], [color blueComponent] );
	
	app->setColor( fliColor );
}

- (IBAction)fullScreenClicked:(id)sender
{
	app->setFullScreen( true );
}

@end
