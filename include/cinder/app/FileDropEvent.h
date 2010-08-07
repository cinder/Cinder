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
#include "cinder/app/Event.h"

#include <vector>
#include <string>

namespace cinder { namespace app {

//! Represents a file-drop event, typically received from Windows Explorer or Mac OS X Finder
class FileDropEvent : public Event {
  public:
	FileDropEvent( int aX, int aY, const std::vector<std::string> &aFiles )
		: Event(), mX( aX ), mY( aY ), mFiles( aFiles )
	{}
	
	//! Returns the X coordinate of the mouse during the event
	int			getX() const { return mX; }
	//! Returns the Y coordinate of the mouse during the event	
	int			getY() const { return mY; }
	//! Returns the coordinates of the mouse during the event
	Vec2i		getPos() const { return Vec2i( mX, mY ); }

	//! Returns the vector of file paths which were dropped
	const std::vector<std::string>&	getFiles() const { return mFiles; }
	//! Returns the number of files dropped during the event
	size_t							getNumFiles() const { return mFiles.size(); }
	//! Returns the path for file number \a index. 
	const std::string&				getFile( size_t index ) const { return mFiles.at(index); }

  private:
	int							mX, mY;
	std::vector<std::string>	mFiles;
};

} } // namespace cinder::app