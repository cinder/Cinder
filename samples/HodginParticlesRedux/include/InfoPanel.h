
#pragma once

#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"

class InfoPanel {
 public:
	InfoPanel();
	void createTexture();
	void update( ci::vec2 windowDim, float counter );
	void render( ci::vec2 windowDim );
	void toggleState();
	
	ci::vec2			mLoc;
	
	float				mOpacity;
	float				mCounter;
	bool				mState;
	int					mFadeTime;
	ci::gl::Texture	mTexture;
};
