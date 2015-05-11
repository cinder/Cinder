#pragma once
#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Perlin.h"
#include <vector>

class Particle {
 public:
	Particle();
	Particle( ci::vec2, ci::vec2 );
	void pullToCenter();
	void applyPerlin( const ci::Perlin &perlin );
	void update( const ci::Channel32f &channel, const ci::ivec2 &mouseLoc );
	void draw();
	
	ci::vec2	mLoc;
	ci::vec2	mVel;
	ci::vec2	mAcc;
	
	float		mDecay;
	float		mRadius, mRadiusDest;
	float		mScale;
	
	float		mMass;

	int			mAge;
	int			mLifespan;
	bool		mIsDead;
};