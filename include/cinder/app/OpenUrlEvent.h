//
//  OpenUrlEvent.h
//  cinder
//
//  Created by Tom Carden on 8/30/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once

#include "cinder/app/Event.h"

namespace cinder { namespace app {

// class for carrying data about application:openURL:sourceApplication:annotation: calls to AppCocoaTouch
// (previously handleOpenURL, now deprecated in iOS)

class OpenUrlEvent : public Event {
  public:
    
    OpenUrlEvent(const std::string &url) : mUrl(url) 
    {
    }
    
    std::string getUrl() const { return mUrl; }
    
  private:
    
    std::string mUrl;
};    
    
} }