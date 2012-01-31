/*
 * Code Copyright 2011 Chris McKenzie ( http://chrismckenzie.com )
 * Used with permission for the Cinder Project ( http://libcinder.org )
*/

#include "cinder/Timeline.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Easing.h"
#include "cinder/Rand.h"

#include "Character.h"

using namespace ci;
using namespace std;

Character::Character( gl::TextureFontRef textureFont, string character, Matrix44f matrix )
{
	mTextureFont = textureFont;
	mChar = character;
	
	mColorStart = ColorAf( 1.0f, 0.5f, 0.0f, 0.0f );
	mColorCur	= mColorStart;
	
	float hue = Rand::randFloat( 0.55f, 0.6f );
	float sat = Rand::randFloat( 0.5f, 1.0f );
	mColorDest	= ColorAf( CM_HSV, hue, sat, 1.0f, 1.0f );
	mMatrix = mDestMatrix = matrix;
	
	mKernBounds = Rectf( 0.0f, 0.0f, mTextureFont->measureString( mChar ).x, mTextureFont->getAscent() );
	
	mIsDead = false;
}

void Character::animIn( Timeline &timeline, Matrix44f matrix )
{
	mDestMatrix = matrix;
	timeline.apply( &mColorCur, mColorDest, 1.0f, EaseOutAtan( 20 ) );
	timeline.apply( &mMatrix, matrix, 0.5f, EaseOutAtan( 10 ) );
}

void Character::animOut( Timeline &timeline, Matrix44f matrix )
{
	mDestMatrix = matrix;
	timeline.apply( &mColorCur, mColorStart, 1.0f, EaseOutQuad() ).finishFn( bind( &Character::onAnimOut, this ) );
	timeline.apply( &mMatrix, matrix, 1.0f, EaseOutQuad() );
}

void Character::onAnimOut()
{
	mIsDead = true;
}

Rectf Character::getKernBounds() const
{
	return mKernBounds;
}

bool Character::isDead() const
{
	return mIsDead;
}

Matrix44f Character::getDestMatrix() const
{
	return mDestMatrix;
}

void Character::draw() const
{
	gl::color( mColorCur );
	gl::pushMatrices();
		Matrix44f m = mMatrix;
		m.scale( Vec3f( 1.0f, -1.0f, 1.0 ) );
		gl::multModelView( m );
		mTextureFont->drawString( mChar, mKernBounds.getCenter() - Vec2f( mKernBounds.getWidth(), 0.0f ) );
	gl::popMatrices();
}
