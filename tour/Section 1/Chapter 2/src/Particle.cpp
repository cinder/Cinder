#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;

Particle::Particle()
{
}

Particle::Particle( Vec2f loc )
{
	mLoc	= loc;
	mDir	= Rand::randVec2f();
	mVel	= 0.0f;
	mRadius	= 4.0f;
}	
	
void Particle::update( const Channel32f &channel )
{
	mRadius = channel.getValue( mLoc ) * 7.0f;

	//float gray = channel.getValue( mLoc );
	//mColor = Color( gray, gray, gray );
	
	mLoc += mDir * mVel;
}

void Particle::draw()
{
	//gl::color( mColor );
	gl::drawSolidCircle( mLoc, mRadius );
}
