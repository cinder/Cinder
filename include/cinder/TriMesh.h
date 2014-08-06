/*
 Copyright (c) 2010, The Barbarian Group
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
	
class TriMesh : public geom::Source {
 public:
	class Format {
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

	TriMesh( const Format &format );
	TriMesh( const geom::Source &source );
	
	virtual void	loadInto( geom::Target *target ) const override;
	
	void		clear();
	
	bool		hasNormals() const { return !mNormals.empty(); }
	bool		hasTangents() const { return !mTangents.empty(); }
	bool		hasBitangents() const { return !mBitangents.empty(); }
	bool		hasColors() const { return !mColors.empty(); }
	bool		hasColorsRgb() const { return mColorsDims == 3 && !mColors.empty(); }
	bool		hasColorsRgba() const { return mColorsDims == 4 && !mColors.empty(); }
	//! Returns whether the TriMesh has texture coordinates for unit 0
	bool		hasTexCoords() const { return !mTexCoords0.empty(); }
	//! Returns whether the TriMesh has texture coordinates for unit 0
	bool		hasTexCoords0() const { return !mTexCoords0.empty(); }
	//! Returns whether the TriMesh has texture coordinates for unit 1
	bool		hasTexCoords1() const { return !mTexCoords1.empty(); }
	//! Returns whether the TriMesh has texture coordinates for unit 2
	bool		hasTexCoords2() const { return !mTexCoords2.empty(); }
	//! Returns whether the TriMesh has texture coordinates for unit 3
	bool		hasTexCoords3() const { return !mTexCoords3.empty(); }

	//! Creates a vertex which can be referred to with appendTriangle() or appendIndices() 
	void		appendVertex( const Vec2f &v ) { appendVertices( &v, 1 ); }
	//! Creates a vertex which can be referred to with appendTriangle() or appendIndices() 
	void		appendVertex( const Vec3f &v ) { appendVertices( &v, 1 ); }
	//! Creates a vertex which can be referred to with appendTriangle() or appendIndices() 
	void		appendVertex( const Vec4f &v ) { appendVertices( &v, 1 ); }
	//! Appends multiple vertices to the TriMesh which can be referred to with appendTriangle() or appendIndices() 
	void		appendVertices( const Vec2f *verts, size_t num );
	//! Appends multiple vertices to the TriMesh which can be referred to with appendTriangle() or appendIndices() 
	void		appendVertices( const Vec3f *verts, size_t num );
	//! Appends multiple vertices to the TriMesh which can be referred to with appendTriangle() or appendIndices() 
	void		appendVertices( const Vec4f *verts, size_t num );
	//! Appends a single normal  
	void		appendNormal( const Vec3f &v ) { mNormals.push_back( v ); }
	//! appendNormals functions similarly to the appendVertices method, appending multiple normals to be associated with the vertices. 
	void		appendNormals( const Vec3f *normals, size_t num );
	//! Appends a single tangent  
	void		appendTangent( const Vec3f &v ) { mTangents.push_back( v ); }
	//! appendTangents functions similarly to the appendVertices method, appending multiple tangents to be associated with the vertices. 
	void		appendTangents( const Vec3f *tangents, size_t num );
	//! Appends a single tangent  
	void		appendBitangent( const Vec3f &v ) { mBitangents.push_back( v ); }
	//! appendBitangents functions similarly to the appendVertices method, appending multiple bitangents to be associated with the vertices. 
	void		appendBitangents( const Vec3f *bitangents, size_t num );
	//! this sets the color used by a triangle generated by the TriMesh
	void		appendColorRgb( const Color &rgb ) { appendColors( &rgb, 1 ); }
	//! this sets the color used by a triangle generated by the TriMesh
	void		appendColorRgba( const ColorA &rgba ) { appendColors( &rgba, 1 ); }

	//! Synonym for appendTexCoord0; appends a texture coordinate for unit 0
	void		appendTexCoord( const Vec2f &v ) { appendTexCoords0( &v, 1 ); }
	//!	appends a 2D texture coordinate for unit 0
	void		appendTexCoord0( const Vec2f &v ) { appendTexCoords0( &v, 1 ); }
	//! appends a 2D texture coordinate for unit 1
	void		appendTexCoord1( const Vec2f &v ) { appendTexCoords1( &v, 1 ); }
	//! appends a 2D texture coordinate for unit 2
	void		appendTexCoord2( const Vec2f &v ) { appendTexCoords2( &v, 1 ); }
	//! appends a 2D texture coordinate for unit 3
	void		appendTexCoord3( const Vec2f &v ) { appendTexCoords3( &v, 1 ); }

	//!	appends a 3D texture coordinate for unit 0
	void		appendTexCoord0( const Vec3f &v ) { appendTexCoords0( &v, 1 ); }
	//! appends a 3D texture coordinate for unit 1
	void		appendTexCoord1( const Vec3f &v ) { appendTexCoords1( &v, 1 ); }
	//! appends a 3D texture coordinate for unit 2
	void		appendTexCoord2( const Vec3f &v ) { appendTexCoords2( &v, 1 ); }
	//! appends a 3D texture coordinate for unit 3
	void		appendTexCoord3( const Vec3f &v ) { appendTexCoords3( &v, 1 ); }

	//!	appends a 4D texture coordinate for unit 0
	void		appendTexCoord0( const Vec4f &v ) { appendTexCoords0( &v, 1 ); }
	//! appends a 4D texture coordinate for unit 1
	void		appendTexCoord1( const Vec4f &v ) { appendTexCoords1( &v, 1 ); }
	//! appends a 4D texture coordinate for unit 2
	void		appendTexCoord2( const Vec4f &v ) { appendTexCoords2( &v, 1 ); }
	//! appends a 4D texture coordinate for unit 3
	void		appendTexCoord3( const Vec4f &v ) { appendTexCoords3( &v, 1 ); }
	
	//! Appends multiple RGB colors to the TriMesh
	void		appendColors( const Color *rgbs, size_t num );
	//! Appends multiple RGBA colors to the TriMesh
	void		appendColors( const ColorA *rgbas, size_t num );
	
	//! Appends multiple 2D texcoords for unit 0
	void		appendTexCoords0( const Vec2f *texCoords, size_t num );
	//! Appends multiple 2D texcoords for unit 1
	void		appendTexCoords1( const Vec2f *texCoords, size_t num );
	//! Appends multiple 2D texcoords for unit 2
	void		appendTexCoords2( const Vec2f *texCoords, size_t num );
	//! Appends multiple 2D texcoords for unit 3
	void		appendTexCoords3( const Vec2f *texCoords, size_t num );
	
	//! Appends multiple 3D texcoords for unit 0
	void		appendTexCoords0( const Vec3f *texCoords, size_t num );
	//! Appends multiple 3D texcoords for unit 1
	void		appendTexCoords1( const Vec3f *texCoords, size_t num );
	//! Appends multiple 3D texcoords for unit 2
	void		appendTexCoords2( const Vec3f *texCoords, size_t num );
	//! Appends multiple 3D texcoords for unit 3
	void		appendTexCoords3( const Vec3f *texCoords, size_t num );

	//! Appends multiple 4D texcoords for unit 0
	void		appendTexCoords0( const Vec4f *texCoords, size_t num );
	//! Appends multiple 4D texcoords for unit 1
	void		appendTexCoords1( const Vec4f *texCoords, size_t num );
	//! Appends multiple 4D texcoords for unit 2
	void		appendTexCoords2( const Vec4f *texCoords, size_t num );
	//! Appends multiple 4D texcoords for unit 3
	void		appendTexCoords3( const Vec4f *texCoords, size_t num );
	
	/*! after creating three vertices, pass the indices of the three vertices to create a triangle from them. Until this is done, unlike in an OpenGL triangle strip, the 
	 triangle will not actually be generated and stored by the TriMesh
	*/
	void		appendTriangle( uint32_t v0, uint32_t v1, uint32_t v2 )
	{ mIndices.push_back( v0 ); mIndices.push_back( v1 ); mIndices.push_back( v2 ); }
	//! Appends \a num vertices to the TriMesh pointed to by \a indices
	void		appendIndices( const uint32_t *indices, size_t num );

	//! Returns the total number of indices contained by a TriMesh.
	size_t		getNumIndices() const override { return mIndices.size(); }
	//! Returns the total number of triangles contained by a TriMesh.
	size_t		getNumTriangles() const { return mIndices.size() / 3; }
	//! Returns the total number of indices contained by a TriMesh.
	virtual size_t	getNumVertices() const override { if( mPositionsDims ) return mPositions.size() / mPositionsDims; else return 0; }

	//! Puts the 3 vertices of triangle number \a idx into \a a, \a b and \a c. Assumes vertices are 3D
	void		getTriangleVertices( size_t idx, Vec3f *a, Vec3f *b, Vec3f *c ) const;
	//! Puts the 3 vertices of triangle number \a idx into \a a, \a b and \a c. Assumes vertices are 2D
	void		getTriangleVertices( size_t idx, Vec2f *a, Vec2f *b, Vec2f *c ) const;
	//! Puts the 3 normals of triangle number \a idx into \a a, \a b and \a c.
	void		getTriangleNormals( size_t idx, Vec3f *a, Vec3f *b, Vec3f *c ) const;
	//! Puts the 3 tangents of triangle number \a idx into \a a, \a b and \a c.
	void		getTriangleTangents( size_t idx, Vec3f *a, Vec3f *b, Vec3f *c ) const;
	//! Puts the 3 bitangents of triangle number \a idx into \a a, \a b and \a c.
	void		getTriangleBitangents( size_t idx, Vec3f *a, Vec3f *b, Vec3f *c ) const;


	//! Returns all the vertices for a mesh in a std::vector as Vec<DIM>f. For example, to get 3D vertices, call getVertices<3>().
	template<uint8_t DIM>
	const typename VECDIM<DIM,float>::TYPE*	getVertices() const { assert(mPositionsDims==DIM); return (typename VECDIM<DIM,float>::TYPE*)mPositions.data(); }
	//! Returns all the vertices for a mesh in a std::vector as Vec<DIM>f. For example, to get 3D vertices, call getVertices<3>().
	template<uint8_t DIM>
	typename VECDIM<DIM,float>::TYPE*		getVertices() { assert(mPositionsDims==DIM); return (typename VECDIM<DIM,float>::TYPE*)mPositions.data(); }
	//! Returns all the normals for a mesh in a std::vector as Vec3f objects. There will be one of these for each vertex in the mesh
	std::vector<Vec3f>&				getNormals() { return mNormals; }
	//! Returns all the normals for a mesh in a std::vector as Vec3f objects. There will be one of these for each vertex in the mesh
	const std::vector<Vec3f>&		getNormals() const { return mNormals; }
	//! Returns all the tangents for a mesh in a std::vector as Vec3f objects. There will be one of these for each vertex in the mesh
	std::vector<Vec3f>&				getTangents() { return mTangents; }
	//! Returns all the tangents for a mesh in a std::vector as Vec3f objects. There will be one of these for each vertex in the mesh
	const std::vector<Vec3f>&		getTangents() const { return mTangents; }
	//! Returns all the bitangents for a mesh in a std::vector as Vec3f objects. There will be one of these for each vertex in the mesh
	std::vector<Vec3f>&				getBitangents() { return mBitangents; }
	//! Returns all the bitangents for a mesh in a std::vector as Vec3f objects. There will be one of these for each vertex in the mesh
	const std::vector<Vec3f>&		getBitangents() const { return mBitangents; }
	/*//! Returns a std::vector of RGB colors of the triangles faces. There will be one of these for each vertex in the mesh
	std::vector<Color>&				getColorsRgb() { return mColorsRGB; }
	//! Returns a std::vector of RGB colors of the triangles faces. There will be one of these for each vertex in the mesh
	const std::vector<Color>&		getColorsRgb() const { return mColorsRGB; }
	//! Returns a std::vector of RGBA colors of the triangles faces. There will be one of these for each vertex in the mesh
	std::vector<ColorA>&			getColorsRgba() { return mColorsRGBA; }
	//! Returns a std::vector of RGBA colors of the triangles faces. There will be one of these for each vertex in the mesh
	const std::vector<ColorA>&		getColorsRgba() const { return mColorsRGBA; }
	//! Returns a std::vector of Texture coordinates as Vec2fs. There will be one texture coord for each vertex in the TriMesh
	std::vector<Vec2f>&				getTexCoords() { return mTexCoords; }	
	//! Returns a std::vector of Texture coordinates as Vec2fs. There will be one texture coord for each vertex in the TriMesh
	const std::vector<Vec2f>&		getTexCoords() const { return mTexCoords; }	*/
	//! Trimesh indices are ordered such that the indices of triangle T are { indices[T*3+0], indices[T*3+1], indices[T*3+2] }
	std::vector<uint32_t>&			getIndices() { return mIndices; }		
	//! Trimesh indices are ordered such that the indices of triangle T are { indices[T*3+0], indices[T*3+1], indices[T*3+2] }
	const std::vector<uint32_t>&	getIndices() const { return mIndices; }		

	//! Calculates the bounding box of all vertices. Fails if the positions are not 3D.
	AxisAlignedBox3f	calcBoundingBox() const;
	//! Calculates the bounding box of all vertices as transformed by \a transform. Fails if the positions are not 3D.
	AxisAlignedBox3f	calcBoundingBox( const Matrix44f &transform ) const;

	//! This allows you read a TriMesh in from a data file, for instance an .obj file. At present .obj and .dat files are supported
	void		read( DataSourceRef in );
	//! This allows to you write a mesh out to a data file. At present .obj and .dat files are supported.
	void		write( DataTargetRef out ) const;

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
							   const std::vector<Vec3f> &normals, const std::vector<Vec3f> &positions,
							   const std::vector<Vec2f> &texCoords );*/

	// geom::Source virtuals
	virtual geom::Primitive		getPrimitive() const override { return geom::Primitive::TRIANGLES; }
	
	virtual uint8_t		getAttribDims( geom::Attrib attr ) const override;

  protected:
	void		getAttribPointer( geom::Attrib attr, const float **resultPtr, size_t *resultStrideBytes, uint8_t *resultDims ) const;
	void		copyAttrib( geom::Attrib attr, uint8_t dims, size_t stride, const float *srcData, size_t count );

	//! Returns whether or not the vertex, color etc. at both indices is the same.
	bool		isEqual( uint32_t indexA, uint32_t indexB ) const;

	uint8_t		mPositionsDims, mNormalsDims, mTangentsDims, mBitangentsDims, mColorsDims;
	uint8_t		mTexCoords0Dims, mTexCoords1Dims, mTexCoords2Dims, mTexCoords3Dims;
  
	std::vector<float>		mPositions;
	std::vector<float>		mColors;
	std::vector<Vec3f>		mNormals; // always dim=3
	std::vector<Vec3f>		mTangents; // always dim=3
	std::vector<Vec3f>		mBitangents; // always dim=3
	std::vector<float>		mTexCoords0, mTexCoords1, mTexCoords2, mTexCoords3;
	std::vector<uint32_t>	mIndices;
	
	friend class TriMeshGeomTarget;
};

} // namespace cinder
