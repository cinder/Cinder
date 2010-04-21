#pragma once

#include "flint/app/App.h"
#include "flint/qtime/QuickTime.h"
#include "flint/gl/Texture.h"
#include "Module.h"

class QuickTimeApp : public Module {
 public:
	void	setup( fli::app::App *app );
	void	update( fli::app::App *app, bool visible );
	void	draw( fli::app::App *app, const fli::Vec4d &camLoc );
	
	fli::shared_ptr<fli::qtime::MovieGLTexture>	mMovie;
	const fli::gl::Texture		*mTexture;
};