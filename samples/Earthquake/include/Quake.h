#pragma once

#include "cinder/gl/Texture.h"
#include "cinder/Vector.h"
#include "cinder/Text.h"
#include <string>

class Quake {
 public:
	Quake();
	Quake( float aLat, float aLong, float aMag, std::string aTitle );
	void setLoc();
	
	float mLat;
	float mLong;
	float mMag;
	std::string mTitle;
	
	ci::Vec3f mLoc;
	ci::Vec3f mLocTip;
	ci::Vec3f mLocTipAnchor;
	ci::gl::Texture mLabel;
};