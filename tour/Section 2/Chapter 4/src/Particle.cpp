#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;
using std::vector;

Particle::Particle()
{
}

Particle::Particle( vec3 pos, vec3 vel )
{
	mPos			= pos;
	mTailPos		= pos;
	mVel			= vel;
	mVelNormal		= vec3( 0, 1, 0 );
	mAcc			= vec3( 0 );
	
	mColor			= ColorA( 1.0f, 1.0f, 1.0f, 1.0f );
	
	mNeighborPos	= vec3( 0 );
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

void Particle::pullToCenter( const vec3 &center )
{
	vec3 dirToCenter = mPos - center;
	float distToCenter = glm::length( dirToCenter );
	float maxDistance = 300.0f;
	
	if( distToCenter > maxDistance ){
		dirToCenter = glm::normalize( dirToCenter );
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
	mVelNormal = glm::normalize( mVel );
	
	limitSpeed();
	
	mPos += mVel;
	if( flatten )
		mPos.z = 0.0f;
	
	mTailPos = mPos - ( mVelNormal * mLength );
	mVel *= mDecay;
	mAcc = vec3(0);
	
	float c = mNumNeighbors/50.0f;
	mColor = ColorA( CM_HSV, math<float>::max( 1.0f - c, 0.0f ), c, c + 0.5f, 1.0f );
	
	mNeighborPos = vec3( 0 );
	mNumNeighbors = 0;
}

void Particle::limitSpeed()
{
	float vLengthSqrd = glm::length2( mVel );
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

void Particle::drawTail( gl::VertBatch& batch )
{
	batch.color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	batch.vertex( mPos );
	batch.color( ColorA( 1.0f, 0.0f, 0.0f, 1.0f ) );
	batch.vertex( mTailPos );
}

void Particle::addNeighborPos( vec3 pos )
{
	mNeighborPos += pos;
	mNumNeighbors ++;
}


