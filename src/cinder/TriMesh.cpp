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

void TriMesh::clear()
{
	mVertices.clear();
	mNormals.clear();
	mTexCoords.clear();
	mIndices.clear();
}

void TriMesh::appendVertices( const Vec4d *verts, size_t num )
{
	for( size_t v = 0; v < num; ++v )
		mVertices.push_back( Vec3f( (float)verts[v].x, (float)verts[v].y, (float)verts[v].z ) );
}

void TriMesh::appendNormals( const Vec4d *normals, size_t num )
{
	for( size_t v = 0; v < num; ++v )
		mNormals.push_back( Vec3f( (float)normals[v].x, (float)normals[v].y, (float)normals[v].z ) );
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

	for( vector<size_t>::const_iterator it = mIndices.begin(); it != mIndices.end(); ++it ) {
		out->writeLittle( static_cast<uint32_t>( *it) );
	}
}

} // namespace cinder
