#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"

class TestApp : public ci::app::AppCocoaTouch {
  public:
	virtual void	setup();
	void			touchesBegan( ci::app::TouchEvent event );
	virtual void	resize( int width, int height );
	virtual void	update();
	virtual void	draw();
	
	
	ci::mat4	mCubeRotation;
	ci::gl::Texture mTex;
	ci::CameraPersp	mCam;
	ci::Font		mFont;
};
