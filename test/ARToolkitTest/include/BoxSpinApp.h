#pragma once

#include "flint/app/App.h"
#include "flint/qtime/QuickTime.h"
#include "flint/gl/Texture.h"
#include "flint/gl/GLSLProg.h"
#include "Module.h"

class BoxSpinApp : public Module {
 public:
	void	setup( fli::app::App *app );
	void	update( fli::app::App *app, bool visible );
	void	draw( fli::app::App *app, const fli::Vec4d &camLoc );
	
	float	mAngle, mAge;
	bool	mAlive;
	fli::gl::Texture		*mShadowTexture, *mLogoTexture;
	fli::gl::GLSLProg		*mBoxShader;
};