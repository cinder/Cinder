/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org
 
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

#include "LocationManager.h"

#include <CoreLocation/CoreLocation.h>

@interface LocationManagerDelegate : NSObject<CLLocationManagerDelegate> {
  @public
	cinder::LocationManager		*mMgr;
	cinder::LocationEvent		*mMostRecentLocationPtr;
	uint32_t					mErrorCount;
}

- (void)locationManager:(CLLocationManager *)manager didFailWithError:(NSError *)error;
- (LocationManagerDelegate*)init:(cinder::LocationManager*)mgr mostRecentLocationPtr:(cinder::LocationEvent*)mostRecentLocationPtr;
- (void)locationManager:( CLLocationManager * )manager didUpdateToLocation:( CLLocation * )newLocation fromLocation:( CLLocation * )oldLocation;
#if defined( CINDER_COCOA_TOUCH )
- (void)locationManager:( CLLocationManager * )manager didUpdateHeading:( CLHeading * )heading;
#endif

@end

@implementation LocationManagerDelegate

- (void)locationManager:(CLLocationManager *)manager didFailWithError:(NSError *)error
{
	++mErrorCount;
}

- (LocationManagerDelegate*)init:(cinder::LocationManager*)mgr mostRecentLocationPtr:(cinder::LocationEvent*)mostRecentLocationPtr
{
	self = [super init];
	
	mMgr = mgr;
	mMostRecentLocationPtr = mostRecentLocationPtr;
	mErrorCount = 0;
	
	return self;
}

- (void)locationManager:(CLLocationManager*)manager didUpdateToLocation:(CLLocation*)newLocation fromLocation:( CLLocation * )oldLocation
{
	if( newLocation.horizontalAccuracy < 0 )
		return; // not enough accuracy

	*mMostRecentLocationPtr = cinder::LocationEvent( cinder::Vec2f( newLocation.coordinate.latitude, newLocation.coordinate.longitude ), newLocation.speed,
								   newLocation.altitude, newLocation.horizontalAccuracy, newLocation.verticalAccuracy );

	mMgr->emitLocationChanged( *mMostRecentLocationPtr );
}

#if defined( CINDER_COCOA_TOUCH )
- (void)locationManager:(CLLocationManager*)manager didUpdateHeading:(CLHeading*)heading
{
    ci::Vec3f data( heading.x, heading.y, heading.z );
    std::string description( [heading.description UTF8String] );
	mMgr->emitHeadingChanged( cinder::HeadingEvent( heading.magneticHeading, heading.trueHeading, heading.headingAccuracy, description, data ) );
}
#endif

@end

namespace cinder {

LocationEvent LocationManager::sMostRecentLocation;

LocationManager::LocationManager()
	: mClLocationManager( 0 ), mDelegate( 0 )
{
}

LocationManager::~LocationManager()
{
	if( mClLocationManager )
		[mClLocationManager release];
	if( mDelegate )
		[mDelegate release];
}

LocationManager* LocationManager::get()
{
	static LocationManager *sInst = 0;
	if( ! sInst ) {
		sInst = new LocationManager;
	}
	
	return sInst;
}

void LocationManager::enable( float accuracyInMeters, float distanceFilter, float headingFilter )
{
	get()->enableImpl( accuracyInMeters, distanceFilter, headingFilter );
}

void LocationManager::enableImpl( float accuracyInMeters, float distanceFilter, float headingFilter )
{
	if( ! mClLocationManager ) {
		mClLocationManager = [[CLLocationManager alloc] init];
		mDelegate = [[LocationManagerDelegate alloc] init:this mostRecentLocationPtr:&sMostRecentLocation];
		mClLocationManager.delegate = mDelegate;
	}

#if defined( CINDER_COCOA_TOUCH )
	[mClLocationManager startUpdatingHeading];
#endif
	[mClLocationManager startUpdatingLocation];
	CLLocation *newLocation = mClLocationManager.location;	
	sMostRecentLocation = cinder::LocationEvent( cinder::Vec2f( newLocation.coordinate.latitude, newLocation.coordinate.longitude ), newLocation.speed,
								   newLocation.altitude, newLocation.horizontalAccuracy, newLocation.verticalAccuracy );
}

void LocationManager::disable()
{
	get()->disableImpl();
}

void LocationManager::disableImpl()
{
	if( mClLocationManager ) {
#if defined( CINDER_COCOA_TOUCH )
		[mClLocationManager stopUpdatingHeading];
#endif
		[mClLocationManager stopUpdatingLocation];	
	}
}

bool LocationManager::isEnabled()
{
	return get()->isEnabledImpl();
}

bool LocationManager::isEnabledImpl() const
{
	return [CLLocationManager locationServicesEnabled];
}

uint32_t LocationManager::getErrorCountImpl() const
{
	return mDelegate->mErrorCount;
}

} // namespace cinder