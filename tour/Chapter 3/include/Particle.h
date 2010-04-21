#pragma once
#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"

#include <vector>

class Particle {
 public:
	Particle();
	Particle( ci::Vec2f );
	void update( const ci::Channel32f &channel, const ci::Vec2i &mouseLoc );
	void draw();
	
	ci::Vec2f	mLoc;
	ci::Vec2f	mLocPer;
	ci::Vec2f	mDir;
	ci::Vec2f	mDirToCursor;
	float		mVel;
	
	float		mRadius;
	float		mScale;
	ci::Color	mColor;
};