
#pragma once

#include "cinder/Vector.h"
#include "cinder/Color.h"

#include <vector>

class Particle {
 public:
	Particle( ci::Vec3f aLoc, ci::Vec3f aVel );
	void move();
	void update();
	void render( float heat );
	void renderLineStripTrail();
	void renderQuadStripTrail();
	void finish();
	
	int			mLen;
	float		mInvLen;
	std::vector<ci::Vec3f> mLoc;
	ci::Vec3f	mVel;
	ci::Vec3f	mAcc;

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
