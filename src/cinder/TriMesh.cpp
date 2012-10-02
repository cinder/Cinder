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

#include "cinder/TriMesh.h"

using std::vector;

namespace cinder {

/////////////////////////////////////////////////////////////////////////////////////////////////
// TriMesh
void TriMesh::clear()
{
	mVertices.clear();
	mNormals.clear();
	mColorsRGB.clear();
	mColorsRGBA.clear();
	mTexCoords.clear();
	mIndices.clear();
}

void TriMesh::appendVertices( const Vec3f *verts, size_t num )
{
	mVertices.insert( mVertices.end(), verts, verts + num );
}

void TriMesh::appendVertices( const Vec4d *verts, size_t num )
{
	for( size_t v = 0; v < num; ++v )
		mVertices.push_back( Vec3f( (float)verts[v].x, (float)verts[v].y, (float)verts[v].z ) );
}

void TriMesh::appendIndices( uint32_t *indices, size_t num )
{
	mIndices.insert( mIndices.end(), indices, indices + num );
}

void TriMesh::appendNormals( const Vec3f *normals, size_t num )
{
	mNormals.insert( mNormals.end(), normals, normals + num );
}

void TriMesh::appendNormals( const Vec4d *normals, size_t num )
{
	for( size_t v = 0; v < num; ++v )
		mNormals.push_back( Vec3f( (float)normals[v].x, (float)normals[v].y, (float)normals[v].z ) );
}

void TriMesh::appendColorsRgb( const Color *rgbs, size_t num )
{
	mColorsRGB.insert( mColorsRGB.end(), rgbs, rgbs + num );
}

void TriMesh::appendColorsRgba( const ColorA *rgbas, size_t num )
{
	mColorsRGBA.insert( mColorsRGBA.end(), rgbas, rgbas + num );
}

void TriMesh::appendTexCoords( const Vec2f *texcoords, size_t num )
{
	mTexCoords.insert( mTexCoords.end(), texcoords, texcoords + num );
}

void TriMesh::getTriangleVertices( size_t idx, Vec3f *a, Vec3f *b, Vec3f *c ) const
{
	*a = mVertices[ mIndices[idx * 3] ];
	*b = mVertices[ mIndices[idx * 3 + 1] ];
	*c = mVertices[ mIndices[idx * 3 + 2] ];
}

AxisAlignedBox3f TriMesh::calcBoundingBox() const
{
	if( mVertices.empty() )
		return AxisAlignedBox3f( Vec3f::zero(), Vec3f::zero() );

	Vec3f min(mVertices[0]), max(mVertices[0]);
	for( size_t i = 1; i < mVertices.size(); ++i ) {
		if( mVertices[i].x < min.x )
			min.x = mVertices[i].x;
		else if( mVertices[i].x > max.x )
			max.x = mVertices[i].x;
		if( mVertices[i].y < min.y )
			min.y = mVertices[i].y;
		else if( mVertices[i].y > max.y )
			max.y = mVertices[i].y;
		if( mVertices[i].z < min.z )
			min.z = mVertices[i].z;
		else if( mVertices[i].z > max.z )
			max.z = mVertices[i].z;
	}
	
	return AxisAlignedBox3f( min, max );
}

AxisAlignedBox3f TriMesh::calcBoundingBox( const Matrix44f &transform ) const
{
	if( mVertices.empty() )
		return AxisAlignedBox3f( Vec3f::zero(), Vec3f::zero() );

	Vec3f min( transform.transformPointAffine( mVertices[0] ) );
	Vec3f max( min );
	for( size_t i = 0; i < mVertices.size(); ++i ) {
		Vec3f v = transform.transformPointAffine( mVertices[i] );

		if( v.x < min.x )
			min.x = v.x;
		else if( v.x > max.x )
			max.x = v.x;
		if( v.y < min.y )
			min.y = v.y;
		else if( v.y > max.y )
			max.y = v.y;
		if( v.z < min.z )
			min.z = v.z;
		else if( v.z > max.z )
			max.z = v.z;
	}

	return AxisAlignedBox3f( min, max );
}


void TriMesh::read( DataSourceRef dataSource )
{
	IStreamRef in = dataSource->createStream();
	clear();

	uint8_t versionNumber;
	in->read( &versionNumber );
	
	uint32_t numVertices, numNormals, numTexCoords, numIndices;
	in->readLittle( &numVertices );
	in->readLittle( &numNormals );
	in->readLittle( &numTexCoords );
	in->readLittle( &numIndices );
	
	for( size_t idx = 0; idx < numVertices; ++idx ) {
		Vec3f v;
		in->readLittle( &v.x ); in->readLittle( &v.y ); in->readLittle( &v.z );
		mVertices.push_back( v );
	}

	for( size_t idx = 0; idx < numNormals; ++idx ) {
		Vec3f v;
		in->readLittle( &v.x ); in->readLittle( &v.y ); in->readLittle( &v.z );
		mNormals.push_back( v );
	}

	for( size_t idx = 0; idx < numTexCoords; ++idx ) {
		Vec2f v;
		in->readLittle( &v.x ); in->readLittle( &v.y );
		mTexCoords.push_back( v );
	}

	for( size_t idx = 0; idx < numIndices; ++idx ) {
		uint32_t v;
		in->readLittle( &v );
		mIndices.push_back( v );
	}
}

void TriMesh::write( DataTargetRef dataTarget ) const
{
	OStreamRef out = dataTarget->getStream();
	
	const uint8_t versionNumber = 1;
	out->write( versionNumber );
	
	out->writeLittle( static_cast<uint32_t>( mVertices.size() ) );
	out->writeLittle( static_cast<uint32_t>( mNormals.size() ) );
	out->writeLittle( static_cast<uint32_t>( mTexCoords.size() ) );
	out->writeLittle( static_cast<uint32_t>( mIndices.size() ) );
	
	for( vector<Vec3f>::const_iterator it = mVertices.begin(); it != mVertices.end(); ++it ) {
		out->writeLittle( it->x ); out->writeLittle( it->y ); out->writeLittle( it->z );
	}

	for( vector<Vec3f>::const_iterator it = mNormals.begin(); it != mNormals.end(); ++it ) {
		out->writeLittle( it->x ); out->writeLittle( it->y ); out->writeLittle( it->z );
	}

	for( vector<Vec2f>::const_iterator it = mTexCoords.begin(); it != mTexCoords.end(); ++it ) {
		out->writeLittle( it->x ); out->writeLittle( it->y );
	}

	for( vector<uint32_t>::const_iterator it = mIndices.begin(); it != mIndices.end(); ++it ) {
		out->writeLittle( *it );
	}
}

void TriMesh::recalculateNormals()
{
	mNormals.assign( mVertices.size(), Vec3f::zero() );

	size_t n = getNumTriangles();
	for( size_t i = 0; i < n; ++i ) {
		uint32_t index0 = mIndices[i * 3];
		uint32_t index1 = mIndices[i * 3 + 1];
		uint32_t index2 = mIndices[i * 3 + 2];

		Vec3f v0 = mVertices[ index0 ];
		Vec3f v1 = mVertices[ index1 ];
		Vec3f v2 = mVertices[ index2 ];

		Vec3f e0 = v1 - v0;
		Vec3f e1 = v2 - v0;
		Vec3f normal = e0.cross(e1).normalized();

		mNormals[ index0 ] += normal;
		mNormals[ index1 ] += normal;
		mNormals[ index2 ] += normal;
	}

	std::for_each( mNormals.begin(), mNormals.end(), std::mem_fun_ref(&Vec3f::normalize) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// TriMesh2d
void TriMesh2d::clear()
{
	mVertices.clear();
	mColorsRgb.clear();
	mColorsRgba.clear();
	mTexCoords.clear();
	mIndices.clear();
}

void TriMesh2d::appendVertices( const Vec2f *verts, size_t num )
{
	mVertices.insert( mVertices.end(), verts, verts + num );
}

void TriMesh2d::appendIndices( uint32_t *indices, size_t num )
{
	mIndices.insert( mIndices.end(), indices, indices + num );
}

void TriMesh2d::appendColorsRgb( const Color *rgbs, size_t num )
{
	mColorsRgb.insert( mColorsRgb.end(), rgbs, rgbs + num );
}

void TriMesh2d::appendColorsRgba( const ColorA *rgbas, size_t num )
{
	mColorsRgba.insert( mColorsRgba.end(), rgbas, rgbas + num );
}

void TriMesh2d::appendTexCoords( const Vec2f *texcoords, size_t num )
{
	mTexCoords.insert( mTexCoords.end(), texcoords, texcoords + num );
}

void TriMesh2d::getTriangleVertices( size_t idx, Vec2f *a, Vec2f *b, Vec2f *c ) const
{
	*a = mVertices[ mIndices[idx * 3] ];
	*b = mVertices[ mIndices[idx * 3 + 1] ];
	*c = mVertices[ mIndices[idx * 3 + 2] ];
}

Rectf TriMesh2d::calcBoundingBox() const
{
	if( mVertices.empty() )
		return Rectf( Vec2f::zero(), Vec2f::zero() );

	Vec2f min(mVertices[0]), max(mVertices[0]);
	for( size_t i = 1; i < mVertices.size(); ++i ) {
		if( mVertices[i].x < min.x )
			min.x = mVertices[i].x;
		else if( mVertices[i].x > max.x )
			max.x = mVertices[i].x;
		if( mVertices[i].y < min.y )
			min.y = mVertices[i].y;
		else if( mVertices[i].y > max.y )
			max.y = mVertices[i].y;
	}
	
	return Rectf( min, max );
}

} // namespace cinder
