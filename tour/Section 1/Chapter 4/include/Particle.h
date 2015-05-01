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
	void update( const ci::Perlin &perlin, const ci::Channel32f &channel, const ci::ivec2 &mouseLoc );
	void draw();
	
	ci::vec2	mLoc;
	ci::vec2	mVel;
	
	float		mDecay;
	float		mRadius;
	float		mScale;
	
	float		mMass;
	
	int			mAge;
	int			mLifespan;
	float		mAgePer;
	bool		mIsDead;
};