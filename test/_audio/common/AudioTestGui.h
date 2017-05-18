/*
 Copyright (c) 2014, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

// note: this minimal gui set is not meant to be reusable beyond the scope of cinder's audio tests, it is
// merely written in the most convenient way possible in order to provide cross-platform UI functionality.

#pragma once

#include "cinder/gl/gl.h"
#include "cinder/gl/TextureFont.h"
#include <boost/format.hpp>

#include <vector>
#include <cctype>

using namespace ci;

static gl::TextureFontRef getTestWidgetTexFont() {
	static gl::TextureFontRef sTestWidgetTexFont;
	if( ! sTestWidgetTexFont )
		sTestWidgetTexFont = gl::TextureFont::create( ci::Font( ci::Font::getDefault().getName(), 22 ) );
	return sTestWidgetTexFont;
}

struct TestWidget {
	TestWidget() : mHidden( false ), mPadding( 10.0f ) {}

	virtual void draw() {}

	Rectf mBounds;
	ColorA mBackgroundColor;

	gl::TextureFontRef mTexFont;
	bool mHidden;
	float mPadding;
};

inline void drawWidgets( const std::vector<TestWidget *> &widgets )
{
	for( auto w : widgets )
		w->draw();
}

struct Button : public TestWidget {
	Button( bool isToggle = false, const std::string& titleNormal = "", const std::string& titleEnabled = "" )
	: TestWidget(), mIsToggle( isToggle ), mTitleNormal( titleNormal ), mTitleEnabled( titleEnabled )
	{
		mTextColor = Color::white();
		mNormalColor = Color( "SlateGray" );
		mEnabledColor = Color( "RoyalBlue" );
		setEnabled( false );
		mTimeout = 30;
		mFadeFrames = 0;
	}

	void setEnabled( bool b )
	{
		if( b ) {
			mBackgroundColor = mEnabledColor;
		} else {
			mBackgroundColor = mNormalColor;
		}
		mEnabled = b;
	}

	bool hitTest( const ivec2 &pos )
	{
		if( mHidden )
			return false;

		bool b = mBounds.contains( pos );
		if( b ) {
			if( mIsToggle )
				setEnabled( ! mEnabled );
			else {
				setEnabled( true );
				mFadeFrames = mTimeout;
			}
		}

		return b;
	}

	void draw()
	{
		if( mHidden )
			return;
		if( ! mTexFont )
			mTexFont = getTestWidgetTexFont();

		if( mIsToggle || ! mFadeFrames )
			gl::color( mBackgroundColor );
		else {
			mFadeFrames--;
			setEnabled( false );
			gl::color( lerp( mNormalColor, mEnabledColor, (float)mFadeFrames / (float)mTimeout ) );
		}

		gl::drawSolidRect( mBounds );

		std::string& title = mEnabled ? mTitleEnabled : mTitleNormal;

		gl::color( mTextColor );
		mTexFont->drawString( title, vec2( mBounds.x1 + mPadding, mBounds.getCenter().y + mTexFont->getFont().getDescent() ) );
	}

	ColorA mTextColor;
	std::string mTitleNormal, mTitleEnabled;
	ColorA mNormalColor, mEnabledColor;
	bool mEnabled, mIsToggle;
	size_t mTimeout, mFadeFrames;
};

struct HSlider : public TestWidget {
	HSlider() : TestWidget()
	{
		mValue = mValueScaled = 0.0f;
		mMin = 0.0f;
		mMax = 1.0f;
		mBackgroundColor = ColorA( "DarkGreen", 0.75f );
		mValueColor = ColorA( "SpringGreen", 0.75f );
		mTextColor = Color::white();
	}

	void set( float val ) {
		mValueScaled = val;
		mValue = ( mValueScaled - mMin ) / ( mMax - mMin );
	}

	bool hitTest( const ivec2 &pos )
	{
		if( mHidden )
			return false;

		bool b = mBounds.contains( pos );
		if( b ) {
			mValue = ( pos.x - mBounds.x1 ) / mBounds.getWidth();
			mValueScaled = (mMax - mMin) * mValue + mMin;
		}
		return b;
	}

	void draw()
	{
		if( mHidden )
			return;
		if( ! mTexFont )
			mTexFont = getTestWidgetTexFont();

		gl::color( mBackgroundColor );
		gl::drawSolidRect( mBounds );

		auto valFormatted = boost::format( "%0.3f" ) % mValueScaled;

		std::string str = mTitle + ": " + valFormatted.str();
		gl::color( mTextColor );
		mTexFont->drawString( str, vec2( mBounds.x1 + mPadding, mBounds.getCenter().y + mTexFont->getFont().getDescent() ) );

		gl::color( mValueColor );
		gl::drawStrokedRect( mBounds );

		float offset = mBounds.x1 + mBounds.getWidth() * mValue;
		float w = 2.0f;
		Rectf valRect( offset - w, mBounds.y1, offset + w, mBounds.y2 );

		gl::drawSolidRect( valRect );
	}

	float mValue, mValueScaled, mMin, mMax;
	ColorA mTextColor, mValueColor;
	std::string mTitle;
};

struct VSelector : public TestWidget {
	VSelector() : TestWidget()
	{
		mCurrentSectionIndex = 0;
		mBackgroundColor = ColorA( "MidnightBlue", 0.75f );
		mSelectedColor = ColorA( "SpringGreen", 0.95f );
		mUnselectedColor = ColorA::gray( 0.5 );
		mTitleColor = ColorA::gray( 0.75f, 0.5f );
	}

	bool hitTest( const ivec2 &pos )
	{
		if( mHidden )
			return false;

		bool b = mBounds.contains( pos );
		if( b ) {
			int offset = pos.y - (int)mBounds.y1;
			int sectionHeight = (int)mBounds.getHeight() / mSegments.size();
			mCurrentSectionIndex = std::min<size_t>( offset / sectionHeight, mSegments.size() - 1 );
		}
		return b;
	}

	std::string currentSection() const
	{
		if( mSegments.empty() )
			return "";
		
		return mSegments[mCurrentSectionIndex];
	}

	void draw()
	{
		if( mHidden )
			return;

		if( ! mTexFont )
			mTexFont = getTestWidgetTexFont();

		gl::color( mBackgroundColor );
		gl::drawSolidRect( mBounds );

		float sectionHeight = mBounds.getHeight() / mSegments.size();
		Rectf section( mBounds.x1, mBounds.y1, mBounds.x2, mBounds.y1 + sectionHeight );
		gl::color( mUnselectedColor );
		for( size_t i = 0; i < mSegments.size(); i++ ) {
			if( i != mCurrentSectionIndex ) {
				gl::drawStrokedRect( section );
				gl::color( mUnselectedColor );
				mTexFont->drawString( mSegments[i], vec2( section.x1 + mPadding, section.getCenter().y + mTexFont->getFont().getDescent() ) );
			}
			section += vec2( 0.0f, sectionHeight );
		}

		gl::color( mSelectedColor );

		section.y1 = mBounds.y1 + mCurrentSectionIndex * sectionHeight;
		section.y2 = section.y1 + sectionHeight;
		gl::drawStrokedRect( section );

		if( ! mSegments.empty() ) {
			gl::color( mSelectedColor );
			mTexFont->drawString( mSegments[mCurrentSectionIndex], vec2( section.x1 + mPadding, section.getCenter().y + mTexFont->getFont().getDescent() ) );
		}

		if( ! mTitle.empty() ) {
			gl::color( mTitleColor );
			mTexFont->drawString( mTitle, vec2( mBounds.x1 + mPadding, mBounds.y1 - mTexFont->getFont().getDescent() ) );
		}
	}

	std::vector<std::string> mSegments;
	ColorA mSelectedColor, mUnselectedColor, mTitleColor;
	size_t mCurrentSectionIndex;
	std::string mTitle;
};

struct TextInput : public TestWidget {
	enum Format { NUMERICAL, ALL };

	TextInput( Format format = Format::NUMERICAL, const std::string& title = "" )
	: TestWidget(), mFormat( format ), mTitle( title )
	{
		mBackgroundColor = ColorA( "MidnightBlue", 0.65f );
		mTitleColor = ColorA::gray( 0.75f, 0.5f );
		mNormalColor = Color( "SlateGray" );
		mEnabledColor = ColorA( "SpringGreen", 0.95f );
		setSelected( false );

		sTextInputs.push_back( this );
	}

	void setSelected( bool b )
	{
		disableAll();
		mSelected = b;
	}

	bool hitTest( const ivec2 &pos )
	{
		if( mHidden )
			return false;

		bool b = mBounds.contains( pos );
		if( b ) {
			setSelected( true );
		}

		return b;
	}

	void setValue( int value )
	{
		mInputString = std::to_string( value );
	}

	int getValue()
	{
		if( mInputString.empty() )
			return 0;
		
		return stoi( mInputString );
	}

	void processChar( char c )
	{
		if( mFormat == Format::NUMERICAL && ! isdigit( c ) )
			return;

		mInputString.push_back( c );
	}

	void processBackspace()
	{
		if( ! mInputString.empty() )
			mInputString.pop_back();
	}

	static void disableAll()
	{
		for( TextInput *t : sTextInputs )
			t->mSelected = false;
	}

	static TextInput *getCurrentSelected()
	{
		for( TextInput *t : sTextInputs ) {
			if( t->mSelected )
				return t;
		}

		return nullptr;
	}

	void draw()
	{
		if( mHidden )
			return;
		if( ! mTexFont )
			mTexFont = getTestWidgetTexFont();

		gl::color( mBackgroundColor );
		gl::drawSolidRect( mBounds );

		vec2 titleOffset = vec2( mBounds.x1 + mPadding, mBounds.y1 - mTexFont->getFont().getDescent() );
		gl::color( mTitleColor );
		mTexFont->drawString( mTitle, titleOffset );

		vec2 textOffset = vec2( mBounds.x1 + mPadding, mBounds.getCenter().y + mTexFont->getFont().getDescent() );

		gl::color( ( mSelected ? mEnabledColor : mNormalColor ) );

		gl::drawStrokedRect( mBounds );
		mTexFont->drawString( mInputString, textOffset );
	}

	Format	mFormat;
	std::string mTitle, mInputString;
	ColorA mNormalColor, mEnabledColor, mTitleColor;
	bool mSelected;

	static std::vector<struct TextInput *> sTextInputs;
};

std::vector<struct TextInput *> TextInput::sTextInputs;

#define PRINT_GRAPH( context ) {														\
	ci::app::console() << "-------------- Graph configuration: --------------" << endl;	\
	ci::app::console() << context->printGraphToString();								\
	ci::app::console() << "--------------------------------------------------" << endl; \
}
