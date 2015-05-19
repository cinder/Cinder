#pragma once

#include "cinder/gl/Texture.h"
#include "cinder/Vector.h"
#include "cinder/Text.h"

class Quake {
public:
	Quake() {}
	Quake( float aLat, float aLong, float aMag, const std::string &aTitle );

public:
	float                 mLat;
	float                 mLong;
	float                 mMag;
	std::string           mTitle;

	ci::vec3              mLoc;
	ci::vec3              mLocTip;
	ci::vec3              mLocTipAnchor;
	ci::gl::Texture2dRef  mLabel;
};