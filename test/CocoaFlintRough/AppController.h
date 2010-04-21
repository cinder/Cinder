//
//  AppController.h
//  CocoaFlintRough
//
//  Created by andrewfb on 2/21/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "flint/App/FlintView.h"
#include "SimpleApp.h"
#import <Cocoa/Cocoa.h>

@interface AppController : NSObject {
	IBOutlet FlintView		*flintView;
	IBOutlet NSSlider		*subdivisionSlider;
	IBOutlet NSColorWell	*colorWell;
	
	SimpleApp				*app;
}

- (IBAction)subdivisionSliderChanged:(id)sender;
- (IBAction)colorChanged:(id)sender;
- (IBAction)fullScreenClicked:(id)sender;

@end
