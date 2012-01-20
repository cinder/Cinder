/*
 * Code Copyright 2011 Robert Hodgin ( http://roberthodgin.com )
 * Used with permission for the Cinder Project ( http://libcinder.org )
 */

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "Item.h"

using namespace ci;
using namespace std;

Font Item::sSmallFont;
Font Item::sBigFont;

Item::Item( int index, Vec2f pos, const string &title, const string &desc, Surface palette )
	: mIndex(index), mTitle( title ), mDesc( desc ), mPalette(palette)
{
	// TODO: can you reuse layouts? If so, how do you clear the text?
	// textures
	TextLayout layout;
	layout.clear( ColorA( 1, 1, 1, 0 ) );
	layout.setFont( sSmallFont );
	layout.setColor( Color::white() );
	layout.addLine( mTitle );
	mTitleTex = gl::Texture( layout.render( true ) );
	
	TextLayout bigLayout;
	bigLayout.clear( ColorA( 1, 1, 1, 0 ) );
	bigLayout.setFont( sBigFont );
	bigLayout.setColor( Color::white() );
	bigLayout.addLine( mTitle );
	mTitleBigTex = gl::Texture( bigLayout.render( true ) );
	
	// title
	mTitleStart		= pos;
	mTitleDest1		= Vec2f( pos.x - 25.0f, pos.y );
	mTitleFinish	= Vec2f( pos.x - 4.0f, 120.0f );
	mTitleDest2		= Vec2f( mTitleFinish.x - 25.0f, mTitleFinish.y );
	mMouseOverDest	= mTitleStart + Vec2f( 7.0f, 0.0f );
	mTitlePos		= mTitleStart;
	mTitleColor		= Color( 0.7f, 0.7f, 0.7f );
	mTitleAlpha		= 1.0f;
	mTitleWidth		= mTitleTex.getWidth();
	mTitleHeight	= mTitleTex.getHeight();
	
	// desc
	mDescStart		= Vec2f( mTitleStart.x + 25.0f, mTitleFinish.y + mTitleBigTex.getHeight() + 5.0f );
	mDescDest		= Vec2f( mTitleStart.x + 35.0f, mDescStart.y );
	mDescPos		= mDescStart;
	mDescAlpha		= 0.0f;
	TextBox tbox	= TextBox().alignment( TextBox::LEFT ).font( sSmallFont ).size( Vec2i( 650.0f, TextBox::GROW ) ).text( mDesc );
	mDescTex		= gl::Texture( tbox.render() );
	
	// bar
	mBarPos			= pos - Vec2f( 4.0f, 1.0f );
	mBarWidth		= 0.0f;
	mBarHeight		= mTitleHeight + 2.0f;
	mMaxBarWidth	= mTitleWidth + 22.0f;
	mBarColor		= Color::white();
	mBarAlpha		= 0.3f;
	
	mFadeFloat		= 1.0f;
	mIsSelected		= false;
	mIsBeingSelected = false;

	setColors();
}

void Item::setColors()
{
	Surface::Iter iter = mPalette.getIter();
	while( iter.line() ) {
		while( iter.pixel() ) {
			int index = iter.x();
			Color col( iter.r()/255.0f, iter.g()/255.0f, iter.b()/255.0f );
			Vec2f pos = Vec2f( index%4, floor(index/4.0f) ) * 5.0f - Vec2f( 12.0f, 8.0f );
			Rectf rect( -2.0f, -2.0f, 2.0f, 2.0f );
			Swatch swatch( col, mTitleStart + pos + Vec2f( -10.0f, 10.0f ), rect );
			mSwatches.push_back( swatch );
		}
	}
}

bool Item::isPointIn( const Vec2f &pt ) const
{
	return mTitleArea.contains( pt );
}

