#include "Constraint.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using std::list;

Constraint::Constraint( Vec3f normal, float minValue, float maxValue )
{
	mNormal = normal;
	mMinValue = minValue;
	mMaxValue = maxValue;
}


void Constraint::apply( Emitter &emitter )
{
	if( mNormal.x > 0.0f ){
		if( emitter.mLoc.x < mMinValue ){
			emitter.mLoc.x = mMinValue;
			emitter.mIsTouchingConstraint = true;
		} else if( emitter.mLoc.x > mMaxValue ){
			emitter.mLoc.x = mMaxValue;
			emitter.mIsTouchingConstraint = true;
		}
	}
	
	if( mNormal.y > 0.0f ){
		if( emitter.mLoc.y < mMinValue ){
			emitter.mLoc.y = mMinValue;
			emitter.mIsTouchingConstraint = true;
		} else if( emitter.mLoc.y > mMaxValue - emitter.mRadius ){
			emitter.mLoc.y = mMaxValue - emitter.mRadius;
			emitter.mIsTouchingConstraint = true;
		}
	}
	
	if( mNormal.z > 0.0f ){
		if( emitter.mLoc.z < mMinValue ){
			emitter.mLoc.z = mMinValue;
			emitter.mIsTouchingConstraint = true;
		} else if( emitter.mLoc.z > mMaxValue ){
			emitter.mLoc.z = mMaxValue;
			emitter.mIsTouchingConstraint = true;
		}
	}
}


void Constraint::apply( std::list<Particle> &particles )
{
	for( list<Particle>::iterator particleIt = particles.begin(); particleIt != particles.end(); ++particleIt )
	{
		float velMulti = Rand::randFloat( -0.5f, -0.1f );
		if( mNormal.x > 0.0f ){
			if( particleIt->mLoc[0].x < mMinValue ){
				particleIt->mLoc[0].x = mMinValue;
				particleIt->mVel.x *= velMulti;
				particleIt->mIsBouncing = true;
			} else if( particleIt->mLoc[0].x > mMaxValue ){
				particleIt->mLoc[0].x = mMaxValue;
				particleIt->mVel.x *= velMulti;
				particleIt->mIsBouncing = true;
			}
		} 
		
		if( mNormal.y > 0.0f ){
			if( particleIt->mLoc[0].y < mMinValue ){
				particleIt->mLoc[0].y = mMinValue;
				particleIt->mVel.y *= velMulti;
				particleIt->mIsBouncing = true;
			} else if( particleIt->mLoc[0].y > mMaxValue ){
				particleIt->mLoc[0].y = mMaxValue;
				particleIt->mVel.y *= velMulti;
				particleIt->mIsBouncing = true;
			}
		}
		
		if( mNormal.z > 0.0f ){
			if( particleIt->mLoc[0].z < mMinValue ){
				particleIt->mLoc[0].z = mMinValue;
				particleIt->mVel.z *= velMulti;
				particleIt->mIsBouncing = true;
			} else if( particleIt->mLoc[0].z > mMaxValue ){
				particleIt->mLoc[0].z = mMaxValue;
				particleIt->mVel.z *= velMulti;
				particleIt->mIsBouncing = true;
			}
		}

	}
}


