/*
 Copyright (c) 2014, The Cinder Project, All rights reserved.
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

#include "cinder/gl/Ubo.h"
#include "cinder/gl/Context.h"

#if ! defined( CINDER_GL_ES_2 )

namespace cinder { namespace gl {
	
UboRef Ubo::create()
{
	return UboRef( new Ubo() );
}

UboRef Ubo::create( GLsizeiptr allocationSize, const void *data, GLenum usage )
{
	return UboRef( new Ubo( allocationSize, data, usage ) );
}
	
Ubo::Ubo()
	: BufferObj( GL_UNIFORM_BUFFER )
{
	mUsage = GL_DYNAMIC_DRAW;
}

Ubo::Ubo( GLsizeiptr allocationSize, const void *data, GLenum usage )
	: BufferObj( GL_UNIFORM_BUFFER )
{
	bufferData( allocationSize, data, usage );
}

void Ubo::bindBufferBase( GLuint index )
{
	context()->bindBufferBase( mTarget, index, mId );
}

} } // namespace cinder::gl

#endif // ! defined( CINDER_GL_ES_2 )
