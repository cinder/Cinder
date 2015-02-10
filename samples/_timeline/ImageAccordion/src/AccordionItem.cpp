#include "AccordionItem.h"

#include "cinder/Color.h"
#include "cinder/Rand.h"
#include "cinder/Text.h"
#include "cinder/CinderMath.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Texture.h"

#include <string>
#include <cmath>

using namespace ci;
using namespace std;

float quickRound( float n )
{
	return floor(n+0.5f);
}

AccordionItem::AccordionItem( Timeline &timeline, float x, float y, float height, float contractedWidth, float expandedWidth, gl::TextureRef image, string title, string subtitle )
	: mTimeline(timeline), mX(x), mY(y), mWidth(contractedWidth), mHeight(height), mExpandedWidth(expandedWidth), mImage(image), mTitle(title), mSubtitle(subtitle)
{
#if defined( CINDER_COCOA )
	std::string normalFont( "Arial" );
	std::string boldFont( "Arial-BoldMT" );
#else
	std::string normalFont( "Arial" );
	std::string boldFont( "ArialBold" );
#endif
	
	mAnimEase = EaseOutAtan(25);
	mAnimDuration = 0.7f;
	
	mTextAlpha = 0.0f;
	
	TextLayout layout;
	layout.clear( ColorA( 0.6f, 0.6f, 0.6f, 0.0f ) );
	layout.setFont( Font( boldFont, 26 ) );
	layout.setColor( Color( 1, 1, 1 ) );
	layout.addLine( mTitle );
	layout.setFont( Font( normalFont, 16 ) );
	layout.addLine( mSubtitle );
	layout.setBorder(11, 6);
	mText = gl::Texture::create( layout.render( true ) );
	
	update();
}

bool AccordionItem::isPointIn( const vec2 &pt )
{
	return mImageArea.contains( pt );
}

void AccordionItem::animTo( float newX, float newWidth, bool revealText )
{
	mTimeline.apply( &mX, newX, mAnimDuration, mAnimEase );
	mTimeline.apply( &mWidth, newWidth, mAnimDuration, mAnimEase );
	
	if (revealText)
		mTimeline.apply( &mTextAlpha, 1.0f, mAnimDuration*0.3f, EaseNone() );
	else
		mTimeline.apply( &mTextAlpha, 0.0f, mAnimDuration*0.3f, EaseNone() );
}

void AccordionItem::update()
{
	// sample area of image texture to render
	mImageArea = Area(quickRound(mX), quickRound(mY), quickRound(mX + mWidth), quickRound(mY + mHeight));
	
	// rectangle to render text texture
	mTextRect = Rectf(quickRound(mX), quickRound(mY), quickRound( mX + math<float>::min( mWidth, mText->getWidth() ) ), quickRound( mY + math<float>::min( mHeight, mText->getHeight() ) ) );
	// sample area of text texture to render
	mTextArea = Area(0, 0, mTextRect.getWidth(), mTextRect.getHeight() );
}

void AccordionItem::draw()
{
	gl::color( Color(1,1,1) );
	gl::draw( mImage, mImageArea, Rectf(mImageArea) );
	gl::color( ColorA(1,1,1,mTextAlpha) );
	gl::draw( mText, mTextArea, mTextRect );
}
