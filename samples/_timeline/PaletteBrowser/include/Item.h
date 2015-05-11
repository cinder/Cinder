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
#include "Swatch.h"


class Item {
  public:
	Item( int index, ci::vec2 pos, const std::string &title, const std::string &desc, ci::Surface palette );
	void setColors();
	void mouseOver( ci::Timeline &timeline );
	void mouseOff( ci::Timeline &timeline );
	void select( ci::Timeline &timeline, float leftBorder );
	void deselect( ci::Timeline &timeline );
	void update();
	void drawText() const;
	void drawBgBar() const;
	void drawSwatches() const;
	
	void setSelected(){ mIsSelected = true; };
	void setDeselected(){ mIsSelected = false; };
	
	std::string getTitle() const { return mTitle; };
	bool getSelected() const { return mIsSelected; };
	bool isPointIn( const ci::vec2 &pt ) const;
	bool isBelowTextThreshold() const;
	
	int mIndex;

	static void	setFonts( const ci::Font &smallFont, const ci::Font &bigFont );
	
  private:
	// title
	std::string				mTitle;
	ci::gl::Texture2dRef		mTitleTex, mTitleBigTex;
	ci::Anim<ci::vec2>		mTitlePos;
	ci::vec2				mTitleStart, mTitleDest1, mTitleDest2, mTitleFinish;
	ci::vec2				mMouseOverDest;
	ci::Anim<ci::Color>		mTitleColor;
	ci::Anim<float>			mTitleAlpha;
	ci::Area				mTitleArea;
	float					mTitleWidth, mTitleHeight;
	
	// desc
	std::string				mDesc;
	ci::gl::Texture2dRef	mDescTex;
	ci::Anim<ci::vec2>		mDescPos;
	ci::vec2				mDescStart, mDescDest;
	ci::Anim<float>			mDescAlpha;
	
	// swatches
	ci::Surface			mPalette;	
	std::vector<Swatch> mSwatches;	
	
	// bar
	ci::vec2			mBarPos;
	ci::Anim<float>		mBarWidth, mBarHeight;
	float				mMaxBarWidth;
	ci::Rectf			mBarRect;
	ci::Anim<ci::Color> mBarColor;
	ci::Anim<float>		mBarAlpha;
	
	ci::Anim<float>		mFadeFloat;

	// TODO: isBeingSelected is a sloppy fix
	bool				mIsSelected, mIsBeingSelected;
	
	static ci::Font sSmallFont, sBigFont;	// small and large fonts for Text textures
};