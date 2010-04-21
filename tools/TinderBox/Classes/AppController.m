//
//  AppController.m
//  TinderBox
//
//  Created by andrewfb on 2/7/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "AppController.h"
#import "BasicAppController.h"
#import "CocoaTouchAppController.h"
#import "CocoaViewController.h"
#import "PrefsController.h"
#import "ScreenSaverController.h"

@implementation AppController

- (id)init
{
	[super init];
	targetControllers = [[NSMutableArray alloc] init];
	
	saveDialogHasShown = NO;
	
	// Basic App View Controller
	TargetController *vc;
	vc = [[BasicAppController alloc] initWithApp:self];
	[targetControllers addObject:vc];
	[vc release];

	vc = [[CocoaTouchAppController alloc] initWithApp:self];
	[targetControllers addObject:vc];
	[vc release];
	
	// Cocoa View Controller
/*	vc = [[CocoaViewController alloc] initWithApp:self];
	[targetControllers addObject:vc];
	[vc release];*/

	// ScreenSaver Controller
	vc = [[ScreenSaverController alloc] initWithApp:self];
	[targetControllers addObject:vc];
	[vc release];
	
	prefs = [[PrefsController alloc] init:self];
	needsSetupWizard = ( [[prefs flintLocations] count] <= 0 );
	
	return self;
}

- (void)awakeFromNib
{
	NSMenu *targetMenu = [targetPopup menu];
	int itemCount = [targetControllers count];

	// populate the Targets popup
	for( int i = 0; i < itemCount; ++i ) {
		NSViewController *vc = [targetControllers objectAtIndex:i];
		NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:[vc title] action:NULL keyEquivalent:@""];
		[mi setTarget:self];
		[mi setAction:@selector(changeTarget:)];
		[mi setTag:i];
		[targetMenu addItem:mi];
		[mi release];
	}
	
	[self displayTargetViewController:[targetControllers objectAtIndex:0]];
	[targetPopup selectItemAtIndex:0];
	[self changeTarget:[[targetPopup menu] itemAtIndex:0]];

	[self refreshLocations];
}

- (void)dealloc
{
	[targetControllers release];
	[prefs release];
	[super dealloc];
}

- (void)windowDidUpdate:(NSNotification *)notification
{
	if( needsSetupWizard ) {
		// this must be the first time the user has run TinderBox, or the prefs are correct
		// We'll need them to find Cinder and for us.
		needsSetupWizard = NO;
		[self runSetupWizard];
	}
	else if( ! saveDialogHasShown ) {
		saveDialogHasShown = YES;
		[self runSaveDialog:self];
	}
}

- (void)displayTargetViewController:(TargetController *)vc
{
	NSWindow *w = [targetOptionsBox window];
	BOOL ended = [w makeFirstResponder:w];
	if( ! ended ) {
		NSBeep();
		return;
	}
	
	NSView *v = [vc view];
	
	// Compute the new window
	NSSize currentSize = [[targetOptionsBox contentView] frame].size;
	NSSize newSize = [v frame].size;
	float deltaWidth = newSize.width - currentSize.width;
	float deltaHeight = newSize.height - currentSize.height;
	NSRect windowFrame = [w frame];
	windowFrame.size.height += deltaHeight;
	windowFrame.origin.y -= deltaHeight;
	windowFrame.size.width += deltaWidth;
	
	[targetOptionsBox setContentView:nil];
	[w setFrame:windowFrame display:YES animate:YES];
	[targetOptionsBox setContentView:v];
}

- (void)refreshLocations
{
	NSMutableArray *flintLocations = [prefs flintLocations];

	int oldIndex = [flintVersionPopup indexOfSelectedItem];
	[flintVersionPopup removeAllItems];
	
	// populate the Flint Versions popup
	NSMenu *flintVersionMenu = [flintVersionPopup menu];
	for( FlintLocation *loc in flintLocations ) {
		NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:[loc name] action:NULL keyEquivalent:@""];
		[mi setTarget:self];
		[mi setAction:nil];
		[mi setTag:0];
		[flintVersionMenu addItem:mi];
		[mi release];
	}
	
	if( oldIndex != -1 )
		[flintVersionPopup selectItemAtIndex:oldIndex];
}

