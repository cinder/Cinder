/*
 Copyright (c) 2010, The Barbarian Group
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

#include "cinder/App/App.h"
#include "cinder/cocoa/CinderCocoaTouch.h"
#include "cinder/app/TouchEvent.h"
#include "cinder/app/AccelEvent.h"
#include "cinder/app/LocationEvent.h"
#include "cinder/app/HeadingEvent.h"

typedef enum{
    AccuracyBestForNavigation,
    AccuracyBest,
    AccuracyNearestTenMeters,
    AccuracyHundredMeters,
    AccuracyKilometer,
    AccuracyThreeKilometers,
}Accuracy;

namespace cinder { namespace app {
    
    struct AppCocoaTouchState;
    
    class AppCocoaTouch : public App {
    public:
        class Settings : public App::Settings {
        public:
            Settings()
			: App::Settings(), mEnableMultiTouch( true ) {}
            
            //! Registers the app to receive multiTouch events from the operating system. Enabled by default. If disabled, touch events are mapped to mouse events.
            void		enableMultiTouch( bool enable = true ) { mEnableMultiTouch = enable; }
            //! Returns whether the app is registered to receive multiTouch events from the operating system. Enabled by default. If disabled, touch events are mapped to mouse events.
            bool		isMultiTouchEnabled() const { return mEnableMultiTouch; }
            
        private:
            bool		mEnableMultiTouch;
        };
        
        float  compassDegree;
        bool   mShouldDisplayHeadingCalibration;
    
        AppCocoaTouch();
        virtual ~AppCocoaTouch() {}
        
        virtual void		prepareSettings( Settings *settings ) {}
        
        //! Override to respond to the beginning of a multitouch sequence
        virtual void		touchesBegan( TouchEvent event ) {}
        //! Override to respond to movement (drags) during a multitouch sequence
        virtual void		touchesMoved( TouchEvent event ) {}
        //! Override to respond to the end of a multitouch sequence
        virtual void		touchesEnded( TouchEvent event ) {}
        //! Returns a std::vector of all active touches
        const std::vector<TouchEvent::Touch>&	getActiveTouches() const { return mActiveTouches; }	
        //! Returns a Vec3d of the acceleration direction
        virtual void		accelerated( AccelEvent event ) {}
        //! Invoked when a new heading is available, Returns the compass degree.
        virtual void        compassUpdated(HeadingEvent newHeading){}
        //! Invoked when a new location is available. oldLocation may be nil if there is no previous location available.
        virtual void        didUpdateToLocation(LocationEvent oldLocation, LocationEvent newLocation){}
                
        //! Registers a callback for touchesBegan events. Returns a unique identifier which can be used as a parameter to unregisterTouchesBegan().
        CallbackId		registerTouchesBegan( std::function<bool (TouchEvent)> callback ) { return mCallbacksTouchesBegan.registerCb( callback ); }
        //! Registers a callback for touchesBegan events. Returns a unique identifier which can be used as a parameter to unregisterTouchesBegan().
        template<typename T>
        CallbackId		registerTouchesBegan( T *obj, bool (T::*callback)(TouchEvent) ) { return mCallbacksTouchesBegan.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
        //! Unregisters a callback for touchesBegan events.
        void			unregisterTouchesBegan( CallbackId id ) { mCallbacksTouchesBegan.unregisterCb( id ); }
        
        //! Registers a callback for touchesMoved events. Returns a unique identifier which can be used as a parameter to unregisterTouchesMoved().
        CallbackId		registerTouchesMoved( std::function<bool (TouchEvent)> callback ) { return mCallbacksTouchesMoved.registerCb( callback ); }
        //! Registers a callback for touchesMoved events. Returns a unique identifier which can be used as a parameter to unregisterTouchesMoved().
        template<typename T>
        CallbackId		registerTouchesMoved( T *obj, bool (T::*callback)(TouchEvent) ) { return mCallbacksTouchesMoved.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
        //! Unregisters a callback for touchesMoved events.
        void			unregisterTouchesMoved( CallbackId id ) { mCallbacksTouchesMoved.unregisterCb( id ); }
        
        //! Registers a callback for touchesEnded events. Returns a unique identifier which can be used as a parameter to unregisterTouchesEnded().
        CallbackId		registerTouchesEnded( std::function<bool (TouchEvent)> callback ) { return mCallbacksTouchesEnded.registerCb( callback ); }
        //! Registers a callback for touchesEnded events. Returns a unique identifier which can be used as a parameter to unregisterTouchesEnded().
        template<typename T>
        CallbackId		registerTouchesEnded( T *obj, bool (T::*callback)(TouchEvent) ) { return mCallbacksTouchesEnded.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
        //! Unregisters a callback for touchesEnded events.
        void			unregisterTouchesEnded( CallbackId id ) { mCallbacksTouchesEnded.unregisterCb( id ); }
        
        //! Registers a callback for accelerated events. Returns a unique identifier which can be used as a parameter to unregisterAccelerated().
        CallbackId		registerAccelerated( std::function<bool (AccelEvent)> callback ) { return mCallbacksAccelerated.registerCb( callback ); }
        //! Registers a callback for touchesEnded events. Returns a unique identifier which can be used as a parameter to unregisterTouchesEnded().
        template<typename T>
        CallbackId		registerAccelerated( T *obj, bool (T::*callback)(AccelEvent) ) { return mCallbacksAccelerated.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
        //! Unregisters a callback for touchesEnded events.
        void			unregisterAccelerated( CallbackId id ) { mCallbacksAccelerated.unregisterCb( id ); }
        
        
        //! Returns the width of the App's window measured in pixels, or the screen when in full-screen mode.	
        virtual int		getWindowWidth() const;
        //! Returns the height of the App's window measured in pixels, or the screen when in full-screen mode.	
        virtual int		getWindowHeight() const;
        //! Ignored on the iPhone.
        void			setWindowWidth( int windowWidth ) {}
        //! Ignored on the iPhone.
        void			setWindowHeight( int windowHeight ) {}
        //! Ignored on the iPhone.
        void			setWindowSize( int windowWidth, int windowHeight ) {}
        
        //! Enables the device's accelerometer and modifies its filtering. \a updateFrequency represents the frequency with which accelerated() is called, measured in Hz. \a filterFactor represents the amount to weight the current value relative to the previous.
        void enableAccelerometer( float updateFrequency = 30.0f, float filterFactor = 0.1f );
        //! Turns off the accelerometer
        void disableAccelerometer();
        
        //! Enables the device's location services, set displayHeadingCalibration to TRUE to display heading calibration info. The display will remain until heading is calibrated. The default accuracy level desired is AccuracyBest.
        void enableLocationSevices();
        
        void shouldDisplayHeadingCalibration(bool displayHeadingCalibration);
        
        void setAccuracyLevelDesired(Accuracy accuracy);
        
        void setDistanceFilter(float distanceFilter);
        
        
        //! Returns the maximum frame-rate the App will attempt to maintain.
        virtual float		getFrameRate() const;
        //! Sets the maximum frame-rate the App will attempt to maintain.
        virtual void		setFrameRate( float aFrameRate );
        //! Returns whether the App is in full-screen mode or not.
        virtual bool		isFullScreen() const;
        //! Sets whether the active App is in full-screen mode based on \a fullScreen
        virtual void		setFullScreen( bool aFullScreen );
        
        //! Returns the number seconds which have elapsed since the active App launched.
        virtual double		getElapsedSeconds() const;
        
        //! Returns the path to the application on disk
        virtual std::string			getAppPath();
        
        //! Ceases execution of the application. Not implemented yet on iPhone
        virtual void	quit();
        
        //! Returns a pointer to the current global AppBasic
        static AppCocoaTouch*	get() { return sInstance; }
        //! Returns a pointer to the current global AppBasic
        virtual const Settings&	getSettings() const { return mSettings; }
        
        
        //! \cond
        // These are called by application instantation macros and are only used in the launch process
        static void		prepareLaunch() { App::prepareLaunch(); }
        static void		executeLaunch( AppCocoaTouch *app, class Renderer *renderer, const char *title, int argc, char * const argv[] ) { sInstance = app; App::executeLaunch( app, renderer, title, argc, argv ); }
        static void		cleanupLaunch() { App::cleanupLaunch(); }
        
        virtual void	launch( const char *title, int argc, char * const argv[] );
        //! \endcond
        
        // DO NOT CALL - should be private but aren't for esoteric reasons
        //! \cond
        // Internal handlers - these are called into by AppImpl's. If you are calling one of these, you have likely strayed far off the path.
        void		privatePrepareSettings__();
        void		privateTouchesBegan__( const TouchEvent &event );
        void		privateTouchesMoved__( const TouchEvent &event );
        void		privateTouchesEnded__( const TouchEvent &event );
        void		privateSetActiveTouches__( const std::vector<TouchEvent::Touch> &touches ) { mActiveTouches = touches; }
        void		privateAccelerated__( const Vec3f &direction );
        void        privateCompassUpdated__(const float magneticHeading, const float trueHeading, const float headingAccuracy, const char *description, const Vec3f &rawGeoMagnetismVector);
        void        privateDidUpdateToLocation__(const float oldX, const float oldY, const float oldSpeed, const float oldAltitude, const float oldHorizontalAccuracy, const float oldVerticalAccuracy,
                                                 const float newX, const float newY, const float newSpeed, const float newAltitude, const float newHorizontalAccuracy, const float newVerticalAccuracy);
        //! \endcond
        
        //! CLLocationManager methods
        
        //! start updating heading
        void startUpdatingHeading();
        //! stop updating heading
        void stopUpdatingHeading();
        //! start updating location
        void startUpdatingLocation();
        //! stop updating location
        void stopUpdatingLocation();
        
        //! Returns YES if the device supports the heading service, otherwise NO.
        bool headingAvailable();
        
        //! Determines whether the user has location services enabled.
        bool locationServicesEnabled();
        
        //! Return the last location received. Will be nil until a location has been received.
        LocationEvent getLocation();
        
        //! Returns the lateral distance between two locations.
        float distanceBetweenLocations(LocationEvent locationA, LocationEvent locationB);
        
    private:
        friend void		setupCocoaTouchWindow( AppCocoaTouch *app );
        
        // The state is contained in a struct in order to avoid this .h needing to be compiled as Objective-C++
        std::shared_ptr<AppCocoaTouchState>		mState;
        
        static AppCocoaTouch	*sInstance;	
        Settings				mSettings;
        
        std::vector<TouchEvent::Touch>	mActiveTouches;
        
        CallbackMgr<bool (TouchEvent)>		mCallbacksTouchesBegan, mCallbacksTouchesMoved, mCallbacksTouchesEnded;
        CallbackMgr<bool (AccelEvent)>		mCallbacksAccelerated;
        
        float					mAccelFilterFactor;
        Vec3f					mLastAccel, mLastRawAccel;
        
    };
    
} } // namespace cinder::app

#define CINDER_APP_COCOA_TOUCH( APP, RENDERER )								\
int main( int argc, char *argv[] ) {										\
cinder::app::AppCocoaTouch::prepareLaunch();							\
cinder::app::AppCocoaTouch *app = new APP;								\
cinder::app::Renderer *ren = new RENDERER;								\
cinder::app::AppCocoaTouch::executeLaunch( app, ren, #APP, argc, argv );\
cinder::app::AppCocoaTouch::cleanupLaunch();							\
return 0;																\
}
