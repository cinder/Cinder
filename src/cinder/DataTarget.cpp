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

#include "cinder/DataTarget.h"
#include "cinder/Log.h"

namespace cinder {

/////////////////////////////////////////////////////////////////////////////
// DataTarget
void DataTarget::setFilePathHint( const fs::path &aFilePathHint )
{
	mFilePathHint = aFilePathHint;
}

const fs::path& DataTarget::getFilePath() const
{
	return mFilePath;
}

const Url& DataTarget::getUrl() const
{
	return mUrl;
}

const fs::path& DataTarget::getFilePathHint() const
{
	return mFilePathHint;
}

/////////////////////////////////////////////////////////////////////////////
// DataTargetPath
DataTargetPathRef DataTargetPath::createRef( const fs::path &path )
{
	return DataTargetPathRef( new DataTargetPath( path ) );
}

DataTargetPath::DataTargetPath( const fs::path &path )
	: DataTarget( path, Url() )
{
	setFilePathHint( path );
}

OStreamRef DataTargetPath::getStream()
{
	if( ! mStream )
		mStream = writeFileStream( mFilePath );
		
	return mStream;
}

/////////////////////////////////////////////////////////////////////////////
// DataTargetStream
DataTargetStreamRef DataTargetStream::createRef( OStreamRef stream )
{
	return DataTargetStreamRef( new DataTargetStream( stream ) );
}

DataTargetStream::DataTargetStream( OStreamRef stream )
	: DataTarget( "", Url() ), mStream( stream )
{
	setFilePathHint( mStream->getFileName() );
}

/////////////////////////////////////////////////////////////////////////////
// Free functions
DataTargetPathRef writeFile( const fs::path &path, bool createParents )
{
	if( createParents ) {
		if( ! path.parent_path().empty() )
			fs::create_directories( path.parent_path() );
	}
	
 	return DataTargetPath::createRef( path );
}

} // namespace cinder