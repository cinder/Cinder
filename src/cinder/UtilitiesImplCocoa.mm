/*
 *  UtilitiesImplCocoaTouch.mm
 *  cinder
 *
 *  Created by David Wicks on 12/17/10.
 *  Copyright 2010 David Wicks. All rights reserved.
 *
 */

#include "Utilities.h"
#import <UIKit/UIKit.h>

namespace cinder
{
	void launchWebBrowser( const Url &url )
	{
		[[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithCString:url.c_str() encoding:NSUnicodeStringEncoding]]];
	}
}