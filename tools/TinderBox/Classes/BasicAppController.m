//
//  BasicAppController.m
//  TinderBox
//
//  Created by andrewfb on 2/7/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "BasicAppController.h"
#import "AppController.h"

@implementation BasicAppController

- (id)initWithApp:(AppController *)aApp
{
	if( ! [super initWithApp:aApp nibName:@"Basic App"] )
		return nil;
		
	return self;
}

- (NSMutableDictionary*)prepareConditionals
{
	NSMutableDictionary *array = [[[NSMutableDictionary alloc] init] autorelease];
	
	[array setObject:[NSNumber numberWithBool:[vc9Button state] == NSOnState] forKey:@"«VC9»"];
	
	return array;
}

@end
