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

#pragma once

#include "cinder/gl/platform.h"
#include "cinder/Exception.h"

#include <array>

namespace cinder { namespace gl {

#if ! defined( CINDER_GL_ES )

typedef std::shared_ptr<class Query> QueryRef;

//! Base class for OpenGL Query objects.
class CI_API Query {
  public:
	static QueryRef create( GLenum target );

	virtual ~Query();
	
	//! Returns the id of this object.
	GLuint		getId() const { return mId; }
	//! Returns the target of this object.
	GLenum		getTarget() const { return mTarget; }

	//! Begins a query.
	virtual void begin();
	//! Ends a query.
	virtual void end();
	
	//! Returns the value of the underlying query with GLint precision.
	GLint		getValueInt() const;
	//! Returns the value of the underlying query with GLuint precision.
	GLuint		getValueUInt() const;
	//! Returns the value of the underlying query with GLint64 precision.
	GLint64		getValueInt64() const;
	//! Returns the value of the underlying query with GLuint64 precision.
	GLuint64	getValueUInt64() const;
	
	//! Writes the value of the query to the buffer bound to \c GL_QUERY_BUFFER, offset by \a params. Writes to address \a params when none is bound.
	void		getValueInt( GLint* params ) const;
	//! Writes the value of the query to the buffer bound to \c GL_QUERY_BUFFER, offset by \a params. Writes to address \a params when none is bound.
	void		getValueUInt( GLuint* params ) const;
	//! Writes the value of the query to the buffer bound to \c GL_QUERY_BUFFER, offset by \a params. Writes to address \a params when none is bound.
	void		getValueInt64( GLint64* params ) const;
	//! Writes the value of the query to the buffer bound to \c GL_QUERY_BUFFER, offset by \a params. Writes to address \a params when none is bound.
	void		getValueUInt64( GLuint64* params ) const;

	//! Returns whether the query object is ready.
	bool		isReady() const;
	//! Returns whether the query's id is valid (corresponds to an actual query object).
	bool		isValid() const;
  protected:
	Query( GLuint target );
	
	GLuint mId;
	GLenum mTarget;
  private:
	Query( const Query& );
	const Query& operator=( const Query& );
};

typedef std::shared_ptr<class QueryTimeSwapped> QueryTimeSwappedRef;

//! Represents a pair of GL_TIME_ELAPSED Query objects, swapped on each end() call.
class CI_API QueryTimeSwapped {
  public:
	//! Constructs a swap buffer of GL_TIME_ELAPSED queries.
	static QueryTimeSwappedRef create();

	//! Begins the time elapsed query.
	void begin();
	//! Ends the time elapsed query and swaps the currently active query buffer.
	void end();
		
	//! Returns the elapsed nano seconds from the previous frame query. This is the native data type of the underlying query object. Throws logic_error if not stopped.
	uint64_t	getElapsedNanoseconds() const;
	//! Returns the elapsed Milliseconds from the previous frame query. Converts from what gl returns, which is nano seconds. Throws logic_error if not stopped.
	double		getElapsedMilliseconds() const;
	//! Returns the elapsed seconds from the previous frame query. Converts from what gl returns, which is nano seconds. Throws logic_error if not stopped.
	double		getElapsedSeconds() const;
	
  private:
	QueryTimeSwapped();
	//! Swap the currently active query buffer.
	inline void swap();

	QueryTimeSwapped( const QueryTimeSwapped& );
	const QueryTimeSwapped& operator=( const QueryTimeSwapped& );

	std::array<QueryRef, 2>	mQueryBuffers;
	bool					mIsStopped;
	size_t					mSwapIndex;
};
	
class CI_API QueryException : public Exception {
  public:
	QueryException( const std::string &description ) : Exception( description ) { }
};

#endif // ! defined( CINDER_GL_ES )

} } // namespace cinder::gl
