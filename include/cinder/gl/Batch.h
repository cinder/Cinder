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

#include "cinder/gl/gl.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/GeomIo.h"

#include <map>
#include <string>

namespace cinder { namespace gl {

typedef std::shared_ptr<class Batch>		BatchRef;
typedef std::shared_ptr<class VertBatch>	VertBatchRef;

class Batch {
  public:
	//! Maps a geom::Attrib to a named attribute in the GlslProg
	typedef std::map<geom::Attrib,std::string> AttributeMapping;

	//! Builds a Batch from a VboMesh and a GlslProg. Attributes defined in \a attributeMapping override the default mapping between AttributeSemantics and GlslProg attribute names
	static BatchRef		create( const VboMeshRef &vboMesh, const gl::GlslProgRef &glsl, const AttributeMapping &attributeMapping = AttributeMapping() );
	//! Builds a Batch from a geom::Source and a GlslProg. Attributes defined in \a attributeMapping override the default mapping
	static BatchRef		create( const geom::Source &source, const gl::GlslProgRef &glsl, const AttributeMapping &attributeMapping = AttributeMapping() );
	
	void			draw();
#if (! defined( CINDER_GL_ES_2 )) || defined( CINDER_COCOA_TOUCH )
	void			drawInstanced( GLsizei instanceCount );
#endif
	void			bind();

	//! Returns OpenGL primitive type (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc)
	GLenum			getPrimitive() const { return mVboMesh->getGlPrimitive(); }
	//! Returns the total number of vertices in the associated geometry
	size_t			getNumVertices() const { return mVboMesh->getNumVertices(); }
	//! Returns the number of element indices in the associated geometry; 0 for non-indexed geometry
	size_t			getNumIndices() const { return mVboMesh->getNumIndices(); }
	//! Returns the data type for indices; GL_UNSIGNED_INT or GL_UNSIGNED_SHORT
	GLenum			getIndexDataType() const { return mVboMesh->getIndexDataType(); }
	//! Returns the shader associated with the Batch
	const GlslProgRef&	getGlslProg() const	{ return mGlsl; }
	//! Replaces the shader associated with the Batch. Issues a warning if not all attributes were able to match.
	void			replaceGlslProg( const GlslProgRef& glsl );
	//! Returns the VAO mapping the Batch's geometry to its shader
	const VaoRef&	getVao() const { return mVao; }
	//! Returns the VboMesh associated with the Batch
	VboMeshRef		getVboMesh() const { return mVboMesh; }
	//! Replaces the VboMesh associated with the Batch. Issues a warning if not all attributes were able to match.
	void			replaceVboMesh( const VboMeshRef &vboMesh );

	//! Changes the GL context the Batch is associated with
	void			reassignContext( Context *context );

  protected:
	Batch( const VboMeshRef &vboMesh, const gl::GlslProgRef &glsl, const AttributeMapping &attributeMapping );
	Batch( const geom::Source &source, const gl::GlslProgRef &glsl, const AttributeMapping &attributeMapping );

	void	init( const geom::Source &source, const gl::GlslProgRef &glsl );
	void	initVao( const AttributeMapping &attributeMapping = AttributeMapping() );
		
	VboMeshRef				mVboMesh;
	VaoRef					mVao;
	GlslProgRef				mGlsl;
	AttributeMapping		mAttribMapping;
	
	friend class BatchGeomTarget;
};

//! Cannot be shared across contexts
class VertBatch {
  public:
	//! If \a useContextDefaultBuffers is \c true, uses default buffers for the context, saving allocations; suitable for single draw.
	VertBatch( GLenum primType = GL_POINTS, bool useContextDefaultBuffers = true );
	//! If \a useContextDefaultBuffers is \c false, allocates and uses internal buffers and VAO; suitable for multiple draws.
	static VertBatchRef create( GLenum primType = GL_POINTS, bool useContextDefaultBuffers = false );

	void	setType( GLenum type );
	GLenum	getType() const { return mPrimType; }
	
	void	color( float r, float g, float b, float a = 1.0f ) { color( ColorAf( r, g, b, a ) ); }
	void	color( const Colorf &color );
	void	color( const ColorAf &color );

	void	normal( float x, float y, float z ) { normal( vec3( x, y, z ) ); }
	void	normal( const vec3 &n );

	void	vertex( float x, float y, float z = 0, float w = 1 ) { addVertex( vec4( x, y, z, w ) ); }
	void	vertex( const vec2 &v ) { addVertex( vec4( v.x, v.y, 0, 1 ) ); }
	void	vertex( const vec3 &v ) { addVertex( vec4( v.x, v.y, v.z, 1 ) ); }
	void	vertex( const vec4 &v );
	//! Sets the vertex and the color simultaneously. Should not be called if you have called color() prior.
	void	vertex( const vec4 &v, const ColorAf &c );

	void	texCoord( float s, float t, float r = 0, float q = 1 ) { texCoord( vec4( s, t, r, q ) ); }	
	void	texCoord( const vec2 &t ) { texCoord( vec4( t.x, t.y, 0, 1 ) ); }
	void	texCoord( const vec3 &t ) { texCoord( vec4( t.x, t.y, t.z, 1 ) ); }
	void	texCoord( const vec4 &t );
	
	void	begin( GLenum type );
	void	end();
	void	clear();

	bool	empty() const { return mVertices.empty(); }
	
	void	draw();
	
  protected:
	void	addVertex( const vec4 &v );
	void	setupBuffers();

	GLenum					mPrimType;

	std::vector<vec4>		mVertices;
	
	std::vector<vec3>		mNormals;
	std::vector<ColorAf>	mColors;
	std::vector<vec4>		mTexCoords;
	
	bool					mOwnsBuffers;
	VaoRef					mVao;
	VboRef					mVbo;
};

} } // namespace cinder::gl