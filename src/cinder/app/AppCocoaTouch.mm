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

#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/CinderViewCocoaTouch.h"
#include "cinder/cocoa/CinderCocoaTouch.h"


namespace cinder { namespace app {
    
    AppCocoaTouch*				AppCocoaTouch::sInstance = 0;
    CLLocationManager           *locationManager;
    
    // This struct serves as a compile firewall for maintaining AppCocoaTouch state information
    struct AppCocoaTouchState {
        CinderViewCocoaTouch		*mCinderView;
        UIWindow					*mWindow;
        CFAbsoluteTime				mStartTime;
    };
    
    void setupCocoaTouchWindow( AppCocoaTouch *app )
    {
        app->privatePrepareSettings__();
        app->mState->mWindow = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
        app->mState->mCinderView = [[CinderViewCocoaTouch alloc] initWithFrame:[[UIScreen mainScreen] bounds] app:app renderer:app->getRenderer()];
        [app->mState->mWindow addSubview:app->mState->mCinderView];
        [app->mState->mCinderView release];
        [app->mState->mWindow makeKeyAndVisible];
        
        [app->mState->mCinderView layoutIfNeeded];
        app->privateSetup__();
        [app->mState->mCinderView setAppSetupCalled:YES];
        app->privateResize__( ci::app::ResizeEvent( ci::Vec2i( [app->mState->mCinderView bounds].size.width, [app->mState->mCinderView bounds].size.height ) ) );
        
        [app->mState->mCinderView startAnimation];
    }
    
} } // namespace cinder::app

@interface CinderAppDelegateIPhone : NSObject <UIApplicationDelegate, UIAccelerometerDelegate, CLLocationManagerDelegate> {
	cinder::app::AppCocoaTouch	*app;
    //    UIWindow				*window;
    //    CinderViewCocoaTouch	*cinderView;
}
@end

@implementation CinderAppDelegateIPhone


- (void) applicationDidFinishLaunching:(UIApplication *)application
{
	app = cinder::app::AppCocoaTouch::get();
	setupCocoaTouchWindow( app );
}

- (void) applicationWillResignActive:(UIApplication *)application
{
    //	[cinderView stopAnimation];
}

- (void) applicationDidBecomeActive:(UIApplication *)application
{
    //	[cinderView startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application didUpdateHeading:(CLHeading *)newHeading
{
    //	[cinderView stopAnimation];
	app->privateShutdown__();
}

- (void)locationManager:(CLLocationManager *)manager didUpdateToLocation:(CLLocation *)newLocation fromLocation:(CLLocation *)oldLocation
{
    app->privateDidUpdateToLocation__(oldLocation.coordinate.latitude, oldLocation.coordinate.longitude, oldLocation.speed, oldLocation.altitude, oldLocation.horizontalAccuracy, oldLocation.verticalAccuracy,
                                      newLocation.coordinate.latitude, newLocation.coordinate.longitude, newLocation.speed, newLocation.altitude, newLocation.horizontalAccuracy, newLocation.verticalAccuracy);
}

- (void) locationManager:(CLLocationManager *)manager didUpdateHeading:(CLHeading *)newHeading
{
    ci::Vec3f rawGeoMagnetismVector(newHeading.x, newHeading.y, newHeading.z);
    const char *tmp = [newHeading.description UTF8String];
    app->privateCompassUpdated__(newHeading.magneticHeading, newHeading.trueHeading, newHeading.headingAccuracy, tmp, rawGeoMagnetismVector);
}

- (BOOL)locationManagerShouldDisplayHeadingCalibration:(CLLocationManager *)manager
{
    return app->mShouldDisplayHeadingCalibration;
}

- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)thisAcceleration 
{
	// Massage the UIAcceleration class into a Vec3f
	ci::Vec3f direction( thisAcceleration.x, thisAcceleration.y, thisAcceleration.z );
	app->privateAccelerated__( direction );
}

