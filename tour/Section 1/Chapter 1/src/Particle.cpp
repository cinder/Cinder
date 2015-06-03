#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

using namespace ci;

Particle::Particle()
{
}

Particle::Particle( vec2 loc )
{
	mLoc	= loc;
	mDir	= Rand::randVec2();
	mVel	= Rand::randFloat( 5.0f );
	mRadius	= 3.0f;
}

void Particle::update()
{
	mLoc += mDir * mVel;
}

void Particle::draw()
{
	gl::drawSolidCircle( mLoc, mRadius );
}
