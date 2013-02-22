#pragma once

#include "cinder/app/AppCocoaView.h"

class _TBOX_PREFIX_App : public cinder::app::AppCocoaView {
  public:
	void				setup();
	void				draw();

	float				mRadius;
	cinder::Colorf		mColor;
};
