//
//  main.m
//  iPhoneTestGl
//
//  Created by andrewfb on 2/26/10.
//  Copyright The Barbarian Group 2010. All rights reserved.
//

#import <UIKit/UIKit.h>

#include "TestApp.h"

ci::app::AppIPhone *gApp;

int main(int argc, char *argv[])
{    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	gApp = new TestApp;
    int retVal = UIApplicationMain(argc, argv, nil, nil);
    [pool release];
    return retVal;
}
