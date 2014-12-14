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
	Character( ci::gl::TextureFontRef textureFont, std::string character,  ci::mat4 matrix );
	
	void	animIn( ci::Timeline &timeline,  ci::mat4 matrix );
	void	animOut( ci::Timeline &timeline,  ci::mat4 matrix );
	
	void	draw() const;
	
	ci::Rectf	getKernBounds() const;
	bool		isDead() const;
	ci::mat4	getDestMatrix() const;
	
  protected:
	void	onAnimOut();
	
	ci::Anim<ci::ColorAf>		mColorCur;
	ci::Anim<ci::mat4>			mMatrix;
	
	ci::ColorAf					mColorStart, mColorDest;
	ci::mat4					mDestMatrix;
	std::string					mChar;
	bool						mIsDead;

	ci::gl::TextureFontRef		mTextureFont;
	ci::Rectf					mKernBounds;
};
