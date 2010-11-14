#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include <vector>

class Particle {
 public:
	Particle();
	Particle( ci::Vec3f pos, ci::Vec3f vel );
	void pullToCenter( const ci::Vec3f &center );
	void update( bool flatten );
	void limitSpeed();
	void draw();
	void drawTail();
	
	ci::Vec3f	mPos;
	ci::Vec3f	mTailPos;
	ci::Vec3f	mVel;
	ci::Vec3f	mVelNormal;
	ci::Vec3f	mAcc;
	
	float		mDecay;
	float		mRadius;
	float		mLength;
	float		mMaxSpeed, mMaxSpeedSqrd;
	float		mMinSpeed, mMinSpeedSqrd;
};