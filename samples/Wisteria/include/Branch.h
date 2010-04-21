#pragma once

#include "cinder/cairo/Cairo.h"
#include "cinder/Color.h"
using namespace ci;

#include <vector>

class Branch {
 public:
	Branch( const Vec2f &aOrigin, float flowerHue, int launchDelay );
	Branch( const Vec2f &aOrigin, int aLifespan, float aSpeed, int aLaunchDelay, float aAngle, float aAngleDelta, float aChangeProb, float aFlowerProb,
		  float aStartEllipseRadius, float aEndEllipseRadius, ColorA aStartColor, ColorA aEndColor, ColorA aFlowerColor, float aScale );

	void update();
	void draw( cairo::Context ctx );
	
	bool isAlive();
	void createBranch( const Vec2f &aOrigin, float baseHue, ColorA flowerColor, int launchDelay );
	
	
	static void		setWindowSize( int aWindowWidth, int aWindowHeight ) { sWindowWidth = aWindowWidth; sWindowHeight = aWindowHeight; }
	static float	randomHue();

 private:
	static int sWindowWidth, sWindowHeight;
	static const int MAX_BRANCHES = 50;
	static const int INITIAL_SPAWN = 20;

	bool	mIsRoot;
	int		mLifespan, mTotalLifespan;
	Vec2f	mOrigin, mPos;
	float	mSpeed;
	int		mLaunchDelay;
	float	mAngle, mAngleDelta;
	float	mChangeProb, mFlowerProb;
	float	mStartEllipseRadius, mEndEllipseRadius;
	ColorA	mStartColor, mEndColor, mFlowerColor;
	float	mScale;

	std::vector<Branch> mBranches; 	
};