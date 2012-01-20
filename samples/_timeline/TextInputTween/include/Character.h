/*
 * Code Copyright 2011 Chris McKenzie ( http://chrismckenzie.com )
 * Used with permission for the Cinder Project ( http://libcinder.org )
*/

#pragma once

#include "cinder/gl/TextureFont.h"
#include "cinder/Timeline.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Rect.h"


class Character {
  public:
	Character() {}
	Character( ci::gl::TextureFontRef textureFont, std::string character,  ci::Matrix44f matrix );
	
	void	animIn( ci::Timeline &timeline,  ci::Matrix44f matrix );
	void	animOut( ci::Timeline &timeline,  ci::Matrix44f matrix );
	
	void	draw() const;
	
	ci::Rectf		getKernBounds() const;
	bool		isDead() const;
	ci::Matrix44f	getDestMatrix() const;
	
  protected:
	void	onAnimOut();
	
	ci::Anim<ci::ColorAf>			mColorCur;
	ci::Anim<ci::Matrix44f>			mMatrix;
	
	ci::ColorAf					mColorStart, mColorDest;
	
	ci::Matrix44f				mDestMatrix;
	
	std::string					mChar;
	
	ci::gl::TextureFontRef		mTextureFont;
	ci::Rectf					mKernBounds;
	
	bool					mIsDead;
};
