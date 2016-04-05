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

/*	The VAO class abstracts Vertex Array Objects in OpenGL through 3 implementation classes,
		VaoImplEs for OpenGL ES 2
		VaoImplCore for desktop OpenGL, both Core and Compatibility profile with approriate extensions
		VaoImplSoftware for implementations without a native VAO class
	
	We don't support the old fixed function data (ie glVertexPointer() and friends).
	
	ci::gl::VAO caches the following state:
		* ELEMENT_ARRAY_BUFFER_BINDING
		* VERTEX_ATTRIB_ARRAY_BUFFER_BINDING per attribute
		* All individual attribute data
	
	The full list is in Table 6.4 of the OpenGL 3.2 Core Profile spec,
		http://www.opengl.org/registry/doc/glspec32.core.20090803.pdf
	
	Note that ARRAY_BUFFER_BINDING is NOT cached
*/

#include "cinder/gl/Vao.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Environment.h"
#include "cinder/gl/ConstantConversions.h"

#include <set>

using namespace std;

namespace cinder { namespace gl {

// defined in VaoImplEs
#if defined( CINDER_GL_ES ) && ( CINDER_GL_ES_VERSION <= CINDER_GL_ES_VERSION_2 )
extern VaoRef createVaoImplEs();
#else
extern VaoRef createVaoImplCore();
#endif
extern VaoRef createVaoImplSoftware();

VaoRef Vao::create()
{
#if defined( CINDER_GL_ES ) && ( CINDER_GL_ES_VERSION <= CINDER_GL_ES_VERSION_2 )
	#if defined( CINDER_COCOA_TOUCH )
		return createVaoImplEs();
	#elif defined( CINDER_GL_ANGLE )
		return createVaoImplSoftware();
	#else
		if( env()->supportsHardwareVao() ) {
			return createVaoImplEs();
		}
		else {
			return createVaoImplSoftware();
		}
	#endif
#else
	return createVaoImplCore();
#endif
}

Vao::Vao()
	: mCtx( gl::context() )
{
	mCtx->vaoCreated( this );
}

Vao::~Vao()
{
	auto ctx = gl::context();
	if( ctx )
		ctx->vaoDeleted( this );
}

void Vao::setContext( Context *context )
{
	mCtx = context;
}

void Vao::bind()
{
	// this will "come back" by calling bindImpl if it's necessary
	mCtx->bindVao( this );
}

void Vao::unbind() const
{
	// this will "come back" by calling bindImpl if it's necessary
	mCtx->bindVao( nullptr );
}

void Vao::replacementBindBegin()
{
	mReplacementBindPrevious = mLayout;
	mLayout.clear();
	bind();
	// a fresh VAO would 
	mCtx->bindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

bool Vao::Layout::findVertexAttribForLocation( GLuint loc, const VertexAttrib **result ) const
{
	for( auto &attrib : mVertexAttribs )
		if( attrib.first == loc ) {
			*result = &attrib.second;
			return true;
		}

	return false;
}

bool Vao::Layout::findVertexAttribForLocation( GLuint loc, VertexAttrib **result )
{
	for( auto &attrib : mVertexAttribs )
		if( attrib.first == loc ) {
			*result = &attrib.second;
			return true;
		}

	return false;
}

void Vao::replacementBindEnd()
{
	// disable any attributes which were enabled in the previous layout
	for( auto &attrib : mReplacementBindPrevious.mVertexAttribs ) {
		if( attrib.second.mEnabled) {
			VertexAttrib *existing;
			if( ( ! mLayout.findVertexAttribForLocation( attrib.first, &existing ) ) || ( ! existing->mEnabled ) )
				disableVertexAttribArrayImpl( attrib.first );
		}
	}

	// TODO: if the user never bound an ELEMENT_ARRAY_BUFFER, they assumed it was 0, so we should make that so by caching whether they changed it
}

void Vao::setLabel( const std::string &label )
{
	mLabel = label;
#if defined( CINDER_GL_HAS_DEBUG_LABEL )
	env()->objectLabel( GL_VERTEX_ARRAY_OBJECT_EXT, mId, (GLsizei)label.size(), label.c_str() );
#elif defined( CINDER_HAS_KHR_DEBUG )
	env()->objectLabel( GL_VERTEX_ARRAY, mId, (GLsizei)label.size(), label.c_str() );
#endif
	
/*	
#if defined( CINDER_COCOA_TOUCH ) || defined( CINDER_ANDROID )
	env()->objectLabel( GL_VERTEX_ARRAY_OBJECT_EXT, mId, (GLsizei)label.size(), label.c_str() );
#elif ! defined( CINDER_GL_ANGLE )
	env()->objectLabel( GL_VERTEX_ARRAY, mId, (GLsizei)label.size(), label.c_str() );
#endif
*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vao::Layout
Vao::Layout::Layout()
	: mElementArrayBufferBinding( 0 ), mCachedArrayBufferBinding( 0 )
{
}

void Vao::Layout::bindBuffer( GLenum target, GLuint buffer )
{
	if( target == GL_ARRAY_BUFFER )
		mCachedArrayBufferBinding = buffer;
	else if( target == GL_ELEMENT_ARRAY_BUFFER )
		mElementArrayBufferBinding = buffer;
}

void Vao::Layout::enableVertexAttribArray( GLuint index )
{
	VertexAttrib *existing;
	if( findVertexAttribForLocation( index, &existing ) ) {
		existing->mEnabled = true;
	}
	else {
		mVertexAttribs.emplace_back( index, VertexAttrib() );
		mVertexAttribs.back().second.mEnabled = true;
	}
}

bool Vao::Layout::isVertexAttribArrayEnabled( GLuint index ) const
{
	const VertexAttrib *existing;
	if( findVertexAttribForLocation( index, &existing ) ) {
		return existing->mEnabled;
	}
	else {
		return false;
	}
}

// We need to make a default VertexAttrib to record the request to disable the location
void Vao::Layout::disableVertexAttribArray( GLuint index )
{
	VertexAttrib *existing;
	if( findVertexAttribForLocation( index, &existing ) ) {
		existing->mEnabled = false;
	}
	else {
		mVertexAttribs.emplace_back( index, VertexAttrib() );
		mVertexAttribs.back().second.mEnabled = false;
	}
}

void Vao::Layout::vertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer )
{
	VertexAttrib *existing;
	if( findVertexAttribForLocation( index, &existing ) ) {
		bool enabled = existing->mEnabled;
		*existing = Vao::VertexAttrib( size, type, normalized, stride, VertexAttrib::FLOAT, pointer, mCachedArrayBufferBinding );
		existing->mEnabled = enabled;
	}
	else {
		mVertexAttribs.emplace_back( index, Vao::VertexAttrib( size, type, normalized, stride, VertexAttrib::FLOAT, pointer, mCachedArrayBufferBinding ) );
		mVertexAttribs.back().second.mEnabled = false;
	}
}

void Vao::Layout::vertexAttribIPointer( GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
	VertexAttrib *existing;
	if( findVertexAttribForLocation( index, &existing ) ) {
		bool enabled = existing->mEnabled;
		*existing = Vao::VertexAttrib( size, type, false, stride, VertexAttrib::INTEGER, pointer, mCachedArrayBufferBinding );
		existing->mEnabled = enabled;
	}
	else {
		mVertexAttribs.emplace_back( index, Vao::VertexAttrib( size, type, false, stride, VertexAttrib::INTEGER, pointer, mCachedArrayBufferBinding ) );
		mVertexAttribs.back().second.mEnabled = false;
	}
}

bool Vao::Layout::isVertexAttribEqual( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, VertexAttrib::PointerType pointerType, const GLvoid *pointer, GLuint arrayBufferBinding ) const
{
	const VertexAttrib *existing;
	if( findVertexAttribForLocation( index, &existing ) ) {
		return existing->mSize == size
			&& existing->mType == type
			&& existing->mNormalized == normalized
			&& existing->mStride == stride
			&& existing->mPointerType == pointerType
			&& existing->mPointer == pointer
			&& existing->mArrayBufferBinding == arrayBufferBinding;
	}
	else
		return false;
}

void Vao::Layout::vertexAttribDivisor( GLuint index, GLuint divisor )
{
	VertexAttrib *existing;
	if( findVertexAttribForLocation( index, &existing ) ) {
		existing->mDivisor = divisor;
	}
	else {
		mVertexAttribs.emplace_back( index, Vao::VertexAttrib() );
		mVertexAttribs.back().second.mDivisor = divisor;
	}
}

void Vao::Layout::clear()
{
	mElementArrayBufferBinding = 0;
	mVertexAttribs.clear();
}

std::ostream& operator<<( std::ostream &os, const Vao &rhs )
{
	os << "ID: " << rhs.getId() << std::endl;
	if( ! rhs.mLabel.empty() )
		os << "  Label: " << rhs.mLabel << std::endl;
	os << "  " << rhs.getLayout();
	
	return os;
}

std::ostream& operator<<( std::ostream &os, const Vao::Layout &rhs )
{
	os << "Cached ARRAY_BUFFER binding: " << rhs.mCachedArrayBufferBinding << "  ELEMENT_ARRAY_BUFFER_BINDING: " << rhs.mElementArrayBufferBinding << std::endl;
	os << "{" << std::endl;
	for( auto &attrib : rhs.mVertexAttribs ) {
		os << "  Loc: " << attrib.first << std::endl;
		os << "        Enabled: " << ( attrib.second.mEnabled ? "TRUE" : "FALSE" ) << std::endl;
		os << "           Size: " << attrib.second.mSize << std::endl;
		os << "           Type: " << gl::constantToString( attrib.second.mType ) << "(" << attrib.second.mType << ")" << std::endl;
		os << "     Normalized: " << ( attrib.second.mNormalized ? "TRUE" : "FALSE" ) << std::endl;
		os << "         Stride: " << attrib.second.mStride << std::endl;
		os << "        Pointer: " << attrib.second.mPointer << "(" << (size_t)attrib.second.mPointer << ")" << std::endl;
		os << "   Array Buffer: " << attrib.second.mArrayBufferBinding << std::endl;
		os << "        Divisor: " << attrib.second.mDivisor << std::endl;
	}
	os << "}";

	return os;
}

} }
