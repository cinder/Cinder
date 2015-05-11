/*
 * Code Copyright 2011 Robert Hodgin ( http://roberthodgin.com )
 * Used with permission for the Cinder Project ( http://libcinder.org )
 */

#pragma once

#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "cinder/Rect.h"
#include "cinder/Area.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/Font.h"
#include "cinder/Text.h"

class Swatch {
  public:
	Swatch( ci::Color color, ci::vec2 pos, ci::Rectf rect );
	
	void mouseOver( ci::Timeline &timeline );
	void mouseOff( ci::Timeline &timeline );
	void scatter( ci::Timeline &timeline, float width, float height );
	void assemble( ci::Timeline &timeline );
	void draw() const;
	void setSelected(){ mIsSelected = true; };
	void setDeselected(){ mIsSelected = false; };

	ci::vec2 mAnchorPos;
	ci::Anim<ci::vec2> mPos;
	ci::Anim<ci::Color> mColor;
	ci::Anim<float> mScale;
	ci::Anim<float> mAlpha;
	ci::Rectf mRect;
	
	bool mIsSelected;
};