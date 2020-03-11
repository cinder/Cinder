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

#include "cinder/gl/BufferObj.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/ConstantConversions.h"
#include "cinder/gl/Environment.h"
#include "cinder/gl/scoped.h"

namespace cinder { namespace gl {

BufferObjRef BufferObj::create( GLenum target, GLsizeiptr allocationSize, const void *data, GLenum usage )
{
	return BufferObjRef( new BufferObj( target, allocationSize, data, usage ) );
}
	
BufferObj::BufferObj( GLenum target )
	: mId( 0 ), mSize( 0 ), mTarget( target ),
#if defined( CINDER_GL_ES )
  #if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
	mUsage( GL_DYNAMIC_DRAW )
  #else	
	mUsage( 0 ) /* GL ES default buffer usage is undefined(?) */
  #endif	
#else
	mUsage( GL_READ_WRITE )
#endif
{
	bool initialized = false;
#if defined( GL_VERSION_4_5 )
	if( GLAD_GL_VERSION_4_5 ) {
		glCreateBuffers( 1, &mId );
		initialized = true;
	}
#endif
	if( ! initialized ) {
		glGenBuffers( 1, &mId );
	}
	gl::context()->bufferCreated( this );
}

BufferObj::BufferObj( GLenum target, GLsizeiptr allocationSize, const void *data, GLenum usage )
	: mId( 0 ), mTarget( target ), mSize( allocationSize ), mUsage( usage )
{
	bool initialized = false;
#if defined( GL_VERSION_4_5 )
	if( GLAD_GL_VERSION_4_5 ) {
		glCreateBuffers( 1, &mId );
		glNamedBufferData( mId, mSize, data, mUsage );
		initialized = true;
	}
#endif
	if( ! initialized ) {
		glGenBuffers( 1, &mId );

		ScopedBuffer bufferBind( mTarget, mId );
		glBufferData( mTarget, mSize, data, mUsage );
	}
	
	gl::context()->bufferCreated( this );
}

BufferObj::~BufferObj()
{	
	auto ctx = gl::context();
	if( ctx )
		ctx->bufferDeleted( this );
		
	glDeleteBuffers( 1, &mId );		
}

void BufferObj::bind() const
{
	context()->bindBuffer( mTarget, mId );
}

void BufferObj::bind( GLenum target ) const
{
	context()->bindBuffer( target, mId );
}

void BufferObj::bufferData( GLsizeiptr size, const GLvoid *data, GLenum usage )
{
	ScopedBuffer bufferBind( mTarget, mId );
	mSize = size;
	mUsage = usage;
	glBufferData( mTarget, mSize, data, usage );
}
	
void BufferObj::bufferSubData( GLintptr offset, GLsizeiptr size, const GLvoid *data )
{
	ScopedBuffer bufferBind( mTarget, mId );
	glBufferSubData( mTarget, offset, size, data );
}

#if ! defined( CINDER_GL_ES )
void BufferObj::getBufferSubData( GLintptr offset, GLsizeiptr size, GLvoid *data )
{
	ScopedBuffer bufferBind( mTarget, mId );
	glGetBufferSubData( mTarget, offset, size, data );
}
#endif

void BufferObj::copyData( GLsizeiptr size, const GLvoid *data )
{
	ScopedBuffer bufferBind( mTarget, mId );
	
	if( size <= mSize )
		glBufferSubData( mTarget, 0, size, data );
	else { // need to reallocate due to inadequate size
		mSize = size;
		glBufferData( mTarget, mSize, data, mUsage );
	}
}

void BufferObj::ensureMinimumSize( GLsizeiptr minimumSize )
{
	if( mSize < minimumSize ) {
		mSize = minimumSize;
		ScopedBuffer bufferBind( mTarget, mId );
		glBufferData( mTarget, mSize, NULL, mUsage );
	}
}

#if defined( CINDER_GL_HAS_MAP_BUFFER )
void* BufferObj::map( GLenum access ) const
{
	ScopedBuffer bufferBind( mTarget, mId );
	return reinterpret_cast<void*>( glMapBuffer( mTarget, access ) );
}
#endif

#if defined( CINDER_GL_HAS_MAP_BUFFER ) || defined( CINDER_GL_HAS_MAP_BUFFER_RANGE )
void* BufferObj::mapWriteOnly()
{
	void* result = nullptr;
	ScopedBuffer bufferBind( mTarget, mId );
#if defined( CINDER_GL_HAS_MAP_BUFFER_RANGE )
	GLbitfield access = GL_MAP_WRITE_BIT;
	result = reinterpret_cast<void*>( glMapBufferRange( mTarget, 0, mSize, access ) );
#elif defined( CINDER_GL_HAS_MAP_BUFFER )
	result = reinterpret_cast<void*>( glMapBuffer( mTarget, GL_WRITE_ONLY ) );
#endif
	return result;
}

void* BufferObj::mapReplace()
{
	ScopedBuffer bufferBind( mTarget, mId );
	void* result = nullptr;
#if defined( CINDER_GL_HAS_MAP_BUFFER_RANGE )
	GLbitfield access = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
	result = reinterpret_cast<void*>( glMapBufferRange( mTarget, 0, mSize, access ) );
#elif defined( CINDER_GL_HAS_MAP_BUFFER )
	glBufferData( mTarget, mSize, nullptr, mUsage );
	result = reinterpret_cast<void*>( glMapBuffer( mTarget, GL_WRITE_ONLY ) );
#endif
	return result;
}
#endif

#if defined( CINDER_GL_HAS_MAP_BUFFER_RANGE )
void* BufferObj::mapBufferRange( GLintptr offset, GLsizeiptr length, GLbitfield access ) const
{
	ScopedBuffer bufferBind( mTarget, mId );
    return reinterpret_cast<void*>( glMapBufferRange( mTarget, offset, length, access ) );
}
#endif

#if defined( CINDER_GL_HAS_MAP_BUFFER ) || defined( CINDER_GL_HAS_MAP_BUFFER_RANGE )
void BufferObj::unmap() const
{
	ScopedBuffer bufferBind( mTarget, mId );
    GLboolean result = glUnmapBuffer( mTarget );
    if ( GL_TRUE != result ) {
        //throw BufferFailedUnmapExc();
    }
}
#endif

size_t BufferObj::getSize() const
{
	return mSize;
}
	
void BufferObj::setTarget( GLenum target )
{
	mTarget = target;
}
	
GLenum BufferObj::getUsage() const
{
	return mUsage;
}

void BufferObj::setUsage( GLenum usage )
{
	mUsage = usage;
}
	
void BufferObj::unbind() const
{
	context()->bindBuffer( mTarget, 0 );
}

GLuint BufferObj::getBindingConstantForTarget( GLenum target )
{
	switch( target ) {
		case GL_ARRAY_BUFFER:
			return GL_ARRAY_BUFFER_BINDING;
		case GL_ELEMENT_ARRAY_BUFFER:
			return GL_ELEMENT_ARRAY_BUFFER_BINDING;
#if ! defined( CINDER_GL_ES )
		case GL_PIXEL_PACK_BUFFER:
			return GL_PIXEL_PACK_BUFFER_BINDING;
		case GL_PIXEL_UNPACK_BUFFER:
			return GL_PIXEL_UNPACK_BUFFER_BINDING;
		case GL_TEXTURE_BUFFER:
			return GL_TEXTURE_BINDING_BUFFER;
		case GL_TRANSFORM_FEEDBACK_BUFFER:
			return GL_TRANSFORM_FEEDBACK_BUFFER_BINDING;
		case GL_UNIFORM_BUFFER:
			return GL_UNIFORM_BUFFER_BINDING;
		case GL_DRAW_INDIRECT_BUFFER:
			return GL_DRAW_INDIRECT_BUFFER_BINDING;
		case GL_QUERY_BUFFER:
			return GL_QUERY_BUFFER_BINDING;
		case GL_ATOMIC_COUNTER_BUFFER:
			return GL_ATOMIC_COUNTER_BUFFER_BINDING;
#endif
		default:
			return 0;
	}
}

void BufferObj::setLabel( const std::string &label )
{
	mLabel = label;
#if defined( CINDER_GL_HAS_DEBUG_LABEL )
	env()->objectLabel( GL_BUFFER_OBJECT_EXT, mId, (GLsizei)label.size(), label.c_str() );
#elif defined( CINDER_HAS_KHR_DEBUG )
	env()->objectLabel( GL_BUFFER, mId, (GLsizei)label.size(), label.c_str() );
#endif
}

void BufferObj::bufferStorage( GLsizeiptr size, const void* data, GLbitfield flags ) const
{
#if defined( GL_VERSION_4_4 )
	CI_ASSERT( GLAD_GL_VERSION_4_4 );
	glBufferStorage( mTarget, size, data, flags );
#else
	throw gl::Exception( "bufferStorage unimplemented if GL_VERSION_4_4 is not accessible." );
#endif
}

void BufferObj::namedBufferStorage( GLsizeiptr size, const void* data, GLbitfield flags ) const
{
#if defined( GL_VERSION_4_5 )
	CI_ASSERT( GLAD_GL_VERSION_4_5 );
	glNamedBufferStorage( mId, size, data, flags );
#else
	throw gl::Exception( "namedBufferStorage unimplemented if GL_VERSION_4_5 is not accessible." );
#endif
}

std::ostream& operator<<( std::ostream &os, const BufferObj &rhs )
{
	os << "ID: " << rhs.mId << std::endl;
	os << " Target: " << gl::constantToString( rhs.mTarget ) << "(" << rhs.mTarget << ")" << std::endl;
	os << "   Size: " << rhs.mSize << std::endl;
	os << "  Usage: " << gl::constantToString( rhs.mUsage ) << "(" << rhs.mUsage << ")" << std::endl;
	if( ! rhs.mLabel.empty() )
		os << "  Label: " << rhs.mLabel << std::endl;

	return os;
}

} } // namespace cinder::gl
