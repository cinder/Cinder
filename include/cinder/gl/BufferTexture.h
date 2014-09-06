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

#pragma once

#include "cinder/gl/gl.h"
#include "cinder/gl/BufferObj.h"

namespace cinder { namespace gl {

#if ! defined( CINDER_GL_ES )
	
typedef std::shared_ptr<class BufferTexture> BufferTextureRef;

class BufferTexture {
  public:
	static BufferTextureRef create( const BufferObjRef &buffer, GLenum internalFormat );
	static BufferTextureRef create( const void *data, size_t numBytes, GLenum internalFormat, GLenum usage = GL_STATIC_DRAW );
	
	~BufferTexture();
	
	//! Binds the underlying Texture representation of your BufferObj
	void bindTexture( uint8_t textureUnit = 0  );
	//! Unbinds the underlying Texture representation of your BufferObj
	void unbindTexture( uint8_t textureUnit = 0 );
	
	//! Set or Reset the BufferObj this Buffer Texture is associated with
	void setBuffer( const BufferObjRef &buffer, GLenum internalFormat );
	
	//! Returns the gl system id for this Buffer Texture
	GLuint				getId() { return mId; }
	//! Returns the target for the Buffer Texture. \c GL_TEXTURE_BUFFER is the only allowable target.
	GLenum				getTarget() { return mTarget; }
	//! Returns the internalFormat of the Buffer Texture. It represents the way your BufferObj data is built.
	GLint				getInternalFormat() { return mInternalFormat; }
	//! Returns the BufferObj associated with this Buffer Texture currently.
	BufferObjRef&		getBufferObj() { return mBufferObj; }
	const BufferObjRef& getBufferObj() const { return mBufferObj; }
	
  private:
	BufferTexture( const BufferObjRef &buffer, GLenum internalFormat );
	
	GLenum			mTarget;
	GLuint			mId;
	mutable GLenum	mInternalFormat;
	BufferObjRef	mBufferObj;
};
	
#endif
	
} } // gl // cinder
