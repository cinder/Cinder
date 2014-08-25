#pragma once
#include "cinder/Channel.h"
#include "cinder/Vector.h"

#include <vector>

class Particle {
 public:
	Particle();
	Particle( ci::vec2 );
	void update();
	void draw();
	
	ci::vec2	mLoc;
	ci::vec2	mDir;
	float		mVel;
	
	float		mRadius;
};