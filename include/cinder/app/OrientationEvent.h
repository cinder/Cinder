/*
 Copyright (c) 2011, The Cinder Project: http://libcinder.org
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/Event.h"

namespace cinder { namespace app {

    enum DeviceOrientation { 
        UNKNOWN_ORIENTATION = UIDeviceOrientationUnknown,
        // Device oriented vertically, home button on the bottom:
        PORTRAIT_ORIENTATION = UIDeviceOrientationPortrait, 
        // Device oriented vertically, home button on the top:
        UPSIDE_DOWN_PORTRAIT_ORIENTATION = UIDeviceOrientationPortraitUpsideDown,  
        // Device oriented horizontally, home button on the right:
        LANDSCAPE_LEFT_ORIENTATION = UIDeviceOrientationLandscapeLeft,       
        // Device oriented horizontally, home button on the left:
        LANDSCAPE_RIGHT_ORIENTATION = UIDeviceOrientationLandscapeRight,      
        // Device oriented flat, face up:
        FACE_UP_ORIENTATION = UIDeviceOrientationFaceUp,              
        // Device oriented flat, face down:
        FACE_DOWN_ORIENTATION = UIDeviceOrientationFaceDown             
    };
    
    class OrientationEvent : public Event {
    public:
        
        OrientationEvent( const DeviceOrientation orientation, const DeviceOrientation prevOrientation )
		: Event(), mOrientation( orientation ), mPrevOrientation( prevOrientation )
        {
        }
        
        DeviceOrientation getOrientation() const { return mOrientation; }
        DeviceOrientation getPrevOrientation() const { return mPrevOrientation; }
        
        // TODO: getMatrix() analagous to AccelEvent::getMatrix;
        
    private:
        DeviceOrientation mOrientation, mPrevOrientation;
    };
    
    // For convenience only
    inline std::ostream& operator<<( std::ostream &out, const OrientationEvent &event )
    {
        out << event.getOrientation() << " raw: " << event.getPrevOrientation();
        return out;
    }
    
} } // namespace cinder::app
