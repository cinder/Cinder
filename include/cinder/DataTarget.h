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
#include "cinder/Url.h"
#include "cinder/Stream.h"

#include <string>

namespace cinder {

typedef std::shared_ptr<class DataTarget>	DataTargetRef;

class CI_API DataTarget { 
  public:
	virtual bool	providesFilePath() = 0;
	virtual bool	providesUrl() = 0;
	
	const fs::path&			getFilePath() const;
	const Url&				getUrl() const;
	const fs::path&			getFilePathHint() const;

	virtual OStreamRef		getStream() = 0;

  protected:
	DataTarget( const fs::path &aFilePath, const Url &aUrl )
		: mFilePath( aFilePath ), mUrl( aUrl ) 
	{}
	virtual ~DataTarget() {}
	
	void	setFilePathHint( const fs::path &aFilePathHint );
	
//	Buffer				mBuffer; // TODO: is this used? if not remove
	fs::path			mFilePath;
	fs::path			mFilePathHint;
	Url					mUrl;
};


typedef std::shared_ptr<class DataTargetPath>	DataTargetPathRef;

class CI_API DataTargetPath : public DataTarget {
  public:
	static DataTargetPathRef	createRef( const fs::path &path );
	
	virtual bool	providesFilePath() { return true; }
	virtual bool	providesUrl() { return false; }
	
	virtual OStreamRef		getStream();
	
  protected:
	explicit DataTargetPath( const fs::path &path );
  
	OStreamFileRef	mStream;  
};


typedef std::shared_ptr<class DataTargetStream>	DataTargetStreamRef;

class CI_API DataTargetStream : public DataTarget {
  public:
	static DataTargetStreamRef	createRef( OStreamRef stream );

	virtual bool	providesFilePath() { return false; }
	virtual bool	providesUrl() { return false; }
	
	virtual OStreamRef		getStream() { return mStream; }
	
  protected:
	DataTargetStream( OStreamRef stream );
  
	OStreamRef	mStream;  	
};

//! Returns a DataTarget to file path \a path, and optionally creates any necessary directories when \a createParents is \c true.
CI_API DataTargetPathRef writeFile( const fs::path &path, bool createParents = true );

} // namespace cinder