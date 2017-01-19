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

#include "cinder/gl/platform.h"
#include "cinder/gl/BufferObj.h"

#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <iosfwd>

namespace cinder { namespace gl {

class Context;
typedef std::shared_ptr<class Vao> VaoRef;

class CI_API Vao : public std::enable_shared_from_this<Vao> {
  public:
	struct Layout;
	
	static VaoRef		create();
	virtual ~Vao();
	
	void	bind();
	void	unbind() const;

	//! Changes the GL context the VAO is associated with
	virtual void	reassignContext( Context *context ) = 0;

	GLuint			getId() const { return mId; }
	const Layout&	getLayout() const { return mLayout; }

	//! An efficiency to prevent creating new VAOs. Call this, then operate as if a new VAO has been bound, and then before drawing against the VAO, call replacementBindEnd().
	void	replacementBindBegin();
	//! Assumes replacementBindBegin() was called previously and that \a this is the currently bound VAO.
	void	replacementBindEnd();

	//! Returns the debugging label associated with the VAO.
	const std::string&	getLabel() const { return mLabel; }
	//! Sets the debugging label associated with the VAO. Calls glObjectLabel() when available.
	void				setLabel( const std::string &label );

	struct VertexAttrib {
		typedef enum { FLOAT, INTEGER } PointerType;
	
		VertexAttrib()
			: mEnabled( false ), mSize( 0 ), mType( GL_FLOAT ), mNormalized( false ), mStride( 0 ), mPointer( 0 ), mArrayBufferBinding( 0 ), mDivisor( 0 ), mPointerType( FLOAT )
		{}
		
		VertexAttrib( GLint size, GLenum type, GLboolean normalized, GLsizei stride, PointerType pointerType, const GLvoid* pointer, GLuint arrayBufferBinding, GLuint divisor = 0 )
			: mEnabled( false ), mSize( size ), mType( type ), mNormalized( normalized ), mStride( stride ), mPointerType( pointerType ), mPointer( pointer ), mArrayBufferBinding( arrayBufferBinding ), mDivisor( divisor )
		{}
		
		void		setDivisor( GLuint divisor ) { mDivisor = divisor; }
		
		bool			mEnabled;
		GLint			mSize;
		GLenum			mType;
		GLboolean		mNormalized;
		GLsizei			mStride;
		PointerType		mPointerType;
		const GLvoid*	mPointer;
		GLuint			mArrayBufferBinding;
		GLuint			mDivisor;
	};

	//! Represent a software-only mirror of the state a VAO records. Can be used directly for efficient swapping (primarily by the gl:: convenience functions)
	struct Layout {
		Layout();
		
		//! The equivalent of glBindBuffer( \a target, \a binding )
		void	bindBuffer( GLenum target, GLuint buffer );
		//! Returns whether the vertex attribute array at \a index is enabled or not
		bool	isVertexAttribArrayEnabled( GLuint index ) const;
		//! The equivalent of glEnableVertexAttribArray( \a index )
		void	enableVertexAttribArray( GLuint index );
		//! The equivalent of glDisableVertexAttribArray( \a index )
		void	disableVertexAttribArray( GLuint index );
		//! Sets the vertex attribute data
		void	vertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer );
		//! Sets the integer vertex attribute data
		void	vertexAttribIPointer( GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer );
		//! Returns whether the existing attribPointer information at \a index is equal to the supplied params
		bool	isVertexAttribEqual( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, VertexAttrib::PointerType pointerType, const GLvoid *pointer, GLuint arrayBufferBinding ) const;
		//! Sets the instancing divisor
		void	vertexAttribDivisor( GLuint index, GLuint divisor );

		//! Sets to the equivalent of a newly bound VAO (which means it does not overwrite the mCachedArrayBufferBinding value)
		void	clear();

		//! Returns \c true if found an attribute at \a loc, and sets \a result to point to the relevant VertexAttrib in \a mVertexAttribs
		bool	findVertexAttribForLocation( GLuint loc, VertexAttrib **result );
		//! Returns \c true if found an attribute at \a loc, and sets \a result to point to the relevant VertexAttrib in \a mVertexAttribs
		bool	findVertexAttribForLocation( GLuint loc, const VertexAttrib **result ) const;

		const std::vector<std::pair<GLuint,VertexAttrib>>&	getVertexAttribs() const { return mVertexAttribs; }

		GLuint									mElementArrayBufferBinding;
		GLuint									mCachedArrayBufferBinding; // this represent a cache of the Context's value, but VAOs do not record GL_ARRAY_BUFFER_BINDING
		// <id,VertexAttrib>
		std::vector<std::pair<GLuint,VertexAttrib>>	mVertexAttribs;

		friend class Vao;
	};
	
  protected:
	Vao();

	//! only necessary when VAO is created without
	void setContext( Context *context );

	// Does the actual work of binding the VAO; called by Context
	virtual void	bindImpl( class Context *context ) = 0;
	// Does the actual work of unbinding the VAO; called by Context
	virtual void	unbindImpl( class Context *context ) = 0;
	// Analogous to glEnableVertexAttribArray(). Expects this to be the currently bound VAO; called by Context
	virtual void	enableVertexAttribArrayImpl( GLuint index ) = 0;
	// Analogous to glDisableVertexAttribArray(). Expects this to be the currently bound VAO; called by Context
	virtual void	disableVertexAttribArrayImpl( GLuint index ) = 0;
	// Analogous to glVertexAttribPointer(). Expects this to be the currently bound VAO; called by Context
	virtual void	vertexAttribPointerImpl( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer ) = 0;
	// Analogous to glVertexAttribIPointer(). Expects this to be the currently bound VAO; called by Context
	virtual void	vertexAttribIPointerImpl( GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer ) = 0;
	// Analogous to glVertexAttribDivisor(). Expects this to be the currently bound VAO; called by Context
	virtual void	vertexAttribDivisorImpl( GLuint index, GLuint divisor ) = 0;
	// Caches the currently bound buffer; called by Context when GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER changes
	virtual void	reflectBindBufferImpl( GLenum target, GLuint buffer ) = 0;

	GLuint							mId;
	Context							*mCtx;
	Layout							mLayout, mReplacementBindPrevious;
	std::string						mLabel;

	friend Context;
	friend CI_API std::ostream& operator<<( std::ostream &lhs, const VaoRef &rhs );
	friend CI_API std::ostream& operator<<( std::ostream &lhs, const Vao &rhs );
};

// Convenience method for dumping VAO contents to a std::ostream
CI_API std::ostream& operator<<( std::ostream &lhs, const Vao &rhs );
// Convenience method for dumping Vao::Layout contents to a std::ostream
CI_API std::ostream& operator<<( std::ostream &lhs, const Vao::Layout &rhs );
	
} }
