//
//  HeadingEvent.h
//  cinder
//
//  Created by  on 04/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/Event.h"

namespace cinder { namespace app {
    
    class HeadingEvent : public Event {
    public:
        
        HeadingEvent(float magneticHeading, float trueHeading, float headingAccuracy, const char *description, const Vec3f &rawGeoMagnetismVector):
        Event(), mMagneticHeading(magneticHeading), mTrueHeading(trueHeading), mHeadingAccuracy(headingAccuracy), mDescription(description), mRawGeoMagnetismVector(rawGeoMagnetismVector){};
        
        float getMagneticHeading(){return mMagneticHeading;}
        float getTrueHeading(){return mTrueHeading;}
        float getHeadingAccuracy(){return mHeadingAccuracy;}
        const char *getDescription(){return mDescription;}
        Vec3f getRawGeoMagnetismVector(){return mRawGeoMagnetismVector;}
        
    private:
        
        float mMagneticHeading;
        float mTrueHeading;
        float mHeadingAccuracy;
        const char  *mDescription;
        Vec3f mRawGeoMagnetismVector;
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
    };
    
    
    
    
    
    
    
    
    
    
    
}}