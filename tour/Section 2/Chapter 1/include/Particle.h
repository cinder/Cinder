#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include <vector>

class Particle {
 public:
	Particle();
	Particle( ci::Vec3f, ci::Vec3f );
	void pullToCenter( const ci::Vec3f &center );
	void update();
	void draw();
	
	ci::Vec3f	mPos;
	ci::Vec3f	mVel;
	ci::Vec3f	mAcc;
	
	float		mDecay;
	float		mRadius;
};