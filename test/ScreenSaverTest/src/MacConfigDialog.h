#pragma once

#include "Configuration.h"

namespace cinder { namespace app {
	class AppScreenSaver;
} }

#if defined( __OBJC__ )

#import <Cocoa/Cocoa.h>

@interface ConfigWindowController : NSWindowController
{
  @public
	IBOutlet NSWindow 	*window;
	IBOutlet NSButton 	*okButton;
	IBOutlet NSButton 	*cancelButton;
	IBOutlet NSButton	*drawCinderLogoButton;
	
	Configuration					*config;
	cinder::app::AppScreenSaver		*app;
}

- (IBAction)cancelClick:(id)sender;
- (IBAction)okClick:(id)sender;

@end

#endif // defined( __OBJC__ )

extern NSWindow* 	getConfigDialogMac( cinder::app::AppScreenSaver *app, Configuration *outputConfig );
extern void			loadConfigMac( cinder::app::AppScreenSaver *app, Configuration *config );