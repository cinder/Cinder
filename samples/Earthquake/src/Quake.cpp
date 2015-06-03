/*
 *  Quake.cpp
 *  HodginParticles
 *
 *  Created by Robert Hodgin on 7/1/09.
 *  Copyright 2009 The Barbarian Group. All rights reserved.
 *
 */

#include "Quake.h"
#include "cinder/CinderMath.h"

#include <sstream>

using namespace ci;
using namespace std;

Quake::Quake( float aLat, float aLong, float aMag, const string &aTitle )
{
	mLat = aLat;
	mLong = aLong;
	mMag = aMag;
	mTitle = aTitle;

	std::string magnitudeStr = toString( mMag );

	TextLayout layout;

	if( mMag > 5.5 ) {
		layout.setFont( Font( "HelveticaNeue-Bold", mMag * mMag + 26.0f ) );
		layout.setColor( Color( 1, 0, 0 ) );
	}
	else {
		layout.setFont( Font( "HelveticaNeue-Bold", mMag * mMag + 10.0f ) );
		layout.setColor( Color( 1, 1, 1 ) );
	}
	layout.addCenteredLine( magnitudeStr );

	if( mMag > 5.5 ) {
		layout.setLeadingOffset( -10 );
		layout.setFont( Font( "HelveticaNeue", mMag + 16 ) );
		layout.setColor( Color( 1, 1, 1 ) );
		layout.addCenteredLine( mTitle );
	}

	mLabel = gl::Texture2d::create( layout.render( true ) );

	float theta = glm::radians( 90 - mLat );
	float phi = glm::radians( 180 + mLong );

	mLoc = vec3( sin( theta ) * sin( phi ), cos( theta ), sin( theta ) * cos( phi ) );
}

std::string Quake::toString( float f, uint8_t precision )
{
	std::stringstream result;
	result << std::fixed << std::setprecision( precision ) << f;

	return result.str();
}
