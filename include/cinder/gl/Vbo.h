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

#include "cinder/gl/BufferObj.h"

#include <vector>

namespace cinder { namespace gl {
	
typedef std::shared_ptr<class Vbo> VboRef;

class CI_API Vbo : public BufferObj {
  public:
	static VboRef	create( GLenum target );
	static VboRef	create( GLenum target, GLsizeiptr allocationSize, const void *data = nullptr, GLenum usage = GL_STATIC_DRAW );
	template<typename T>
	static VboRef	create( GLenum target, const std::vector<T> &v, GLenum usage = GL_STATIC_DRAW )
		{ return create( target, v.size() * sizeof(T), v.data(), usage ); }
	
  protected:
	Vbo( GLenum target );
	Vbo( GLenum target, GLsizeiptr allocationSize, const void *data, GLenum usage );
};
	
} } // namespace cinder::gl