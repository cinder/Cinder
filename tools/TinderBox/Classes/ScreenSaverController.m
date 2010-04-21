//
//  ScreenSaverController.m
//  TinderBox
//
//  Created by Andrew Bell on 3/24/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "ScreenSaverController.h"


@implementation ScreenSaverController

- (id)initWithApp:(AppController *)aApp
{
	if( ! [super initWithApp:aApp nibName:@"ScreenSaver"] )
		return nil;
		
	return self;
}

- (NSMutableDictionary*)prepareConditionals
{
	NSMutableDictionary *array = [[[NSMutableDictionary alloc] init] autorelease];
	
	[array setObject:[NSNumber numberWithBool:[vc8Button state] == NSOnState] forKey:@"«VC9»"];
	
	return array;
}

@end
