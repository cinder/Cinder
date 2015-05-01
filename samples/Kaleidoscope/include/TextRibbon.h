//
//	TextRibbon.h
//	Instascope
//
//	Created by Greg Kepler on 6/26/12.
//	Copyright (c) 2012 The Barbarian Group. All rights reserved.
//

#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timeline.h"
#include "cinder/Text.h"


class TextRibbon {	  
  public:
	TextRibbon();
	void	update( std::string tag, std::string user );
	void	draw();
	void	ribbonOut(float delay);
	
  private:
	void			makeText();
	void			drawTextShape();
	void			ribbonIn(float delay);
	
	std::string			mTag, mUser;
	ci::vec2			mTextPos, mRibbonSize;
	ci::Font			mUserFont, mTagFont;
	ci::ColorA			mCol, mTextCol;
	
	ci::gl::TextureRef	mUserTex, mTagTex;
	ci::TextBox			mTagBox, mUserBox;
	
	ci::Anim<ci::vec2> mCurPos;
	ci::Anim<float>		mCurAlpha;
};