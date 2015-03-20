#include "MyCinderApp.h"

#include "cinder/app/cocoa/CinderViewMac.h"

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
	IBOutlet CinderViewMac	*cinderView;
	IBOutlet NSWindow		*window;
	IBOutlet NSColorWell	*colorWell;
	
	MyCinderApp				*mApp;
}

@property (assign) IBOutlet NSWindow *window;

- (IBAction)subdivisionSliderChanged:(id)sender;
- (IBAction)colorChanged:(id)sender;

@end
