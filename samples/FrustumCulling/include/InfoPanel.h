
#pragma once

#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"

class InfoPanel {
 public:
	InfoPanel();
	void createTexture();
	void update( float counter );
	void render();
	void toggleState();
	
	ci::Vec2f	mLoc;
	
	float		mOpacity;
	float		mCounter;
	bool		mState;
	int			mFadeTime;
	ci::gl::Texture	mTexture;
};
