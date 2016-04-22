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

#include "cinder/gl/Vbo.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/GeomIo.h"

#include <map>
#include <string>

namespace cinder { namespace gl {

class Batch;
typedef std::shared_ptr<Batch>		BatchRef;
class VertBatch;
typedef std::shared_ptr<VertBatch>	VertBatchRef;

class Batch {
  public:
	//! Maps a geom::Attrib to a named attribute in the GlslProg
	typedef std::map<geom::Attrib,std::string> AttributeMapping;

	//! Builds a Batch from a VboMesh and a GlslProg. Attributes defined in \a attributeMapping override the default mapping between AttributeSemantics and GlslProg attribute names
	static BatchRef		create( const VboMeshRef &vboMesh, const gl::GlslProgRef &glsl, const AttributeMapping &attributeMapping = AttributeMapping() );
	//! Builds a Batch from a geom::Source and a GlslProg. Attributes defined in \a attributeMapping override the default mapping
	static BatchRef		create( const geom::Source &source, const gl::GlslProgRef &glsl, const AttributeMapping &attributeMapping = AttributeMapping() );
	
	//! Draws the Batch. Optionally specify a \a first vertex/element and a \a count. Otherwise the entire geometry will be drawn.
	void			draw( GLint first = 0, GLsizei count = -1 );
#if defined( CINDER_GL_HAS_DRAW_INSTANCED )
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
	const VaoRef	getVao() const { return mVao; }
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

//! Allows simple creation of basic geometry in a style similar to OpenGL immediate mode.
//! Can be used to construct a gl::Batch for better performance, or can draw() directly.
//! Used with a series of calls to color(), normal(), and/or texCoord(), followed by a call to vertex(), per-vertex.
class VertBatch : public geom::Source {
  public:
	//! Constructs a VertBatch with primitive type \a primType. Default is \c GL_POINTS. If \a useContextDefaultBuffers is \c true, uses default buffers for the context, saving allocations; suitable for single draw.
	VertBatch( GLenum primType = GL_POINTS, bool useContextDefaultBuffers = true );
	//! Creates a VertBatchRef with primitive type \a primType. Default is \c GL_POINTS. If \a useContextDefaultBuffers is \c false, allocates and uses internal buffers and VAO; suitable for multiple draws.
	static VertBatchRef create( GLenum primType = GL_POINTS, bool useContextDefaultBuffers = false );

	//! Sets the primitive type of the VertBatch. Default is \c GL_POINTS.
	void	setType( GLenum type );
	//! Returns the primitive type of the VertBatch. Default is \c GL_POINTS.
	GLenum	getType() const { return mPrimType; }

	//! Sets the color for the current vertex (attribute geom::COLOR)
	void	color( float r, float g, float b, float a = 1.0f ) { color( ColorAf( r, g, b, a ) ); }
	//! Sets the color for the current vertex (attribute geom::COLOR)
	void	color( const Colorf &color );
	//! Sets the color for the current vertex (attribute geom::COLOR)
	void	color( const ColorAf &color );

	//! Sets the normal for the current vertex (attribute geom::NORMAL)
	void	normal( float x, float y, float z ) { normal( vec3( x, y, z ) ); }
	//! Sets the normal for the current vertex (attribute geom::NORMAL)
	void	normal( const vec3 &n );

	//! Sets the texCoord0 for the current vertex (attribute geom::TEX_COORD_0)
	void	texCoord( float s, float t, float r = 0, float q = 1 ) { texCoord0( vec4( s, t, r, q ) ); }
	//! Sets the texCoord0 for the current vertex (attribute geom::TEX_COORD_0)
	void	texCoord( const vec2 &t ) { texCoord0( vec4( t.x, t.y, 0, 1 ) ); }
	//! Sets the texCoord0 for the current vertex (attribute geom::TEX_COORD_0)
	void	texCoord( const vec3 &t ) { texCoord0( vec4( t.x, t.y, t.z, 1 ) ); }
	//! Sets the texCoord0 for the current vertex (attribute geom::TEX_COORD_0)
	void	texCoord( const vec4 &t ) { texCoord0( t ); }

