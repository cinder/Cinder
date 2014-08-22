
#pragma once

#include "cinder/gl/Texture.h"

class InfoPanel {
 public:
	InfoPanel();
	void createTexture();
	void update( float counter );
	void render();
	void toggleState();
	
	ci::vec2				mLoc;
	
	float					mOpacity;
	float					mCounter;
	bool					mState;
	int						mFadeTime;
	ci::gl::Texture2dRef	mTexture;
};
