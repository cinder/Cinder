/*
 Copyright (c) 2015, The Barbarian Group
 All rights reserved.
 
 Portions of this code (C) Paul Houx
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

#include <vector>
#include "cinder/Vector.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/DataSource.h"
#include "cinder/DataTarget.h"
#include "cinder/Matrix.h"
#include "cinder/Color.h"
#include "cinder/Rect.h"
#include "cinder/GeomIo.h"

namespace cinder {

typedef std::shared_ptr<class TriMesh>		TriMeshRef;
	
class CI_API TriMesh : public geom::Source {
 public:
	class CI_API Format {
	  public:
		Format();
		
		Format&		positions( uint8_t dims = 3 ) { mPositionsDims = dims; return *this; }
		Format&		normals() { mNormalsDims = 3; return *this; }
		Format&		tangents() { mTangentsDims = 3; return *this; }
		Format&		bitangents() { mBitangentsDims = 3; return *this; }

		Format&		colors( uint8_t dims = 3 ) { mColorsDims = dims; return *this; }
		//! Enables and establishes the dimensions of texture coords for unit 0
		Format&		texCoords( uint8_t dims = 2 ) { mTexCoords0Dims = dims; return *this; }
		//! Enables and establishes the dimensions of texture coords for unit 0
		Format&		texCoords0( uint8_t dims = 2 ) { mTexCoords0Dims = dims; return *this; }
		//! Enables and establishes the dimensions of texture coords for unit 1
		Format&		texCoords1( uint8_t dims = 2 ) { mTexCoords1Dims = dims; return *this; }
		//! Enables and establishes the dimensions of texture coords for unit 2
		Format&		texCoords2( uint8_t dims = 2 ) { mTexCoords2Dims = dims; return *this; }
		//! Enables and establishes the dimensions of texture coords for unit 3
		Format&		texCoords3( uint8_t dims = 2 ) { mTexCoords3Dims = dims; return *this; }
		
		uint8_t		mPositionsDims, mNormalsDims, mTangentsDims, mBitangentsDims, mColorsDims;
		uint8_t		mTexCoords0Dims, mTexCoords1Dims, mTexCoords2Dims, mTexCoords3Dims;
	};

	static TriMeshRef	create() { return TriMeshRef( new TriMesh( Format().positions().normals().texCoords() ) ); }
	static TriMeshRef	create( const Format &format ) { return TriMeshRef( new TriMesh( format ) ); }
	static TriMeshRef	create( const geom::Source &source ) { return TriMeshRef( new TriMesh( source ) ); }
	static TriMeshRef	create( const geom::Source &source, const Format &format ) { return TriMeshRef( new TriMesh( source, format ) ); }

	TriMesh( const Format &format = Format().positions().normals().texCoords() );
	TriMesh( const geom::Source &source );
	TriMesh( const geom::Source &source, const Format &format );
	
	//! Creates a suitable TriMesh::Format for representing a geom::Source \a source
	static Format		formatFromSource( const geom::Source &source );
	
	void			loadInto( geom::Target *target, const geom::AttribSet &requestedAttribs ) const override;
	geom::Source*	clone() const override { return new TriMesh( *this ); }
	
	void		clear();
	
	bool		hasNormals() const { return ! mNormals.empty(); }
	bool		hasTangents() const { return ! mTangents.empty(); }
	bool		hasBitangents() const { return ! mBitangents.empty(); }
	bool		hasColors() const { return ! mColors.empty(); }
	bool		hasColorsRgb() const { return mColorsDims == 3 && ! mColors.empty(); }
	bool		hasColorsRgba() const { return mColorsDims == 4 && ! mColors.empty(); }
	//! Returns whether the TriMesh has texture coordinates for unit 0
	bool		hasTexCoords() const { return ! mTexCoords0.empty(); }
	//! Returns whether the TriMesh has texture coordinates for unit 0
	bool		hasTexCoords0() const { return ! mTexCoords0.empty(); }
	//! Returns whether the TriMesh has texture coordinates for unit 1
	bool		hasTexCoords1() const { return ! mTexCoords1.empty(); }
	//! Returns whether the TriMesh has texture coordinates for unit 2
	bool		hasTexCoords2() const { return ! mTexCoords2.empty(); }
	//! Returns whether the TriMesh has texture coordinates for unit 3
	bool		hasTexCoords3() const { return ! mTexCoords3.empty(); }

	//! Appends a position which can be referred to with appendTriangle() or appendIndices()
	void		appendPosition( const vec2 &position ) { appendPositions( &position, 1 ); }
	//! Appends a position which can be referred to with appendTriangle() or appendIndices()
	void		appendPosition( const vec3 &position ) { appendPositions( &position, 1 ); }
	//! Appends a position which can be referred to with appendTriangle() or appendIndices()
	void		appendPosition( const vec4 &position ) { appendPositions( &position, 1 ); }
	//! Appends multiple vertices to the TriMesh which can be referred to with appendTriangle() or appendIndices() 
	void		appendPositions( const vec2 *positions, size_t num );
	//! Appends multiple vertices to the TriMesh which can be referred to with appendTriangle() or appendIndices() 
	void		appendPositions( const vec3 *positions, size_t num );
	//! Appends multiple vertices to the TriMesh which can be referred to with appendTriangle() or appendIndices() 
	void		appendPositions( const vec4 *positions, size_t num );
	//! Appends a single normal  
	void		appendNormal( const vec3 &normal ) { mNormals.push_back( normal ); }
	//! Functions similarly to appendPositions(), appending multiple normals at once.
	void		appendNormals( const vec3 *normals, size_t num );
	//! Appends a single tangent  
	void		appendTangent( const vec3 &tangent ) { mTangents.push_back( tangent ); }
	//! Functions similarly to appendPositions(), appending multiple tangents at once.
	void		appendTangents( const vec3 *tangents, size_t num );
	//! Appends a single bitangent
	void		appendBitangent( const vec3 &bitangent ) { mBitangents.push_back( bitangent ); }
	//! Functions similarly to appendPositions(), appending multiple bitangents at once.
	void		appendBitangents( const vec3 *bitangents, size_t num );
	//! Appends a single RGB color
	void		appendColorRgb( const Color &color ) { appendColors( &color, 1 ); }
	//! Appends a single RGBA color
	void		appendColorRgba( const ColorA &color ) { appendColors( &color, 1 ); }

	//! Synonym for appendTexCoord0; appends a texture coordinate for unit 0
	void		appendTexCoord( const vec2 &v ) { appendTexCoords0( &v, 1 ); }
	//!	appends a 2D texture coordinate for unit 0
	void		appendTexCoord0( const vec2 &v ) { appendTexCoords0( &v, 1 ); }
	//! appends a 2D texture coordinate for unit 1
	void		appendTexCoord1( const vec2 &v ) { appendTexCoords1( &v, 1 ); }
	//! appends a 2D texture coordinate for unit 2
	void		appendTexCoord2( const vec2 &v ) { appendTexCoords2( &v, 1 ); }
	//! appends a 2D texture coordinate for unit 3
	void		appendTexCoord3( const vec2 &v ) { appendTexCoords3( &v, 1 ); }

	//!	appends a 3D texture coordinate for unit 0
	void		appendTexCoord0( const vec3 &v ) { appendTexCoords0( &v, 1 ); }
	//! appends a 3D texture coordinate for unit 1
	void		appendTexCoord1( const vec3 &v ) { appendTexCoords1( &v, 1 ); }
	//! appends a 3D texture coordinate for unit 2
	void		appendTexCoord2( const vec3 &v ) { appendTexCoords2( &v, 1 ); }
	//! appends a 3D texture coordinate for unit 3
	void		appendTexCoord3( const vec3 &v ) { appendTexCoords3( &v, 1 ); }

	//!	appends a 4D texture coordinate for unit 0
	void		appendTexCoord0( const vec4 &v ) { appendTexCoords0( &v, 1 ); }
	//! appends a 4D texture coordinate for unit 1
	void		appendTexCoord1( const vec4 &v ) { appendTexCoords1( &v, 1 ); }
	//! appends a 4D texture coordinate for unit 2
	void		appendTexCoord2( const vec4 &v ) { appendTexCoords2( &v, 1 ); }
	//! appends a 4D texture coordinate for unit 3
	void		appendTexCoord3( const vec4 &v ) { appendTexCoords3( &v, 1 ); }
	
	//! Appends multiple RGB colors to the TriMesh
	void		appendColors( const Color *rgbs, size_t num );
	//! Appends multiple RGBA colors to the TriMesh
	void		appendColors( const ColorA *rgbas, size_t num );
	
	//! Appends multiple 2D texcoords for unit 0
	void		appendTexCoords0( const vec2 *texCoords, size_t num );
	//! Appends multiple 2D texcoords for unit 1
	void		appendTexCoords1( const vec2 *texCoords, size_t num );
	//! Appends multiple 2D texcoords for unit 2
	void		appendTexCoords2( const vec2 *texCoords, size_t num );
	//! Appends multiple 2D texcoords for unit 3
	void		appendTexCoords3( const vec2 *texCoords, size_t num );
	
	//! Appends multiple 3D texcoords for unit 0
	void		appendTexCoords0( const vec3 *texCoords, size_t num );
	//! Appends multiple 3D texcoords for unit 1
	void		appendTexCoords1( const vec3 *texCoords, size_t num );
	//! Appends multiple 3D texcoords for unit 2
	void		appendTexCoords2( const vec3 *texCoords, size_t num );
	//! Appends multiple 3D texcoords for unit 3
	void		appendTexCoords3( const vec3 *texCoords, size_t num );

	//! Appends multiple 4D texcoords for unit 0
	void		appendTexCoords0( const vec4 *texCoords, size_t num );
	//! Appends multiple 4D texcoords for unit 1
	void		appendTexCoords1( const vec4 *texCoords, size_t num );
	//! Appends multiple 4D texcoords for unit 2
	void		appendTexCoords2( const vec4 *texCoords, size_t num );
	//! Appends multiple 4D texcoords for unit 3
	void		appendTexCoords3( const vec4 *texCoords, size_t num );
	
	//! Appends a single triange whose 3 vertices are at indices \a v0, \a v1 and \a v2
	void		appendTriangle( uint32_t v0, uint32_t v1, uint32_t v2 )
	{ mIndices.push_back( v0 ); mIndices.push_back( v1 ); mIndices.push_back( v2 ); }
	//! Appends \a num indices to the TriMesh pointed to by \a indices
	void		appendIndices( const uint32_t *indices, size_t num );

	//! Returns the total number of indices in the TriMesh.
	size_t		getNumIndices() const override { return mIndices.size(); }
	//! Returns the total number of triangles contained by the TriMesh.
	size_t		getNumTriangles() const { return mIndices.size() / 3; }
	//! Returns the total number of indices contained by the TriMesh.
	size_t		getNumVertices() const override { if( mPositionsDims ) return mPositions.size() / mPositionsDims; else return 0; }

	//! Copies the 3 vertices of triangle number \a idx into \a a, \a b and \a c. Assumes vertices are 3D
	void		getTriangleVertices( size_t idx, vec3 *a, vec3 *b, vec3 *c ) const;
	//! Copies the 3 vertices of triangle number \a idx into \a a, \a b and \a c. Assumes vertices are 2D
	void		getTriangleVertices( size_t idx, vec2 *a, vec2 *b, vec2 *c ) const;
	//! Copies the 3 normals of triangle number \a idx into \a a, \a b and \a c.
	void		getTriangleNormals( size_t idx, vec3 *a, vec3 *b, vec3 *c ) const;
	//! Copies the 3 tangents of triangle number \a idx into \a a, \a b and \a c.
	void		getTriangleTangents( size_t idx, vec3 *a, vec3 *b, vec3 *c ) const;
	//! Copies the 3 bitangents of triangle number \a idx into \a a, \a b and \a c.
	void		getTriangleBitangents( size_t idx, vec3 *a, vec3 *b, vec3 *c ) const;


	//! Returns a pointer to the positions of the mesh as vec<DIM>*. For example, for a TriMesh with 3D vertices, call getPositions<3>().
	template<uint8_t DIM>
	const typename VECDIM<DIM,float>::TYPE*	getPositions() const { assert(mPositionsDims==DIM); return (typename VECDIM<DIM,float>::TYPE*)mPositions.data(); }
	//! Returns a pointer to the positions of theTriMesh as vec<DIM>*. For example, for a TriMesh with 3D vertices, call getPositions<3>().
	template<uint8_t DIM>
	typename VECDIM<DIM,float>::TYPE*		getPositions() { assert(mPositionsDims==DIM); return (typename VECDIM<DIM,float>::TYPE*)mPositions.data(); }
	//! Returns a reference to the std::vector<vec3> for the TriMesh's normals.
	std::vector<vec3>&				getNormals() { return mNormals; }
	//! Returns a reference to the std::vector<vec3> for the TriMesh's normals.
	const std::vector<vec3>&		getNormals() const { return mNormals; }
	//! Returns a reference to the std::vector<vec3> for the TriMesh's tangents.
	std::vector<vec3>&				getTangents() { return mTangents; }
	//! Returns a reference to the std::vector<vec3> for the TriMesh's tangents.
	const std::vector<vec3>&		getTangents() const { return mTangents; }
	//! Returns a reference to the std::vector<vec3> for the TriMesh's bitangents.
	std::vector<vec3>&				getBitangents() { return mBitangents; }
	//! Returns a reference to the std::vector<vec3> for the TriMesh's bitangents.
	const std::vector<vec3>&		getBitangents() const { return mBitangents; }
	//! Returns a pointer to the colors of the TriMesh vec<DIM>*. For example, to get RGB colors, call getColors<3>().
	template<uint8_t DIM>
	typename COLORDIM<DIM,float>::TYPE*			getColors() { assert(mColorsDims==DIM); return (typename COLORDIM<DIM,float>::TYPE*)mColors.data(); }
	//! Returns a pointer to the colors of the TriMesh vec<DIM>*. For example, to get RGB colors, call getColors<3>().
	template<uint8_t DIM>
	const typename COLORDIM<DIM,float>::TYPE*	getColors() const { assert(mColorsDims==DIM); return (typename COLORDIM<DIM,float>::TYPE*)mColors.data(); }
	//! Returns a pointer to the TexCoord0 values of the TriMesh vec<DIM>*. For example, if the TriMesh has 2D TexCoord0 values, use getTexCoords0<2>().
	template<uint8_t DIM>
	typename VECDIM<DIM,float>::TYPE*		getTexCoords0() { assert(mTexCoords0Dims==DIM); return (typename VECDIM<DIM,float>::TYPE*)mTexCoords0.data(); }
	//! Returns a pointer to the TexCoord0 values of the TriMesh vec<DIM>*. For example, if the TriMesh has 2D TexCoord0 values, use getTexCoords0<2>().
	template<uint8_t DIM>
	const typename VECDIM<DIM,float>::TYPE*	getTexCoords0() const { assert(mTexCoords0Dims==DIM); return (typename VECDIM<DIM,float>::TYPE*)mTexCoords0.data(); }
	//! Returns a pointer to the TexCoord1 values of the TriMesh vec<DIM>*. For example, if the TriMesh has 2D TexCoord1 values, use getTexCoords1<2>().
	template<uint8_t DIM>
	typename VECDIM<DIM,float>::TYPE*		getTexCoords1() { assert(mTexCoords1Dims==DIM); return (typename VECDIM<DIM,float>::TYPE*)mTexCoords1.data(); }
	//! Returns a pointer to the TexCoord1 values of the TriMesh vec<DIM>*. For example, if the TriMesh has 2D TexCoord1 values, use getTexCoords1<2>().
	template<uint8_t DIM>
	const typename VECDIM<DIM,float>::TYPE*	getTexCoords1() const { assert(mTexCoords1Dims==DIM); return (typename VECDIM<DIM,float>::TYPE*)mTexCoords1.data(); }
	//! Returns a pointer to the TexCoord2 values of the TriMesh vec<DIM>*. For example, if the TriMesh has 2D TexCoord2 values, use getTexCoords2<2>().
	template<uint8_t DIM>
	typename VECDIM<DIM,float>::TYPE*		getTexCoords2() { assert(mTexCoords2Dims==DIM); return (typename VECDIM<DIM,float>::TYPE*)mTexCoords2.data(); }
	//! Returns a pointer to the TexCoord2 values of the TriMesh vec<DIM>*. For example, if the TriMesh has 2D TexCoord2 values, use getTexCoords2<2>().
	template<uint8_t DIM>
	const typename VECDIM<DIM,float>::TYPE*	getTexCoords2() const { assert(mTexCoords2Dims==DIM); return (typename VECDIM<DIM,float>::TYPE*)mTexCoords2.data(); }
	//! Returns a pointer to the TexCoord3 values of the TriMesh vec<DIM>*. For example, if the TriMesh has 2D TexCoord3 values, use getTexCoords3<2>().
	template<uint8_t DIM>
	typename VECDIM<DIM,float>::TYPE*		getTexCoords3() { assert(mTexCoords3Dims==DIM); return (typename VECDIM<DIM,float>::TYPE*)mTexCoords3.data(); }
	//! Returns a pointer to the TexCoord3 values of the TriMesh vec<DIM>*. For example, if the TriMesh has 2D TexCoord3 values, use getTexCoords3<2>().
	template<uint8_t DIM>
	const typename VECDIM<DIM,float>::TYPE*	getTexCoords3() const { assert(mTexCoords3Dims==DIM); return (typename VECDIM<DIM,float>::TYPE*)mTexCoords3.data(); }
	//! Trimesh indices are ordered such that the indices of triangle T are { indices[T*3+0], indices[T*3+1], indices[T*3+2] }
	std::vector<uint32_t>&			getIndices() { return mIndices; }
	//! Trimesh indices are ordered such that the indices of triangle T are { indices[T*3+0], indices[T*3+1], indices[T*3+2] }
	const std::vector<uint32_t>&	getIndices() const { return mIndices; }

	//! Returns a const reference to the position buffer.
	const std::vector<float>& getBufferPositions() const { return mPositions; }
	//! Returns a reference to the position buffer.
	std::vector<float>& getBufferPositions() { return mPositions; }
	//! Returns a const reference to the colors buffer.
	const std::vector<float>& getBufferColors() const { return mColors; }
	//! Returns a reference to the colors buffer.
	std::vector<float>& getBufferColors() { return mColors; }
	//! Returns a const reference to the texCoords0 buffer.
	const std::vector<float>& getBufferTexCoords0() const { return mTexCoords0; }
	//! Returns a reference to the texCoords0 buffer.
	std::vector<float>& getBufferTexCoords0() { return mTexCoords0; }
	//! Returns a const reference to the texCoords1 buffer.
	const std::vector<float>& getBufferTexCoords1() const { return mTexCoords1; }
	//! Returns a reference to the texCoords1 buffer.
	std::vector<float>& getBufferTexCoords1() { return mTexCoords1; }
	//! Returns a const reference to the texCoords2 buffer.
	const std::vector<float>& getBufferTexCoords2() const { return mTexCoords2; }
	//! Returns a reference to the texCoords2 buffer.
	std::vector<float>& getBufferTexCoords2() { return mTexCoords2; }
	//! Returns a const reference to the texCoords3 buffer.
	const std::vector<float>& getBufferTexCoords3() const { return mTexCoords3; }
	//! Returns a reference to the texCoords3 buffer.
	std::vector<float>& getBufferTexCoords3() { return mTexCoords3; }

	//! Calculates the bounding box of all vertices. Fails if the positions are not 3D.
	AxisAlignedBox	calcBoundingBox() const;
	//! Calculates the bounding box of all vertices as transformed by \a transform. Fails if the positions are not 3D.
	AxisAlignedBox	calcBoundingBox( const mat4 &transform ) const;

	//! Fills this TriMesh with the data from a binary file, which was created with TriMesh::write().
	void		read( const DataSourceRef &dataSource );
	//! Writes this TriMesh out to a binary data file.
	void		write( const DataTargetRef &dataTarget ) const { write( dataTarget, ~0u ); }
	//! Writes this TriMesh out to a binary data file. If \a writeNormals or \a writeTangents is \c true, normals and/or tangents are written to the file.
	void		write( const DataTargetRef &dataTarget, bool writeNormals, bool writeTangents ) const;
	//! Writes this TriMesh out to a binary data file. You can specify which attributes to write by supplying a list of \a attribs.
	void		write( const DataTargetRef &dataTarget, const std::set<geom::Attrib> &attribs ) const;

	/*! Adds or replaces normals by calculating them from the vertices and faces. If \a smooth is TRUE,
		similar vertices are grouped together to calculate their average. This will not change the mesh,
		nor will it affect texture mapping. If \a weighted is TRUE, larger polygons contribute more to
		the calculated normal. Renormalization requires 3D vertices. */
	bool		recalculateNormals( bool smooth = false, bool weighted = false );
	//! Adds or replaces tangents by calculating them from the normals and texture coordinates. Requires 3D normals and 2D texture coordinates.
	bool		recalculateTangents();
	//! Adds or replaces bitangents by calculating them from the normals and tangents. Requires 3D normals and tangents.
	bool		recalculateBitangents();

	/*! Subdivide each triangle of the TriMesh into \a division times division triangles. Division less than 2 leaves the mesh unaltered.
		Optionally, vertices are normalized if \a normalize is TRUE. */
	void		subdivide( int division = 2, bool normalize = false );

	//! Create TriMesh from vectors of vertex data.
