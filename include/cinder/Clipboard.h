/*
 Copyright (c) 2010, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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
#include "cinder/ImageIo.h"

#include <set>

namespace cinder {

class ScopedClipboard {
public:
	ScopedClipboard();
	~ScopedClipboard();

	//! Returns whether the clipboard contains a string
	bool hasString();
	//! Returns whether the clipboard contains an image
	bool hasImage();

	//! Returns the clipboard contents as a UTF-8 string or an empty string if the clipboard does not contain a string
	std::string    getString();
	//! Returns the clipboard contents as an ImageSourceRef or a null if the clipboard does not contain an image
	ImageSourceRef getImage();

	//! Sets the clipboard contents to the UTF-8 string \a str
	void setString( const std::string &str );
	//! Sets the clipboard contents to the ImageSource \a image
	void setImage( ImageSourceRef image, ImageTarget::Options options = ImageTarget::Options() );

#if defined( CINDER_MSW )
	bool clipboardContainsFormat( const std::set<unsigned int> &formats );
#endif
};

class Clipboard {
public:
	//! Returns whether the clipboard contains a string
	static bool hasString() { ScopedClipboard cb; return cb.hasString(); }
	//! Returns whether the clipboard contains an image
	static bool hasImage() { ScopedClipboard cb; return cb.hasImage(); }

	//! Returns the clipboard contents as a UTF-8 string or an empty string if the clipboard does not contain a string
	static std::string    getString() { ScopedClipboard cb; return cb.getString(); }
	//! Returns the clipboard contents as an ImageSourceRef or a null if the clipboard does not contain an image
	static ImageSourceRef getImage() { ScopedClipboard cb; return cb.getImage(); }

	//! Sets the clipboard contents to the UTF-8 string \a str
	static void setString( const std::string &str ) { ScopedClipboard cb; cb.setString( str ); }
	//! Sets the clipboard contents to the ImageSource \a image
	static void setImage( ImageSourceRef image, ImageTarget::Options options = ImageTarget::Options() ) { ScopedClipboard cb; cb.setImage( image, options ); }
};

} // namespace cinder