#pragma once
#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Perlin.h"
#include <vector>

class Particle {
 public:
	Particle();
	Particle( ci::Vec2f, ci::Vec2f );
	void update( const ci::Perlin &perlin, const ci::Channel32f &channel, const ci::Vec2i &mouseLoc );
	void draw();
	
	ci::Vec2f	mLoc;
	ci::Vec2f	mVel;
	ci::Vec2f	mDirToCursor;
	
	float		mDecay;
	float		mRadius;
	float		mScale;
	
	float		mMass;
	float		mInvMass;
	float		mCharge;
	
	int			mAge;
	int			mLifespan;
	float		mAgePer;
	bool		mIsDead;
};