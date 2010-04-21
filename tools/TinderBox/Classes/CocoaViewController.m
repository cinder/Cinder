//
//  CocoaViewController.m
//  TinderBox
//
//  Created by andrewfb on 2/7/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "CocoaViewController.h"


@implementation CocoaViewController

- (id)initWithApp:(AppController *)aApp;
{
	if( ! [super initWithApp:aApp nibName:@"Cocoa View"] )
		return nil;
	
	app = aApp;
	return self;
}

- (void)createProject
{
}

@end
