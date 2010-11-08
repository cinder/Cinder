#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include <vector>

class Particle {
 public:
	Particle();
	Particle( ci::Vec3f pos, ci::Vec3f vel, bool followed );
	void pullToCenter( const ci::Vec3f &center );
	void update( bool flatten );
	void draw();
	void drawTail();
	void limitSpeed();
	void addNeighborPos( ci::Vec3f pos );

	ci::Vec3f	mPos;
	ci::Vec3f	mTailPos;
	ci::Vec3f	mVel;
	ci::Vec3f	mVelNormal;
	ci::Vec3f	mAcc;
	
	ci::Vec3f	mNeighborPos;
	int			mNumNeighbors;

	ci::Color	mColor;
		
	float		mDecay;
	float		mRadius;
	float		mLength;
	float		mMaxSpeed, mMaxSpeedSqrd;
	float		mMinSpeed, mMinSpeedSqrd;
	float		mFear;
	float		mCrowdFactor;

	bool		mIsDead;
	bool		mFollowed;
};