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

#include <vector>
#include <deque>
#include <string>

namespace cinder {

#if ! defined( CINDER_COCOA_TOUCH )
class TextLayout {
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
  
	std::deque<shared_ptr<class Line> >		mLines;
};
#endif

/** \brief Renders a single string and returns it as a Surface.
	Optional \a baselineOffset pointer will receive how many pixels of descender are in the returned Surface. Offset rendering the result Surface by this amount in order to preserve a consistent baseline.
	Consider gl::drawString() as a more convenient alternative.
**/

#if defined( CINDER_COCOA_TOUCH )
Surface renderStringPow2( const std::string &str, const Font &font, const ColorA &color, Vec2i *actualSize, float *baselineOffset = 0 );
#else
Surface renderString( const std::string &str, const Font &font, const ColorA &color, float *baselineOffset = 0 );
#endif

} // namespace cinder