	//! Sets the texCoord0 for the current vertex (attribute geom::TEX_COORD_0)
	void	texCoord0( float s, float t, float r = 0, float q = 1 ) { texCoord0( vec4( s, t, r, q ) ); }
	//! Sets the texCoord0 for the current vertex (attribute geom::TEX_COORD_0)
	void	texCoord0( const vec2 &t ) { texCoord0( vec4( t.x, t.y, 0, 1 ) ); }
	//! Sets the texCoord0 for the current vertex (attribute geom::TEX_COORD_0)
	void	texCoord0( const vec3 &t ) { texCoord0( vec4( t.x, t.y, t.z, 1 ) ); }
	//! Sets the texCoord0 for the current vertex (attribute geom::TEX_COORD_0)
	void	texCoord0( const vec4 &t );

	//! Sets the texCoord1 for the current vertex (attribute geom::TEX_COORD_1)
	void	texCoord1( float s, float t, float r = 0, float q = 1 ) { texCoord1( vec4( s, t, r, q ) ); }
	//! Sets the texCoord1 for the current vertex (attribute geom::TEX_COORD_1)
	void	texCoord1( const vec2 &t ) { texCoord1( vec4( t.x, t.y, 0, 1 ) ); }
	//! Sets the texCoord1 for the current vertex (attribute geom::TEX_COORD_1)
	void	texCoord1( const vec3 &t ) { texCoord1( vec4( t.x, t.y, t.z, 1 ) ); }
	//! Sets the texCoord1 for the current vertex (attribute geom::TEX_COORD_1)
	void	texCoord1( const vec4 &t );

	//! Locks the values for the current vertex and sets its position (attribute geom::POSITION). Call after any corresponding calls to color(), normal() and/or texCoord().
	void	vertex( float x, float y, float z = 0, float w = 1 ) { addVertex( vec4( x, y, z, w ) ); }
	//! Locks the values for the current vertex and sets its position (attribute geom::POSITION). Call after any corresponding calls to color(), normal() and/or texCoord().
	void	vertex( const vec2 &v ) { addVertex( vec4( v.x, v.y, 0, 1 ) ); }
	//! Locks the values for the current vertex and sets its position (attribute geom::POSITION). Call after any corresponding calls to color(), normal() and/or texCoord().
	void	vertex( const vec3 &v ) { addVertex( vec4( v.x, v.y, v.z, 1 ) ); }
	//! Locks the values for the current vertex and sets its position (attribute geom::POSITION). Call after any corresponding calls to color(), normal() and/or texCoord().
	void	vertex( const vec4 &v );
	//! Locks the values for the current vertex and sets its position (attribute geom::POSITION) and color (geom::COLOR). Call after any corresponding calls to color(), normal() and/or texCoord(). Should not be called if you have called color() prior.
	void	vertex( const vec4 &v, const ColorAf &c );

	//! Resets the sequence of vertices and sets the VertBatch's primitive type.
	void	begin( GLenum type );
	//! No-op. Present for parity with legacy immediate mode.
	void	end() {}
	
	//! Clears all vertices recorded by the VertBatch
	void	clear();

	//! Returns \c true if no vertices have been added to the VertBatch.
	bool	empty() const { return mVertices.empty(); }
	
	//! Draws the VertBatch using the gl::Context's currently active shader. For multiple draws consider constructing a gl::Batch using the VertBatch instead.
	void	draw();
	
  protected:
	void	addVertex( const vec4 &v );
	void	setupBuffers();

	// geom::Source implementation
	size_t			getNumVertices() const override;
	size_t			getNumIndices() const override;
	geom::Primitive	getPrimitive() const override;
	uint8_t			getAttribDims( geom::Attrib attr ) const override;

	void			loadInto( geom::Target *target, const geom::AttribSet &requestedAttribs ) const override;
	VertBatch*		clone() const override { return new VertBatch( *this ); }

	geom::AttribSet	getAvailableAttribs() const override;
	

	GLenum					mPrimType;

	std::vector<vec4>		mVertices;
	
	std::vector<vec3>		mNormals;
	std::vector<ColorAf>	mColors;
	std::vector<vec4>		mTexCoords0, mTexCoords1;
	
	bool					mOwnsBuffers;
	bool					mForceUpdate;
	Vao*					mVao;
	VaoRef					mVaoStorage;
	VboRef					mVbo;
};

} } // namespace cinder::gl