- (void) dealloc
{
    //	[window release];
    //	[cinderView release];
	
	[super dealloc];
}

@end


namespace cinder { namespace app {
    
    AppCocoaTouch::AppCocoaTouch()
	: App()
    {
        mState = std::shared_ptr<AppCocoaTouchState>( new AppCocoaTouchState() );
        mState->mStartTime = ::CFAbsoluteTimeGetCurrent();
        mLastAccel = mLastRawAccel = Vec3f::zero();
    }
    
    void AppCocoaTouch::launch( const char *title, int argc, char * const argv[] )
    {
        ::UIApplicationMain( argc, const_cast<char**>( argv ), nil, @"CinderAppDelegateIPhone" );
    }
    
    int	AppCocoaTouch::getWindowWidth() const
    {
        ::CGRect bounds = [mState->mCinderView bounds];
        if( [mState->mCinderView respondsToSelector:NSSelectorFromString(@"contentScaleFactor")] )
            return ::CGRectGetWidth( bounds ) * mState->mCinderView.contentScaleFactor;
        else
            return ::CGRectGetWidth( bounds );
    }
    
    int	AppCocoaTouch::getWindowHeight() const
    {
        ::CGRect bounds = [mState->mCinderView bounds];
        if( [mState->mCinderView respondsToSelector:NSSelectorFromString(@"contentScaleFactor")] )
            return ::CGRectGetHeight( bounds ) * mState->mCinderView.contentScaleFactor;
        else
            return ::CGRectGetHeight( bounds );
    }
    
    //! Enables the accelerometer
    void AppCocoaTouch::enableAccelerometer( float updateFrequency, float filterFactor )
    {
        mAccelFilterFactor = filterFactor;
        if( updateFrequency <= 0 )
            updateFrequency = 30.0f;
        
        [[UIAccelerometer sharedAccelerometer] setUpdateInterval:1.0 / updateFrequency];
        CinderAppDelegateIPhone *appDel = (CinderAppDelegateIPhone *)[[UIApplication sharedApplication] delegate];
        [[UIAccelerometer sharedAccelerometer] setDelegate:appDel];
    }
    
    void AppCocoaTouch::disableAccelerometer() {
        
        [[UIAccelerometer sharedAccelerometer] setDelegate:nil];
    }
    
    void AppCocoaTouch::enableLocationSevices() {
        CinderAppDelegateIPhone *appDel = (CinderAppDelegateIPhone *)[[UIApplication sharedApplication] delegate];
        locationManager=[[[CLLocationManager alloc] init] retain];
        locationManager.delegate=appDel;
        NSLog(@"compass enabled");
    }    
    
    void AppCocoaTouch::shouldDisplayHeadingCalibration(bool displayHeadingCalibration)
    {
        mShouldDisplayHeadingCalibration=displayHeadingCalibration;
    }
    
    void AppCocoaTouch::setAccuracyLevelDesired(Accuracy accuracy)
    {
        CLLocationAccuracy locationManagerAccuracy;
        switch (accuracy) {
            case AccuracyBestForNavigation:
                locationManagerAccuracy=kCLLocationAccuracyBestForNavigation;
                break;
            case AccuracyBest:
                locationManagerAccuracy=kCLLocationAccuracyBest;
                break;
            case AccuracyNearestTenMeters:
                locationManagerAccuracy=kCLLocationAccuracyNearestTenMeters;
                break;
            case AccuracyHundredMeters:
                locationManagerAccuracy=kCLLocationAccuracyHundredMeters;
                break;
            case AccuracyKilometer:
                locationManagerAccuracy=kCLLocationAccuracyKilometer;
                break;
            case AccuracyThreeKilometers:
                locationManagerAccuracy=kCLLocationAccuracyThreeKilometers;
                break;
            default:
                break;
        }
        locationManager.desiredAccuracy=locationManagerAccuracy;
        NSLog(@"The desired accuracy is %f",locationManager.desiredAccuracy);
    }
    
