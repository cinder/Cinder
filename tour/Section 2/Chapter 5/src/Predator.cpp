#include "Predator.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;

Predator::Predator()
{
}

Predator::Predator( Vec3f pos, Vec3f vel )
{
	mLen			= 5;
	mInvLen			= 1.0f/(float)mLen;

	for( int i=0; i<mLen; ++i ) {
		mPos.push_back( pos );
	}
	
	mVel			= vel;
	mVelNormal		= Vec3f::yAxis();
	mAcc			= Vec3f::zero();
	
	mNeighborPos	= Vec3f::zero();
	mNumNeighbors	= 0;
	mMaxSpeed		= Rand::randFloat( 4.0f, 4.5f );
	mMaxSpeedSqrd	= mMaxSpeed * mMaxSpeed;
	mMinSpeed		= Rand::randFloat( 1.0f, 1.5f );
	mMinSpeedSqrd	= mMinSpeed * mMinSpeed;

	mColor			= ColorA( 1.0f, 0.0f, 0.0f, 1.0f );
	
	mDecay			= 0.99f;
	mRadius			= 2.0f;
	mLength			= 25.0f;
	mHunger			= 1.0f;
	
	mIsDead			= false;
	mIsHungry		= true;
}

void Predator::pullToCenter( const Vec3f &center )
{
	Vec3f dirToCenter	= mPos[0] - center;
	float distToCenter	= dirToCenter.length();
	float maxDistance	= 600.0f;
	
	if( distToCenter > maxDistance ){
		float pullStrength = 0.0001f;
		mVel -= dirToCenter.normalized() * ( ( distToCenter - maxDistance ) * pullStrength );
	}
}	

void Predator::update( bool flatten )
{	
	mVel += mAcc;
	
	if( flatten ) mAcc.z = 0.0f;
	mVel += mAcc;
	mVelNormal = mVel.safeNormalized();
	
	limitSpeed();
	
	
	for( int i=mLen-1; i>0; i-- ){
		mPos[i] = mPos[i-1];
	}
	mPos[0] += mVel;
	
	if( flatten )
		mPos[0].z = 0.0f;
	
	mVel *= mDecay;
	
	mAcc = Vec3f::zero();
	mNeighborPos = Vec3f::zero();
	mNumNeighbors = 0;
	
	mHunger += 0.001f;
	mHunger = constrain( mHunger, 0.0f, 1.0f );
	
	if( mHunger > 0.5f ) mIsHungry = true;
}

void Predator::limitSpeed()
{
	float maxSpeed = mMaxSpeed + mHunger * 3.0f;
	float maxSpeedSqrd = maxSpeed * maxSpeed;
	float vLengthSqrd = mVel.lengthSquared();
	if( vLengthSqrd > maxSpeedSqrd ){
		mVel = mVelNormal * maxSpeed;
		
	} else if( vLengthSqrd < mMinSpeedSqrd ){
		mVel = mVelNormal * mMinSpeed;
	}
}

void Predator::draw()
{
	glColor4f( mColor );
	Vec3f vel = mVelNormal * mLength;
	gl::drawVector( mPos[0] - mVel, mPos[0], mLength * 0.85f, 3.0f + mHunger );
}

void Predator::drawTail()
{
	gl::vertex( mPos[0] );
	gl::vertex( mPos[1] );
}

void Predator::addNeighborPos( Vec3f pos )
{
	mNeighborPos += pos;
	mNumNeighbors ++;
}

