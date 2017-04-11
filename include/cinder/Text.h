/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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

#pragma once

#include "cinder/Cinder.h"

#include "cinder/Surface.h"
#include "cinder/Font.h"
#include "cinder/Vector.h"

#include <vector>
#include <deque>
#include <string>
#include <map> 

// Core Text forward declarations
#if defined( CINDER_COCOA )
struct __CTFrame;
struct __CTLine;
#endif

namespace cinder {

class CI_API TextLayout {
 public:
	/*! \brief This is an abstract line
	 
	 Makes a TextLayout Object.
	 
	 */
	TextLayout();

	//! Sets the background color for the TextLayout. Implicit opqaue alpha.
	void	clear( const Color &color );
	//! Sets the background color and alpha for the TextLayout to unpremulitiplied color \a color
	void	clear( const ColorA &color );	
	
	//! Adds a left-justified line of text to the layout. Assumes UTF-8 encoding.
	void	addLine( const std::string &line );
	//! Adds a centered line of text to the layout. Assumes UTF-8 encoding.
	void	addCenteredLine( const std::string &line );
	//! Adds a right-justified line of text to the layout. Assumes UTF-8 encoding.
	void	addRightLine( const std::string &line );
	//! Appends string \a str to the current line. Assumes UTF-8 encoding.
	void	append( const std::string &str );

	void	setFont( const Font &font );
	//! Sets the currently active color. Implicit opqaue alpha.
	void	setColor( const Color &color );
	//! Sets the currently active color and alpha.
	void	setColor( const ColorA &color );
	//! Sets an offset relative to the default leading (the vertical space between lines).
	void	setLeadingOffset( float leadingOffset );

	//! Adds a \a horizontal pixel border to the left and the right sides, and a \a vertical border to the top and bottom
	void	setBorder( int horizontal, int vertical );

	//! Returns a Surface into which the TextLayout is rendered. If \a useAlpha the Surface will contain an alpha channel. If \a premultiplied the alpha will be premulitplied.
	Surface		render( bool useAlpha = false, bool premultiplied = false );
	
 private:
	ColorA	mBackgroundColor;
	Font	mCurrentFont;
	ColorA	mCurrentColor;
	float	mCurrentLeadingOffset;
	int		mHorizontalBorder, mVerticalBorder;
  
	std::deque<std::shared_ptr<class Line> >		mLines;
};

class CI_API TextBox {
  public:
	typedef enum Alignment { LEFT, CENTER, RIGHT } Alignment;
	enum { GROW = 0 };
	
	TextBox() : mAlign( LEFT ), mSize( GROW, GROW ), mFont( Font::getDefault() ), mInvalid( true ), mColor( 1, 1, 1, 1 ), mBackgroundColor( 0, 0, 0, 0 ), mPremultiplied( false ), mLigate( true ) {}

	TextBox&			size( ivec2 sz ) { setSize( sz ); return *this; }
	TextBox&			size( int width, int height ) { setSize( ivec2( width, height ) ); return *this; }
	ivec2				getSize() const { return mSize; }
	void				setSize( ivec2 sz ) { mSize = sz; mInvalid = true; }

	TextBox&			text( const std::string &t ) { setText( t ); return *this; }
	const std::string&	getText() const { return mText; }
	void				setText( const std::string &t ) { mText = t; mInvalid = true; }
	void				appendText( const std::string &t ) { mText += t; mInvalid = true; }

	TextBox&			font( const Font &f ) { setFont( f ); return *this; }
	const Font&			getFont() const { return mFont; }
	void				setFont( const Font &f ) { mFont = f; mInvalid = true; }

	TextBox&			alignment( Alignment align ) { setAlignment( align ); return *this; }
	Alignment			getAlignment() const { return mAlign; }
	void				setAlignment( Alignment align ) { mAlign = align; mInvalid = true; }

	TextBox&			color( ColorA color ) { setColor( color ); return *this; }
	ColorA				getColor() const { return mColor; }
	void				setColor( ColorA color ) { mColor = color; mInvalid = true; }

	TextBox&			backgroundColor( ColorA bgColor ) { setBackgroundColor( bgColor ); return *this; }
	ColorA				getBackgroundColor() const { return mBackgroundColor; }
	void				setBackgroundColor( ColorA bgColor ) { mBackgroundColor = bgColor; }

	TextBox&			premultiplied( bool premult = true ) { setPremultiplied( premult ); return *this; }
	bool				getPremultiplied() const { return mPremultiplied; }
	void				setPremultiplied( bool premult ) { mPremultiplied = premult; }

	TextBox&			ligate( bool ligateText = true ) { setLigate( ligateText ); return *this; }
	bool				getLigate() const { return mLigate; }
	void				setLigate( bool ligateText ) { mLigate = ligateText; }

	vec2				measure() const;
	/** Returns a vector of pairs of glyph indices and the position of their left baselines
		\warning Does not support word wrapping on Windows. **/
#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )	
	std::vector<std::pair<Font::Glyph,vec2>>	measureGlyphs( const std::map<Font::Glyph, Font::GlyphMetrics>* cachedGlyphMetrics = nullptr ) const;
#else
	std::vector<std::pair<Font::Glyph,vec2>>	measureGlyphs() const;
#endif

	Surface				render( vec2 offset = vec2() );

  protected:
	Alignment		mAlign;
	ivec2			mSize;
	std::string		mText;
	Font			mFont;
	ColorA			mColor, mBackgroundColor;
	bool			mPremultiplied;
	bool			mLigate;
	mutable bool	mInvalid;

	mutable vec2	mCalculatedSize;
#if defined( CINDER_COCOA )
	void			createLines() const;

	mutable std::vector<std::pair<std::shared_ptr<const __CTLine>,vec2> >	mLines;
#elif defined( CINDER_MSW_DESKTOP )
	std::vector<std::string>	calculateLineBreaks() const;
	void						calculate() const;

	mutable std::u16string	mWideText;
#elif defined( CINDER_UWP ) || defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
	std::vector<std::string>	calculateLineBreaks( const std::map<Font::Glyph, Font::GlyphMetrics>* cachedGlyphMetrics = nullptr ) const;
	void 						calculate() const;
#endif
};

/** \brief Renders a single string and returns it as a Surface.
	Optional \a baselineOffset pointer will receive how many pixels of descender are in the returned Surface. Offset rendering the result Surface by this amount in order to preserve a consistent baseline.
	Consider gl::drawString() as a more convenient alternative.
**/
#if defined( CINDER_COCOA_TOUCH )
Surface renderStringPow2( const std::string &str, const Font &font, const ColorA &color, ivec2 *actualSize, float *baselineOffset = 0 );
#else
Surface renderString( const std::string &str, const Font &font, const ColorA &color, float *baselineOffset = 0 );
#endif

} // namespace cinder
