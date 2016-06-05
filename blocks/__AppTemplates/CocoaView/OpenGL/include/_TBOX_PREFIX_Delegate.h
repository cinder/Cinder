
#include "cinder/app/cocoa/CinderViewMac.h"
#include "_TBOX_PREFIX_App.h"
#include "cinder/app/RendererGl.h"
#import <Cocoa/Cocoa.h>

@interface _TBOX_PREFIX_Delegate : NSObject <NSApplicationDelegate>
{
	IBOutlet CinderViewMac	*cinderView;
	IBOutlet NSWindow		*window;
	IBOutlet NSColorWell	*colorWell;

	_TBOX_PREFIX_App		*mApp;
}

@property (assign) IBOutlet NSWindow *window;

- (IBAction)subdivisionSliderChanged:(id)sender;
- (IBAction)colorChanged:(id)sender;

@end
