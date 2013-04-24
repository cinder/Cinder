#include "MyCinderApp.h"

#include "cinder/app/CinderView.h"

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
	IBOutlet CinderView		*cinderView;
	IBOutlet NSWindow		*window;
	IBOutlet NSColorWell	*colorWell;
	
	MyCinderApp				*mApp;
}

@property (assign) IBOutlet NSWindow *window;

- (IBAction)subdivisionSliderChanged:(id)sender;
- (IBAction)colorChanged:(id)sender;

@end
