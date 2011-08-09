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
        
        LocationEvent(LocationCoordinate2D coord, float speed, float altitude, float horizontalAccuracy, float verticalAccuracy):
                        Event(), mCoordinate(coord), mSpeed(speed), mAltitude(altitude), mHorizontalAccuracy(horizontalAccuracy),mVerticalAccuracy(verticalAccuracy){};
        
        LocationCoordinate2D getCoordinate(){return mCoordinate;}
        float getLatitude(){return mCoordinate.latitude;}
        float getLongitude(){return mCoordinate.longitude;}
        float getSpeed(){return mSpeed;}
        float getAltitude(){return mAltitude;}
        float getHorizontalAccuracy(){return mHorizontalAccuracy;}
        float getVerticalAccuracy(){return mVerticalAccuracy;}
        
        
        
    private:
        LocationCoordinate2D mCoordinate;
        float                mSpeed;
        float                mAltitude;
        float                mVerticalAccuracy;
        float                mHorizontalAccuracy;


















    };











}}