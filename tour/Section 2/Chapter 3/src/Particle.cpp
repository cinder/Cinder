#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

using namespace ci;

Particle::Particle()
{
}

Particle::Particle( vec3 pos, vec3 vel )
{
	mPos			= pos;
	mTailPos		= pos;
	mVel			= vel;
	mVelNormal		= vec3::yAxis();
	mAcc			= vec3::zero();
	
	mMaxSpeed		= Rand::randFloat( 2.5f, 3.0f );
	mMaxSpeedSqrd	= mMaxSpeed * mMaxSpeed;
	mMinSpeed		= Rand::randFloat( 1.0f, 1.5f );
	mMinSpeedSqrd	= mMinSpeed * mMinSpeed;
	
	mDecay			= 0.99f;
	mRadius			= 2.0f;
	mLength			= 10.0f;
}

void Particle::pullToCenter( const vec3 &center )
{
	vec3 dirToCenter = mPos - center;
	float distToCenter = dirToCenter.length();
	float maxDistance = 300.0f;
	
	if( distToCenter > maxDistance ){
		dirToCenter.normalize();
		float pullStrength = 0.0001f;
		mVel -= dirToCenter * ( ( distToCenter - maxDistance ) * pullStrength );
	}
}	

void Particle::update( bool flatten )
{	
	if( flatten ) mAcc.z = 0.0f;
	mVel += mAcc;
	mVelNormal = mVel.normalized();
	
	limitSpeed();
	
	mPos += mVel;
	mTailPos = mPos - mVelNormal * mLength;
	
	if( flatten ) mPos.z = 0.0f;
		
	mVel *= mDecay;
	mAcc = vec3::zero();
}

void Particle::limitSpeed()
{
	float vLengthSqrd = mVel.lengthSquared();
	if( vLengthSqrd > mMaxSpeedSqrd ){
		mVel = mVelNormal * mMaxSpeed;
		
	} else if( vLengthSqrd < mMinSpeedSqrd ){
		mVel = mVelNormal * mMinSpeed;
	}
}

void Particle::draw()
{
	gl::drawSphere( mPos, mRadius, 8 );
}

void Particle::drawTail()
{
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	gl::vertex( mPos );
	gl::color( ColorA( 1.0f, 0.0f, 0.0f, 1.0f ) );
	gl::vertex( mTailPos );
}
