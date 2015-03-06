#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;
using std::vector;

Particle::Particle()
{
}

Particle::Particle( vec3 pos, vec3 vel, bool followed )
{
	mPos			= pos;
	mTailPos		= pos;
	mVel			= vel;
	mVelNormal		= vec3( 0, 1, 0 );
	mAcc			= vec3( 0 );
	
	mNeighborPos	= vec3( 0 );
	mNumNeighbors	= 0;
	mMaxSpeed		= Rand::randFloat( 2.5f, 4.0f );
	mMaxSpeedSqrd	= mMaxSpeed * mMaxSpeed;
	mMinSpeed		= Rand::randFloat( 1.0f, 1.5f );
	mMinSpeedSqrd	= mMinSpeed * mMinSpeed;
	
	mColor			= ColorA( 1.0f, 1.0f, 1.0f, 1.0f );
		
	mDecay			= 0.99f;
	mRadius			= 1.0f;
	mLength			= 5.0f;
	mFear			= 1.0f;
	mCrowdFactor	= 1.0f;
	
	mIsDead			= false;
	mFollowed		= followed;
}

void Particle::pullToCenter( const vec3 &center )
{
	vec3 dirToCenter = mPos - center;
	float distToCenter = glm::length( dirToCenter );
	float distThresh = 200.0f;
	
	if( distToCenter > distThresh ){
		dirToCenter = glm::normalize( dirToCenter );
		float pullStrength = 0.00025f;
		mVel -= dirToCenter * ( ( distToCenter - distThresh ) * pullStrength );
	}
}


void Particle::update( bool flatten )
{	
	mCrowdFactor -= ( mCrowdFactor - ( 1.0f - mNumNeighbors * 0.02f ) ) * 0.1f;
	mCrowdFactor = constrain( mCrowdFactor, 0.5f, 1.0f );
	
	mFear -= ( mFear - 0.0f ) * 0.2f;
	
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
	
	float c = math<float>::min( mNumNeighbors/50.0f, 1.0f );
	mColor = ColorA( CM_HSV, 1.0f - c, c, c * 0.5f + 0.5f, 1.0f );
	
	mAcc = vec3( 0 );
	mNeighborPos = vec3( 0 );
	mNumNeighbors = 0;
}

void Particle::limitSpeed()
{
	float maxSpeed = mMaxSpeed + mCrowdFactor;
	float maxSpeedSqrd = maxSpeed * maxSpeed;
	
	float vLengthSqrd = glm::length2( mVel );
	if( vLengthSqrd > maxSpeedSqrd ){
		mVel = mVelNormal * maxSpeed;
		
	} else if( vLengthSqrd < mMinSpeedSqrd ){
		mVel = mVelNormal * mMinSpeed;
	}
	mVel *= (1.0 + mFear );
}

void Particle::draw()
{
	gl::ScopedColor color( mColor );
	gl::drawVector( mPos - mVelNormal * mLength, mPos - mVelNormal * mLength * 0.75f, mLength * 0.7f, mRadius );
}

void Particle::drawTail( gl::VertBatch& batch )
{
	batch.color( mColor );
	batch.vertex( mPos );
	batch.color( ColorA( mColor.r, mColor.g, mColor.b, 0.01f ) );
	batch.vertex( mTailPos );
}

void Particle::addNeighborPos( vec3 pos )
{
	mNeighborPos += pos;
	mNumNeighbors ++;
}


