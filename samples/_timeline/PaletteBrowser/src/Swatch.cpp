/*
 * Code Copyright 2011 Robert Hodgin ( http://roberthodgin.com )
 * Used with permission for the Cinder Project ( http://libcinder.org )
 */

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "Swatch.h"
#include <boost/foreach.hpp>

using namespace ci;
using namespace std;

Swatch::Swatch( Color color, Vec2f pos, Rectf rect )
  : mColor( color ), mPos( pos ), mAnchorPos( pos ), mRect( rect ), mScale( 1.0f )
{
	mAlpha = 1.0f;
	mIsSelected = false;
}

void Swatch::mouseOver( Timeline &timeline )
{
	timeline.apply( &mScale, 1.0f, 0.2f, EaseOutAtan( 10 ) );
}

void Swatch::mouseOff( Timeline &timeline )
{
	timeline.apply( &mScale, 1.0f, 0.2f, EaseOutAtan( 10 ) );
}

void Swatch::scatter( Timeline &timeline, float width, float height )
{
	Vec2f pos1( 0.0f, mPos().y );
	Vec2f pos2( 0.0f, height );
	Vec2f pos3 = Vec2f( Rand::randFloat( width ), Rand::randFloat( height - 50.0f, height + 50.0f ) );//pos2 + rVec;
	
	float dur = Rand::randFloat( 0.25f, 0.5f );
	
	// pos
	timeline.apply( &mPos, mPos(), 0.2f, EaseInAtan( 10 ) ).finishFn( bind( &Swatch::setSelected, this ) );
	timeline.appendTo( &mPos, pos2, 0.0001f, EaseNone() );
	timeline.appendTo( &mPos, pos3, dur, EaseOutAtan( 20 ) );
	
	// alpha
	timeline.apply( &mAlpha, 0.0f, 0.2f, EaseInAtan( 10 ) );
	timeline.appendTo( &mAlpha, 1.0f, dur, EaseOutAtan( 10 ) );
	
	// scale
	float s = (float)Rand::randInt( 3, 7 );
	timeline.apply( &mScale, 1.0f, 0.2f, EaseInAtan( 10 ) );
	timeline.appendTo( &mScale, s*s, dur, EaseOutAtan( 20 ) );
}

void Swatch::assemble( Timeline &timeline )
{
	float dur = 0.2f;
//	mPos = mAnchorPos + Vec2f( 15.0f, 0.0f );
	timeline.apply( &mPos, mAnchorPos, dur, EaseOutAtan( 10 ) ).finishFn( bind( &Swatch::setDeselected, this ) );
	timeline.apply( &mScale, 1.0f, dur, EaseOutAtan( 10 ) );
}

void Swatch::draw() const
{
	Rectf scaledRect = mRect * mScale;
	if( !mIsSelected ){
		gl::color( ColorA( Color::black(), mAlpha ) );
		gl::drawSolidRect( scaledRect.inflated( Vec2f( 1.0f, 1.0f ) ) + mPos );
	}
	gl::color( ColorA( mColor, mAlpha ) );
	gl::drawSolidRect( scaledRect + mPos );
}