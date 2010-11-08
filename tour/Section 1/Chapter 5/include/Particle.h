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
	void pullToCenter();
	void applyPerlin( const ci::Perlin &perlin );
	void update( const ci::Channel32f &channel, const ci::Vec2i &mouseLoc );
	void draw();
	
	ci::Vec2f	mLoc;
	ci::Vec2f	mVel;
	ci::Vec2f	mAcc;
	
	float		mDecay;
	float		mRadius, mRadiusDest;
	float		mScale;
	
	float		mMass;

	int			mAge;
	int			mLifespan;
	bool		mIsDead;
};