#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;

Particle::Particle()
{
}

Particle::Particle( Vec2f loc, Vec2f vel )
{
	mLoc			= loc;
	mVel			= vel;

	mDecay			= Rand::randFloat( 0.96f, 0.99f );
	mRadius			= 0.0f;
	mScale			= 3.0f;
	mAge			= 0;
	mLifespan		= Rand::randInt( 50, 250 );
	mAgePer			= 1.0f;
	mIsDead			= false;
}	

void Particle::update( const Perlin &perlin, const Channel32f &channel, const Vec2i &mouseLoc )
{	
	// get Perlin noise value based on my location and
	// elapsed seconds and turn this noise value into an angle.
	float nX = mLoc.x * 0.005f;
	float nY = mLoc.y * 0.005f;
	float nZ = app::getElapsedSeconds() * 0.1f;
	float noise = perlin.fBm( nX, nY, nZ );
	float angle = noise * 15.0f;
	Vec2f noiseVector( cos( angle ), sin( angle ) );

	mVel += noiseVector * 0.2f * ( 1.0f - mAgePer );
	mLoc += mVel;
	mVel *= mDecay;

	mRadius	= mScale * mAgePer;
	
	mAge++;
	if( mAge > mLifespan ) mIsDead = true;
	mAgePer = 1.0f - ( (float)mAge/(float)mLifespan );
}

void Particle::draw()
{
	gl::drawSolidCircle( mLoc, mRadius * mAgePer );
}



