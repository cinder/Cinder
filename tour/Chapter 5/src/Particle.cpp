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
	mAcc			= Vec2f::zero();
	
	mDecay			= Rand::randFloat( 0.95f, 0.951f );
	mRadius			= 3.0f + Rand::randFloat( 0.1 );
	mRadiusDest		= 3.0f;
	mMass			= mRadius * mRadius * 0.0001f + 0.01f;
	mScale			= 3.0f;
	mAge			= 0;
	mLifespan		= Rand::randInt( 50, 250 );
	mIsDead			= false;
}

void Particle::pullToCenter()
{
	Vec2f dirToCenter = mLoc - app::getWindowCenter();
	mVel -= dirToCenter * mMass * 0.025f;
}	

void Particle::applyPerlin( const Perlin &perlin )
{
	float nX = mLoc.x * 0.005f;
	float nY = mLoc.y * 0.005f;
	float nZ = app::getElapsedFrames() * 0.0025f;
	float noise = perlin.fBm( nX, nY, nZ );
	float angle = noise * 15.0f;
	Vec2f noiseVector( cos( angle ), sin( angle ) );
	mVel += noiseVector * mMass * 5.0f;
}

void Particle::update( const Channel32f &channel, const Vec2i &mouseLoc )
{	
	mVel += mAcc;

	float maxVel = mRadius + 0.0025f;
	
	float velLength = mVel.lengthSquared() + 0.1f;
	if( velLength > maxVel*maxVel ){
		mVel.normalize();
		mVel *= maxVel;
	}
	
	mLoc += mVel;
	mVel *= mDecay;
	mAcc.set( 0, 0 );
	
	if( mLoc.x >= 0.0f && mLoc.x <= (float)app::getWindowWidth() - 1 &&
		mLoc.y >= 0.0f && mLoc.y <= (float)app::getWindowHeight() - 1 ){
		
		mRadiusDest	= channel.getValue( mLoc ) * mScale + 0.5f;
	} else {
		mRadiusDest = 0.1f;
	}
	
	mRadius -= ( mRadius - mRadiusDest ) * 0.1f;
	mMass		= mRadius * mRadius * 0.0001f + 0.01f;
	
	//Uncomment these lines to enable particles to die over time
	//mAge++;
	//if( mAge > mLifespan ) mIsDead = true;
}

void Particle::draw()
{
	gl::drawSolidCircle( mLoc, mRadius );
}



