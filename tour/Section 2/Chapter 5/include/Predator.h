#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include <vector>

class Predator {
 public:
	Predator();
	Predator( ci::Vec3f pos, ci::Vec3f vel );
	void pullToCenter( const ci::Vec3f &center );
	void update( bool flatten );
	void limitSpeed();
	void draw();
	void drawTail();
	void addNeighborPos( ci::Vec3f pos );
	
	
	int			mLen;
	float		mInvLen;
	std::vector<ci::Vec3f> mPos;
	
	ci::Vec3f	mVel;
	ci::Vec3f	mVelNormal;
	ci::Vec3f	mAcc;
	
	ci::Color	mColor;
	
	ci::Vec3f	mNeighborPos;
	int			mNumNeighbors;
	
	float		mDecay;
	float		mRadius;
	float		mLength;
	float		mMaxSpeed, mMaxSpeedSqrd;
	float		mMinSpeed, mMinSpeedSqrd;
	float		mHunger;

	bool		mIsHungry;
	bool		mIsDead;
};