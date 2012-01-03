/*
 * Code Copyright 2011 Chris McKenzie ( http://chrismckenzie.com )
 * Used with permission for the Cinder Project ( http://libcinder.org )
*/

#include "cinder/Timeline.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Easing.h"

#include "Character.h"

Character::Character( gl::TextureFontRef textureFont, string character,  Matrix44f matrix )
{
	mTextureFont = textureFont;
	mChar = character;
	
	mColor = ColorAf( 1.0f, 1.0f, 1.0f, 0.0f );
	mMatrix = mDestMatrix = matrix;
	
	mKernBounds = Rectf( 0.0f, 0.0f, mTextureFont->measureString( mChar ).x, mTextureFont->getAscent() );
	
	mIsDead = false;
}

void Character::animIn( Timeline &timeline, Matrix44f matrix )
{
	mDestMatrix = matrix;
	timeline.apply( &mColor, ColorAf( 1.0f, 1.0f, 1.0f, 0.5f ), 1.0f, EaseOutAtan( 10 ) );
	timeline.apply( &mMatrix, matrix, 1.0f, EaseOutAtan( 10 ) );
}

void Character::animOut( Timeline &timeline, Matrix44f matrix )
{
	mDestMatrix = matrix;
	timeline.apply( &mColor, ColorAf( 1.0f, 1.0f, 1.0f, 0.0f ), 1.0f, EaseOutQuad() ).finishFn( bind( &Character::onAnimOut, this ) );
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
	gl::color( mColor );
	gl::pushMatrices();
		Matrix44f m = mMatrix;
		m.scale( Vec3f( 1.0f, -1.0f, 1.0 ) );
		gl::multModelView( m );
		mTextureFont->drawString( mChar, mKernBounds.getCenter() - Vec2f( mKernBounds.getWidth(), 0.0f ) );
	gl::popMatrices();
}
