#include "cinder/TriMesh.h"
#include "cinder/app/AppBasic.h"

using std::vector;

namespace cinder {

/////////////////////////////////////////////////////////////////////////////////////////////////
// TriMeshGeomTarget
class TriMeshGeomTarget : public geom::Target {
  public:
	TriMeshGeomTarget( TriMesh *mesh )
		: mMesh( mesh )
	{}
	
	virtual uint8_t	getAttribDims( geom::Attrib attr ) const override;
	virtual void copyAttrib( geom::Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count ) override;
	virtual void copyIndices( geom::Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex ) override;
	
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

void TriMeshGeomTarget::copyIndices( geom::Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex )
{
	mMesh->mIndices.resize( numIndices );
	copyIndexDataForceTriangles( primitive, source, numIndices, mMesh->mIndices.data() );
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

void TriMesh::appendVertices( const vec2 *verts, size_t num )
{
	assert( mPositionsDims == 2 );
	mPositions.insert( mPositions.end(), (const float*)verts, (const float*)verts + num * 2 );
}

void TriMesh::appendVertices( const vec3 *verts, size_t num )
{
	assert( mPositionsDims == 3 );
	mPositions.insert( mPositions.end(), (const float*)verts, (const float*)verts + num * 3 );
}

void TriMesh::appendVertices( const vec4 *verts, size_t num )
{
	assert( mPositionsDims == 4 );
	mPositions.insert( mPositions.end(), (const float*)verts, (const float*)verts + num * 4 );
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

AxisAlignedBox3f TriMesh::calcBoundingBox() const
{
	assert( mPositionsDims == 3 );
	if( mPositions.empty() )
		return AxisAlignedBox3f( vec3(), vec3() );

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
	
	return AxisAlignedBox3f( min, max );
}

AxisAlignedBox3f TriMesh::calcBoundingBox( const mat4 &transform ) const
{
	assert( mPositionsDims == 3 );
	if( mPositions.empty() )
		return AxisAlignedBox3f( vec3(), vec3() );

	const vec3 &temp = *(const vec3*)(&mPositions[0]);
	vec3 min = vec3( transform * vec4( temp, 1 ) );
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

	return AxisAlignedBox3f( min, max );
}


void TriMesh::read( DataSourceRef dataSource )
{
	IStreamRef in = dataSource->createStream();
	clear();

	uint8_t versionNumber;
	in->read( &versionNumber );
	
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

void TriMesh::write( DataTargetRef dataTarget ) const
{
	assert(mPositionsDims == 3);
	assert(mTexCoords0Dims == 2);

	// note: tangents and bitangents are not written, because these can be reconstructed

	OStreamRef out = dataTarget->getStream();
	
	const uint8_t versionNumber = 1;
	out->write( versionNumber );
	
	out->writeLittle( static_cast<uint32_t>( mPositions.size() / 3 ) );
	out->writeLittle( static_cast<uint32_t>( mNormals.size() ) );
	out->writeLittle( static_cast<uint32_t>( mTexCoords0.size() / 2 ) );
	out->writeLittle( static_cast<uint32_t>( mIndices.size() ) );
	
	for( auto it = mPositions.begin(); it != mPositions.end(); ++it ) {
		out->writeLittle( *it );
	}

	for( vector<vec3>::const_iterator it = mNormals.begin(); it != mNormals.end(); ++it ) {
		out->writeLittle( it->x ); out->writeLittle( it->y ); out->writeLittle( it->z );
	}

	for( auto it = mTexCoords0.begin(); it != mTexCoords0.end(); ++it ) {
		out->writeLittle( *it );
	}

	for( vector<uint32_t>::const_iterator it = mIndices.begin(); it != mIndices.end(); ++it ) {
		out->writeLittle( *it );
	}
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
		if( ! weighted )
			normal = normalize( normal );

		mNormals[ index0 ] += normal;
		mNormals[ index1 ] += normal;
		mNormals[ index2 ] += normal;
	}

	// ???: Is this extra normalizing necessary? It is done in the above loop if weighted = true
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

// Code taken from:
// Lengyel, Eric. "Computing Tangent Space Basis Vectors for an Arbitrary Mesh". 
// Terathon Software 3D Graphics Library, 2001.
// http://www.terathon.com/code/tangent.html
bool TriMesh::recalculateTangents()
{
	// requires valid 2D texture coords and 3D normals
	if( mTexCoords0.empty() || mTexCoords0Dims != 2 )
		return false;

	if( ! hasNormals() )
		return false;

	mTangents.assign( mNormals.size(), vec3() );

	size_t n = getNumTriangles();
	for( size_t i = 0; i < n; ++i ) {
		uint32_t index0 = mIndices[i * 3];
		uint32_t index1 = mIndices[i * 3 + 1];
		uint32_t index2 = mIndices[i * 3 + 2];

		const vec3 &v0 = *(const vec3*)(&mPositions[index0*3]);
		const vec3 &v1 = *(const vec3*)(&mPositions[index1*3]);
		const vec3 &v2 = *(const vec3*)(&mPositions[index2*3]);

		const vec2 &w0 = *(const vec2*)(&mTexCoords0[index0*2]);
		const vec2 &w1 = *(const vec2*)(&mTexCoords0[index1*2]);
		const vec2 &w2 = *(const vec2*)(&mTexCoords0[index2*2]);

		float x1 = v1.x - v0.x;
		float x2 = v2.x - v0.x;
		float y1 = v1.y - v0.y;
		float y2 = v2.y - v0.y;
		float z1 = v1.z - v0.z;
		float z2 = v2.z - v0.z;

		float s1 = w1.x - w0.x;
		float s2 = w2.x - w0.x;
		float t1 = w1.y - w0.y;
		float t2 = w2.y - w0.y;

		float r = 1.0f / (s1 * t2 - s2 * t1);
		vec3 tangent((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);

		mTangents[ index0 ] += tangent;
		mTangents[ index1 ] += tangent;
		mTangents[ index2 ] += tangent;
	}

	n = getNumVertices();
	for( size_t i = 0; i < n; ++i ) {
		vec3 normal = mNormals[i];
		vec3 tangent = mTangents[i];
		mTangents[i] = normalize( tangent - normal * dot( normal, tangent ) );
	}

	mTangentsDims = 3;

	return true;
}

bool TriMesh::recalculateBitangents()
{
	// requires valid 3D tangents and normals
	if( !(hasTangents() || recalculateTangents()) )
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
	const uint32_t numVerticesPerTriangle = (division + 2) * (division + 1) / 2;
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
						appendVertex( lerpBilinear2( v0, v1, v2 ) );
					}
					else if( mPositionsDims == 3 ) {
						const vec3 &v0 = *(const vec3*)(&mPositions[index0*3]);
						const vec3 &v1 = *(const vec3*)(&mPositions[index1*3]);
						const vec3 &v2 = *(const vec3*)(&mPositions[index2*3]);
						appendVertex( lerpBilinear3( v0, v1, v2 ) );
					}
					else if( mPositionsDims == 4 ) {
						const vec4 &v0 = *(const vec4*)(&mPositions[index0*4]);
						const vec4 &v1 = *(const vec4*)(&mPositions[index1*4]);
						const vec4 &v2 = *(const vec4*)(&mPositions[index2*4]);
						appendVertex( lerpBilinear4( v0, v1, v2 ) );
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

void TriMesh::copyAttrib( geom::Attrib attr, uint8_t dims, size_t stride, const float *srcData, size_t numVertices )
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
	const size_t numPositions = getNumVertices();

	if( indexA >= numPositions || indexB >= numPositions )
		return false;

	// positions
	const vec3 &a = *reinterpret_cast<const vec3*>(&mPositions[indexA*mPositionsDims]);
	const vec3 &b = *reinterpret_cast<const vec3*>(&mPositions[indexB*mPositionsDims]);
	if( distance2( a, b ) > FLT_EPSILON )
		return false;

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

#if 0

TriMesh TriMesh::createCircle( const ivec2 &resolution )
{
	return createRing( resolution, 0.0f );
}

TriMesh TriMesh::createCube( const ivec3 &resolution )
{
	vector<ColorAf> colors;
	vector<uint32_t> indices;
	vector<vec3> normals;
	vector<vec3> positions;
	vector<vec2> texCoords;
	
	ci::TriMesh front	= createSquare( ivec2( resolution.x, resolution.y ) );
	ci::TriMesh left	= createSquare( ivec2( resolution.z, resolution.y ) );
	ci::TriMesh top		= createSquare( ivec2( resolution.x, resolution.z ) );
	
	vec3 normal;
	vec3 offset;
	mat4 transform;
	
	// Back
	normal = vec3( 0.0f, 0.0f, -1.0f );
	offset = normal * 0.5f;
	transform.setToIdentity();
	transform.translate( offset );
	for ( vector<vec3>::iterator iter = front.getVertices().begin(); iter != front.getVertices().end(); ++iter ) {
		positions.push_back( transform.transformPoint( *iter ) );
		normals.push_back( normal );
	}
	for ( vector<vec2>::iterator iter = front.getTexCoords().begin(); iter != front.getTexCoords().end(); ++iter ) {
		texCoords.push_back( *iter );
	}
	
	// Bottom
	normal = vec3( 0.0f, -1.0f, 0.0f );
	offset = normal * 0.5f;
	transform.setToIdentity();
	transform.translate( offset );
	transform.rotate( vec3( -(float)M_PI * 0.5f, 0.0f, 0.0f ) );
	transform.translate( offset * -1.0f );
	transform.translate( offset );
	for ( vector<vec3>::iterator iter = top.getVertices().begin(); iter != top.getVertices().end(); ++iter ) {
		positions.push_back( transform.transformPoint( *iter ) );
		normals.push_back( normal );
	}
	for ( vector<vec2>::iterator iter = top.getTexCoords().begin(); iter != top.getTexCoords().end(); ++iter ) {
		texCoords.push_back( *iter );
	}
	
	normal = vec3( 0.0f, 0.0f, 1.0f );
	offset = normal * 0.5f;
	transform.setToIdentity();
	transform.translate( offset );
	for ( vector<vec3>::iterator iter = front.getVertices().begin(); iter != front.getVertices().end(); ++iter ) {
		positions.push_back( transform.transformPoint( *iter ) );
		normals.push_back( normal );
	}
	for ( vector<vec2>::iterator iter = front.getTexCoords().begin(); iter != front.getTexCoords().end(); ++iter ) {
		texCoords.push_back( *iter );
	}
	
	normal = vec3( -1.0f, 0.0f, 0.0f );
	offset = normal * 0.5f;
	transform.setToIdentity();
	transform.translate( offset );
	transform.rotate( vec3( 0.0f, -(float)M_PI * 0.5f, 0.0f ) );
	transform.translate( offset * -1.0f );
	transform.translate( offset );
	for ( vector<vec3>::iterator iter = left.getVertices().begin(); iter != left.getVertices().end(); ++iter ) {
		positions.push_back( transform.transformPoint( *iter ) );
		normals.push_back( normal );
	}
	for ( vector<vec2>::iterator iter = left.getTexCoords().begin(); iter != left.getTexCoords().end(); ++iter ) {
		texCoords.push_back( *iter );
	}
	
	// Right
	normal = vec3( 1.0f, 0.0f, 0.0f );
	offset = normal * 0.5f;
	transform.setToIdentity();
	transform.translate( offset );
	transform.rotate( vec3( 0.0f, (float)M_PI * 0.5f, 0.0f ) );
	transform.translate( offset * -1.0f );
	transform.translate( offset );
	for ( vector<vec3>::iterator iter = left.getVertices().begin(); iter != left.getVertices().end(); ++iter ) {
		positions.push_back( transform.transformPoint( *iter ) );
		normals.push_back( normal );
	}
	for ( vector<vec2>::iterator iter = left.getTexCoords().begin(); iter != left.getTexCoords().end(); ++iter ) {
		texCoords.push_back( *iter );
	}
	
	normal = vec3( 0.0f, 1.0f, 0.0f );
	offset = normal * 0.5f;
	transform.setToIdentity();
	transform.translate( offset );
	transform.rotate( vec3( (float)M_PI * 0.5f, 0.0f, 0.0f ) );
	transform.translate( offset * -1.0f );
	transform.translate( offset );
	for ( vector<vec3>::iterator iter = top.getVertices().begin(); iter != top.getVertices().end(); ++iter ) {
		positions.push_back( transform.transformPoint( *iter ) );
		normals.push_back( normal );
	}
	for ( vector<vec2>::iterator iter = top.getTexCoords().begin(); iter != top.getTexCoords().end(); ++iter ) {
		texCoords.push_back( *iter );
	}
	
	ColorAf color = ColorAf::white();
	for ( uint32_t i = 0; i < positions.size(); ++i ) {
		indices.push_back( i );
		colors.push_back( color );
	}
	
	TriMesh mesh = TriMesh::create( indices, colors, normals, positions, texCoords );
	
	colors.clear();
	indices.clear();
	normals.clear();
	positions.clear();
	texCoords.clear();
	
	return mesh;
}

TriMesh TriMesh::createCylinder( const ivec2 &resolution, float topRadius, float baseRadius, bool closeTop, bool closeBase )
{
	vector<ColorAf> colors;
	vector<uint32_t> indices;
	vector<vec3> normals;
	vector<vec3> positions;
	vector<vec3> srcNormals;
	vector<vec3> srcPositions;
	vector<vec2> srcTexCoords;
	vector<vec2> texCoords;
	
	float delta = ( 2.0f * (float)M_PI ) / (float)resolution.x;
	float step	= 1.0f / (float)resolution.y;
	float ud	= 1.0f / (float)resolution.x;
	
	int32_t p = 0;
	for ( float phi = 0.0f; p <= resolution.y; ++p, phi += step ) {
		int32_t t	= 0;
		float u		= 0.0f;
		for ( float theta = 0.0f; t < resolution.x; ++t, u += ud, theta += delta ) {
			
			float radius = lerp( baseRadius, topRadius, phi );
			
			vec3 position(
						   math<float>::cos( theta ) * radius,
						   phi - 0.5f,
						   math<float>::sin( theta ) * radius
						   );
			srcPositions.push_back( position );
			
			vec3 normal = vec3( position.x, 0.0f, position.z ).normalized();
			normal.y = 0.0f;
			srcNormals.push_back( normal );
			
			vec2 texCoord( u, phi );
			srcTexCoords.push_back( texCoord );
		}
	}
	
	srcNormals.push_back( vec3( 0.0f, 1.0f, 0.0f ) );
	srcNormals.push_back( vec3( 0.0f, -1.0f, 0.0f ) );
	srcPositions.push_back( vec3( 0.0f, -0.5f, 0.0f ) );
	srcPositions.push_back( vec3( 0.0f, 0.5f, 0.0f ) );
	srcTexCoords.push_back( vec2( 0.0f, 0.0f ) );
	srcTexCoords.push_back( vec2( 0.0f, 1.0f ) );
	int32_t topCenter		= (int32_t)srcPositions.size() - 1;
	int32_t bottomCenter	= topCenter - 1;
	
	if ( closeTop ) {
		for ( int32_t t = 0; t < resolution.x; ++t ) {
			int32_t n = t + 1 >= resolution.x ? 0 : t + 1;
			
			normals.push_back( srcNormals[ topCenter ] );
			normals.push_back( srcNormals[ topCenter ] );
			normals.push_back( srcNormals[ topCenter ] );
			
			positions.push_back( srcPositions[ topCenter ] );
			positions.push_back( srcPositions[ ( resolution.x * resolution.y ) + n ] );
			positions.push_back( srcPositions[ ( resolution.x * resolution.y ) + t ] );
			
			texCoords.push_back( srcTexCoords[ topCenter ] );
			texCoords.push_back( srcTexCoords[ topCenter ] );
			texCoords.push_back( srcTexCoords[ topCenter ] );
		}
	}
	
	for ( int32_t p = 0; p < resolution.y; ++p ) {
		for ( int32_t t = 0; t < resolution.x; ++t ) {
			int32_t n = t + 1 >= resolution.x ? 0 : t + 1;
			
			int32_t index0 = ( p + 0 ) * resolution.x + t;
			int32_t index1 = ( p + 0 ) * resolution.x + n;
			int32_t index2 = ( p + 1 ) * resolution.x + t;
			int32_t index3 = ( p + 1 ) * resolution.x + n;
			
			normals.push_back( srcNormals[ index0 ] );
			normals.push_back( srcNormals[ index2 ] );
			normals.push_back( srcNormals[ index1 ] );
			normals.push_back( srcNormals[ index1 ] );
			normals.push_back( srcNormals[ index2 ] );
			normals.push_back( srcNormals[ index3 ] );
			
			positions.push_back( srcPositions[ index0 ] );
			positions.push_back( srcPositions[ index2 ] );
			positions.push_back( srcPositions[ index1 ] );
			positions.push_back( srcPositions[ index1 ] );
			positions.push_back( srcPositions[ index2 ] );
			positions.push_back( srcPositions[ index3 ] );
			
			texCoords.push_back( srcTexCoords[ index0 ] );
			texCoords.push_back( srcTexCoords[ index2 ] );
			texCoords.push_back( srcTexCoords[ index1 ] );
			texCoords.push_back( srcTexCoords[ index1 ] );
			texCoords.push_back( srcTexCoords[ index2 ] );
			texCoords.push_back( srcTexCoords[ index3 ] );
		}
	}
	
	if ( closeBase ) {
		for ( int32_t t = 0; t < resolution.x; ++t ) {
			int32_t n = t + 1 >= resolution.x ? 0 : t + 1;
			
			normals.push_back( srcNormals[ bottomCenter ] );
			normals.push_back( srcNormals[ bottomCenter ] );
			normals.push_back( srcNormals[ bottomCenter ] );
			
			positions.push_back( srcPositions[ bottomCenter ] );
			positions.push_back( srcPositions[ n ] );
			positions.push_back( srcPositions[ t ] );
			
			texCoords.push_back( srcTexCoords[ bottomCenter ] );
			texCoords.push_back( srcTexCoords[ bottomCenter ] );
			texCoords.push_back( srcTexCoords[ bottomCenter ] );
		}
	}
	
	ColorAf color = ColorAf::white();
	for ( uint32_t i = 0; i < positions.size(); ++i ) {
		indices.push_back( i );
		colors.push_back( color );
	}
	
	TriMesh mesh = TriMesh::create( indices, colors, normals, positions, texCoords );
	
	colors.clear();
	indices.clear();
	normals.clear();
	positions.clear();
	srcNormals.clear();
	srcPositions.clear();
	srcTexCoords.clear();
	texCoords.clear();
	
	return mesh;
}

TriMesh TriMesh::createIcosahedron( uint32_t division )
{
	vector<ColorAf> colors;
	vector<vec3> positions;
	vector<vec3> normals;
	vector<vec2> texCoords;
	vector<uint32_t> indices;
	
	const float t	= 0.5f + 0.5f * math<float>::sqrt( 5.0f );
	const float one	= 1.0f / math<float>::sqrt( 1.0f + t * t );
	const float tau	= t * one;
	const float pi	= (float)M_PI;
	
	normals.push_back( vec3(  one, 0.0f,  tau ) );
	normals.push_back( vec3(  one, 0.0f, -tau ) );
	normals.push_back( vec3( -one, 0.0f, -tau ) );
	normals.push_back( vec3( -one, 0.0f,  tau ) );
	
	normals.push_back( vec3(  tau,  one, 0.0f ) );
	normals.push_back( vec3( -tau,  one, 0.0f ) );
	normals.push_back( vec3( -tau, -one, 0.0f ) );
	normals.push_back( vec3(  tau, -one, 0.0f ) );
	
	normals.push_back( vec3( 0.0f,  tau,  one ) );
	normals.push_back( vec3( 0.0f, -tau,  one ) );
	normals.push_back( vec3( 0.0f, -tau, -one ) );
	normals.push_back( vec3( 0.0f,  tau, -one ) );
	
	for ( size_t i = 0; i < 12; ++i ) {
		positions.push_back( normals[ i ] * 0.5f );
	}
	
	static const size_t numIndices = 60;
	uint32_t indexArray[ numIndices ] = {
		0, 8, 3,	0, 3, 9,
		1, 2, 11,	1, 10, 2,
		4, 0, 7,	4, 7, 1,
		6, 3, 5,	6, 5, 2,
		8, 4, 11,	8, 11, 5,
		9, 10, 7,	9, 6, 10,
		8, 0, 4,	11, 4, 1,
		0, 9, 7,	1, 7, 10,
		3, 8, 5,	2, 5, 11,
		3, 6, 9,	2, 10, 6
	};
	
	ColorAf color = ColorAf::white();
	for ( size_t i = 0; i < numIndices; ++i ) {
		indices.push_back( indexArray[ i ] );
		colors.push_back( color );
	}
	
	for ( vector<vec3>::const_iterator iter = normals.begin(); iter != normals.end(); ++iter ) {
		float u = 0.5f + 0.5f * math<float>::atan2( iter->x, iter->z ) / pi;
		float v = 0.5f - math<float>::asin( iter->y ) / pi;
		vec2 texCoord( u, v );
		texCoords.push_back( texCoord );
	}
	
	TriMesh mesh = TriMesh::create( indices, colors, normals, positions, texCoords );
	
	if ( division > 1 ) {
		mesh = subdivide( mesh, division, true );
	}
	
	colors.clear();
	indices.clear();
	normals.clear();
	positions.clear();
	texCoords.clear();
	
	return mesh;
}

TriMesh TriMesh::createRing( const ivec2 &resolution, float ratio )
{
	vector<ColorAf> colors;
	vector<uint32_t> indices;
	vector<vec3> normals;
	vector<vec3> positions;
	vector<vec2> texCoords;
	
	vec3 norm0( 0.0f, 0.0f, 1.0f );
	
	float delta = ( (float)M_PI * 2.0f ) / (float)resolution.x;
	float width	= 1.0f - ratio;
	float step	= width / (float)resolution.y;
	
	int32_t p = 0;
	for ( float phi = 0.0f; p < resolution.y; ++p, phi += step ) {
		
		float innerRadius = phi + 0.0f + ratio;
		float outerRadius = phi + step + ratio;
		
		int32_t t = 0;
		for ( float theta = 0.0f; t < resolution.x; ++t, theta += delta ) {
			float ct	= math<float>::cos( theta );
			float st	= math<float>::sin( theta );
			float ctn	= math<float>::cos( theta + delta );
			float stn	= math<float>::sin( theta + delta );
			
			vec3 pos0 = vec3( ct, st, 0.0f ) * innerRadius;
			vec3 pos1 = vec3( ctn, stn, 0.0f ) * innerRadius;
			vec3 pos2 = vec3( ct, st, 0.0f ) * outerRadius;
			vec3 pos3 = vec3( ctn, stn, 0.0f ) * outerRadius;
			if ( t >= resolution.x - 1 ) {
				ctn		= math<float>::cos( 0.0f );
				stn		= math<float>::sin( 0.0f );
				pos1	= vec3( ctn, stn, 0.0f ) * innerRadius;
				pos3	= vec3( ctn, stn, 0.0f ) * outerRadius;
			}
			
			vec2 texCoord0 = ( pos0.xy() + vec2::one() ) * 0.5f;
			vec2 texCoord1 = ( pos1.xy() + vec2::one() ) * 0.5f;
			vec2 texCoord2 = ( pos2.xy() + vec2::one() ) * 0.5f;
			vec2 texCoord3 = ( pos3.xy() + vec2::one() ) * 0.5f;
			
			positions.push_back( pos0 );
			positions.push_back( pos2 );
			positions.push_back( pos1 );
			positions.push_back( pos1 );
			positions.push_back( pos2 );
			positions.push_back( pos3 );
			
			texCoords.push_back( texCoord0 );
			texCoords.push_back( texCoord2 );
			texCoords.push_back( texCoord1 );
			texCoords.push_back( texCoord1 );
			texCoords.push_back( texCoord2 );
			texCoords.push_back( texCoord3 );
		}
	}
	
	ColorAf color = ColorAf::white();
	for ( uint32_t i = 0; i < positions.size(); ++i ) {
		colors.push_back( color );
		indices.push_back( i );
		normals.push_back( norm0 );
	}
	
	TriMesh mesh = TriMesh::create( indices, colors, normals, positions, texCoords );
	
	colors.clear();
	indices.clear();
	normals.clear();
	positions.clear();
	texCoords.clear();
	
	return mesh;
}

TriMesh TriMesh::createSphere( const ivec2 &resolution )
{
	vector<ColorAf> colors;
	vector<uint32_t> indices;
	vector<vec3> normals;
	vector<vec3> positions;
	vector<vec2> texCoords;
	
	float step = (float)M_PI / (float)resolution.y;
	float delta = ((float)M_PI * 2.0f) / (float)resolution.x;
	
	int32_t p = 0;
	for ( float phi = 0.0f; p <= resolution.y; p++, phi += step ) {
		int32_t t = 0;
		
		uint32_t a = (uint32_t)( ( p + 0 ) * resolution.x );
		uint32_t b = (uint32_t)( ( p + 1 ) * resolution.x );
		
		for ( float theta = delta; t < resolution.x; t++, theta += delta ) {
			float sinPhi = math<float>::sin( phi );
			float x = sinPhi * math<float>::cos( theta );
			float y = sinPhi * math<float>::sin( theta );
			float z = -math<float>::cos( phi );
			vec3 position( x, y, z );
			vec3 normal = position.normalized();
			vec2 texCoord = ( normal.xy() + vec2::one() ) * 0.5f;
			
			normals.push_back( normal );
			positions.push_back( position );
			texCoords.push_back( texCoord );
			
			uint32_t n = (uint32_t)( t + 1 >= resolution.x ? 0 : t + 1 );
			indices.push_back( a + t );
			indices.push_back( b + t );
			indices.push_back( a + n );
			indices.push_back( a + n );
			indices.push_back( b + t );
			indices.push_back( b + n );
		}
	}
	
	for ( vector<uint32_t>::iterator iter = indices.begin(); iter != indices.end(); ) {
		if ( *iter < positions.size() ) {
			++iter;
		} else {
			iter = indices.erase( iter );
		}
	}
	
	ColorAf color = ColorAf::white();
	for ( uint32_t i = 0; i < positions.size(); ++i ) {
		colors.push_back( color );
	}
	
	TriMesh mesh = TriMesh::create( indices, colors, normals, positions, texCoords );
	
	colors.clear();
	indices.clear();
	normals.clear();
	positions.clear();
	texCoords.clear();
	
	return mesh;
}

TriMesh TriMesh::createSquare( const ivec2 &resolution )
{
	vector<ColorAf> colors;
	vector<uint32_t> indices;
	vector<vec3> normals;
	vector<vec3> positions;
	vector<vec2> texCoords;
	
	vec3 norm0( 0.0f, 0.0f, 1.0f );
	
	vec2 scale( 1.0f / math<float>::max( (float)resolution.x, 1.0f ), 1.0f / math<float>::max( (float)resolution.y, 1.0f ) );
	uint32_t index = 0;
	for ( int32_t y = 0; y < resolution.y; ++y ) {
		for ( int32_t x = 0; x < resolution.x; ++x, ++index ) {
			
			float x1 = (float)x * scale.x;
			float y1 = (float)y * scale.y;
			float x2 = (float)( x + 1 ) * scale.x;
			float y2 = (float)( y + 1 ) * scale.y;
			
			vec3 pos0( x1 - 0.5f, y1 - 0.5f, 0.0f );
			vec3 pos1( x2 - 0.5f, y1 - 0.5f, 0.0f );
			vec3 pos2( x1 - 0.5f, y2 - 0.5f, 0.0f );
			vec3 pos3( x2 - 0.5f, y2 - 0.5f, 0.0f );
			
			vec2 texCoord0( x1, y1 );
			vec2 texCoord1( x2, y1 );
			vec2 texCoord2( x1, y2 );
			vec2 texCoord3( x2, y2 );
			
			positions.push_back( pos2 );
			positions.push_back( pos1 );
			positions.push_back( pos0 );
			positions.push_back( pos1 );
			positions.push_back( pos2 );
			positions.push_back( pos3 );
			
			texCoords.push_back( texCoord2 );
			texCoords.push_back( texCoord1 );
			texCoords.push_back( texCoord0 );
			texCoords.push_back( texCoord1 );
			texCoords.push_back( texCoord2 );
			texCoords.push_back( texCoord3 );
			
			for ( uint32_t i = 0; i < 6; ++i ) {
				indices.push_back( index * 6 + i );
				normals.push_back( norm0 );
			}
		}
	}
	
	ColorAf color = ColorAf::white();
	for ( uint32_t i = 0; i < positions.size(); ++i ) {
		colors.push_back( color );
	}
	
	TriMesh mesh = TriMesh::create( indices, colors, normals, positions, texCoords );
	
	colors.clear();
	indices.clear();
	normals.clear();
	positions.clear();
	texCoords.clear();
	
	return mesh;
	
}

TriMesh TriMesh::createTorus( const ivec2 &resolution, float ratio )
{
	vector<ColorAf> colors;
	vector<uint32_t> indices;
	vector<vec3> normals;
	vector<vec3> positions;
	vector<vec3> srcNormals;
	vector<vec3> srcPositions;
	vector<vec2> srcTexCoords;
	vector<vec2> texCoords;
	
	float pi			= (float)M_PI;
	float delta			= ( 2.0f * pi ) / (float)resolution.y;
	float step			= ( 2.0f * pi ) / (float)resolution.x;
	float ud			= 1.0f / (float)resolution.y;
	float vd			= 1.0f / (float)resolution.x;
	
	float outerRadius	= 0.5f / (1.0f + ratio);
	float innerRadius	= outerRadius * ratio;
	
	int32_t p			= 0;
	float v				= 0.0f;
	for ( float phi = 0.0f; p < resolution.x; ++p, v += vd, phi += step ) {
		float cosPhi = math<float>::cos( phi - pi );
		float sinPhi = math<float>::sin( phi - pi );
		
		int32_t t = 0;
		float u = 0.0f;
		for ( float theta = 0.0f; t < resolution.y; ++t, u += ud, theta += delta ) {
			float cosTheta = math<float>::cos( theta );
			float sinTheta = math<float>::sin( theta );
			
			float rct	= outerRadius + innerRadius * cosTheta;
			float x		= cosPhi * rct;
			float y		= sinPhi * rct;
			float z		= sinTheta * innerRadius;
			
			vec3 normal( -cosTheta * cosTheta, sinPhi * cosTheta, sinTheta );
			vec3 position( x, y, z );
			vec2 texCoord( u, v );
			
			positions.push_back( position );
			normals.push_back( normal );
			texCoords.push_back( texCoord );
		}
	}
	
	for ( p = 0; p < resolution.x; ++p ) {
		int32_t a = ( p + 0 ) * resolution.y;
		int32_t b = ( p + 1 >= resolution.x ? 0 : p + 1 ) * resolution.y;
		
		for ( int32_t t = 0; t < resolution.y; ++t ) {
			int32_t n = t + 1 >= resolution.y ? 0 : t + 1;
			
			uint32_t index0 = (uint32_t)( a + t );
			uint32_t index1 = (uint32_t)( a + n );
			uint32_t index2 = (uint32_t)( b + t );
			uint32_t index3 = (uint32_t)( b + n );
			
			indices.push_back( index0 );
			indices.push_back( index2 );
			indices.push_back( index1 );
			indices.push_back( index1 );
			indices.push_back( index2 );
			indices.push_back( index3 );
		}
	}
	
	ColorAf color = ColorAf::white();
	for ( uint32_t i = 0; i < positions.size(); ++i ) {
		colors.push_back( color );
	}
	
	TriMesh mesh = TriMesh::create( indices, colors, normals, positions, texCoords );
	
	colors.clear();
	indices.clear();
	normals.clear();
	positions.clear();
	srcNormals.clear();
	srcPositions.clear();
	srcTexCoords.clear();
	texCoords.clear();
	
	return mesh;
}

#endif
/*
Rectf TriMesh2d::calcBoundingBox() const
{
	if( mPositions.empty() )
		return Rectf( vec2(), vec2() );

	vec2 min(mPositions[0]), max(mPositions[0]);
	for( size_t i = 1; i < mPositions.size(); ++i ) {
		if( mPositions[i].x < min.x )
			min.x = mPositions[i].x;
		else if( mPositions[i].x > max.x )
			max.x = mPositions[i].x;
		if( mPositions[i].y < min.y )
			min.y = mPositions[i].y;
		else if( mPositions[i].y > max.y )
			max.y = mPositions[i].y;
	}
	
	return Rectf( min, max );
}*/
	
/////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace cinder
