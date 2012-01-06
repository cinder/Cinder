/*
 * Code Copyright 2011 Robert Hodgin ( http://roberthodgin.com ) and Andrew Bell ( http://drawnline.net )
 * Used with permission for the Cinder Project ( http://libcinder.org )
 */

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Timeline.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/gl/TextureFont.h"

#include <string>

class WordNode {
  public:
	WordNode() {}
	WordNode( const std::string &word );

	const std::string&	getWord() const { return mWord; }
	
	bool	shouldBeDeleted() const;
	void	setShouldBeDeleted();
	
	bool	isSelected() const;
	void	setSelected();

	bool	isPointInside( const ci::Vec2f &pt ) const;
	
	void	draw() const;

	// Timelined properties		
	ci::Anim<ci::Vec2f>		mPos;
	ci::Anim<float>			mRadius;
	float					mRadiusDest;
	ci::Anim<ci::ColorA>	mColor;


	static	void	setFont( ci::gl::TextureFontRef font );
  protected:
	bool			mIsSelected;
	bool			mShouldBeDeleted;
	std::string		mWord;
	float			mWordPixelLength;

	static ci::gl::TextureFontRef	sFont;
};
