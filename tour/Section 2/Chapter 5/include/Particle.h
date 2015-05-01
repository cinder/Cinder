#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include <vector>

class Particle {
 public:
	Particle();
	Particle( ci::vec3 pos, ci::vec3 vel, bool followed );
	void pullToCenter( const ci::vec3 &center );
	void update( bool flatten );
	void draw();
	void drawTail();
	void limitSpeed();
	void addNeighborPos( ci::vec3 pos );

	ci::vec3	mPos;
	ci::vec3	mTailPos;
	ci::vec3	mVel;
	ci::vec3	mVelNormal;
	ci::vec3	mAcc;
	
	ci::vec3	mNeighborPos;
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