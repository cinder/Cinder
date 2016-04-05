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

// Concrete implementation of VAO for OpenGL ES 2.
// Should only be instantiated by Vao::create() in the presence of GL_OES_vertex_array_object


#include "cinder/gl/platform.h"

#if defined( CINDER_GL_ES ) && ! defined( CINDER_GL_ANGLE )

#include "cinder/gl/Vao.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Context.h"
#include "cinder/Log.h"

namespace cinder { namespace gl {

class VaoImplEs : public Vao {
  public:
	virtual ~VaoImplEs();
	
	VaoImplEs();

	// Does the actual "work" of binding the VAO; called by Context
	void	bindImpl( class Context *context ) override;
	void	unbindImpl( class Context *context ) override;
	void	enableVertexAttribArrayImpl( GLuint index ) override;
	void	disableVertexAttribArrayImpl( GLuint index ) override;
	void	vertexAttribPointerImpl( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer ) override;
	void	vertexAttribIPointerImpl( GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer ) override;
	void	vertexAttribDivisorImpl( GLuint index, GLuint divisor ) override;
	void	reflectBindBufferImpl( GLenum target, GLuint buffer ) override;
	void	reassignContext( Context *newContext ) override;

	void	reassignImpl( Context *newContext );
	
	friend class Context;
};

// Called by Vao::create()
VaoRef createVaoImplEs()
{
	return VaoRef( new VaoImplEs );
}
	
VaoImplEs::VaoImplEs()
{
	mId	= 0;

	glGenVertexArrays( 1, &mId );
}

VaoImplEs::~VaoImplEs()
{
	glDeleteVertexArrays( 1, &mId );
}

void VaoImplEs::bindImpl( Context *context )
{
	if( context && ( context != mCtx ) ) {
		CI_LOG_W( "VAO bound against different context from allocation. Reassigning context." );
		reassignImpl( context );
	}

	glBindVertexArray( mId );	

	if( context ) {
		context->reflectBufferBinding( GL_ELEMENT_ARRAY_BUFFER, mLayout.mElementArrayBufferBinding );
		mLayout.mCachedArrayBufferBinding = context->getBufferBinding( GL_ARRAY_BUFFER );
	}
}

void VaoImplEs::reassignContext( Context *newContext )
{
	reassignImpl( newContext );
}

void VaoImplEs::reassignImpl( Context *newContext )
{
	if( newContext == mCtx )
		return;

	mCtx = newContext;

	// generate
	glGenVertexArrays( 1, &mId );

	// assign
	glBindVertexArray( mId );

	// instantiate the VAO using the layout
	auto oldBuffer = mCtx->getBufferBinding( GL_ARRAY_BUFFER );

	for( auto attribIt = mLayout.mVertexAttribs.begin(); attribIt != mLayout.mVertexAttribs.end(); ++attribIt ) {
		if( attribIt->second.mEnabled ) {
			glEnableVertexAttribArray( attribIt->first );
			glBindBuffer( GL_ARRAY_BUFFER, attribIt->second.mArrayBufferBinding );
			if( attribIt->second.mPointerType == Vao::VertexAttrib::FLOAT )
				glVertexAttribPointer( attribIt->first, attribIt->second.mSize, attribIt->second.mType, attribIt->second.mNormalized, attribIt->second.mStride, attribIt->second.mPointer );
			else
				CI_LOG_E( "Attempt to use integer AttribPointer on ES 2. Ignoring." );
		}
	}

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mLayout.mElementArrayBufferBinding );
	// we need to bind this directly to prevent the gl::Context's caching from subverting our restoration of the old GL_ARRAY_BUFFER
	glBindBuffer( GL_ARRAY_BUFFER, oldBuffer );
}


void VaoImplEs::unbindImpl( Context *context )
{
	glBindVertexArray( 0 );

	mCtx->invalidateBufferBindingCache( GL_ELEMENT_ARRAY_BUFFER );
}

void VaoImplEs::enableVertexAttribArrayImpl( GLuint index )
{
	if( ! mLayout.isVertexAttribArrayEnabled( index ) ) {
		mLayout.enableVertexAttribArray( index );
		glEnableVertexAttribArray( index );
	}
}

void VaoImplEs::disableVertexAttribArrayImpl( GLuint index )
{
	mLayout.disableVertexAttribArray( index );
	
	glDisableVertexAttribArray( index );
}

void VaoImplEs::vertexAttribPointerImpl( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer )
{
	// test to see if the layout doesn't already reflect this, so we can avoid a redundant call to glVertexAttribPointer
	if( ! mLayout.isVertexAttribEqual( index, size, type, normalized, stride, VertexAttrib::FLOAT, pointer, mLayout.mCachedArrayBufferBinding ) ) {
		mLayout.vertexAttribPointer( index, size, type, normalized, stride, pointer );
		glVertexAttribPointer( index, size, type, normalized, stride, pointer );
	}
}

void VaoImplEs::vertexAttribIPointerImpl( GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
// currently a no-op although this is implemented in ES 3
#if 0
	// test to see if the layout doesn't already reflect this, so we can avoid a redundant call to glVertexAttribPointer
	if( ! mLayout.isVertexAttribEqual( index, size, type, normalized, stride, VertexAttrib::INTEGER, pointer, mLayout.mCachedArrayBufferBinding ) ) {
		mLayout.vertexAttribPointer( index, size, type, normalized, stride, pointer );
		glVertexAttribIPointer( index, size, type, normalized, stride, pointer );
	}
#endif
}

void VaoImplEs::vertexAttribDivisorImpl( GLuint index, GLuint divisor )
{
	mLayout.vertexAttribDivisor( index, divisor );
	glVertexAttribDivisor( index, divisor );
}

void VaoImplEs::reflectBindBufferImpl( GLenum target, GLuint buffer )
{
	mLayout.bindBuffer( target, buffer );

	glBindBuffer( target, buffer );
}

} }

#endif // defined( CINDER_GL_ES ) && ! defined( CINDER_GL_ANGLE )