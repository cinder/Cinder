/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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
#include "cinder/Filesystem.h"

namespace cinder {

typedef std::shared_ptr<class DataSource>	DataSourceRef;

class CI_API DataSource { 
  public:
	virtual bool	isFilePath() = 0;
	virtual bool	isUrl() = 0;
	
	const fs::path&		getFilePath();
	const Url&			getUrl();
	const fs::path&		getFilePathHint();

	BufferRef			getBuffer();
	virtual IStreamRef	createStream() = 0;

  protected:
	DataSource( const fs::path &aFilePath, const Url &aUrl )
		: mFilePath( aFilePath ), mUrl( aUrl ) 
	{}
	virtual ~DataSource() {}
	
	virtual	void	createBuffer() = 0;
	
	void	setFilePathHint( const fs::path &aFilePathHint );
	
	BufferRef			mBuffer;
	fs::path			mFilePath;
	fs::path			mFilePathHint;
	Url					mUrl;
};


typedef std::shared_ptr<class DataSourcePath>	DataSourcePathRef;

class CI_API DataSourcePath : public DataSource {
  public:
	static DataSourcePathRef	create( const fs::path &path );

	virtual bool	isFilePath() { return true; }
	virtual bool	isUrl() { return false; }

	virtual IStreamRef	createStream();

  protected:
	explicit DataSourcePath( const fs::path &path );
	
	virtual	void	createBuffer();
	
	IStreamFileRef	mStream;	
};


#if defined( CINDER_ANDROID )
typedef std::shared_ptr<class DataSourceAndroidAsset>	DataSourceAndroidAssetRef;

class DataSourceAndroidAsset : public DataSource {
 public:

 	static DataSourceAndroidAssetRef	create( const fs::path &path );

	virtual bool	isFilePath() { return true; }
	virtual bool	isUrl() { return false; }

	virtual IStreamRef	createStream();

  protected:
	explicit DataSourceAndroidAsset( const fs::path &path );
	
	virtual	void	createBuffer();
	
	IStreamAndroidAssetRef	mStream;
};
#endif


CI_API DataSourceRef loadFile( const fs::path &path );

#if ! defined( CINDER_UWP )
typedef std::shared_ptr<class DataSourceUrl>	DataSourceUrlRef;

class CI_API DataSourceUrl : public DataSource {
  public:
	static DataSourceUrlRef	create( const Url &Url, const UrlOptions &options = UrlOptions() );

	virtual bool	isFilePath() { return false; }
	virtual bool	isUrl() { return true; }

	virtual IStreamRef	createStream();

	const UrlOptions&	getOptions() const { return mOptions; }

  protected:
	DataSourceUrl( const Url &Url, const UrlOptions &options );
	
	virtual	void	createBuffer();

	UrlOptions		mOptions;
	IStreamUrlRef	mStream;
};
#endif // if !defined( CINDER_UWP )

CI_API DataSourceRef		loadUrl( const Url &Url, const UrlOptions &options = UrlOptions() );
CI_API inline DataSourceRef	loadUrl( const std::string &urlString, const UrlOptions &options = UrlOptions() ) { return loadUrl( Url( urlString ), options ); }

typedef std::shared_ptr<class DataSourceBuffer>	DataSourceBufferRef;

class CI_API DataSourceBuffer : public DataSource {
  public:
	static DataSourceBufferRef		create( const BufferRef &buffer, const fs::path &filePathHint = "" );

	virtual bool	isFilePath() { return false; }
	virtual bool	isUrl() { return false; }

	virtual IStreamRef	createStream();
	
  protected:
	DataSourceBuffer( const BufferRef &buffer );
	
	virtual	void	createBuffer();
	
	IStreamMemRef	mStream;
};

} // namespace cinder
