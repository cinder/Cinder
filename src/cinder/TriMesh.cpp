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

#include "cinder/TriMesh.h"
#include "cinder/Exception.h"
#if defined( CINDER_ANDROID )
	#include "cinder/android/CinderAndroid.h"
#endif 

using namespace std;

namespace cinder {

/////////////////////////////////////////////////////////////////////////////////////////////////
// TriMeshGeomTarget
class TriMeshGeomTarget : public geom::Target {
  public:
	TriMeshGeomTarget( TriMesh *mesh )
		: mMesh( mesh )
	{}
	
	uint8_t	getAttribDims( geom::Attrib attr ) const override;
	void copyAttrib( geom::Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count ) override;
	void copyIndices( geom::Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex ) override;
	
  protected:
	TriMesh		*mMesh;
};

uint8_t	TriMeshGeomTarget::getAttribDims( geom::Attrib attr ) const
{
	return mMesh->getAttribDims( attr );
}

void TriMeshGeomTarget::copyAttrib( geom::Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count )
{
	// copyAttrib will ignore it if 'mMesh' doesn't have this attribute
	mMesh->copyAttrib( attr, dims, strideBytes, srcData, count );
}

void TriMeshGeomTarget::copyIndices( geom::Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t /*requiredBytesPerIndex*/ )
{
	size_t targetNumIndices = numIndices;
	switch( primitive ) {
		case geom::Primitive::TRIANGLE_STRIP :
		case geom::Primitive::TRIANGLE_FAN :
			targetNumIndices = ( numIndices - 2 ) * 3;
		break;
		default: // All good
		break;
	}
			
	mMesh->mIndices.resize( targetNumIndices );
	copyIndexDataForceTriangles( primitive, source, numIndices, 0, mMesh->mIndices.data() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// TriMesh::Format
TriMesh::Format::Format()
{
	mPositionsDims = mNormalsDims = mTangentsDims = mBitangentsDims = mColorsDims = 0;
	mTexCoords0Dims = mTexCoords1Dims = mTexCoords2Dims = mTexCoords3Dims = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// TriMesh
TriMesh::TriMesh( const TriMesh::Format &format )
{
	initFromFormat( format );
}

TriMesh::TriMesh( const geom::Source &source )
{
	initFromFormat( formatFromSource( source ) );
	loadFromSource( source );
}

TriMesh::TriMesh( const geom::Source &source, const TriMesh::Format &format )
{
	initFromFormat( format );
	loadFromSource( source );
}

void TriMesh::loadFromSource( const geom::Source &source )
{
	geom::AttribSet attribs;
	if( mPositionsDims ) attribs.insert( geom::Attrib::POSITION );
	if( mNormalsDims ) attribs.insert( geom::Attrib::NORMAL );
	if( mTangentsDims ) attribs.insert( geom::Attrib::TANGENT );
	if( mBitangentsDims ) attribs.insert( geom::Attrib::BITANGENT );
	if( mColorsDims ) attribs.insert( geom::Attrib::COLOR );
	if( mTexCoords0Dims ) attribs.insert( geom::Attrib::TEX_COORD_0 );
	if( mTexCoords1Dims ) attribs.insert( geom::Attrib::TEX_COORD_1 );
	if( mTexCoords2Dims ) attribs.insert( geom::Attrib::TEX_COORD_2 );
	if( mTexCoords3Dims ) attribs.insert( geom::Attrib::TEX_COORD_3 );
	
	TriMeshGeomTarget target( this );
	source.loadInto( &target, attribs );
	
	// if source is-nonindexed, generate indices
	if( source.getNumIndices() == 0 )
		target.generateIndices( source.getPrimitive(), source.getNumVertices() );
}

void TriMesh::initFromFormat( const TriMesh::Format &format )
{
	mPositionsDims = format.mPositionsDims;
	mNormalsDims = format.mNormalsDims;
	mTangentsDims = format.mTangentsDims;
	mBitangentsDims = format.mBitangentsDims;
	mColorsDims = format.mColorsDims;
	mTexCoords0Dims = format.mTexCoords0Dims;
	mTexCoords1Dims = format.mTexCoords1Dims;
	mTexCoords2Dims = format.mTexCoords2Dims;
	mTexCoords3Dims = format.mTexCoords3Dims;
}

TriMesh::Format TriMesh::formatFromSource( const geom::Source &source )
{
	Format result;

	// positions
	if( source.getAttribDims( geom::Attrib::POSITION ) > 0 )
		result.mPositionsDims = source.getAttribDims( geom::Attrib::POSITION );

	// normals
	if( source.getAttribDims( geom::Attrib::NORMAL ) > 0 )
		result.mNormalsDims = 3;

	// tangents
	if( source.getAttribDims( geom::Attrib::TANGENT ) > 0 )
		result.mTangentsDims = 3;

	// bitangents
	if( source.getAttribDims( geom::Attrib::BITANGENT ) > 0 )
		result.mBitangentsDims = 3;

	// colors
	if( source.getAttribDims( geom::Attrib::COLOR ) > 0 )
		result.mColorsDims = source.getAttribDims( geom::Attrib::COLOR );

	// tex coords 0
	if( source.getAttribDims( geom::Attrib::TEX_COORD_0 ) > 0 )
		result.mTexCoords0Dims = source.getAttribDims( geom::Attrib::TEX_COORD_0 );

	// tex coords 1
	if( source.getAttribDims( geom::Attrib::TEX_COORD_1 ) > 0 )
		result.mTexCoords1Dims = source.getAttribDims( geom::Attrib::TEX_COORD_1 );

	// tex coords 2
	if( source.getAttribDims( geom::Attrib::TEX_COORD_2 ) > 0 )
		result.mTexCoords2Dims = source.getAttribDims( geom::Attrib::TEX_COORD_2 );

	// tex coords 3
	if( source.getAttribDims( geom::Attrib::TEX_COORD_3 ) > 0 )
		result.mTexCoords3Dims = source.getAttribDims( geom::Attrib::TEX_COORD_3 );
	
	return result;
}

void TriMesh::loadInto( geom::Target *target, const geom::AttribSet &requestedAttribs ) const
{
	// copy attributes
	for( auto &attrib : requestedAttribs ) {
		size_t attribDims = getAttribDims( (geom::Attrib)attrib );
		if( attribDims ) {
			uint8_t dims;
			const float *pointer;
			size_t strideBytes;

			getAttribPointer( attrib, &pointer, &strideBytes, &dims );
			
			if( pointer )
				target->copyAttrib( attrib, dims, strideBytes, pointer, getNumVertices() );
		}
	}
	
	// copy indices
	if( getNumIndices() )
		target->copyIndices( geom::Primitive::TRIANGLES, mIndices.data(), getNumIndices(), 4 /* bytes per index */ );
}

void TriMesh::clear()
{
	mPositions.clear();
	mNormals.clear();
	mTangents.clear();
	mBitangents.clear();
	mColors.clear();
	mTexCoords0.clear();
	mTexCoords1.clear();
	mTexCoords2.clear();
	mTexCoords3.clear();
	mIndices.clear();
}

void TriMesh::appendPositions( const vec2 *positions, size_t num )
{
	assert( mPositionsDims == 2 );
	mPositions.insert( mPositions.end(), (const float*)positions, (const float*)positions + num * 2 );
}

void TriMesh::appendPositions( const vec3 *positions, size_t num )
{
	assert( mPositionsDims == 3 );
	mPositions.insert( mPositions.end(), (const float*)positions, (const float*)positions + num * 3 );
}

void TriMesh::appendPositions( const vec4 *positions, size_t num )
{
	assert( mPositionsDims == 4 );
	mPositions.insert( mPositions.end(), (const float*)positions, (const float*)positions + num * 4 );
}

void TriMesh::appendIndices( const uint32_t *indices, size_t num )
{
	mIndices.insert( mIndices.end(), indices, indices + num );
}

void TriMesh::appendNormals( const vec3 *normals, size_t num )
{
	assert( mNormalsDims == 3 );
	mNormals.insert( mNormals.end(), normals, normals + num );
}

void TriMesh::appendTangents( const vec3 *tangents, size_t num )
{
	assert( mTangentsDims == 3 );
	mTangents.insert( mTangents.end(), tangents, tangents + num );
}

void TriMesh::appendBitangents( const vec3 *bitangents, size_t num )
{
	assert( mBitangentsDims == 3 );
	mBitangents.insert( mBitangents.end(), bitangents, bitangents + num );
}

void TriMesh::appendColors( const Color *rgbs, size_t num )
{
	assert( mColorsDims == 3 );
	mColors.insert( mColors.end(), (const float*)rgbs, (const float*)rgbs + num * 3 );
}

void TriMesh::appendColors( const ColorA *rgbas, size_t num )
{
	assert( mColorsDims == 4 );
	mColors.insert( mColors.end(), (const float*)rgbas, (const float*)rgbas + num * 4 );
}

///////////////////////////////////////////////////////////////////////////////////////////
// appendTexCoords*( vec2 )

void TriMesh::appendTexCoords0( const vec2 *texCoords, size_t num )
{
	assert( mTexCoords0Dims == 2 );
	mTexCoords0.insert( mTexCoords0.end(), (const float*)texCoords, (const float*)texCoords + num * 2 );
}

void TriMesh::appendTexCoords1( const vec2 *texCoords, size_t num )
{
	assert( mTexCoords1Dims == 2 );
	mTexCoords1.insert( mTexCoords1.end(), (const float*)texCoords, (const float*)texCoords + num * 2 );
}

void TriMesh::appendTexCoords2( const vec2 *texCoords, size_t num )
{
	assert( mTexCoords2Dims == 2 );
	mTexCoords2.insert( mTexCoords2.end(), (const float*)texCoords, (const float*)texCoords + num * 2 );
}

void TriMesh::appendTexCoords3( const vec2 *texCoords, size_t num )
{
	assert( mTexCoords3Dims == 2 );
	mTexCoords3.insert( mTexCoords3.end(), (const float*)texCoords, (const float*)texCoords + num * 2 );
}

///////////////////////////////////////////////////////////////////////////////////////////
// appendTexCoords*( vec3 )
void TriMesh::appendTexCoords0( const vec3 *texCoords, size_t num )
{
	assert( mTexCoords0Dims == 3 );
	mTexCoords0.insert( mTexCoords0.end(), (const float*)texCoords, (const float*)texCoords + num * 3 );
}

void TriMesh::appendTexCoords1( const vec3 *texCoords, size_t num )
{
	assert( mTexCoords1Dims == 3 );
	mTexCoords1.insert( mTexCoords1.end(), (const float*)texCoords, (const float*)texCoords + num * 3 );
}

void TriMesh::appendTexCoords2( const vec3 *texCoords, size_t num )
{
	assert( mTexCoords2Dims == 3 );
	mTexCoords2.insert( mTexCoords2.end(), (const float*)texCoords, (const float*)texCoords + num * 3 );
}

void TriMesh::appendTexCoords3( const vec3 *texCoords, size_t num )
{
	assert( mTexCoords3Dims == 3 );
	mTexCoords3.insert( mTexCoords3.end(), (const float*)texCoords, (const float*)texCoords + num * 3 );
}

///////////////////////////////////////////////////////////////////////////////////////////
// appendTexCoords*( vec4 )
void TriMesh::appendTexCoords0( const vec4 *texCoords, size_t num )
{
	assert( mTexCoords0Dims == 4 );
	mTexCoords0.insert( mTexCoords0.end(), (const float*)texCoords, (const float*)texCoords + num * 4 );
}

void TriMesh::appendTexCoords1( const vec4 *texCoords, size_t num )
{
	assert( mTexCoords1Dims == 4 );
	mTexCoords1.insert( mTexCoords1.end(), (const float*)texCoords, (const float*)texCoords + num * 4 );
}

void TriMesh::appendTexCoords2( const vec4 *texCoords, size_t num )
{
	assert( mTexCoords2Dims == 4 );
	mTexCoords2.insert( mTexCoords2.end(), (const float*)texCoords, (const float*)texCoords + num * 4 );
}

void TriMesh::appendTexCoords3( const vec4 *texCoords, size_t num )
{
	assert( mTexCoords3Dims == 4 );
	mTexCoords3.insert( mTexCoords3.end(), (const float*)texCoords, (const float*)texCoords + num * 4 );
}

///////////////////////////////////////////////////////////////////////////////////////////

void TriMesh::getTriangleVertices( size_t idx, vec3 *a, vec3 *b, vec3 *c ) const
{
	assert( mPositionsDims == 3 );
	*a = vec3( mPositions[mIndices[idx * 3] * 3 + 0], mPositions[mIndices[idx * 3] * 3 + 1], mPositions[ mIndices[idx * 3] * 3 + 2 ] );
	*b = vec3( mPositions[mIndices[idx * 3 + 1] * 3 + 0], mPositions[mIndices[idx * 3 + 1] * 3 + 1], mPositions[ mIndices[idx * 3 + 1] * 3 + 2 ] );
	*c = vec3( mPositions[mIndices[idx * 3 + 2] * 3 + 0], mPositions[mIndices[idx * 3 + 2] * 3 + 1], mPositions[ mIndices[idx * 3 + 2] * 3 + 2 ] );
}

void TriMesh::getTriangleVertices( size_t idx, vec2 *a, vec2 *b, vec2 *c ) const
{
	assert( mPositionsDims == 2 );
	*a = vec2( mPositions[mIndices[idx * 3] * 2 + 0], mPositions[mIndices[idx * 3] * 2 + 1] );
	*b = vec2( mPositions[mIndices[idx * 3 + 1] * 2 + 0], mPositions[mIndices[idx * 3 + 1] * 2 + 1] );
	*c = vec2( mPositions[mIndices[idx * 3 + 2] * 2 + 0], mPositions[mIndices[idx * 3 + 2] * 2 + 1] );
}

void TriMesh::getTriangleNormals( size_t idx, vec3 *a, vec3 *b, vec3 *c ) const
{
	assert( mNormalsDims == 3 );
	*a = mNormals[mIndices[idx * 3 + 0]];
	*b = mNormals[mIndices[idx * 3 + 1]];
	*c = mNormals[mIndices[idx * 3 + 2]];
}

void TriMesh::getTriangleTangents( size_t idx, vec3 *a, vec3 *b, vec3 *c ) const
{
	assert( mTangentsDims == 3 );
	*a = mTangents[mIndices[idx * 3 + 0]];
	*b = mTangents[mIndices[idx * 3 + 1]];
	*c = mTangents[mIndices[idx * 3 + 2]];
}

void TriMesh::getTriangleBitangents( size_t idx, vec3 *a, vec3 *b, vec3 *c ) const
{
	assert( mBitangentsDims == 3 );
	*a = mBitangents[mIndices[idx * 3 + 0]];
	*b = mBitangents[mIndices[idx * 3 + 1]];
	*c = mBitangents[mIndices[idx * 3 + 2]];
}

AxisAlignedBox TriMesh::calcBoundingBox() const
{
	assert( mPositionsDims == 3 );
	if( mPositions.empty() )
		return AxisAlignedBox( vec3(), vec3() );

	vec3 min(*(const vec3*)(&mPositions[0])), max(*(const vec3*)(&mPositions[0]));
	for( size_t i = 1; i < mPositions.size() / 3; ++i ) {
		const vec3 &v = *(const vec3*)(&mPositions[i*3]);
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
	
	return AxisAlignedBox( min, max );
}

AxisAlignedBox TriMesh::calcBoundingBox( const mat4 &transform ) const
{
	assert( mPositionsDims == 3 );
	if( mPositions.empty() )
		return AxisAlignedBox( vec3(), vec3() );

	const vec3 &t1 = *(const vec3*)(&mPositions[0]);
	vec3 min = vec3( transform * vec4( t1, 1 ) );
	vec3 max = min;
	for( size_t i = 0; i < mPositions.size() / 3; ++i ) {
		const vec3 &temp = *(const vec3*)(&mPositions[i*3]);
		vec3 v = vec3( transform * vec4( temp, 1 ) );

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

	return AxisAlignedBox( min, max );
}

void TriMesh::read( const DataSourceRef &dataSource )
{
	IStreamRef in = dataSource->createStream();

	uint8_t versionNumber;
	in->read( &versionNumber );

	if( versionNumber == 1 ) {
		clear();
		readImplV1( in );
	}
	else if( versionNumber == 2 ) {
		clear();
		readImplV2( in );
	}
	else {
		throw Exception( "TriMesh::read() error: wrong version number. expected version = 1 or 2, version read: " + std::to_string( versionNumber ) );
	}
}

void TriMesh::write( const DataTargetRef &dataTarget, bool writeNormals, bool writeTangents ) const
{
	uint32_t mask = ~0u;
	if( ! writeNormals )
		mask &= ~toMask( geom::NORMAL );
	if( ! writeTangents )
		mask &= ~toMask( geom::TANGENT ) & ~toMask( geom::BITANGENT );
	write( dataTarget, mask );
}

void TriMesh::write( const DataTargetRef &dataTarget, const std::set<geom::Attrib> &attribs ) const
{
	uint32_t mask = 0;
	for( auto &attrib : attribs )
		mask |= toMask( attrib );
	write( dataTarget, mask );
}

void TriMesh::write( const DataTargetRef &dataTarget, uint32_t writeMask ) const
{
	OStreamRef out = dataTarget->getStream();

	auto writeAttrib = [out, writeMask]( uint32_t attrib, uint8_t dims, size_t numFloats, const void *ptr ) {
		if( numFloats > 0 && ( writeMask & attrib ) ) {
			out->writeLittle( attrib );
			out->writeLittle( dims );
			out->writeLittle( (uint32_t)numFloats );
			out->writeData( ptr, numFloats * sizeof( float ) );
		}
	};

	const uint8_t version = 2;
	out->write( version );

	out->writeLittle( static_cast<uint32_t>( mIndices.size() ) );
	if( ! mIndices.empty() )
		out->writeData( mIndices.data(), mIndices.size() * sizeof( uint32_t ) );

	writeAttrib( toMask( geom::POSITION ), mPositionsDims, mPositions.size(), mPositions.data() );
	writeAttrib( toMask( geom::COLOR ), mColorsDims, mColors.size(), mColors.data() );
	writeAttrib( toMask( geom::NORMAL ), mNormalsDims, mNormals.size() * 3, mNormals.data() );
	writeAttrib( toMask( geom::TEX_COORD_0 ), mTexCoords0Dims, mTexCoords0.size(), mTexCoords0.data() );
	writeAttrib( toMask( geom::TEX_COORD_1 ), mTexCoords1Dims, mTexCoords1.size(), mTexCoords1.data() );
	writeAttrib( toMask( geom::TEX_COORD_2 ), mTexCoords2Dims, mTexCoords2.size(), mTexCoords2.data() );
	writeAttrib( toMask( geom::TEX_COORD_3 ), mTexCoords3Dims, mTexCoords3.size(), mTexCoords3.data() );
	writeAttrib( toMask( geom::TANGENT ), mTangentsDims, mTangents.size() * 3, mTangents.data() );
	writeAttrib( toMask( geom::BITANGENT ), mBitangentsDims, mBitangents.size() * 3, mBitangents.data() );
}

// used in 0.9.0
void TriMesh::readImplV2( const IStreamRef &in )
{
	auto readAttribf = [in]( uint8_t *dims, std::vector<float> *data ) {
		if( in->isEof() ) return;

		in->readLittle( dims );

		uint32_t sizeInFloats;
		in->readLittle( &sizeInFloats );

		data->resize( sizeInFloats );
		in->readData( data->data(), sizeInFloats * sizeof( float ) );
	};

	auto readAttribVec3f = [in]( uint8_t *dims, std::vector<vec3> *data ) {
		if( in->isEof() ) return;

		in->readLittle( dims );

		uint32_t sizeInFloats;
		in->readLittle( &sizeInFloats );

		data->resize( sizeInFloats * sizeof( float ) / sizeof( vec3 ) );
		in->readData( data->data(), sizeInFloats * sizeof( float ) );
	};

	// indices
	uint32_t numIndices;
	in->readLittle( &numIndices );
	if( numIndices > 0 ) {
		mIndices.resize( numIndices );
		in->readData( mIndices.data(), numIndices * sizeof( uint32_t ) );
	}

	// attribs
	uint32_t attrib;
	while( ! in->isEof() ) {
		in->readLittle( &attrib );

		switch( fromMask( attrib ) ) {
			case geom::POSITION:
				readAttribf( &mPositionsDims, &mPositions );
			break;
			case geom::COLOR:
				readAttribf( &mColorsDims, &mColors );
			break;
			case geom::NORMAL:
				readAttribVec3f( &mNormalsDims, &mNormals );
			break;
			case geom::TEX_COORD_0:
				readAttribf( &mTexCoords0Dims, &mTexCoords0 );
			break;
			case geom::TEX_COORD_1:
				readAttribf( &mTexCoords1Dims, &mTexCoords1 );
			break;
			case geom::TEX_COORD_2:
				readAttribf( &mTexCoords2Dims, &mTexCoords2 );
			break;
			case geom::TEX_COORD_3:
				readAttribf( &mTexCoords3Dims, &mTexCoords3 );
			break;
			case geom::TANGENT:
				readAttribVec3f( &mTangentsDims, &mTangents );
			break;
			case geom::BITANGENT:
				readAttribVec3f( &mBitangentsDims, &mBitangents );
			break;
			default:
				throw Exception( "TriMesh::read() error: Invalid file contents." );
			break;
		}
	}
}

// used in 0.8.6 and early glNext
void TriMesh::readImplV1( const IStreamRef &in )
{
	uint32_t numPositions, numNormals, numTexCoords, numIndices;
	in->readLittle( &numPositions );
	in->readLittle( &numNormals );
	in->readLittle( &numTexCoords );
	in->readLittle( &numIndices );

	for( size_t idx = 0; idx < numPositions; ++idx ) {
		for( int v = 0; v < 3; ++v ) {
			float f;
			in->readLittle( &f );
			mPositions.push_back( f );
		}
	}

	for( size_t idx = 0; idx < numNormals; ++idx ) {
		vec3 v;
		in->readLittle( &v.x ); in->readLittle( &v.y ); in->readLittle( &v.z );
		mNormals.push_back( v );
	}

	for( size_t idx = 0; idx < numTexCoords; ++idx ) {
		for( int v = 0; v < 2; ++v ) {
			float f;
			in->readLittle( &f );
			mTexCoords0.push_back( f );
		}
	}

	for( size_t idx = 0; idx < numIndices; ++idx ) {
		uint32_t v;
		in->readLittle( &v );
		mIndices.push_back( v );
	}

	mPositionsDims = 3;
	mTexCoords0Dims = 2;
}

bool TriMesh::recalculateNormals( bool smooth, bool weighted )
{
	// requires valid indices and 3D vertices
	if( mIndices.empty() || mPositions.empty() || mPositionsDims != 3 )
		return false;

	size_t numPositions = mPositions.size() / 3;
	mNormals.assign( numPositions, vec3() );

	// for smooth renormalization, we first find all unique vertices and keep track of them
	std::vector<uint32_t> uniquePositions;
	if( smooth ) {
		uniquePositions.assign( numPositions, 0 );

		for( uint32_t i = 0; i < numPositions; ++i ) {
			if( uniquePositions[i] == 0 ) {
				uniquePositions[i] = i + 1;
				const vec3 &v0 = *(const vec3*)(&mPositions[i * 3]);
				for( size_t j = i + 1; j < numPositions; ++j ) {
					const vec3 &v1 = *(const vec3*)(&mPositions[j * 3]);
					if( length2( v1 - v0 ) < FLT_EPSILON )
						uniquePositions[j] = uniquePositions[i];
				}
			}
		}
	}

	// perform surface normalization
	uint32_t index0, index1, index2;
	size_t numTriangles = getNumTriangles();
	for( size_t i = 0; i < numTriangles; ++i ) {
		if( smooth ) {
			index0 = uniquePositions[mIndices[i * 3 + 0]] - 1;
			index1 = uniquePositions[mIndices[i * 3 + 1]] - 1;
			index2 = uniquePositions[mIndices[i * 3 + 2]] - 1;
		}
		else {
			index0 = mIndices[i*3+0];
			index1 = mIndices[i*3+1];
			index2 = mIndices[i*3+2];
		}

		const vec3 &v0 = *(const vec3*)(&mPositions[index0 * 3]);
		const vec3 &v1 = *(const vec3*)(&mPositions[index1 * 3]);
		const vec3 &v2 = *(const vec3*)(&mPositions[index2 * 3]);

		vec3 e0 = v1 - v0;
		vec3 e1 = v2 - v0;
		vec3 e2 = v2 - v1;

		if( length2( e0 ) < FLT_EPSILON )
			continue;
		if( length2( e1 ) < FLT_EPSILON )
			continue;
		if( length2( e2 ) < FLT_EPSILON )
			continue;

		vec3 normal = cross( e0, e1 );

		// if not weighted, every normal has an equal contribution
		if( ! weighted )
			normal = normalize( normal );

		mNormals[ index0 ] += normal;
		mNormals[ index1 ] += normal;
		mNormals[ index2 ] += normal;
	}

	// now normalize the summed normals
	for( auto &normal : mNormals ) {
		normal = normalize( normal );
	}

	// copy normals to corresponding non-unique vertices
	if( smooth ) {
		for( size_t i = 0; i < numPositions; ++i ) {
			mNormals[i] = mNormals[uniquePositions[i]-1];
		}
	}

	mNormalsDims = 3;

	return true;
}

bool TriMesh::recalculateTangents()
{
	// requires valid 2D texture coords and 3D normals
	if( mTexCoords0.empty() || mTexCoords0Dims != 2 )
		return false;

	if( ! hasNormals() )
		return false;

	mTangents.clear();

	const vec3 *positions = reinterpret_cast<const vec3*>( mPositions.data() );
	const vec3 *normals = reinterpret_cast<const vec3*>( mNormals.data() );
	const vec2 *texCoords = reinterpret_cast<const vec2*>( mTexCoords0.data() );
	geom::calculateTangents( mIndices.size(), mIndices.data(), mPositions.size() / 3, positions, normals, texCoords, &mTangents, nullptr );

	mTangentsDims = 3;

	return true;
}

bool TriMesh::recalculateBitangents()
{
	// requires valid 3D tangents and normals
	if( ! ( hasTangents() || recalculateTangents() ) )
		return false;

	mBitangents.assign( mNormals.size(), vec3() );

	size_t n = getNumVertices();
	for( size_t i = 0; i < n; ++i )
		mBitangents[i] = normalize( cross( mNormals[i], mTangents[i] ) );

	mBitangentsDims = 3;

	return true;
}

//! TODO: optimize memory allocations
void TriMesh::subdivide( int division, bool normalize )
{
	if( division < 2 )
		return;

	// keep track of newly added vertices (enough for a single subdivided triangle)
	const int numVerticesPerTriangle = (division + 2) * (division + 1) / 2;
	std::vector<uint32_t> indices(numVerticesPerTriangle);

	// subdivide a single triangle at a time
	size_t numTriangles = getNumTriangles();
	for( size_t t = 0; t < numTriangles; ++t ) {
		// the original indices forming the triangle
		const uint32_t index0 = mIndices[t * 3 + 0];
		const uint32_t index1 = mIndices[t * 3 + 1];
		const uint32_t index2 = mIndices[t * 3 + 2];

		// keep track of how many vertices we have added
		size_t n = 0;

		const float rcp = 1.f / division;
		for( int j = 0; j <= division; ++j ) {
			const float div = 1.f / (division - j);
			for( int i = 0; i <= (division - j); ++i ) {
				if( i == 0 && j == 0) {
					indices[n++] = index0;
				}
				else if( i == division ) {
					indices[n++] = index1;
				}
				else if( j == division ) {
					indices[n++] = index2;
				}
				else {
					indices[n++] = (uint32_t)getNumVertices();

					// lambda closures for bilinear interpolation. TODO: make private templated function to reduce code reduncancy
					auto lerpBilinear2 = [&] (const vec2 &a, const vec2 &b, const vec2 &c) {
						const vec2 d = mix( a, c, j * rcp );
						const vec2 e = mix( b, c, j * rcp );
						return mix( d, e, i * div );
					};
					
					auto lerpBilinear3 = [&] (const vec3 &a, const vec3 &b, const vec3 &c) {
						const vec3 d = mix( a, c, j * rcp );
						const vec3 e = mix( b, c, j * rcp );
						return mix( d, e, i * div );
					};
					
					auto lerpBilinear4 = [&] (const vec4 &a, const vec4 &b, const vec4 &c) {
						const vec4 d = mix( a, c, j * rcp );
						const vec4 e = mix( b, c, j * rcp );
						return mix( d, e, i * div );
					};

					// generate interpolated vertex and its attributes (warning: massive boilerplate code incoming!)
					if( mPositionsDims == 2 ) {
						const vec2 &v0 = *(const vec2*)(&mPositions[index0*2]);
						const vec2 &v1 = *(const vec2*)(&mPositions[index1*2]);
						const vec2 &v2 = *(const vec2*)(&mPositions[index2*2]);
						appendPosition( lerpBilinear2( v0, v1, v2 ) );
					}
					else if( mPositionsDims == 3 ) {
						const vec3 &v0 = *(const vec3*)(&mPositions[index0*3]);
						const vec3 &v1 = *(const vec3*)(&mPositions[index1*3]);
						const vec3 &v2 = *(const vec3*)(&mPositions[index2*3]);
						appendPosition( lerpBilinear3( v0, v1, v2 ) );
					}
					else if( mPositionsDims == 4 ) {
						const vec4 &v0 = *(const vec4*)(&mPositions[index0*4]);
						const vec4 &v1 = *(const vec4*)(&mPositions[index1*4]);
						const vec4 &v2 = *(const vec4*)(&mPositions[index2*4]);
						appendPosition( lerpBilinear4( v0, v1, v2 ) );
					}

					if( hasNormals() ) {
						const vec3 &v0 = mNormals[index0];
						const vec3 &v1 = mNormals[index1];
						const vec3 &v2 = mNormals[index2];
						appendNormal( lerpBilinear3( v0, v1, v2 ) );
					}

					if( hasTangents() ) {
						const vec3 &v0 = mTangents[index0];
						const vec3 &v1 = mTangents[index1];
						const vec3 &v2 = mTangents[index2];
						appendTangent( lerpBilinear3( v0, v1, v2 ) );
					}

					if( hasBitangents() ) {
						const vec3 &v0 = mBitangents[index0];
						const vec3 &v1 = mBitangents[index1];
						const vec3 &v2 = mBitangents[index2];
						appendTangent( lerpBilinear3( v0, v1, v2 ) );
					}

					if( hasColorsRgb() ) {
						const vec3 &v0 = *(const vec3*)(&mColors[index0*3]);
						const vec3 &v1 = *(const vec3*)(&mColors[index1*3]);
						const vec3 &v2 = *(const vec3*)(&mColors[index2*3]);
						const vec3 c = lerpBilinear3( v0, v1, v2 );
						appendColorRgb( Color( c.x, c.y, c.z ) );
					}
					else if( hasColorsRgba() ) {
						const vec4 &v0 = *(const vec4*)(&mColors[index0*4]);
						const vec4 &v1 = *(const vec4*)(&mColors[index1*4]);
						const vec4 &v2 = *(const vec4*)(&mColors[index2*4]);
						const vec4 c = lerpBilinear4( v0, v1, v2 );
						appendColorRgba( ColorA( c.x, c.y, c.z, c.w ) );
					}

					if( hasTexCoords0() ) {
						if( mTexCoords0Dims == 2 ) {
							const vec2 &v0 = *(const vec2*)(&mTexCoords0[index0*2]);
							const vec2 &v1 = *(const vec2*)(&mTexCoords0[index1*2]);
							const vec2 &v2 = *(const vec2*)(&mTexCoords0[index2*2]);
							appendTexCoord0( lerpBilinear2( v0, v1, v2 ) );
						}
						else if( mTexCoords0Dims == 3 ) {
							const vec3 &v0 = *(const vec3*)(&mTexCoords0[index0*3]);
							const vec3 &v1 = *(const vec3*)(&mTexCoords0[index1*3]);
							const vec3 &v2 = *(const vec3*)(&mTexCoords0[index2*3]);
							appendTexCoord0( lerpBilinear3( v0, v1, v2 ) );
						}
						else if( mTexCoords0Dims == 4 ) {
							const vec4 &v0 = *(const vec4*)(&mTexCoords0[index0*4]);
							const vec4 &v1 = *(const vec4*)(&mTexCoords0[index1*4]);
							const vec4 &v2 = *(const vec4*)(&mTexCoords0[index2*4]);
							appendTexCoord0( lerpBilinear4( v0, v1, v2 ) );
						}
					}

					if( hasTexCoords1() ) {
						if( mTexCoords1Dims == 2 ) {
							const vec2 &v0 = *(const vec2*)(&mTexCoords1[index0*2]);
							const vec2 &v1 = *(const vec2*)(&mTexCoords1[index1*2]);
							const vec2 &v2 = *(const vec2*)(&mTexCoords1[index2*2]);
							appendTexCoord1( lerpBilinear2( v0, v1, v2 ) );
						}
						else if( mTexCoords1Dims == 3 ) {
							const vec3 &v0 = *(const vec3*)(&mTexCoords1[index0*3]);
							const vec3 &v1 = *(const vec3*)(&mTexCoords1[index1*3]);
							const vec3 &v2 = *(const vec3*)(&mTexCoords1[index2*3]);
							appendTexCoord1( lerpBilinear3( v0, v1, v2 ) );
						}
						else if( mTexCoords1Dims == 4 ) {
							const vec4 &v0 = *(const vec4*)(&mTexCoords1[index0*4]);
							const vec4 &v1 = *(const vec4*)(&mTexCoords1[index1*4]);
							const vec4 &v2 = *(const vec4*)(&mTexCoords1[index2*4]);
							appendTexCoord1( lerpBilinear4( v0, v1, v2 ) );
						}
					}

					if( hasTexCoords2() ) {
						if( mTexCoords2Dims == 2 ) {
							const vec2 &v0 = *(const vec2*)(&mTexCoords2[index0*2]);
							const vec2 &v1 = *(const vec2*)(&mTexCoords2[index1*2]);
							const vec2 &v2 = *(const vec2*)(&mTexCoords2[index2*2]);
							appendTexCoord2( lerpBilinear2( v0, v1, v2 ) );
						}
						else if( mTexCoords2Dims == 3 ) {
							const vec3 &v0 = *(const vec3*)(&mTexCoords2[index0*3]);
							const vec3 &v1 = *(const vec3*)(&mTexCoords2[index1*3]);
							const vec3 &v2 = *(const vec3*)(&mTexCoords2[index2*3]);
							appendTexCoord2( lerpBilinear3( v0, v1, v2 ) );
						}
						else if( mTexCoords2Dims == 4 ) {
							const vec4 &v0 = *(const vec4*)(&mTexCoords2[index0*4]);
							const vec4 &v1 = *(const vec4*)(&mTexCoords2[index1*4]);
							const vec4 &v2 = *(const vec4*)(&mTexCoords2[index2*4]);
							appendTexCoord2( lerpBilinear4( v0, v1, v2 ) );
						}
					}

					if( hasTexCoords3() ) {
						if( mTexCoords3Dims == 2 ) {
							const vec2 &v0 = *(const vec2*)(&mTexCoords3[index0*2]);
							const vec2 &v1 = *(const vec2*)(&mTexCoords3[index1*2]);
							const vec2 &v2 = *(const vec2*)(&mTexCoords3[index2*2]);
							appendTexCoord3( lerpBilinear2( v0, v1, v2 ) );
						}
						else if( mTexCoords3Dims == 3 ) {
							const vec3 &v0 = *(const vec3*)(&mTexCoords3[index0*3]);
							const vec3 &v1 = *(const vec3*)(&mTexCoords3[index1*3]);
							const vec3 &v2 = *(const vec3*)(&mTexCoords3[index2*3]);
							appendTexCoord3( lerpBilinear3( v0, v1, v2 ) );
						}
						else if( mTexCoords3Dims == 4 ) {
							const vec4 &v0 = *(const vec4*)(&mTexCoords3[index0*4]);
							const vec4 &v1 = *(const vec4*)(&mTexCoords3[index1*4]);
							const vec4 &v2 = *(const vec4*)(&mTexCoords3[index2*4]);
							appendTexCoord3( lerpBilinear4( v0, v1, v2 ) );
						}
					}
				}
			}
		}

		// create new triangles by adding the newly generated vertices to the index buffer
		int d = division, m = 0;
		for( int j = 0; j < numVerticesPerTriangle; ++j ) {
			if( j == 0 ) {
				// adjust existing triangle indices now
				mIndices[t*3+0] = indices[j];
				mIndices[t*3+1] = indices[j+1];
				mIndices[t*3+2] = indices[j+1+d];
			}
			else if( (j - m) < d ) {
				appendTriangle( indices[j], indices[j+1], indices[j+1+d] );

				if( (j - m) > 0 )
					appendTriangle( indices[j], indices[j+1+d], indices[j+d] );
			}
			else {
				m += (d + 1);
				d--;
			}
		}
	}

	// normalize
	if( normalize ) {
		size_t numVertices = getNumVertices();
		if( mPositionsDims == 2 ) {
			for( size_t i = 0; i < numVertices; ++i ) {
				vec2 &v = *(vec2*)(&mPositions[i*2]);
				v = glm::normalize( v );
			}
		}
		else if( mPositionsDims == 3 ) {
			for( size_t i = 0; i < numVertices; ++i ) {
				vec3 &v = *(vec3*)(&mPositions[i*3]);
				v = glm::normalize( v );
			}
		}
		else if( mPositionsDims == 4 ) {
			for( size_t i = 0; i < numVertices; ++i ) {
				vec4 &v = *(vec4*)(&mPositions[i*4]);
				v = glm::normalize( v );
			}
		}
	}
}

uint8_t TriMesh::getAttribDims( geom::Attrib attr ) const
{
	switch( attr ) {
		case geom::Attrib::POSITION: return mPositionsDims;
		case geom::Attrib::COLOR: return mColorsDims;
		case geom::Attrib::TEX_COORD_0: return mTexCoords0Dims;
		case geom::Attrib::TEX_COORD_1: return mTexCoords1Dims;
		case geom::Attrib::TEX_COORD_2: return mTexCoords2Dims;
		case geom::Attrib::TEX_COORD_3: return mTexCoords3Dims;
		case geom::Attrib::NORMAL: return mNormalsDims;
		case geom::Attrib::TANGENT: return mTangentsDims;
		case geom::Attrib::BITANGENT: return mBitangentsDims;
		default:
			return 0;
	}
}

geom::AttribSet	TriMesh::getAvailableAttribs() const
{
	geom::AttribSet result;

	if( mPositionsDims ) result.insert( geom::Attrib::POSITION );
	if( mColorsDims ) result.insert( geom::Attrib::COLOR );
	if( mTexCoords0Dims ) result.insert( geom::Attrib::TEX_COORD_0 );
	if( mTexCoords1Dims ) result.insert( geom::Attrib::TEX_COORD_1 );
	if( mTexCoords2Dims ) result.insert( geom::Attrib::TEX_COORD_2 );
	if( mTexCoords3Dims ) result.insert( geom::Attrib::TEX_COORD_3 );
	if( mNormalsDims ) result.insert( geom::Attrib::NORMAL );
	if( mTangentsDims ) result.insert( geom::Attrib::TANGENT );
	if( mBitangentsDims ) result.insert( geom::Attrib::BITANGENT );

	return result;
}

void TriMesh::getAttribPointer( geom::Attrib attr, const float **resultPtr, size_t *resultStrideBytes, uint8_t *resultDims ) const
{
	switch( attr ) {
		case geom::Attrib::POSITION: *resultPtr = (const float*)mPositions.data(); *resultStrideBytes = 0; *resultDims = mPositionsDims; break;
		case geom::Attrib::COLOR: *resultPtr = (const float*)mColors.data(); *resultStrideBytes = 0; *resultDims = mColorsDims; break;
		case geom::Attrib::TEX_COORD_0: *resultPtr = (const float*)mTexCoords0.data(); *resultStrideBytes = 0; *resultDims = mTexCoords0Dims; break;
		case geom::Attrib::TEX_COORD_1: *resultPtr = (const float*)mTexCoords1.data(); *resultStrideBytes = 0; *resultDims = mTexCoords1Dims; break;
		case geom::Attrib::TEX_COORD_2: *resultPtr = (const float*)mTexCoords2.data(); *resultStrideBytes = 0; *resultDims = mTexCoords2Dims; break;
		case geom::Attrib::TEX_COORD_3: *resultPtr = (const float*)mTexCoords3.data(); *resultStrideBytes = 0; *resultDims = mTexCoords3Dims; break;
		case geom::Attrib::NORMAL: *resultPtr = (const float*)mNormals.data(); *resultStrideBytes = 0; *resultDims = mNormalsDims; break;
		case geom::Attrib::TANGENT: *resultPtr = (const float*)mTangents.data(); *resultStrideBytes = 0; *resultDims = mTangentsDims; break;
		case geom::Attrib::BITANGENT: *resultPtr = (const float*)mBitangents.data(); *resultStrideBytes = 0; *resultDims = mBitangentsDims; break;
		default:
			*resultPtr = nullptr; *resultStrideBytes = 0; *resultDims = 0;
	}
}

void TriMesh::copyAttrib( geom::Attrib attr, uint8_t dims, size_t /*stride*/, const float *srcData, size_t numVertices )
{
	if( getAttribDims( attr ) == 0 )
		return;

	switch( attr ) {
		case geom::Attrib::POSITION:
			mPositions.resize( mPositionsDims * numVertices );
			geom::copyData( dims, srcData, numVertices, mPositionsDims, 0, mPositions.data() );
		break;
		case geom::Attrib::COLOR:
			mColors.resize( mColorsDims * numVertices );
			geom::copyData( dims, srcData, numVertices, mColorsDims, 0, mColors.data() );
		break;
		case geom::Attrib::TEX_COORD_0:
			mTexCoords0.resize( mTexCoords0Dims * numVertices );
			geom::copyData( dims, srcData, numVertices, mTexCoords0Dims, 0, mTexCoords0.data() );
		break;
		case geom::Attrib::TEX_COORD_1:
			mTexCoords1.resize( mTexCoords1Dims * numVertices );
			geom::copyData( dims, srcData, numVertices, mTexCoords1Dims, 0, mTexCoords1.data() );
		break;
		case geom::Attrib::TEX_COORD_2:
			mTexCoords2.resize( mTexCoords2Dims * numVertices );
			geom::copyData( dims, srcData, numVertices, mTexCoords2Dims, 0, mTexCoords2.data() );
		break;
		case geom::Attrib::TEX_COORD_3:
			mTexCoords3.resize( mTexCoords3Dims * numVertices );
			geom::copyData( dims, srcData, numVertices, mTexCoords3Dims, 0, mTexCoords3.data() );
		break;
		case geom::Attrib::NORMAL:
			mNormals.resize( numVertices );
			geom::copyData( dims, srcData, numVertices, 3, 0, (float*)mNormals.data() );
		break;
		case geom::Attrib::TANGENT:
			mTangents.resize( numVertices );
			geom::copyData( dims, srcData, numVertices, 3, 0, (float*)mTangents.data() );
		break;
		case geom::Attrib::BITANGENT:
			mBitangents.resize( numVertices );
			geom::copyData( dims, srcData, numVertices, 3, 0, (float*)mBitangents.data() );
		break;
		default:
			throw geom::ExcMissingAttrib();
	}
}

bool TriMesh::verticesEqual( uint32_t indexA, uint32_t indexB ) const
{
	{
		const size_t numPositions = getNumVertices();
		if (indexA >= numPositions || indexB >= numPositions)
			return false;
	}

	// positions
	{
		const vec3 &a = *reinterpret_cast<const vec3*>(&mPositions[indexA*mPositionsDims]);
		const vec3 &b = *reinterpret_cast<const vec3*>(&mPositions[indexB*mPositionsDims]);
		if( distance2( a, b ) > FLT_EPSILON)
			return false;
	}

	if( mColorsDims > 0 ) {
		if( mColorsDims == 3 ) {
			const vec3 &a = *reinterpret_cast<const vec3*>(&mColors[indexA*mColorsDims]);
			const vec3 &b = *reinterpret_cast<const vec3*>(&mColors[indexB*mColorsDims]);
			if( distance2( a, b ) > FLT_EPSILON )
			return false;
		}
		else if( mColorsDims == 4 ) {
			const vec4 &a = *reinterpret_cast<const vec4*>(&mColors[indexA*mColorsDims]);
			const vec4 &b = *reinterpret_cast<const vec4*>(&mColors[indexB*mColorsDims]);
			if( distance2( a, b ) > FLT_EPSILON )
			return false;
		}
	}

	if( mNormalsDims > 0 ) {
		const vec3 &a = *reinterpret_cast<const vec3*>(&mNormals[indexA*mNormalsDims]);
		const vec3 &b = *reinterpret_cast<const vec3*>(&mNormals[indexB*mNormalsDims]);
		if( distance2( a, b ) > FLT_EPSILON )
		return false;
	}

	if( mTexCoords0Dims > 0 ) {
		if( mTexCoords0Dims == 2 ) {
			const vec2 &a = *reinterpret_cast<const vec2*>(&mTexCoords0[indexA*mTexCoords0Dims]);
			const vec2 &b = *reinterpret_cast<const vec2*>(&mTexCoords0[indexB*mTexCoords0Dims]);
			if( distance2( a, b ) > FLT_EPSILON )
			return false;
		}
		else if( mTexCoords0Dims == 3 ) {
			const vec3 &a = *reinterpret_cast<const vec3*>(&mTexCoords0[indexA*mTexCoords0Dims]);
			const vec3 &b = *reinterpret_cast<const vec3*>(&mTexCoords0[indexB*mTexCoords0Dims]);
			if( distance2( a, b ) > FLT_EPSILON )
			return false;
		}
		else if( mTexCoords0Dims == 4 ) {
			const vec4 &a = *reinterpret_cast<const vec4*>(&mTexCoords0[indexA*mTexCoords0Dims]);
			const vec4 &b = *reinterpret_cast<const vec4*>(&mTexCoords0[indexB*mTexCoords0Dims]);
			if( distance2( a, b ) > FLT_EPSILON )
			return false;
		}
	}

	if( mTexCoords1Dims > 0 ) {
		if( mTexCoords1Dims == 2 ) {
			const vec2 &a = *reinterpret_cast<const vec2*>(&mTexCoords1[indexA*mTexCoords1Dims]);
			const vec2 &b = *reinterpret_cast<const vec2*>(&mTexCoords1[indexB*mTexCoords1Dims]);
			if( distance2( a, b ) > FLT_EPSILON )
			return false;
		}
		else if( mTexCoords1Dims == 3 ) {
			const vec3 &a = *reinterpret_cast<const vec3*>(&mTexCoords1[indexA*mTexCoords1Dims]);
			const vec3 &b = *reinterpret_cast<const vec3*>(&mTexCoords1[indexB*mTexCoords1Dims]);
			if( distance2( a, b ) > FLT_EPSILON )
			return false;
		}
		else if( mTexCoords1Dims == 4 ) {
			const vec4 &a = *reinterpret_cast<const vec4*>(&mTexCoords1[indexA*mTexCoords1Dims]);
			const vec4 &b = *reinterpret_cast<const vec4*>(&mTexCoords1[indexB*mTexCoords1Dims]);
			if( distance2( a, b ) > FLT_EPSILON )
			return false;
		}
	}

	if( mTexCoords2Dims > 0 ) {
		if( mTexCoords2Dims == 2 ) {
			const vec2 &a = *reinterpret_cast<const vec2*>(&mTexCoords2[indexA*mTexCoords2Dims]);
			const vec2 &b = *reinterpret_cast<const vec2*>(&mTexCoords2[indexB*mTexCoords2Dims]);
			if( distance2( a, b ) > FLT_EPSILON )
			return false;
		}
		else if( mTexCoords2Dims == 3 ) {
			const vec3 &a = *reinterpret_cast<const vec3*>(&mTexCoords2[indexA*mTexCoords2Dims]);
			const vec3 &b = *reinterpret_cast<const vec3*>(&mTexCoords2[indexB*mTexCoords2Dims]);
			if( distance2( a, b ) > FLT_EPSILON )
			return false;
		}
		else if( mTexCoords2Dims == 4 ) {
			const vec4 &a = *reinterpret_cast<const vec4*>(&mTexCoords2[indexA*mTexCoords2Dims]);
			const vec4 &b = *reinterpret_cast<const vec4*>(&mTexCoords2[indexB*mTexCoords2Dims]);
			if( distance2( a, b ) > FLT_EPSILON )
			return false;
		}
	}

	if( mTexCoords3Dims > 0 ) {
		if( mTexCoords3Dims == 2 ) {
			const vec2 &a = *reinterpret_cast<const vec2*>(&mTexCoords3[indexA*mTexCoords3Dims]);
			const vec2 &b = *reinterpret_cast<const vec2*>(&mTexCoords3[indexB*mTexCoords3Dims]);
			if( distance2( a, b ) > FLT_EPSILON )
			return false;
		}
		else if( mTexCoords3Dims == 3 ) {
			const vec3 &a = *reinterpret_cast<const vec3*>(&mTexCoords3[indexA*mTexCoords3Dims]);
			const vec3 &b = *reinterpret_cast<const vec3*>(&mTexCoords3[indexB*mTexCoords3Dims]);
			if( distance2( a, b ) > FLT_EPSILON )
			return false;
		}
		else if( mTexCoords3Dims == 4 ) {
			const vec4 &a = *reinterpret_cast<const vec4*>(&mTexCoords3[indexA*mTexCoords3Dims]);
			const vec4 &b = *reinterpret_cast<const vec4*>(&mTexCoords3[indexB*mTexCoords3Dims]);
			if( distance2( a, b ) > FLT_EPSILON )
			return false;
		}
	}

	if( mTangentsDims > 0 ) {
		const vec3 &a = *reinterpret_cast<const vec3*>(&mTangents[indexA*mTangentsDims]);
		const vec3 &b = *reinterpret_cast<const vec3*>(&mTangents[indexB*mTangentsDims]);
		if( distance2( a, b ) > FLT_EPSILON )
		return false;
	}

	if( mBitangentsDims > 0 ) {
		const vec3 &a = *reinterpret_cast<const vec3*>(&mBitangents[indexA*mBitangentsDims]);
		const vec3 &b = *reinterpret_cast<const vec3*>(&mBitangents[indexB*mBitangentsDims]);
		if( distance2( a, b ) > FLT_EPSILON )
		return false;
	}

	// TODO: bone index and weight

	return true;
}

uint32_t TriMesh::toMask( geom::Attrib attrib )
{
	switch( attrib ) {
		case geom::POSITION:	return 0x00000001;
		case geom::COLOR:		return 0x00000002;
		case geom::TEX_COORD_0: return 0x00000004;
		case geom::TEX_COORD_1: return 0x00000008;
		case geom::TEX_COORD_2: return 0x00000010;
		case geom::TEX_COORD_3: return 0x00000020;
		case geom::NORMAL:		return 0x00000100;
		case geom::TANGENT:		return 0x00000200;
		case geom::BITANGENT:	return 0x00000400;
		case geom::BONE_INDEX:	return 0x00001000;
		case geom::BONE_WEIGHT:	return 0x00002000;
		case geom::CUSTOM_0:	return 0x00010000;
		case geom::CUSTOM_1:	return 0x00020000;
		case geom::CUSTOM_2:	return 0x00040000;
		case geom::CUSTOM_3:	return 0x00080000;
		case geom::CUSTOM_4:	return 0x00100000;
		case geom::CUSTOM_5:	return 0x00200000;
		case geom::CUSTOM_6:	return 0x00400000;
		case geom::CUSTOM_7:	return 0x00800000;
		case geom::CUSTOM_8:	return 0x01000000;
		case geom::CUSTOM_9:	return 0x02000000;
		default:
			throw Exception( "TriMesh i/o error: Unexpected attrib" );
			return 0;
	}
}

geom::Attrib TriMesh::fromMask( uint32_t attrib )
{
	switch( attrib ) {
		case 0x00000001: return geom::POSITION;
		case 0x00000002: return geom::COLOR;
		case 0x00000004: return geom::TEX_COORD_0;
		case 0x00000008: return geom::TEX_COORD_1;
		case 0x00000010: return geom::TEX_COORD_2;
		case 0x00000020: return geom::TEX_COORD_3;
		case 0x00000100: return geom::NORMAL;
		case 0x00000200: return geom::TANGENT;
		case 0x00000400: return geom::BITANGENT;
		case 0x00001000: return geom::BONE_INDEX;
		case 0x00002000: return geom::BONE_WEIGHT;
		case 0x00010000: return geom::CUSTOM_0;
		case 0x00020000: return geom::CUSTOM_1;
		case 0x00040000: return geom::CUSTOM_2;
		case 0x00080000: return geom::CUSTOM_3;
		case 0x00100000: return geom::CUSTOM_4;
		case 0x00200000: return geom::CUSTOM_5;
		case 0x00400000: return geom::CUSTOM_6;
		case 0x00800000: return geom::CUSTOM_7;
		case 0x01000000: return geom::CUSTOM_8;
		case 0x02000000: return geom::CUSTOM_9;
		default:
			throw Exception( "TriMesh i/o error: Invalid mask" );
			return geom::NUM_ATTRIBS;
	}
}

} // namespace cinder
