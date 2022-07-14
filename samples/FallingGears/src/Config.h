
#pragma once

#include "cinder/Json.h"
#include "cinder/gl/Texture.h"

struct GearData {
	ci::gl::TextureRef	mImageTex;
};

class Config {
  public:

	// singleton access
	static Config* instance();

	void reload();
	const std::vector<GearData>&	getGearData() const	{ return mGears; }

	ci::ImageSourceRef	getBackgroundImage() const;
	float getDescentSpeed() const;

  private:
	Config();

	void loadGearData();

	ci::Json				mData;
	std::vector<GearData>	mGears;
};
