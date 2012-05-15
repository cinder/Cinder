#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;
using std::vector;

Particle::Particle()
{
}

Particle::Particle( Vec3f pos, Vec3f vel )
{
	mPos			= pos;
	mTailPos		= pos;
	mVel			= vel;
	mVelNormal		= Vec3f::yAxis();
	mAcc			= Vec3f::zero();
	
	mColor			= ColorA( 1.0f, 1.0f, 1.0f, 1.0f );
	
	mNeighborPos	= Vec3f::zero();
	mNumNeighbors	= 0;
	mMaxSpeed		= Rand::randFloat( 2.5f, 3.0f );
	mMaxSpeedSqrd	= mMaxSpeed * mMaxSpeed;
	mMinSpeed		= Rand::randFloat( 1.0f, 1.5f );
	mMinSpeedSqrd	= mMinSpeed * mMinSpeed;
	
	mDecay			= 0.99f;
	mRadius			= 1.0f;
	mLength			= 10.0f;
	mCrowdFactor	= 1.0f;
}

void Particle::pullToCenter( const Vec3f &center )
{
	Vec3f dirToCenter = mPos - center;
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
	mCrowdFactor -= ( mCrowdFactor - ( 1.0f - mNumNeighbors * 0.01f ) ) * 0.1f;
	
	if( flatten )
		mAcc.z = 0.0f;
	
	
	mVel += mAcc;
	mVelNormal = mVel.normalized();
	
	limitSpeed();
	
	mPos += mVel;
	if( flatten )
		mPos.z = 0.0f;
	
	mTailPos = mPos - ( mVelNormal * mLength );
	mVel *= mDecay;
	mAcc = Vec3f::zero();
	
	float c = mNumNeighbors/50.0f;
	mColor = ColorA( CM_HSV, math<float>::max( 1.0f - c, 0.0f ), c, c + 0.5f, 1.0f );
	
	mNeighborPos = Vec3f::zero();
	mNumNeighbors = 0;
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
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	gl::drawSphere( mPos, mRadius, 8 );
}

void Particle::drawTail()
{
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	gl::vertex( mPos );
	gl::color( ColorA( 1.0f, 0.0f, 0.0f, 1.0f ) );
	gl::vertex( mTailPos );
}

void Particle::addNeighborPos( Vec3f pos )
{
	mNeighborPos += pos;
	mNumNeighbors ++;
}


