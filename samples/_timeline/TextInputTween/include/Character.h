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

using namespace ci;
using namespace std;

class Character {
  public:
	Character() {}
	Character( gl::TextureFontRef textureFont, string character,  Matrix44f matrix );
	
	void	animIn( Timeline &timeline,  Matrix44f matrix );
	void	animOut( Timeline &timeline,  Matrix44f matrix );
	
	void	draw() const;
	
	Rectf		getKernBounds() const;
	bool		isDead() const;
	Matrix44f	getDestMatrix() const;
	
  protected:
	void	onAnimOut();
	
	Anim<ColorAf>			mColorCur;
	Anim<Matrix44f>			mMatrix;
	
	ColorAf					mColorStart, mColorDest;
	
	Matrix44f				mDestMatrix;
	
	string					mChar;
	
	gl::TextureFontRef		mTextureFont;
	Rectf					mKernBounds;
	
	bool					mIsDead;
};
