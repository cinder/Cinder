/*
 *  UtilitiesImplCocoaTouch.mm
 *  cinder
 *
 *  Created by David Wicks on 12/17/10.
 *  Copyright 2010 David Wicks. All rights reserved.
 *
 */

#include "Utilities.h"

#if defined( CINDER_COCOA_TOUCH )
#import <UIKit/UIKit.h>
#elif defined( CINDER_COCOA )
#import <AppKit/NSWorkspace.h>
#import <Foundation/NSString.h>
#import <Foundation/NSURL.h>
#endif

namespace cinder
{
	void launchWebBrowser( const Url &url )
	{
		NSString *nsString = [NSString stringWithCString:url.c_str() encoding:NSUTF8StringEncoding];
		NSURL *nsUrl = [NSURL URLWithString:nsString];
	#if defined( CINDER_COCOA_TOUCH )
		[[UIApplication sharedApplication] openURL:nsUrl ];
	#else
		[[NSWorkspace sharedWorkspace] openURL:nsUrl ];
	#endif
		[nsString release];
		[nsUrl release];
	}
}