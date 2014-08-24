#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include <vector>

class Predator {
 public:
	Predator();
	Predator( ci::vec3 pos, ci::vec3 vel );
	void pullToCenter( const ci::vec3 &center );
	void update( bool flatten );
	void limitSpeed();
	void draw();
	void drawTail();
	void addNeighborPos( ci::vec3 pos );
	
	
	int			mLen;
	float		mInvLen;
	std::vector<ci::vec3> mPos;
	
	ci::vec3	mVel;
	ci::vec3	mVelNormal;
	ci::vec3	mAcc;
	
	ci::Color	mColor;
	
	ci::vec3	mNeighborPos;
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