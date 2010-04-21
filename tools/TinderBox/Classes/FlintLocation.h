//
//  FlintLocation.h
//  TinderBox
//
//  Created by Andrew Bell on 2/20/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface FlintLocation : NSObject {
	NSString *name, *flintPath, *winPrefix;
}

@property (copy) NSString *name;
@property (copy) NSString *flintPath;
@property (copy) NSString *winPrefix;

- (void)encodeWithCoder:(NSCoder *)coder;
- (id)initWithCoder:(NSCoder *)coder;

@end
