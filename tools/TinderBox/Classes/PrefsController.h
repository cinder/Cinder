//
//  PrefsController.h
//  TinderBox
//
//  Created by Andrew Bell on 3/12/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "FlintLocation.h"

@class AppController;

@interface PrefsController : NSWindowController {
	IBOutlet NSTableView		*flintLocationsTable;
	
	AppController		*app;
	
	NSMutableArray *flintLocations;
}

@property (readonly,copy) NSMutableArray *flintLocations;

- (void)writeDefaults;
- (id)init:(AppController*)aApp;
- (IBAction)newLocation:(id)sender;
- (IBAction)removeLocation:(id)sender;

@end
