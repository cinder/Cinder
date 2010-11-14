/*
 Copyright (c) 2010, The Barbarian Group
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

namespace cinder {

class TriMesh {
 public:

	void		clear();
	
	bool		hasNormals() const { return ! mNormals.empty(); }
	bool		hasColorsRGB() const { return ! mColorsRGB.empty(); }
	bool		hasColorsRGBA() const { return ! mColorsRGBA.empty(); }
	bool		hasTexCoords() const { return ! mTexCoords.empty(); }

	void		appendVertex( const Vec3f &v ) { mVertices.push_back( v ); }
	void		appendVertices( const Vec4d *verts, size_t num );
	void		appendNormal( const Vec3f &v ) { mNormals.push_back( v ); }
	void		appendNormals( const Vec4d *normals, size_t num );
	void		appendColorRGB( const Color &rgb ) { mColorsRGB.push_back( rgb ); }
	void		appendColorRGBA( const ColorA &rgba ) { mColorsRGBA.push_back( rgba ); }
	void		appendTexCoord( const Vec2f &v ) { mTexCoords.push_back( v ); }
	void		appendTriangle( size_t v0, size_t v1, size_t v2 )
	{ mIndices.push_back( v0 ); mIndices.push_back( v1 ); mIndices.push_back( v2 ); }

	size_t		getNumIndices() const { return mIndices.size(); }
	size_t		getNumTriangles() const { return mIndices.size() / 3; }
	size_t		getNumVertices() const { return mVertices.size(); }

	//! Puts the 3 vertices of triangle number \a idx into \a a, \a b and \a c.
	void		getTriangleVertices( size_t idx, Vec3f *a, Vec3f *b, Vec3f *c ) const;

	const std::vector<Vec3f>&		getVertices() const { return mVertices; }
	const std::vector<Vec3f>&		getNormals() const { return mNormals; }
	const std::vector<Color>&		getColorsRGB() const { return mColorsRGB; }
	const std::vector<ColorA>&		getColorsRGBA() const { return mColorsRGBA; }
	const std::vector<Vec2f>&		getTexCoords() const { return mTexCoords; }	
	//! Trimesh indices are ordered such that the indices of triangle T are { indices[T*3+0], indices[T*3+1], indices[T*3+2] }
	const std::vector<size_t>&		getIndices() const { return mIndices; }		

	//! Calculates the bounding box of all vertices
	AxisAlignedBox3f	calcBoundingBox() const;
	//! Calculates the bounding box of all vertices as transformed by \a transform
	AxisAlignedBox3f	calcBoundingBox( const Matrix44f &transform ) const;

	void		read( DataSourceRef in );
	void		write( DataTargetRef out ) const;
	
 private:
	std::vector<Vec3f>		mVertices;
	std::vector<Vec3f>		mNormals;
	std::vector<Color>		mColorsRGB;
	std::vector<ColorA>		mColorsRGBA;
	std::vector<Vec2f>		mTexCoords;
	std::vector<size_t>		mIndices;
};

} // namespace cinder
