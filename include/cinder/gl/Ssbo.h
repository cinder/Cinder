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

#if defined( CINDER_MSW ) || defined( CINDER_LINUX )

namespace cinder {
	namespace gl {
		
		//! Represents an OpenGL Shader Storage Buffer Object
		template<class T>
		class SsboT : public BufferObj {
		public:
			typedef std::shared_ptr<SsboT<T>>			Ref;

			//! Creates a shader storage buffer object with storage for \a allocationSize bytes, and filled with data \a data if it is not NULL.
			static inline Ref	create( const std::vector<T> &data, GLenum usage = GL_STATIC_DRAW )
			{
				return Ref( new SsboT<T>( data, usage ) );
			}

			//! Creates a shader storage buffer object with storage for \a allocationSize bytes, and filled with data \a data if it is not NULL.
			static inline Ref	create( GLsizeiptr allocationSize, const void *data = nullptr, GLenum usage = GL_STATIC_DRAW )
			{
				return Ref( new SsboT<T>( allocationSize, data, usage ) );
			}

			//! Map to type.
			inline T *mapT( GLbitfield access ) const { return reinterpret_cast<T *>( map( access ) ); }
			//! Map buffer range to type
			inline T *mapBufferRangeT( GLintptr offset, GLsizeiptr length, GLbitfield access ) { return reinterpret_cast<T *>( mapBufferRange( offset, length, access ) ); }
			//! Analogous to glBindBufferBase.
			inline void bindBase( GLuint index ) const { glBindBufferBase( mTarget, index, mId ); }
			//! Analogous to bufferStorage.
			inline void bufferStorage( const std::vector<T> &data, GLbitfield flags ) const { glBufferStorage( mTarget, data.size(), &( data[0] ), flags ); }

		protected:
			SsboT( const std::vector<T> &data, GLenum usage = GL_STATIC_DRAW )
				: BufferObj( GL_SHADER_STORAGE_BUFFER, sizeof( T ) * data.size(), &( data[0] ), usage )
			{
			}
			SsboT( GLsizeiptr allocationSize, const void *data = nullptr, GLenum usage = GL_STATIC_DRAW )
				: BufferObj( GL_SHADER_STORAGE_BUFFER, allocationSize, data, usage )
			{
			}
		};
	}
}

#endif // #if defined( CINDER_MSW ) || defined( CINDER_LINUX )
