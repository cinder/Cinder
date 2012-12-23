#pragma once

#import <UIKit/UIKit.h>

#include "cinder/Function.h"

@interface NativeViewController : UINavigationController

- (void)addCinderViewToFront;
- (void)addCinderViewAsBarButton;

@property (nonatomic) std::function<void()> infoButtonCallback;

@end
