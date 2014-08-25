
#pragma once

#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"

class InfoPanel {
 public:
	InfoPanel();
	void createTexture();
	void update( ci::vec2 aWindowDim, float aCounter );
	void render( ci::vec2 aWindowDim );
	void toggleState();
	
	ci::vec2			mLoc;
	
	float				mOpacity;
	float				mCounter;
	bool				mState;
	ci::gl::Texture	mTexture;
};
