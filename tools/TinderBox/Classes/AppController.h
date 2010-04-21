//
//  AppController.h
//  TinderBox
//
//  Created by andrewfb on 2/7/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "FlintLocation.h"

@class TargetController;
@class PrefsController;

@interface AppController : NSObject {
	IBOutlet NSPopUpButton *targetPopup;
	IBOutlet NSPopUpButton *templatePopup;
	IBOutlet NSTextField *projectNameText;
	IBOutlet NSTextField *namingPrefixText;
	IBOutlet NSTextField *locationText;
	IBOutlet NSButton *locationButton;
	IBOutlet NSPopUpButton *flintVersionPopup;
	IBOutlet NSBox *targetOptionsBox;
	IBOutlet NSButton *createButton;
	IBOutlet NSWindow *win;

	BOOL saveDialogHasShown;
	BOOL needsSetupWizard;
	
	NSMutableArray *targetTypes;
	NSMutableArray *targetControllers;
	
	PrefsController *prefs;
}

- (IBAction)createProject:(id)sender;
- (IBAction)changeTarget:(id)sender;
- (IBAction)runSaveDialog:(id)sender;
- (IBAction)showPreferences:(id)sender;
- (void)runSetupWizard;
- (void)displayTargetViewController:(TargetController *)vc;
- (void)refreshLocations;

- (NSString *)getTargetTemplatesDir:(NSString *)targetName;
- (NSString *)projectName;
- (NSString *)projectLocation;
- (NSString *)namingPrefix;
- (int)getTemplateIndex;
- (NSString *)flintPath;
- (NSString *)winPrefix;

@end