    float AppCocoaTouch::getAccuracyLevelDesired()
    {
        return locationManager.desiredAccuracy;
    }
    
    void AppCocoaTouch::setDistanceFilter(float distanceFilter)
    {
        locationManager.distanceFilter=distanceFilter;
    }
    
    float AppCocoaTouch::getDistanceFilter()
    {
        return locationManager.distanceFilter;
    }
    
    void AppCocoaTouch::setHeadingFilter(float headingFilter)
    {
        locationManager.headingFilter=headingFilter;
    }
    
    float AppCocoaTouch::getHeadingFilter()
    {
        return locationManager.headingFilter;
    }
    
    //! Returns the maximum frame-rate the App will attempt to maintain.
    float AppCocoaTouch::getFrameRate() const
    {
        return 0;
    }
    
    //! Sets the maximum frame-rate the App will attempt to maintain.
    void AppCocoaTouch::setFrameRate( float aFrameRate )
    {
    }
    
    //! Returns whether the App is in full-screen mode or not.
    bool AppCocoaTouch::isFullScreen() const
    {
        return true;
    }
    
    //! Sets whether the active App is in full-screen mode based on \a fullScreen
    void AppCocoaTouch::setFullScreen( bool aFullScreen )
    {
    }
    
    double AppCocoaTouch::getElapsedSeconds() const
    {
        CFAbsoluteTime currentTime = ::CFAbsoluteTimeGetCurrent();
        return ( currentTime - mState->mStartTime );
    }
    
    std::string AppCocoaTouch::getAppPath()
    { 
        return [[[NSBundle mainBundle] bundlePath] UTF8String];
    }
    
    void AppCocoaTouch::quit()
    {
        return;
    }
    
    void AppCocoaTouch::privatePrepareSettings__()
    {
        prepareSettings( &mSettings );
    }
    
    void AppCocoaTouch::privateTouchesBegan__( const TouchEvent &event )
    {
        bool handled = false;
        for( CallbackMgr<bool (TouchEvent)>::iterator cbIter = mCallbacksTouchesBegan.begin(); ( cbIter != mCallbacksTouchesBegan.end() ) && ( ! handled ); ++cbIter )
            handled = (cbIter->second)( event );		
        if( ! handled )	
            touchesBegan( event );
    }
    
    void AppCocoaTouch::privateTouchesMoved__( const TouchEvent &event )
    {	
        bool handled = false;
        for( CallbackMgr<bool (TouchEvent)>::iterator cbIter = mCallbacksTouchesMoved.begin(); ( cbIter != mCallbacksTouchesMoved.end() ) && ( ! handled ); ++cbIter )
            handled = (cbIter->second)( event );		
        if( ! handled )	
            touchesMoved( event );
    }
    
    void AppCocoaTouch::privateTouchesEnded__( const TouchEvent &event )
    {	
        bool handled = false;
        for( CallbackMgr<bool (TouchEvent)>::iterator cbIter = mCallbacksTouchesEnded.begin(); ( cbIter != mCallbacksTouchesEnded.end() ) && ( ! handled ); ++cbIter )
            handled = (cbIter->second)( event );		
        if( ! handled )	
            touchesEnded( event );
    }
    
    void AppCocoaTouch::privateAccelerated__( const Vec3f &direction )
    {
        Vec3f filtered = mLastAccel * (1.0f - mAccelFilterFactor) + direction * mAccelFilterFactor;
        
        AccelEvent event( filtered, direction, mLastAccel, mLastRawAccel );
        
        bool handled = false;
        for( CallbackMgr<bool (AccelEvent)>::iterator cbIter = mCallbacksAccelerated.begin(); ( cbIter != mCallbacksAccelerated.end() ) && ( ! handled ); ++cbIter )
            handled = (cbIter->second)( event );		
        if( ! handled )	
            accelerated( event );
        
        mLastAccel = filtered;
        mLastRawAccel = direction;
    }
    
