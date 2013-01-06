#pragma once

#include "cinder/app/AppCocoaTouch.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"

class TestApp : public ci::app::AppCocoaTouch {
  public:
	virtual void	setup();
	void			touchesBegan( ci::app::TouchEvent event );
	virtual void	resize( int width, int height );
	virtual void	update();
	virtual void	draw();
	
	
	ci::Matrix44f	mCubeRotation;
	ci::gl::Texture mTex;
	ci::CameraPersp	mCam;
	ci::Font		mFont;
};
