#pragma once

#include "Module.h"

#include "flint/app/App.h"
#include "flint/gl/Texture.h"
#include "flint/gl/GLSLProg.h"
#include "flint/TriMesh.h"

class CarModule : public Module {
 public:
	void	setup( fli::app::App *app );
	void	update( fli::app::App *app, bool visible );

	void	drawWheel( const fli::Vec3f &pos, float angle );
	void	draw( fli::app::App *app, const fli::Vec4d &camLoc );

	float	mAngle, mAge;
	bool	mAlive;
	fli::gl::Texture		*mRoadTexture;
	fli::gl::GLSLProg		*mBoxShader, *mRoadShader;
	fli::TriMesh			mBodyMesh, mWheelMesh, mRoadMesh;
	
	int		mRoadTravelOffset;
	float	mRoadLength;
	
	fli::Vec3f				mFrontRight, mFrontLeft, mBackRight, mBackLeft;
};