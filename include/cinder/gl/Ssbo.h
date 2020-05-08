/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.

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
#include "cinder/gl/gl.h"

#if ( defined( CINDER_MSW ) && ! defined( CINDER_GL_ANGLE ) ) || ( defined( CINDER_LINUX ) && ! defined( CINDER_GL_ES ) )

#include "cinder/gl/BufferObj.h"

namespace cinder { namespace gl {

class Ssbo;
typedef std::shared_ptr<Ssbo>	SsboRef;

class CI_API Ssbo : public BufferObj {
public:
	//! Creates a shader storage buffer object with storage for \a allocationSize bytes, and filled with data \a data if it is not NULL.
	static inline SsboRef	create( GLsizeiptr allocationSize, const void *data = nullptr, GLenum usage = GL_STATIC_DRAW )
	{
		return SsboRef( new Ssbo( allocationSize, data, usage ) );
	}

	//! Bind base.
	inline void bindBase( GLuint index ) { glBindBufferBase( mTarget, index, mId );  mBase = index; }
	//! Unbinds the buffer.
	inline void unbindBase() { glBindBufferBase( mTarget, mBase, 0 ); mBase = 0; }
protected:
	Ssbo( GLsizeiptr allocationSize, const void *data = nullptr, GLenum usage = GL_STATIC_DRAW )
		: BufferObj( GL_SHADER_STORAGE_BUFFER, allocationSize, data, usage ),
			mBase( 0 )
	{
	}
	GLuint mBase;
};

} } // cinder::gl

#endif // ( defined( CINDER_MSW ) && ! defined( CINDER_GL_ANGLE ) ) || defined( CINDER_LINUX )
