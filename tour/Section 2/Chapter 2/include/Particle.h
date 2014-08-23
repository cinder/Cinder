#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include <vector>

class Particle {
 public:
	Particle();
	Particle( ci::vec3 pos, ci::vec3 vel );
	void pullToCenter( const ci::vec3 &center );
	void update( bool flatten );
	void limitSpeed();
	void draw();
	void drawTail();
	
	ci::vec3	mPos;
	ci::vec3	mTailPos;
	ci::vec3	mVel;
	ci::vec3	mVelNormal;
	ci::vec3	mAcc;
	
	float		mDecay;
	float		mRadius;
	float		mLength;
	float		mMaxSpeed, mMaxSpeedSqrd;
	float		mMinSpeed, mMinSpeedSqrd;

	bool		mIsDead;
};