#include "Force.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;

Force::Force( int aType, float aMagnitude )
	: mType( aType ), mMagnitude( aMagnitude )
{
	mEnabled = true;
}


void Force::update()
{
}


void Force::apply( std::list<Particle> &particles, int counter )
{
}


void Force::setMagnitude( float mag )
{
	mMagnitude = mag;
}



void Force::enableIfType( int type, bool enable )
{
	if( mType == type )
		mEnabled = enable;
}

