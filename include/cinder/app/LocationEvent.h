//
//  LocationEvent.h
//  cinder
//
//  Created by  on 01/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/Event.h"

typedef struct {
	float latitude;
	float longitude;
} LocationCoordinate2D;

namespace cinder { namespace app {

    class LocationEvent : public Event {
    public:
        
        LocationEvent(LocationCoordinate2D coord): Event(), mCoordinate( coord ){};
        
        LocationCoordinate2D getCoordinate(){return mCoordinate;}
        float getLatitude(){return mCoordinate.latitude;}
        float getLongitude(){return mCoordinate.longitude;}
        
        
        
        
        
    private:
        LocationCoordinate2D mCoordinate;


















    };











}}