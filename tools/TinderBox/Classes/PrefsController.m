//
//  PrefsController.m
//  TinderBox
//
//  Created by Andrew Bell on 3/12/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "PrefsController.h"
#import "AppController.h"

@implementation PrefsController
@synthesize flintLocations;

- (id)init:(AppController*)aApp
{
	self = [super initWithWindowNibName:@"Preferences"];
	
	app = aApp;

	flintLocations = [[NSMutableArray alloc] init];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	if( ! [defaults objectForKey:@"UserPreferencesValidated"] ) {
		[defaults setBool:YES forKey:@"UserPreferencesValidated"];
	}
	
	NSData *flintLocationsData = [[NSUserDefaults standardUserDefaults] dataForKey:@"FlintLocations"];
	if( flintLocationsData != nil ) {
		flintLocations = (NSMutableArray *)[NSUnarchiver unarchiveObjectWithData:flintLocationsData];
		[flintLocations retain];
	}
	NSLog( @"locs: %@", flintLocations );	
//[flintLocations removeAllObjects];	
	return self;
}

- (void)windowDidLoad
{
}

- (void)writeDefaults
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSData *theData = [NSArchiver archivedDataWithRootObject:flintLocations];
	[defaults setObject:theData forKey:@"FlintLocations"];
	[defaults synchronize];
}

- (void)windowWillClose:(NSNotification *)notification
{
	[self writeDefaults];
}

- (IBAction)newLocation:(id)sender
{
	FlintLocation *loc = [[FlintLocation alloc] init];
	loc.name = @"New Flint";
	[flintLocations addObject:loc];
	[loc release];
	[flintLocationsTable reloadData];
}

- (IBAction)removeLocation:(id)sender
{
	NSIndexSet *rows = [flintLocationsTable selectedRowIndexes];
	
	if( [rows count] > 0 ) {
		unsigned int row = [rows lastIndex];
		
		while( row != NSNotFound ) {
			[flintLocations removeObjectAtIndex:row];
			row = [rows indexLessThanIndex:row];
		}
		[flintLocationsTable reloadData];
	}
}

- (int)numberOfRowsInTableView:(NSTableView *)tableView
{
	if( tableView == flintLocationsTable )
		return [flintLocations count];
	else
		return 0;
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)rowIndex
{
	FlintLocation *loc = [flintLocations objectAtIndex:rowIndex];
	return [loc valueForKey:[aTableColumn identifier]];
}

- (void)tableView:(NSTableView *)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn *)aTableColumn row:(int)rowIndex
{
	FlintLocation *loc = [flintLocations objectAtIndex:rowIndex];
	[loc setValue:anObject forKey:[aTableColumn identifier]];
	
	if( ! [[aTableColumn identifier] isEqualToString:@"name"] )
		[app refreshLocations];
}

@end
