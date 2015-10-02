/*
 * Code Copyright 2011 Robert Hodgin ( http://roberthodgin.com ) and Andrew Bell ( http://drawnline.net )
 * Used with permission for the Cinder Project ( http://libcinder.org )
 */

#include "WordNode.h"
#include "cinder/gl/gl.h"

#include <iostream>

using namespace std;
using namespace ci;

gl::TextureFontRef	WordNode::sFont;

WordNode::WordNode( const string &word )
	: mWord( word ), mShouldBeDeleted( false ), mIsSelected( false )
{
	mWordPixelLength	= sFont->measureString( mWord ).x;
}

bool WordNode::isPointInside( const vec2 &pt ) const
{
	return glm::distance( pt, mPos() ) < mRadius;
}

void WordNode::setShouldBeDeleted()
{
	mShouldBeDeleted = true;
}

bool WordNode::shouldBeDeleted() const
{
	return mShouldBeDeleted;
}

void WordNode::setSelected()
{
	mIsSelected = true;
}

bool WordNode::isSelected() const
{
	return mIsSelected;
}

void WordNode::draw() const
{
	// draw shadows
	gl::color( ColorA( 0, 0, 0, mColor().a * 0.5f ) );
	vec2 p	= mPos();
	p += vec2( 1.0f, 1.0f );
	gl::drawSolidRect( Rectf( p.x - mRadius, p.y - mRadius, p.x + mRadius, p.y + mRadius ) );
	p += vec2( 1.0f, 1.0f );
	gl::drawSolidRect( Rectf( p.x - mRadius, p.y - mRadius, p.x + mRadius, p.y + mRadius ) );

	
	// draw color circle
	gl::color( mColor );
	p = mPos();
	gl::drawSolidRect( Rectf( p.x - mRadius, p.y - mRadius, p.x + mRadius, p.y + mRadius ) );

	
	// draw string
	// biggest square that can fit in the circle is radius * sqrt(2) per side  x^2 = (r^2)/2
	const float squareSide = sqrtf( ( mRadius * mRadius ) / 2.0f );
	float pixelScale = std::min( squareSide / mWordPixelLength, squareSide / 72 ) * 2.25f;
	gl::TextureFont::DrawOptions options = gl::TextureFont::DrawOptions().scale( pixelScale ).pixelSnap( false );

	const vec2 offset = (vec2)mPos + vec2( -mRadius + ( mRadius * 2 - mWordPixelLength * pixelScale ) / 2, mRadius - (mRadius * 2.0f - 20 * pixelScale ) / 2 );

	gl::color( ColorA( Color::black(), mColor().a * 0.5f ) );
	sFont->drawString( mWord, offset + vec2( pixelScale, pixelScale ) * 1.5f, options );

	gl::color( ColorA( ( mColor() + Color::white() ) * 0.65f, mColor().a ) );
	sFont->drawString( mWord, offset, options );
}

void WordNode::setFont( gl::TextureFontRef font )
{
	sFont = font;
}
