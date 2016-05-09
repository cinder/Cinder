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
	cinder::app::AppBase::Settings settings;
	cinder::app::RendererRef renderer( new cinder::app::RendererGl );
	_TBOX_PREFIX_App::initialize(&settings, renderer);

	mApp = new _TBOX_PREFIX_App;
	mApp->setupCinderView( cinderView );
	mApp->launch();

	[window setAcceptsMouseMovedEvents:YES];

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
