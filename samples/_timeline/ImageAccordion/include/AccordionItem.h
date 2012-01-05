
#pragma once

#include "cinder/Timeline.h"
#include "cinder/Easing.h"
#include "cinder/Color.h"
#include "cinder/Rect.h"
#include "cinder/Area.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"

#include <string>
#include <vector>

class AccordionItem {
  public:	
	AccordionItem( ci::Timeline &timeline, float x, float y, float height, float contractedWidth, float expandedHeight, ci::gl::Texture image, std::string title, std::string subtitle );
	void update();
	void draw();
	
	bool isPointIn( const ci::Vec2f &pt );
	void animTo( float newX, float newWidth, bool revealText = false );

  private:	
	ci::Anim<float>	mX, mWidth;
	float			mY, mHeight;
	float			mExpandedWidth;
	ci::Anim<float>	mTextAlpha;
	
	std::string		mTitle, mSubtitle;
	
	ci::gl::Texture		mImage, mText;
	ci::Rectf			mTextRect;
	ci::Area			mImageArea,  mTextArea;
	
	ci::Timeline	&mTimeline;
	float			mAnimDuration;
	ci::EaseFn		mAnimEase;
};
