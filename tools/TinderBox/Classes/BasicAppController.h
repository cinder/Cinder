//
//  BasicAppController.h
//  TinderBox
//
//  Created by andrewfb on 2/7/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "TargetController.h"

@interface BasicAppController : TargetController {
	IBOutlet NSButton	*vc9Button;
}

- (id)initWithApp:(AppController *)aApp;
- (NSMutableDictionary*)prepareConditionals;

@end
