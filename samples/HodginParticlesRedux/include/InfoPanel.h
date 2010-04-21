
#pragma once

#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"

class InfoPanel {
 public:
	InfoPanel();
	void createTexture();
	void update( ci::Vec2f windowDim, float counter );
	void render( ci::Vec2f windowDim );
	void toggleState();
	
	ci::Vec2f			mLoc;
	
	float				mOpacity;
	float				mCounter;
	bool				mState;
	int					mFadeTime;
	ci::gl::Texture	mTexture;
};
