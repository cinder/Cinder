/*
 Copyright (c) 2013, The Cinder Project, All rights reserved.
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

#include "cinder/gl/platform.h"

#include <memory>
#include <iosfwd>
#include <string>

namespace cinder { namespace gl {

typedef std::shared_ptr<class BufferObj>	BufferObjRef;

class CI_API BufferObj {
  public:
	~BufferObj();
	
	static BufferObjRef create( GLenum target, GLsizeiptr allocationSize, const void *data, GLenum usage );
	
	//! Binds to the associated target.
	void		bind() const;
	//! Binds to an arbitrary target \a target.
	void		bind( GLenum target ) const;
	//! Unbinds from the associated target.
	void		unbind() const;

	//! Analogous to glBufferData()	
	void		bufferData( GLsizeiptr size, const GLvoid *data, GLenum usage );
	//! Analogous to glBufferSubData()
	void		bufferSubData( GLintptr offset, GLsizeiptr size, const GLvoid *data );
#if ! defined( CINDER_GL_ES )
	//! Returns some or all of the data from the buffer object currently bound for this objects \a target.
	void		getBufferSubData( GLintptr offset, GLsizeiptr size, GLvoid *data );
#endif // ! defined( CINDER_GL_ES )
	//! Calls bufferSubData when the size is adequate, otherwise calls bufferData, forcing a reallocation of the data store
	void		copyData( GLsizeiptr size, const GLvoid *data );
	//! Reallocates the buffer if its size is smaller than \a minimumSize. This destroys the contents of the buffer if it must be reallocated.
	void		ensureMinimumSize( GLsizeiptr minimumSize );
	
#if defined( CINDER_GL_HAS_MAP_BUFFER )
	//! Analogous to glMapBuffer(). \a access must be \c GL_READ_ONLY, \c GL_WRITE_ONLY, or \c GL_READ_WRITE. On iOS ES 2 only \c GL_WRITE_ONLY_OES is valid.
	void*				map( GLenum access ) const;
#endif

#if defined( CINDER_GL_HAS_MAP_BUFFER ) || defined( CINDER_GL_HAS_MAP_BUFFER_RANGE )
	//! Maps the Buffer for writing, but does not invalidate the Buffer's existing contents. Slower than mapReplace(). Abstracts glMapBuffer() vs. glMapBufferRange() with appropriate write-only parameters for the platform.
	void*				mapWriteOnly();
	//! Invalidates the Buffer's existing contents and maps it for writing. Preferable to mapWriteOnly() when invalidation is acceptable. Abstracts glMapBuffer() vs. glMapBufferRange() with appropriate write-only parameters for the platform.
	void*				mapReplace();
#endif
	
#if defined( CINDER_GL_HAS_MAP_BUFFER_RANGE )
	//! Analogous to glMapBufferRange(). On iOS ES 2 only \c GL_WRITE_ONLY_OES is valid.
	void*				mapBufferRange( GLintptr offset, GLsizeiptr length, GLbitfield access ) const;
#endif

#if defined( CINDER_GL_HAS_MAP_BUFFER ) || defined( CINDER_GL_HAS_MAP_BUFFER_RANGE )
	void				unmap() const;
#endif
	
	GLuint				getId() const { return mId; }
	size_t				getSize() const;
	
	GLenum				getTarget() const { return mTarget; }
	void				setTarget( GLenum target );
	
	GLenum				getUsage() const;
	void				setUsage( GLenum usage );

	//! Returns the appropriate parameter to glGetIntegerv() for a specific target; ie GL_ARRAY_BUFFER -> GL_ARRAY_BUFFER_BINDING. Returns 0 on failure.
	static GLuint		getBindingConstantForTarget( GLenum target );

	//! Returns the debugging label associated with the Buffer.
	const std::string&	getLabel() const { return mLabel; }
	//! Sets the debugging label associated with the Buffer. Calls glObjectLabel() when available.
	void				setLabel( const std::string &label );
	
	//! Analogous to glBufferStorage .
	void				bufferStorage( GLsizeiptr size, const void* data, GLbitfield flags ) const;
	//! Analogous to glNamedBufferStorage.
	void				namedBufferStorage( GLsizeiptr size, const void* data, GLbitfield flags ) const;
  protected:
	BufferObj( GLenum target );
	BufferObj( GLenum target, GLsizeiptr allocationSize, const void *data, GLenum usage );
	
	GLuint				mId;
	GLsizeiptr			mSize;
	GLenum				mTarget;
	GLenum				mUsage;
	std::string			mLabel; // debug label
	
	friend CI_API std::ostream& operator<<( std::ostream &os, const BufferObj &rhs );
};

CI_API std::ostream& operator<<( std::ostream &os, const BufferObj &rhs );
	
} }
