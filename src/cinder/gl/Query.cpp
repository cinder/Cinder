/*
 Copyright (c) 2014, The Cinder Project
 All rights reserved.
 
 This code is designed for use with the Cinder C++ library, http://libcinder.org

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

#include "cinder/gl/Query.h"
#include "cinder/CinderAssert.h"

namespace cinder { namespace gl {
	
	
/////////////////////////////////////////////////////////////////////////////////
// Query

Query::Query( GLuint target )
	: mTarget( target ), mId( 0 ) 
{
	CI_ASSERT_MSG( context(), "No currently valid gl context." );
	
	glGenQueries( 1, &mId );
}

Query::~Query()
{
	if( mId ) {
		glDeleteQueries( 1, &mId );
	}
}

void Query::begin()
{
	glBeginQuery( mTarget, mId );
}

void Query::end()
{
	glEndQuery( mTarget );
}

bool Query::isReady() const
{
	int ready = 0;
	glGetQueryObjectiv( mId, GL_QUERY_RESULT_AVAILABLE, &ready );
	return ready != 0;
}

bool Query::isValid() const
{
	return glIsQuery( mId );
}

GLint Query::getValueInt() const
{
	GLint value = 0;
	glGetQueryObjectiv( mId, GL_QUERY_RESULT, &value );
	return value;
}

GLuint Query::getValueUInt() const
{
	GLuint value = 0;
	glGetQueryObjectuiv( mId, GL_QUERY_RESULT, &value );
	return value;
}

GLint64 Query::getValueInt64() const
{
	GLint64 value = 0;
	glGetQueryObjecti64v( mId, GL_QUERY_RESULT, &value );
	return value;
}

GLuint64 Query::getValueUInt64() const
{
	GLuint64 value = 0;
	glGetQueryObjectui64v( mId, GL_QUERY_RESULT, &value );
	return value;
}

QueryRef Query::create( GLenum target )
{
	return QueryRef( new Query( target ) );
}

/////////////////////////////////////////////////////////////////////////////////
// QueryTimeElapsed
QueryTimeElapsedRef QueryTimeElapsed::create()
{
	return QueryTimeElapsedRef( new QueryTimeElapsed );
}
	
QueryTimeElapsed::QueryTimeElapsed()
	: mSwapIndex( 0 ), mIsStopped( true )
{
	mQueryBuffers[0] = Query::create( GL_TIME_ELAPSED );
	mQueryBuffers[1] = Query::create( GL_TIME_ELAPSED );
}

void QueryTimeElapsed::begin()
{
	if( mIsStopped ) {
		mQueryBuffers[mSwapIndex]->begin();
		mIsStopped = false;
	}
}

void QueryTimeElapsed::end()
{
	if( ! mIsStopped ) {
		mQueryBuffers[mSwapIndex]->end();
		mIsStopped = true;
		swap();
	}
}

void QueryTimeElapsed::swap()
{
	mSwapIndex = static_cast<short>( ! mSwapIndex );
}
	
uint64_t QueryTimeElapsed::getElapsedNanoseconds() const
{
	if( ! mIsStopped ) {
		throw std::logic_error( "Stop must be called before querying the result." );
	}
	const auto& query = mQueryBuffers[mSwapIndex];
	return ( query->isValid() ) ? query->getValueUInt64() : 0;
}

double QueryTimeElapsed::getElapsedMilliseconds() const
{
	return static_cast<double>( getElapsedNanoseconds() ) * 0.000001;
}

double QueryTimeElapsed::getElapsedSeconds() const
{
	return static_cast<double>( getElapsedNanoseconds() ) * 0.000000001;
}

} } // namespace cinder::gl
