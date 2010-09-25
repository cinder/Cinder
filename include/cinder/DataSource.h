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
#include "cinder/Buffer.h"
#include "cinder/Stream.h"

namespace cinder {

typedef shared_ptr<class DataSource>	DataSourceRef;

class DataSource { 
  public:
	virtual bool	isFilePath() = 0;
	virtual bool	isUrl() = 0;
	
	const std::string&	getFilePath();
	const Url&			getUrl();
	const std::string&	getFilePathHint();

	Buffer&				getBuffer();
	virtual IStreamRef	createStream() = 0;

  protected:
	DataSource( const std::string &aFilePath, const Url &aUrl )
		: mFilePath( aFilePath ), mUrl( aUrl ) 
	{}
	virtual ~DataSource() {}
	
	virtual	void	createBuffer() = 0;
	
	void	setFilePathHint( const std::string &aFilePathHint );
	
	Buffer				mBuffer;
	std::string			mFilePath, mFilePathHint;
	Url					mUrl;
};


typedef shared_ptr<class DataSourcePath>	DataSourcePathRef;

class DataSourcePath : public DataSource {
  public:
	static DataSourcePathRef	createRef( const std::string &path );

	virtual bool	isFilePath() { return true; }
	virtual bool	isUrl() { return false; }

	virtual IStreamRef	createStream();

  protected:
	DataSourcePath( const std::string &path );
	
	virtual	void	createBuffer();
	
	IStreamFileRef	mStream;	
};

DataSourcePathRef	loadFile( const std::string &path );

typedef shared_ptr<class DataSourceUrl>	DataSourceUrlRef;

class DataSourceUrl : public DataSource {
  public:
	static DataSourceUrlRef	createRef( const Url &Url );

	virtual bool	isFilePath() { return false; }
	virtual bool	isUrl() { return true; }

	virtual IStreamRef	createStream();

  protected:
	DataSourceUrl( const Url &Url );
	
	virtual	void	createBuffer();

	IStreamUrlRef	mStream;
};

DataSourceUrlRef		loadUrl( const Url &Url );
inline DataSourceUrlRef	loadUrl( const std::string &urlString ) { return loadUrl( Url( urlString ) ); }

typedef shared_ptr<class DataSourceBuffer>	DataSourceBufferRef;

class DataSourceBuffer : public DataSource {
  public:
	static DataSourceBufferRef		createRef( Buffer buffer, const std::string &filePathHint = "" );

	virtual bool	isFilePath() { return false; }
	virtual bool	isUrl() { return false; }

	virtual IStreamRef	createStream();
	
  protected:
	DataSourceBuffer( Buffer aBuffer );
	
	virtual	void	createBuffer();
	
	IStreamMemRef	mStream;
};

} // namespace cinder
