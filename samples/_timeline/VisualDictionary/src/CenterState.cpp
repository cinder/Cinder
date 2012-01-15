/*
 * Code Copyright 2011 Robert Hodgin ( http://roberthodgin.com ) and Andrew Bell ( http://drawnline.net )
 * Used with permission for the Cinder Project ( http://libcinder.org )
 */

#include "cinder/app/AppBasic.h"
#include "CenterState.h"
#include "cinder/gl/gl.h"

#include <list>
#include <algorithm>

using namespace std;
using namespace ci;

gl::TextureFontRef CenterState::sFont;

CenterState::CenterState( float radius )
	: mRadius( radius )
{
	mTextPos() = app::getWindowCenter();
	mTextAlpha() = 0.0f;
}


void CenterState::addCircle(  const string &word, const Color &color, const Vec2f &dir )
{
	mWord				= word;
	mWordPixelLength	= sFont->measureString( mWord ).x;
	mCounter			= 0;
	
	// draw the bg circles and animate their color and radius
	for( list<Circle>::reverse_iterator circleIt = mCircles.rbegin(); circleIt != mCircles.rend(); ++circleIt ){
		circleIt->mRadiusDest += 10.0f;
		app::timeline().apply( &circleIt->mRadius, circleIt->mRadiusDest + 25.0f, 0.2f, EaseInOutQuad() ).timelineEnd( - 0.375f );
		app::timeline().appendTo( &circleIt->mRadius, circleIt->mRadiusDest, 0.2f, EaseInOutQuad() );
	}
	
	mCircles.push_back( Circle( 140.0f, color ) );
	app::timeline().apply( &mCircles.back().mRadius, mCircles.back().mRadiusDest, 0.2f, EaseInQuad() );
	app::timeline().apply( &mCircles.back().mRadius, mCircles.back().mRadiusDest, 0.2f, EaseInQuad() );
	
	mTextAlpha = 0.0f;
	app::timeline().apply( &mTextAlpha, 1.0f, 0.3f, EaseOutAtan( 10 ) );
	
	mTextPos() = app::getWindowCenter() + dir;
	app::timeline().apply( &mTextPos, app::getWindowCenter(), 0.3f, EaseOutAtan( 10 ) );
}

void CenterState::update( const WordNode &currentNode )
{
	int numCircles = mCircles.size();
	
	int index = 0;
	for( list<Circle>::reverse_iterator circleIt = mCircles.rbegin(); circleIt != mCircles.rend(); ++circleIt ){
		if( mCounter%( std::max( numCircles * 6, 24 ) ) == index * 4 ){
			app::timeline().apply( &circleIt->mRadius, circleIt->mRadiusDest + 30.0f, 0.25f, EaseOutQuad() );
			app::timeline().appendTo( &circleIt->mRadius, circleIt->mRadiusDest, 0.25f, EaseInOutQuad() );

			app::timeline().apply( &circleIt->mColor, circleIt->mColorDest * 2.0f, 0.25f, EaseOutQuad() );
			app::timeline().appendTo( &circleIt->mColor, circleIt->mColorDest, 0.25f, EaseInOutQuad() );
		}
		index ++;
	}

	
	mCounter ++;
}

void CenterState::draw()
{
	for( list<Circle>::iterator circleIt = mCircles.begin(); circleIt != mCircles.end(); ++circleIt ){
		circleIt->draw( app::getWindowCenter() );
	}
	
	float radius = 140.0f;
	
	// draw string
	// biggest square that can fit in the circle is radius * sqrt(2) per side  x^2 = (r^2)/2
	const float squareSide = sqrtf( ( radius * radius ) / 2.0f );
	
	float pixelScale = std::min( squareSide / mWordPixelLength, squareSide / 140 ) * 2.0f;
	gl::TextureFont::DrawOptions options = gl::TextureFont::DrawOptions().scale( pixelScale ).pixelSnap( false );
		
	const Vec2f offset = Vec2f( -radius + ( radius * 2 - mWordPixelLength * pixelScale ) / 2, radius - (radius * 2.0f - 60 * pixelScale ) / 2 );
		
	gl::color( ColorA( Color::black(), mTextAlpha * 0.5f ) );
	sFont->drawString( mWord, mTextPos() + offset + Vec2f( pixelScale, pixelScale ) * 1.5f, options );
		
	gl::color( ColorA( Color::white(), mTextAlpha ) );
	sFont->drawString( mWord, mTextPos() + offset, options );
}

void CenterState::setFont( gl::TextureFontRef font )
{
	sFont = font;
}