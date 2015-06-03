/*
 * Code Copyright 2011 Robert Hodgin ( http://roberthodgin.com ) and Andrew Bell ( http://drawnline.net )
 * Used with permission for the Cinder Project ( http://libcinder.org )
 */

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Timeline.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/TextureFont.h"

#include <string>

#include "WordNode.h"

struct Circle {
public:
	Circle( float radius, ci::Color color )
	: mRadius( 0.0f ), mRadiusDest( radius ), mColor( color ), mColorDest( color )
	{}
	void draw( const ci::vec2 &pos ) {
		ci::gl::color( ci::ColorA( 0, 0, 0, 0.25f ) );
		float r = mRadius + 2.0f;
		
		ci::vec2 p = pos;
		p += ci::vec2( 0.5f, 0.5f );
		ci::gl::drawSolidRect( ci::Rectf( p.x - r, p.y - r, p.x + r, p.y + r ) );
		p += ci::vec2( 0.5f, 0.5f );
		ci::gl::drawSolidRect( ci::Rectf( p.x - r, p.y - r, p.x + r, p.y + r ) );
		
		// color foreground
		ci::gl::color( mColor );
		p	= pos;
		r	= mRadius;
		ci::gl::drawSolidRect( ci::Rectf( p.x - r, p.y - r, p.x + r, p.y + r ) );
	}
	
	ci::Anim<float>		mRadius;
	ci::Anim<ci::Color>	mColor;
	float				mRadiusDest;
	ci::Color			mColorDest;
};


class CenterState {
public:
	CenterState(){}
	CenterState( float radius );
	
	void	setWord( const std::string &word ){ mWord = word; };
	const std::string&	getWord() const { return mWord; }

	void	addCircle(  const std::string &word, const ci::Color &color, const ci::vec2 &dir );
	void	update( const WordNode &currentNode );
	void	draw();
	
	std::list<Circle>	mCircles;
	
	static	void	setFont( ci::gl::TextureFontRef font );
	
	int				mCounter;
	float			mRadius;
	ci::Anim<ci::vec2>	mTextPos;
	ci::Anim<float>		mTextAlpha;
protected:
	std::string		mWord;
	float			mWordPixelLength;
	
	static ci::gl::TextureFontRef	sFont;
};
