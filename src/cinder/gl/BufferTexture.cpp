/*
 Copyright (c) 2014, The Cinder Project: http://libcinder.org
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

#include "cinder/gl/BufferTexture.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/scoped.h"

namespace cinder { namespace gl {
	
#if ! defined( CINDER_GL_ES )
	
BufferTextureRef BufferTexture::create( const BufferObjRef &buffer, GLenum internalFormat )
{
	return BufferTextureRef( new BufferTexture( buffer, internalFormat ) );
}

BufferTextureRef BufferTexture::create( const void *data, size_t numBytes, GLenum internalFormat, GLenum usage )
{
	gl::BufferObjRef buffer = BufferObj::create( GL_TEXTURE_BUFFER, numBytes, data, usage );
	return BufferTexture::create( buffer, internalFormat );
}

BufferTexture::BufferTexture( const BufferObjRef &buffer, GLenum internalFormat )
	: mTarget( GL_TEXTURE_BUFFER )
{
	glGenTextures( 1, &mId );
	setBuffer( buffer, internalFormat );
}

BufferTexture::~BufferTexture()
{
	glDeleteTextures( 1, &mId );
}

void BufferTexture::setBuffer( const BufferObjRef &buffer, GLenum internalFormat )
{
	mInternalFormat = internalFormat;
	mBufferObj = buffer;
	gl::context()->bindTexture( mTarget, mId );
	glTexBuffer( mTarget, mInternalFormat, buffer->getId() );
}

void BufferTexture::bindTexture( uint8_t textureUnit )
{
	ScopedActiveTexture activeTexture( textureUnit );
	gl::context()->bindTexture( mTarget, mId );
}

void BufferTexture::unbindTexture( uint8_t textureUnit )
{
	ScopedActiveTexture activeTexture( textureUnit );
	gl::context()->bindTexture( mTarget, 0 );
}
	
#endif
	
} }

