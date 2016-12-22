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

#if ! defined( CINDER_GL_ES )

#include "cinder/gl/BufferObj.h"

namespace cinder { namespace gl {
	
typedef std::shared_ptr<class Pbo>			PboRef;

//! Represents an OpenGL Pixel Buffer Object
class CI_API Pbo : public BufferObj {
  public:
	//! Creates a PBO with no memory allocated
	static PboRef	create( GLenum target );
	//! Creates a PBO at \a target (generally \c GL_PIXEL_PACK_BUFFER or GL_PIXEL_UNPACK_BUFFER), with storage for \a allocationSize bytes, and filled with data \a data if it is not NULL.
	static PboRef	create( GLenum target, GLsizeiptr allocationSize, const void *data = nullptr, GLenum usage = GL_STATIC_DRAW );

  protected:
	Pbo( GLenum target );
	Pbo( GLenum target, GLsizeiptr allocationSize, const void *data = nullptr, GLenum usage = GL_STATIC_DRAW );
};

} }

#endif // defined( CINDER_GL_ES )
