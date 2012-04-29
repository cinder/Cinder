//
// ROGER / newResize: Replace NSWindow by NSWindowUnlimited
#import "NSWindowUnlimited.h"
@implementation NSWindowUnlimited
- (NSRect)constrainFrameRect:(NSRect)frameRect toScreen:(NSScreen *)screen
{
	return frameRect;
} @end
