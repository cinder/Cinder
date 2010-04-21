//
//  FlintLocation.m
//  TinderBox
//
//  Created by Andrew Bell on 2/20/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "FlintLocation.h"


@implementation FlintLocation
@synthesize name;
@synthesize flintPath;
@synthesize winPrefix;

- (id)init
{
	self = [super init];
	
	name = [NSString stringWithString:@""];
	flintPath = [NSString stringWithString:@""];
	winPrefix = [NSString stringWithString:@""];
	
	return self;
}

- (void)dealloc
{
	[super dealloc];
}


- (void)encodeWithCoder:(NSCoder *)coder
{
	[coder encodeObject:name];
	[coder encodeObject:flintPath];
	[coder encodeObject:winPrefix];
}

- (id)initWithCoder:(NSCoder *)coder
{
	self.name = [coder decodeObject];
	self.flintPath = [coder decodeObject];
	self.winPrefix = [coder decodeObject];
	return self;
}

@end
