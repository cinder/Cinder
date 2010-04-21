
#pragma once

#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"

class InfoPanel {
 public:
	InfoPanel();
	void createTexture();
	void update( ci::Vec2f aWindowDim, float aCounter );
	void render( ci::Vec2f aWindowDim );
	void toggleState();
	
	ci::Vec2f			mLoc;
	
	float				mOpacity;
	float				mCounter;
	bool				mState;
	ci::gl::Texture	mTexture;
};
