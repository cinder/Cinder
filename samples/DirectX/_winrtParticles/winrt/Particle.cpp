#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/dx/dx.h"
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
	mRadius			= 5.0f;
	mScale			= 3.0f;
	mAge			= 0;
	mLifespan		= Rand::randInt( 50, 250);
	mAgePer			= 1.0f;
	mIsDead			= false;
}

void Particle::update( const Perlin &perlin, const Channel32f &channel, const Vec2i &mouseLoc )
{
	float nX = mLoc.x * 0.005f;
	float nY = mLoc.y * 0.005f;
	float nZ = (float)app::getElapsedSeconds()*0.1f;
	float noise = perlin.fBm(nX, nY, nZ );
	float angle = noise * 15.0f;
	Vec2f noiseVector( cos(angle), sin(angle) );

	mVel += noiseVector * 0.5f * ( 1.0f - mAgePer );
	mLoc += mVel;
	mVel *= mDecay;

	mRadius = mScale * mAgePer;

	mAge++;
	if ( mAge > mLifespan ) mIsDead = true;
	mAgePer = 1.0f - ( (float)mAge/(float)mLifespan );
}

void Particle::draw()
{
	float size = mRadius*2;
	dx::drawSolidTriangle( mLoc, mLoc + Vec2f(size,0.0), mLoc + Vec2f(size, size));
	//dx::drawSolidCircle( mLoc, mRadius*mAgePer*2 );
}
