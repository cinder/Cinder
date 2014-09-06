
#pragma once

#include "cinder/Vector.h"
#include "cinder/Color.h"

#include <vector>

class Particle {
 public:
	Particle( ci::vec3 aLoc, ci::vec3 aVel );
	void move();
	void update();
	void render( float heat );
	void renderLineStripTrail();
	void renderQuadStripTrail();
	void finish();
	
	int			mLen;
	float		mInvLen;
	std::vector<ci::vec3> mLoc;
	ci::vec3	mVel;
	ci::vec3	mAcc;

	float		mRadius;
	float		mMass;
	float		mInvMass;
	float		mCharge;
	
	float		mAge;
	float		mLifeSpan;
	float		mAgePer;
	
	bool		mIsBouncing;
	bool		mIsDying;
	bool		mIsDead;
};