void Item::mouseOver( Timeline &timeline )
{
	if( !mIsBeingSelected ){
		float dur = 0.2f;
		
		// title
		timeline.apply( &mTitlePos, mMouseOverDest, dur, EaseOutAtan( 10 ) );
		timeline.apply( &mTitleColor, Color( 1, 1, 1 ), 0.05f, EaseOutAtan( 10 ) );
		
		// bar
		timeline.apply( &mBarColor, Color( 0, 0, 0 ), dur, EaseOutAtan( 10 ) );
		timeline.apply( &mBarAlpha, 0.4f, dur, EaseOutAtan( 10 ) );
		timeline.apply( &mBarWidth, mMaxBarWidth, dur, EaseOutAtan( 10 ) );

		for( std::vector<Swatch>::iterator swatchIt = mSwatches.begin(); swatchIt != mSwatches.end(); ++swatchIt ) {
			swatchIt->mouseOver( timeline );
		}
	}
}

void Item::mouseOff( Timeline &timeline )
{
	if( !mIsBeingSelected ){
		float dur = 0.4f;
		
		// title
		timeline.apply( &mTitlePos, mTitleStart, dur, EaseOutBounce( 1.0f ) );
		timeline.apply( &mTitleColor, Color( 0.7f, 0.7f, 0.7f ), 0.05f, EaseOutAtan( 10 ) );
		
		// bar
		timeline.apply( &mBarColor, Color( 0, 0, 0 ), 0.2f, EaseOutAtan( 10 ) );
		timeline.apply( &mBarAlpha, 0.0f, 0.2f, EaseOutAtan( 10 ) );
		timeline.apply( &mBarWidth, 0.0f, 0.2f, EaseInAtan( 10 ) );

		for( std::vector<Swatch>::iterator swatchIt = mSwatches.begin(); swatchIt != mSwatches.end(); ++swatchIt ) {
			swatchIt->mouseOff( timeline );
		}
	}
}

void Item::select( Timeline &timeline, float leftBorder )
{
	if( !mIsSelected ){
		mIsBeingSelected = true;
		
		mFadeFloat = 0.0f;
		
		float dur1 = 0.2f;
		float dur2 = 0.2f;
		
		// title position
		// set selected after initial animation
		timeline.apply( &mTitlePos, mTitleDest1, dur1, EaseInAtan( 10 ) ).finishFn( bind( &Item::setSelected, this ) );
		timeline.appendTo( &mTitlePos, mTitleDest2 - Vec2f( 25.0f, 0.0f ), 0.0001f, EaseNone() );
		timeline.appendTo( &mTitlePos, mTitleFinish, dur2, EaseOutAtan( 10 ) );
		// title color
		timeline.apply( &mTitleColor, Color( 1, 1, 1 ), dur1, EaseInQuad() );
		// title alpha
		timeline.apply( &mTitleAlpha, 0.0f, dur1, EaseInAtan( 10 ) );
		timeline.appendTo( &mTitleAlpha, 1.0f, dur2, EaseOutAtan( 10 ) );
		
		// desc position
		timeline.apply( &mDescPos, mDescDest, dur2, EaseOutAtan( 10 ) ).timelineEnd( -dur1 );
		// desc alpha
		timeline.apply( &mDescAlpha, 1.0f, dur2, EaseOutQuad() ).timelineEnd( -dur1*0.5f );
		
		// fadeFloat (used to adjust dropshadow offset)
		timeline.apply( &mFadeFloat, 0.0f, dur1, EaseInAtan( 10 ) );
		timeline.appendTo( &mFadeFloat, 0.0f, 0.25f, EaseNone() );
		timeline.appendTo( &mFadeFloat, 1.0f, dur2, EaseOutQuad() );
		
		// bar width
		timeline.apply( &mBarWidth, 0.0f, dur1, EaseInOutAtan( 10 ) );

		for( std::vector<Swatch>::iterator swatchIt = mSwatches.begin(); swatchIt != mSwatches.end(); ++swatchIt ) {
			swatchIt->scatter( timeline, math<float>::max( mTitleBigTex.getWidth(), 500.0f ), mTitleFinish.y + 50.0f );
		}
	}
}

