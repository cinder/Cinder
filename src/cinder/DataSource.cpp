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

#include "cinder/DataSource.h"

namespace cinder {

/////////////////////////////////////////////////////////////////////////////
// DataSource
void DataSource::setFilePathHint( const std::string &aFilePathHint )
{
	mFilePathHint = aFilePathHint;
}

const fs::path& DataSource::getFilePath()
{
	return mFilePath;
}

const Url& DataSource::getUrl()
{
	return mUrl;
}

const std::string& DataSource::getFilePathHint()
{
	return mFilePathHint;
}

Buffer& DataSource::getBuffer()
{
	if( ! mBuffer )
		createBuffer();
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////
// DataSourcePath
DataSourcePathRef DataSourcePath::create( const fs::path &path )
{
	return DataSourcePathRef( new DataSourcePath( path ) );
}

DataSourcePath::DataSourcePath( const fs::path &path )
	: DataSource( path, Url() )
{
	setFilePathHint( path.string() );
}

void DataSourcePath::createBuffer()
{
	// no-op - we already supplied the buffer in the constructor
	IStreamFileRef stream = loadFileStream( mFilePath );
	if( ! stream )
		throw StreamExc();
	mBuffer = loadStreamBuffer( stream );
}

IStreamRef DataSourcePath::createStream()
{
	return loadFileStream( mFilePath );
}

DataSourceRef loadFile( const fs::path &path )
{
	return DataSourcePath::create( path );
}

/////////////////////////////////////////////////////////////////////////////
// DataSourceUrl
DataSourceUrlRef DataSourceUrl::create( const Url &url )
{
	return DataSourceUrlRef( new DataSourceUrl( url ) );
}

DataSourceUrl::DataSourceUrl( const Url &url )
	: DataSource( "", url )
{
	setFilePathHint( url.str() );
}

void DataSourceUrl::createBuffer()
{
	IStreamUrlRef stream = loadUrlStream( mUrl );
	mBuffer = loadStreamBuffer( stream );
}

IStreamRef DataSourceUrl::createStream()
{
	return loadUrlStream( mUrl );
}

DataSourceRef loadUrl( const Url &url )
{
	return DataSourceUrl::create( url );
}

/////////////////////////////////////////////////////////////////////////////
// DataSourceBuffer
DataSourceBufferRef DataSourceBuffer::create( Buffer buffer, const std::string &filePathHint )
{
	DataSourceBufferRef result( new DataSourceBuffer( buffer ) );
	result->setFilePathHint( filePathHint );
	return result;
}

DataSourceBuffer::DataSourceBuffer( Buffer buffer )
	: DataSource( "", Url() )
{
	mBuffer = buffer;
}

void DataSourceBuffer::createBuffer()
{
	// no-op - we already supplied the buffer in the constructor
}

IStreamRef DataSourceBuffer::createStream()
{
	return IStreamMem::create( mBuffer.getData(), mBuffer.getDataSize() );
}

} // namespace cinder