    void AppCocoaTouch::privateCompassUpdated__(const float magneticHeading, const float trueHeading, const float headingAccuracy, const char *description, const Vec3f &rawGeoMagnetismVector)
    {
        HeadingEvent newHeading(magneticHeading, trueHeading, headingAccuracy, description, rawGeoMagnetismVector);
        compassUpdated(newHeading);
    }
    
    void AppCocoaTouch::privateDidUpdateToLocation__(const float oldLatitude, const float oldLongitude, const float oldSpeed, const float oldAltitude, const float oldHorizontalAccuracy, const float oldVerticalAccuracy,
                                                     const float newLatitude, const float newLongitude, const float newSpeed, const float newAltitude, const float newHorizontalAccuracy, const float newVerticalAccuracy){
        LocationCoordinate2D oldLocationCoordinate2D;
        oldLocationCoordinate2D.latitude=oldLatitude;
        oldLocationCoordinate2D.longitude=oldLongitude;
        LocationCoordinate2D newLocationCoordinate2D;
        newLocationCoordinate2D.latitude=newLatitude;
        newLocationCoordinate2D.longitude=newLongitude;
        
        LocationEvent newLocation(newLocationCoordinate2D,newSpeed, newAltitude, newHorizontalAccuracy, newVerticalAccuracy);
        LocationEvent oldLocation(oldLocationCoordinate2D,oldSpeed, oldAltitude, oldHorizontalAccuracy, oldVerticalAccuracy);
        didUpdateToLocation(oldLocation,newLocation);
    }
   
    //!CLLocationMethod
    void AppCocoaTouch::startUpdatingHeading()
    {
        [locationManager startUpdatingHeading];
    }
    
    void AppCocoaTouch::stopUpdatingHeading()
    {
        [locationManager stopUpdatingHeading];
    }
    
    void AppCocoaTouch::startUpdatingLocation()
    {
        [locationManager startUpdatingLocation];
    }
    
    void AppCocoaTouch::stopUpdatingLocation()
    {
        [locationManager stopUpdatingLocation];
    }
    
    bool AppCocoaTouch::headingAvailable()
    {
        return [CLLocationManager headingAvailable];
    }
    
    bool AppCocoaTouch::locationServicesEnabled()
    {
        return [CLLocationManager locationServicesEnabled];
    }
    
    LocationEvent AppCocoaTouch::getLocation()
    {
        NSLog(@"The distance filter is %f",locationManager.distanceFilter);
        LocationCoordinate2D locationCoordinate2D;
        locationCoordinate2D.latitude=locationManager.location.coordinate.latitude;
        locationCoordinate2D.longitude=locationManager.location.coordinate.longitude;
        LocationEvent newLocation(locationCoordinate2D,locationManager.location.speed, locationManager.location.altitude, locationManager.location.horizontalAccuracy, locationManager.location.verticalAccuracy);
        return newLocation;
    }
    
    
    float AppCocoaTouch::distanceBetweenLocations(LocationEvent locationA, LocationEvent locationB)
    {
        CLLocation *mLocationA=[[CLLocation alloc] initWithCoordinate:CLLocationCoordinate2DMake(locationA.getLatitude(), locationA.getLongitude())
                                                             altitude:locationA.getAltitude() 
                                                   horizontalAccuracy:kCLLocationAccuracyBest verticalAccuracy:kCLLocationAccuracyBest 
                                                            timestamp:[NSDate date]];
        CLLocation *mLocationB=[[CLLocation alloc] initWithCoordinate:CLLocationCoordinate2DMake(locationB.getLatitude(), locationB.getLongitude())
                                                             altitude:locationB.getAltitude() 
                                                   horizontalAccuracy:kCLLocationAccuracyBest verticalAccuracy:kCLLocationAccuracyBest 
                                                            timestamp:[NSDate date]]; 
        float distance=[mLocationA distanceFromLocation:mLocationB];
        return distance;
    }
    
    
} } // namespace cinder::app