void Item::deselect( Timeline &timeline )
{
	mIsBeingSelected = false;
	
	float dur1 = 0.2f;
	float dur2 = 0.2f;
	
	// set title position
	// set deselected after initial animation
	timeline.apply( &mTitlePos, mTitleDest2, dur1, EaseInAtan( 10 ) ).finishFn( bind( &Item::setDeselected, this ) );
	timeline.appendTo( &mTitlePos, mTitleDest1, 0.0001f, EaseNone() );
	timeline.appendTo( &mTitlePos, mTitleStart, dur2, EaseOutElastic( 2.0f, 0.5f ) );
	
	// set title color
	timeline.apply( &mTitleColor, Color( 1, 1, 1 ), dur1, EaseInAtan( 10 ) );
	timeline.appendTo( &mTitleColor, Color( 0.7f, 0.7f, 0.7f ), 0.05f, EaseOutAtan( 10 ) );
	
	// set title alpha
	timeline.apply( &mTitleAlpha, 0.0f, dur1, EaseInAtan( 10 ) );
	timeline.appendTo( &mTitleAlpha, 1.0f, dur2, EaseOutAtan( 10 ) );
	
	// set desc position
	timeline.apply( &mDescPos, mDescStart, dur1, EaseInAtan( 10 ) ).timelineEnd();
	
	// set desc alpha
	timeline.apply( &mDescAlpha, 0.0f, dur1, EaseInQuad() ).timelineEnd( -dur1 );
	
	// set bar
	timeline.apply( &mBarWidth, 0.0f, dur1, EaseInAtan( 10 ) );
	mBarAlpha = 0.3f;
	
	mIsSelected = false;
	
	for( std::vector<Swatch>::iterator swatchIt = mSwatches.begin(); swatchIt != mSwatches.end(); ++swatchIt ){
		swatchIt->assemble( timeline );
	}
}

void Item::update()
{
	mTitleArea	= Area( mTitlePos().x - 50.0f, mTitlePos().y - 2.0f, mTitlePos().x + mTitleWidth + 50.0f, mTitlePos().y + mTitleHeight + 2.0f );
	mBarRect	= Rectf( mBarPos.x, mBarPos.y, mBarPos.x + mBarWidth, mBarPos.y + mBarHeight );
}

void Item::drawSwatches() const
{	
	for( std::vector<Swatch>::const_iterator swatchIt = mSwatches.begin(); swatchIt != mSwatches.end(); ++swatchIt ){
		swatchIt->draw();
	}
}

void Item::drawBgBar() const
{
	gl::color( ColorA( mBarColor, mBarAlpha ) );
	gl::drawSolidRect( mBarRect );
}

void Item::drawText() const
{
	if( isBelowTextThreshold() ){
		// title shadow
		gl::color( ColorA( 0, 0, 0, mTitleAlpha() * 0.1f ) );
		gl::draw( mTitleBigTex, mTitlePos() + mFadeFloat() * Vec2f( -3.5f, 3.5f ) );
		gl::color( ColorA( 0, 0, 0, mTitleAlpha() * 0.2f ) );
		gl::draw( mTitleBigTex, mTitlePos() + mFadeFloat() * Vec2f( -2.5f, 2.5f ) );
		gl::color( ColorA( 0, 0, 0, mTitleAlpha() * 0.6f ) );
		gl::draw( mTitleBigTex, mTitlePos() + mFadeFloat() * Vec2f( -1.0f, 1.0f ) );
		
		// title text
		gl::color( ColorA( mTitleColor(), mTitleAlpha() ) );
		gl::draw( mTitleBigTex, mTitlePos() );
		
		// desc shadow
		gl::color( ColorA( 0, 0, 0, mDescAlpha() ) );
		gl::draw( mDescTex, mDescPos() + Vec2f( -1.0f, 1.0f ) );

		// desc text
		gl::color( ColorA( 1, 1, 1, mDescAlpha() ) );
		gl::draw( mDescTex, mDescPos() );
	} else {
		// drop shadow
		gl::color( ColorA( 0, 0, 0, mTitleAlpha() ) );
		gl::draw( mTitleTex, mTitlePos() + Vec2f( -1.0f, 1.0f ) );
		
		// white text
		gl::color( ColorA( mTitleColor(), mTitleAlpha() ) );
		gl::draw( mTitleTex, mTitlePos() );
	}
}

bool Item::isBelowTextThreshold() const
{
	if( mTitlePos().y <= mTitleFinish.y + 1.0f ){
		return true;
	}
	return false;
}

void Item::setFonts( const Font &smallFont, const Font &bigFont )
{
	sSmallFont	= smallFont;
	sBigFont	= bigFont;
}