- (IBAction)changeTarget:(id)sender
{
	int i = [sender tag];
	TargetController *vc = [targetControllers objectAtIndex:i];
	[self displayTargetViewController:vc];
	
	// Setup the templates item
	[templatePopup removeAllItems];
	NSMutableArray *templates = [vc templates];
	for( NSString *s in templates ) {
		NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:s action:NULL keyEquivalent:@""];
		[[templatePopup menu] addItem:mi];
		[mi release];	
	}
}

- (void)savePanelDidEnd:(NSSavePanel *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	[sheet orderOut:nil];
	
	if( returnCode == NSOKButton ) {
		[projectNameText setStringValue:[[sheet filename] lastPathComponent]];
		[namingPrefixText setStringValue:[[sheet filename] lastPathComponent]];		
		[locationText setStringValue:[sheet directory]];
	}
}

- (IBAction)runSaveDialog:(id)sender
{
	NSSavePanel *save = [NSSavePanel savePanel];
	
	[save beginSheetForDirectory:[self projectLocation] file:@"" modalForWindow:win modalDelegate:self didEndSelector:@selector(savePanelDidEnd:returnCode:contextInfo:) contextInfo:NULL];
}

- (IBAction)createProject:(id)sender
{
	// determine current target
	TargetController *currentTarget = [targetControllers objectAtIndex:[targetPopup indexOfSelectedItem]];
	[currentTarget createProject];
}

- (IBAction)showPreferences:(id)sender
{
	[prefs showWindow:self];
}

- (void)runSetupWizard
{
	NSInteger result = NSRunInformationalAlertPanel( @"Welcome", @"This looks like your first time to run TinderBox. Mind showing me where your Cinder directory is?", @"For Sure", nil, nil );
	NSOpenPanel *flintOpen = [NSOpenPanel openPanel];
	[flintOpen setCanChooseFiles:NO];
	[flintOpen setCanChooseDirectories:YES];
	[flintOpen setAllowsMultipleSelection:NO];
	[flintOpen runModalForDirectory:[@"~" stringByExpandingTildeInPath] file:nil types:nil];	
	NSString *flintPath = @"";
	if( result == NSOKButton ) {
		flintPath = [[[flintOpen filenames] objectAtIndex:0] stringByAbbreviatingWithTildeInPath];
	}

	FlintLocation *loc = [[FlintLocation alloc] init];
	[loc setName:@"HEAD"];
	[loc setFlintPath:flintPath];
	[loc setWinPrefix:@""];	
	[[prefs flintLocations] addObject:loc];
	[loc release];
	
	// force the user defaults to be written
	[prefs writeDefaults];
	
	[self refreshLocations];
}

- (NSString *)getTargetTemplatesDir:(NSString *)targetName
{
	return [NSString stringWithFormat:@"%@/Templates/%@",[[NSBundle mainBundle] resourcePath], targetName];
}

- (NSString *)projectName
{
	return [projectNameText stringValue];
}

- (NSString *)namingPrefix
{
	return [namingPrefixText stringValue];
}

- (NSString *)projectLocation
{
	return [locationText stringValue];
}

- (int)getTemplateIndex
{
	return [templatePopup indexOfSelectedItem];
}

- (NSString *)flintPath
{
	FlintLocation *loc = [[prefs flintLocations] objectAtIndex:[flintVersionPopup indexOfSelectedItem]];
	return [[loc flintPath] stringByExpandingTildeInPath];
}

- (NSString *)winPrefix
{
	FlintLocation *loc = [[prefs flintLocations] objectAtIndex:[flintVersionPopup indexOfSelectedItem]];
	return [loc winPrefix];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
	return YES;
}

@end