//
//  ManagingViewController.h
//  TinderBox
//
//  Created by andrewfb on 2/7/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class AppController;

@interface TargetController : NSViewController {
	AppController *app;
	
	NSMutableArray *templatesArray;
}

- (id)initWithApp:(AppController *)aApp nibName:(NSString *)nibName;
- (void)createProject;
- (void)enumerateTemplates;
- (NSMutableArray *)templates;

@end
