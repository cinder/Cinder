#include "cinder/app/AppScreenSaver.h"

#import "MacConfigDialog.h"
#import <ScreenSaver/ScreenSaver.h>

// NOTE: It is important that your NIB has its Window's "Visible at Launch" set to FALSE

static ConfigWindowController *sController = nil;


void loadConfigMac( cinder::app::AppScreenSaver *app, Configuration *config )
{
	NSBundle *ssaverBundle = app->getBundle();
	ScreenSaverDefaults *defaults = [ScreenSaverDefaults defaultsForModuleWithName:[ssaverBundle bundleIdentifier]];
	
	[defaults registerDefaults:[NSDictionary dictionaryWithObjectsAndKeys:
                   @"YES", @"DrawCinderLogo",
                   nil]];
				   
	config->mDrawCinderLogo = [defaults boolForKey:@"DrawCinderLogo"];
}

NSWindow* getConfigDialogMac( cinder::app::AppScreenSaver *app, Configuration *config )
{	
	if( ! sController ) {
		sController = [[ConfigWindowController alloc] initWithWindowNibName:@"MacConfigDialog"];
	}
	sController->config = config;
	sController->app = app;

	loadConfigMac( app, config );

	return [sController window];
}

void saveConfigMac( cinder::app::AppScreenSaver *app, const Configuration *config )
{
	NSBundle *ssaverBundle = app->getBundle();
	ScreenSaverDefaults *defaults = [ScreenSaverDefaults defaultsForModuleWithName:[ssaverBundle bundleIdentifier]];
			
	// Update our defaults
	[defaults setBool:config->mDrawCinderLogo 
			 forKey:@"DrawCinderLogo"];

	// Save the settings to disk
	[defaults synchronize];
}

@interface ConfigWindowController ()

@end

@implementation ConfigWindowController

- (id)initWithWindow:(NSWindow *)windowP
{
    self = [super initWithWindow:windowP];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	[drawCinderLogoButton setState:config->mDrawCinderLogo];
}

- (IBAction)cancelClick:(id)sender
{
	[[NSApplication sharedApplication] endSheet:[sController window]];
}

- (IBAction)okClick:(id)sender
{
	saveConfigMac( app, config );

	// Close the sheet
	[[NSApplication sharedApplication] endSheet:[sController window]];
}

- (IBAction)drawLogoClick:(id)sender
{
	config->mDrawCinderLogo = [sender state] == NSOnState;
}

@end