/*	static TriMesh		create( std::vector<uint32_t> &indices, const std::vector<ColorAf> &colors,
							   const std::vector<vec3> &normals, const std::vector<vec3> &positions,
							   const std::vector<vec2> &texCoords );*/

	// geom::Source virtuals
	geom::Primitive		getPrimitive() const override { return geom::Primitive::TRIANGLES; }
	uint8_t				getAttribDims( geom::Attrib attr ) const override;
	geom::AttribSet		getAvailableAttribs() const override;

  protected:
	void		initFromFormat( const TriMesh::Format &format );
	void		loadFromSource( const geom::Source &source );

	void		getAttribPointer( geom::Attrib attr, const float **resultPtr, size_t *resultStrideBytes, uint8_t *resultDims ) const;
	void		copyAttrib( geom::Attrib attr, uint8_t dims, size_t stride, const float *srcData, size_t count );

	//! Returns whether or not the vertex, color etc. at both indices is the same.
	bool		verticesEqual( uint32_t indexA, uint32_t indexB ) const;

	void		readImplV2( const IStreamRef &in );
	void		readImplV1( const IStreamRef &in );

	/*! Writes this TriMesh out to a binary data file. The \a writeMask parameter can be used to specify
	 * what data should be included (e.g. toMask(POSITION) | toMask(COLOR) )
	 * or what should be excluded (e.g. ~toMask( NORMAL ) & ~toMask( TEX_COORD_0) ). */
	void		write( const DataTargetRef &dataTarget, uint32_t writeMask ) const;

	//! Converts a geom::Attrib to an attribute bitmask.
	static uint32_t	toMask( geom::Attrib attrib );
	//! Converts an attribute bitmask to a geom::Attrib.
	static geom::Attrib	fromMask( uint32_t attrib );

	uint8_t		mPositionsDims, mNormalsDims, mTangentsDims, mBitangentsDims, mColorsDims;
	uint8_t		mTexCoords0Dims, mTexCoords1Dims, mTexCoords2Dims, mTexCoords3Dims;
  
	std::vector<float>		mPositions;
	std::vector<float>		mColors;
	std::vector<vec3>		mNormals; // always dim=3
	std::vector<vec3>		mTangents; // always dim=3
	std::vector<vec3>		mBitangents; // always dim=3
	std::vector<float>		mTexCoords0, mTexCoords1, mTexCoords2, mTexCoords3;
	std::vector<uint32_t>	mIndices;
	
	friend class TriMeshGeomTarget;
};

} // namespace cinder
