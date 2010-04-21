//
//  ScreenSaverController.h
//  TinderBox
//
//  Created by Andrew Bell on 3/24/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "TargetController.h"


@interface ScreenSaverController : TargetController {
	IBOutlet NSButton	*vc8Button;
}

- (id)initWithApp:(AppController *)aApp;
- (NSMutableDictionary*)prepareConditionals;

@end
