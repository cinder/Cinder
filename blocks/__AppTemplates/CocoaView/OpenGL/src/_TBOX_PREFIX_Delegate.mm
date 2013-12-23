#include "cinder/Cinder.h"
#include "cinder/Rand.h"
#import "_TBOX_PREFIX_Delegate.h"

@implementation _TBOX_PREFIX_Delegate

@synthesize window;

- (void)dealloc
{
    [super dealloc];
}
	
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	mApp = new _TBOX_PREFIX_App;
	mApp->prepareLaunch();
	mApp->setupCinderView( cinderView, cinder::app::RendererGl::create() );
	mApp->launch();
	
	colorWell.color = [NSColor colorWithCalibratedRed:mApp->mColor.r green:mApp->mColor.g blue:mApp->mColor.b alpha:1.0f]; 
}

- (IBAction)subdivisionSliderChanged:(id)sender
{
	mApp->mRadius = [sender intValue];
}

- (IBAction)colorChanged:(id)sender
{
	NSColor *color = [sender color];
	mApp->mColor = ci::Colorf( [color redComponent], [color greenComponent], [color blueComponent] );
}

@end
