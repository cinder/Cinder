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

#include "cinder/App/App.h"
#include "cinder/GeomIo.h"
#include "cinder/Quaternion.h"
#include "cinder/Log.h"
#include "cinder/TriMesh.h"
#include "cinder/Triangulate.h"
#include "cinder/BSpline.h"
#include "cinder/Matrix.h"
#include <algorithm>

using namespace std;

namespace cinder { namespace geom {

std::string sAttribNames[(int)Attrib::NUM_ATTRIBS] = {
	"POSITION", "COLOR", "TEX_COORD_0", "TEX_COORD_1", "TEX_COORD_2", "TEX_COORD_3",
	"NORMAL", "TANGENT", "BITANGENT", "BONE_INDEX", "BONE_WEIGHT",
	"CUSTOM_0", "CUSTOM_1", "CUSTOM_2", "CUSTOM_3", "CUSTOM_4", "CUSTOM_5", "CUSTOM_6", "CUSTOM_7", "CUSTOM_8", "CUSTOM_9"
};

std::string attribToString( Attrib attrib )
{
	if( attrib < Attrib::NUM_ATTRIBS )
		return sAttribNames[(int)attrib];
	else
		return "";
}

///////////////////////////////////////////////////////////////////////////////////////
// Modifier
uint8_t	Modifier::getAttribDims( geom::Attrib attr ) const
{
	auto attrIt = mAttribs.find( attr );
	if( attrIt == mAttribs.end() || attrIt->second == IGNORED ) { // not an attribute we're interested in; pass through and ask the target
		return mTarget->getAttribDims( attr );
	}
	else if( (attrIt->second == READ) || (attrIt->second == READ_WRITE) ) { // READ or READ_WRITE implies we want whatever the source has got
		return mSource.getAttribDims( attr );
	}
	else // WRITE means our consumer will be writing this value later so we don't need the source to supply it
		return 0;
}

void Modifier::copyAttrib( Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count )
{
	auto attrIt = mAttribs.find( attr );
	if( (attrIt == mAttribs.end()) || (attrIt->second == IGNORED) ) { // not an attribute we're interested in; pass through to the target
		mTarget->copyAttrib( attr, dims, strideBytes, srcData, count );
	}
	else if( (attrIt->second == READ) || (attrIt->second == READ_WRITE) ) { // READ or READ_WRITE implies we want to capture the values; for READ, we pass them to target
		// make some room for our own copy of this data
		mAttribData[attr] = unique_ptr<float[]>( new float[dims * count] );
		mAttribDims[attr] = dims;
		copyData( dims, srcData, count, dims, 0, mAttribData.at( attr ).get() );
		if( attrIt->second == READ ) // pass through to target when READ but not READ_WRITE
			mTarget->copyAttrib( attr, dims, strideBytes, srcData, count );
	}
	// WRITE means our consumer will be writing this value later
}

void Modifier::copyIndices( Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex )
{
	mNumIndices = numIndices;
	mPrimitive = primitive;
	switch( mIndicesAccess ) {
		case IGNORED: // we just pass the indices through to the target
			mTarget->copyIndices( primitive, source, numIndices, requiredBytesPerIndex );
		break;
		case READ: // capture, and pass through to target
		case READ_WRITE: // capture but don't pass through
			mIndices = unique_ptr<uint32_t[]>( new uint32_t[numIndices] );
			memcpy( mIndices.get(), source, sizeof(uint32_t) * numIndices );
			if( mIndicesAccess == READ )
				mTarget->copyIndices( primitive, source, numIndices, requiredBytesPerIndex );
		break;
		default: // for WRITE we will supply our own indices later so do nothing but record the count
		break;
	}
}

uint8_t	Modifier::getReadAttribDims( Attrib attr ) const
{
	if( mAttribDims.find( attr ) == mAttribDims.end() )
		return 0;
	else
		return mAttribDims.at( attr );
}

// not const because consumer is allowed to overwrite this data
float* Modifier::getReadAttribData( Attrib attr ) const
{
	if( mAttribData.find( attr ) == mAttribData.end() )
		return nullptr;
	else
		return mAttribData.at( attr ).get();
}

///////////////////////////////////////////////////////////////////////////////////////
// BufferLayout
AttribInfo BufferLayout::getAttribInfo( Attrib attrib ) const
{
	for( const auto &atIt : mAttribs )
		if( atIt.getAttrib() == attrib )
			return atIt;

	throw ExcMissingAttrib();
}

bool BufferLayout::hasAttrib( Attrib attrib ) const
{
	for( const auto &atIt : mAttribs )
		if( atIt.getAttrib() == attrib )
			return true;

	return false;
}

uint8_t	BufferLayout::getAttribDims( Attrib attrib ) const
{
	for( const auto &atIt : mAttribs ) {
		if( atIt.getAttrib() == attrib )
			return atIt.getDims();
	}

	return 0;
}

size_t BufferLayout::calcRequiredStorage( size_t numVertices ) const
{
	if( numVertices == 0 )
		return 0;
	
	size_t result = 0;
	for( auto &attrib : mAttribs ) {
		size_t stride = attrib.getStride();
		if( stride == 0 )
			stride = attrib.getByteSize();
		result = std::max( result, attrib.getOffset() + (numVertices-1) * stride + attrib.getByteSize() );
	}
	
	return result;
}

///////////////////////////////////////////////////////////////////////////////////////
// Source
namespace { // these are helper functions for copyData() and copyDataMultAdd

template<uint8_t SRCDIM, uint8_t DSTDIM>
void copyDataImpl( const float *srcData, size_t numElements, size_t dstStrideBytes, float *dstData )
{
	static const float sFillerData[4] = { 0, 0, 0, 1 };
	const uint8_t MINDIM = (SRCDIM < DSTDIM) ? SRCDIM : DSTDIM;

	if( dstStrideBytes == 0 )
		dstStrideBytes = DSTDIM * sizeof(float);

	for( size_t v = 0; v < numElements; ++v ) {
		uint8_t d;
		for( d = 0; d < MINDIM; ++d ) {
			dstData[d] = srcData[d];
		}
		for( ; d < DSTDIM; ++d ) {
			dstData[d] = sFillerData[d];
		}
		srcData += SRCDIM;
		dstData = (float*)((uint8_t*)dstData + dstStrideBytes);
	}
}
} // anonymous namespace

void copyData( uint8_t srcDimensions, const float *srcData, size_t numElements, uint8_t dstDimensions, size_t dstStrideBytes, float *dstData )
{
	// we can get away with a memcpy
	if( (srcDimensions == dstDimensions) && (dstStrideBytes == 0) ) {
		memcpy( dstData, srcData, numElements * srcDimensions * sizeof(float) );
	}
	else {
		switch( srcDimensions ) {
			case 1:
				switch( dstDimensions ) {
					case 1: copyDataImpl<1,1>( srcData, numElements, dstStrideBytes, dstData ); break;
					case 2: copyDataImpl<1,2>( srcData, numElements, dstStrideBytes, dstData ); break;
					case 3: copyDataImpl<1,3>( srcData, numElements, dstStrideBytes, dstData ); break;
					case 4: copyDataImpl<1,4>( srcData, numElements, dstStrideBytes, dstData ); break;
					default: throw ExcIllegalDestDimensions();
				}
			break;
			case 2:
				switch( dstDimensions ) {
					case 1: copyDataImpl<2,1>( srcData, numElements, dstStrideBytes, dstData ); break;
					case 2: copyDataImpl<2,2>( srcData, numElements, dstStrideBytes, dstData ); break;
					case 3: copyDataImpl<2,3>( srcData, numElements, dstStrideBytes, dstData ); break;
					case 4: copyDataImpl<2,4>( srcData, numElements, dstStrideBytes, dstData ); break;
					default: throw ExcIllegalDestDimensions();
				}
			break;
			case 3:
				switch( dstDimensions ) {
					case 1: copyDataImpl<3,1>( srcData, numElements, dstStrideBytes, dstData ); break;
					case 2: copyDataImpl<3,2>( srcData, numElements, dstStrideBytes, dstData ); break;
					case 3: copyDataImpl<3,3>( srcData, numElements, dstStrideBytes, dstData ); break;
					case 4: copyDataImpl<3,4>( srcData, numElements, dstStrideBytes, dstData ); break;
					default: throw ExcIllegalDestDimensions();
				}
			break;
			case 4:
				switch( dstDimensions ) {
					case 1: copyDataImpl<4,1>( srcData, numElements, dstStrideBytes, dstData ); break;
					case 2: copyDataImpl<4,2>( srcData, numElements, dstStrideBytes, dstData ); break;
					case 3: copyDataImpl<4,3>( srcData, numElements, dstStrideBytes, dstData ); break;
					case 4: copyDataImpl<4,4>( srcData, numElements, dstStrideBytes, dstData ); break;
					default: throw ExcIllegalDestDimensions();
				}
			break;
			default:
				throw ExcIllegalSourceDimensions();
		}
	}
}


namespace { 
template<typename T>
void copyIndexDataForceTrianglesImpl( Primitive primitive, const uint32_t *source, size_t numIndices, T *target )
{
	switch( primitive ) {
	case Primitive::LINES:
	case Primitive::LINE_STRIP:
	case Primitive::TRIANGLES:
		memcpy( target, source, sizeof(uint32_t) * numIndices );
		break;
	case Primitive::TRIANGLE_STRIP: { // ABC, CBD, CDE, EDF, etc
		if( numIndices < 3 )
			return;
		size_t outIdx = 0; // (012, 213), (234, 435), etc : (odd,even), (odd,even), etc
		for( size_t i = 0; i < numIndices - 2; ++i ) {
			if( i & 1 ) { // odd
				target[outIdx++] = source[i+1];
				target[outIdx++] = source[0];
				target[outIdx++] = source[i+2];
			}
			else { // even
				target[outIdx++] = source[i];
				target[outIdx++] = source[i+1];
				target[outIdx++] = source[i+2];
			}
		}
									}
									break;
	case Primitive::TRIANGLE_FAN: { // ABC, ACD, ADE, etc
		if( numIndices < 3 )
			return;
		size_t outIdx = 0;
		for( size_t i = 0; i < numIndices - 2; ++i ) {
			target[outIdx++] = source[0];
			target[outIdx++] = source[i+1];
			target[outIdx++] = source[i+2];
		}
									}
	default:
		throw ExcIllegalPrimitiveType();			
		break;
	}
}

} // anonymous namespace


///////////////////////////////////////////////////////////////////////////////////////
// Target
void Target::copyIndexDataForceTriangles( Primitive primitive, const uint32_t *source, size_t numIndices, uint32_t *target )
{
	copyIndexDataForceTrianglesImpl<uint32_t>( primitive, source, numIndices, target );
}

void Target::copyIndexDataForceTriangles( Primitive primitive, const uint32_t *source, size_t numIndices, uint16_t *target )
{
	copyIndexDataForceTrianglesImpl<uint16_t>( primitive, source, numIndices, target );
}

void Target::copyIndexData( const uint32_t *source, size_t numIndices, uint32_t *target )
{
	memcpy( target, source, numIndices * sizeof(float) );
}

void Target::copyIndexData( const uint32_t *source, size_t numIndices, uint16_t *target )
{
	for( size_t v = 0; v < numIndices; ++v )
		target[v] = source[v];
}

uint8_t calcIndicesRequiredBytes( size_t numIndices )
{
	if( numIndices < 256 )
		return 1;
	else if( numIndices < 65536 )
		return 2;
	else
		return 4;
}

void Target::generateIndices( Primitive sourcePrimitive, size_t sourceNumIndices )
{
	unique_ptr<uint32_t[]> indices( new uint32_t[sourceNumIndices] );

	uint32_t count = 0;
	std::generate( indices.get(), indices.get() + sourceNumIndices, [&] { return count++; } );

	uint8_t requiredBytesPerIndex = calcIndicesRequiredBytes( sourceNumIndices );
	copyIndices( sourcePrimitive, indices.get(), sourceNumIndices, requiredBytesPerIndex );
}

///////////////////////////////////////////////////////////////////////////////////////
// Rect
//float Rect::sPositions[4*2] = { 0.5f,-0.5f,	-0.5f,-0.5f,	0.5f,0.5f,	-0.5f,0.5f };
//float Rect::sColors[4*3] = { 1, 0, 1,	0, 0, 1,	1, 1, 1,	0, 1, 1 };
//float Rect::sTexCoords[4*2] = { 1, 1,	0, 1,		1, 0,		0, 0 };
float Rect::sNormals[4*3] = {0, 0, 1,	0, 0, 1,	0, 0, 1,	0, 0, 1 };

Rect::Rect()
{
	// upper-right, upper-left, lower-right, lower-left
	mPositions[0] = vec2(  0.5f, -0.5f );
	mTexCoords[0] = vec2( 1, 1 );
	mColors[0] = ColorAf( 1, 0, 1, 1 );	
	mPositions[1] = vec2( -0.5f, -0.5f );
	mTexCoords[1] = vec2( 0, 1 );
	mColors[1] = ColorAf( 0, 0, 1, 1 );	
	mPositions[2] = vec2(  0.5f,  0.5f );
	mTexCoords[2] = vec2( 1, 0 );
	mColors[2] = ColorAf( 1, 1, 1, 1 );
	mPositions[3] = vec2( -0.5f,  0.5f );
	mTexCoords[3] = vec2( 0, 0 );
	mColors[3] = ColorAf( 0, 1, 1, 1 );
}

Rect::Rect( const Rectf &r )
{
	rect( r );
	
	// upper-right, upper-left, lower-right, lower-left
	mTexCoords[0] = vec2( 1, 1 );
	mColors[0] = ColorAf( 1, 0, 1, 1 );
	mTexCoords[1] = vec2( 0, 1 );
	mColors[1] = ColorAf( 0, 0, 1, 1 );	
	mTexCoords[2] = vec2( 1, 0 );
	mColors[2] = ColorAf( 1, 1, 1, 1 );
	mTexCoords[3] = vec2( 0, 0 );
	mColors[3] = ColorAf( 0, 1, 1, 1 );
}

Rect& Rect::rect( const Rectf &r )
{ 
	mPositions[0] = r.getUpperRight();
	mPositions[1] = r.getUpperLeft();
	mPositions[2] = r.getLowerRight();
	mPositions[3] = r.getLowerLeft();
	return *this;
}

Rect& Rect::colors( const ColorAf &upperLeft, const ColorAf &upperRight, const ColorAf &lowerRight, const ColorAf &lowerLeft )
{
	mColors[0] = upperRight;
	mColors[1] = upperLeft;
	mColors[2] = lowerRight;
	mColors[3] = lowerLeft;
	return *this;
}

Rect& Rect::texCoords( const vec2 &upperLeft, const vec2 &upperRight, const vec2 &lowerRight, const vec2 &lowerLeft )
{
	mTexCoords[0] = upperRight;
	mTexCoords[1] = upperLeft;
	mTexCoords[2] = lowerRight;
	mTexCoords[3] = lowerLeft;
	return *this;
}

void Rect::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	target->copyAttrib( Attrib::POSITION, 2, 0, (const float*)mPositions.data(), 4 );
	target->copyAttrib( Attrib::NORMAL, 3, 0, sNormals, 4 );
	target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, (const float*)mTexCoords.data(), 4 );
	target->copyAttrib( Attrib::COLOR, 4, 0, (const float*)mColors.data(), 4 );
}

uint8_t	Rect::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 2;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 2;
		case Attrib::COLOR: return 4;
		default:
			return 0;
	}
}

AttribSet Rect::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::COLOR };
}

///////////////////////////////////////////////////////////////////////////////////////
// Cube
Cube::Cube()
	: mSubdivisions( 1 ), mSize( 1 ), mHasColors( false )
{
	mColors[0] = Color(1,0,0);
	mColors[1] = Color(0,1,1);
	mColors[2] = Color(0,1,0);
	mColors[3] = Color(1,0,1);
	mColors[4] = Color(0,0,1);
	mColors[5] = Color(1,1,0);
}

Cube& Cube::colors()
{
	mHasColors = true;
	return *this;
}

Cube& Cube::colors( const ColorAf &posX, const ColorAf &negX, const ColorAf &posY, const ColorAf &negY, const ColorAf &posZ, const ColorAf &negZ )
{
	mHasColors = true;
	mColors[0] = posX;
	mColors[1] = negX;
	mColors[2] = posY;
	mColors[3] = negY;
	mColors[4] = posZ;
	mColors[5] = negZ;
	return *this;
}

Cube& Cube::disableColors()
{
	mHasColors = false;
	return *this;
}

size_t Cube::getNumVertices() const
{
	return 2 * ( (mSubdivisions.x+1) * (mSubdivisions.y+1) ) // +-Z
			+ 2 * ( (mSubdivisions.y+1) * (mSubdivisions.z+1) ) // +-X
			+ 2 * ( (mSubdivisions.x+1) * (mSubdivisions.z+1) ); // +-Y
}

size_t Cube::getNumIndices() const
{
	return 2 * 6 * ( mSubdivisions.x * mSubdivisions.y ) // +-Z
			+ 2 * 6 * ( mSubdivisions.y * mSubdivisions.z ) // +-X
			+ 2 * 6 * ( mSubdivisions.x * mSubdivisions.z ); // +-Y
}

uint8_t	Cube::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 2;
		case Attrib::COLOR: return mHasColors ? 4 : 0;
		default:
			return 0;
	}	
}

AttribSet Cube::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::COLOR };
}

void generateFace( const vec3 &faceCenter, const vec3 &uAxis, const vec3 &vAxis, int subdivU, int subdivV,
					vector<vec3> *positions, vector<vec3> *normals,
					const ColorA &color, vector<ColorA> *colors, vector<vec2> *texCoords,
					vector<uint32_t> *indices )
{
	const vec3 normal = normalize( faceCenter );

	const uint32_t baseIdx = (uint32_t)positions->size();

	// fill vertex data
	for( int vi = 0; vi <= subdivV; vi++ ) {
		const float v = vi / float(subdivV);
		for( int ui = 0; ui <= subdivU; ui++ ) {
			const float u = ui / float(subdivU);

			positions->emplace_back( faceCenter + ( u - 0.5f ) * 2.0f * uAxis + ( v - 0.5f ) * 2.0f * vAxis );

			if( normals )
				normals->emplace_back( normal );
			if( colors )
				colors->emplace_back( color );
			if( texCoords )
				texCoords->emplace_back( u, v );
		}
	}

	// 'baseIdx' will correspond to the index of the first vertex we created in this call to generateFace()
//	const uint32_t baseIdx = indices->empty() ? 0 : ( indices->back() + 1 );
	for( int u = 0; u < subdivU; u++ ) {
		for( int v = 0; v < subdivV; v++ ) {
			const int i = u + v * ( subdivU + 1 );

			indices->push_back( baseIdx + i );
			indices->push_back( baseIdx + i + subdivU + 1 );
			indices->push_back( baseIdx + i + 1 );

			indices->push_back( baseIdx + i + 1 );
			indices->push_back( baseIdx + i + subdivU + 1 );
			indices->push_back( baseIdx + i + subdivU + 2 );
			// important the last is the highest idx due to determination of next face's baseIdx
		}
	}
}

void Cube::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	vector<vec3> positions;
	vector<uint32_t> indices;
	vector<vec3> normals;
	vector<ColorA> colors;
	vector<vec2> texCoords;
	vector<vec3> *normalsPtr = nullptr;
	vector<ColorA> *colorsPtr = nullptr;
	vector<vec2> *texCoordsPtr = nullptr;
	
	const size_t numVertices = getNumVertices();
	
	// reserve room in vectors and set pointers to non-null for normals, texcoords and colors as appropriate
	positions.reserve( numVertices );
	indices.reserve( getNumIndices() );
	if( requestedAttribs.count( Attrib::NORMAL ) > 0 ) {
		normals.reserve( numVertices );
		normalsPtr = &normals;
	}
	if( requestedAttribs.count( Attrib::COLOR ) > 0 ) {
		colors.reserve( numVertices );
		colorsPtr = &colors;
	}
	if( requestedAttribs.count( Attrib::TEX_COORD_0 ) > 0 ) {
		texCoords.reserve( numVertices );
		texCoordsPtr = &texCoords;
	}
	
	// +X
	generateFace( vec3(mSize.x,0,0), vec3(0,0,mSize.z), vec3(0,mSize.y,0), mSubdivisions.z, mSubdivisions.y, &positions,
		normalsPtr, mColors[0], colorsPtr, texCoordsPtr, &indices );
	// +Y
	generateFace( vec3(0,mSize.y,0), vec3(mSize.x,0,0), vec3(0,0,mSize.z), mSubdivisions.x, mSubdivisions.z, &positions,
		normalsPtr, mColors[2], colorsPtr, texCoordsPtr, &indices );
	// +Z
	generateFace( vec3(0,0,mSize.z), vec3(0,mSize.y,0), vec3(mSize.x,0,0), mSubdivisions.y, mSubdivisions.x, &positions,
		normalsPtr, mColors[4], colorsPtr, texCoordsPtr, &indices );
	// -X
	generateFace( vec3(-mSize.x,0,0), vec3(0,mSize.y,0), vec3(0,0,mSize.z), mSubdivisions.y, mSubdivisions.z, &positions,
		normalsPtr, mColors[1], colorsPtr, texCoordsPtr, &indices );
	// -Y
	generateFace( vec3(0,-mSize.y,0), vec3(0,0,mSize.z), vec3(mSize.x,0,0), mSubdivisions.z, mSubdivisions.x, &positions,
		normalsPtr, mColors[3], colorsPtr, texCoordsPtr, &indices );
	// -Z
	generateFace( vec3(0,0,-mSize.z), vec3(mSize.x,0,0), vec3(0,mSize.y,0), mSubdivisions.x, mSubdivisions.y, &positions,
		normalsPtr, mColors[5], colorsPtr, texCoordsPtr, &indices );

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*)positions.data(), numVertices );
	if( normalsPtr )
		target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*)normals.data(), numVertices );
	if( texCoordsPtr )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, (const float*)texCoords.data(), numVertices );
	if( colorsPtr )
		target->copyAttrib( Attrib::COLOR, 4, 0, (const float*)colors.data(), numVertices );

	target->copyIndices( Primitive::TRIANGLES, indices.data(), getNumIndices(), calcIndicesRequiredBytes( getNumIndices() ) );
}

///////////////////////////////////////////////////////////////////////////////////////
// Icosahedron

#undef PHI	// take the reciprocal of phi, to obtain an icosahedron that fits a unit cube
#define PHI (1.0f / ((1.0f + math<float>::sqrt(5.0f)) / 2.0f))

float Icosahedron::sPositions[12*3] = { 
	-PHI, 1.0f, 0.0f,    PHI, 1.0f, 0.0f,   -PHI,-1.0f, 0.0f,    PHI,-1.0f, 0.0f,
	0.0f, -PHI, 1.0f,   0.0f,  PHI, 1.0f,   0.0f, -PHI,-1.0f,   0.0f,  PHI,-1.0f,
	1.0f, 0.0f, -PHI,   1.0f, 0.0f,  PHI,  -1.0f, 0.0f, -PHI,  -1.0f, 0.0f,  PHI };

uint32_t Icosahedron::sIndices[60] ={
	0,11, 5, 0, 5, 1, 0, 1, 7, 0, 7,10, 0,10,11,
	1, 5, 9, 5,11, 4,11,10, 2,10, 7, 6, 7, 1, 8,
	3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9,
	4, 9, 5, 2, 4,11, 6, 2,10, 8, 6, 7, 9, 8, 1 };

Icosahedron::Icosahedron()
	: mHasColors( false )
{
}

size_t Icosahedron::getNumVertices() const
{
	return 60;
}

size_t Icosahedron::getNumIndices() const
{
	return 60;
}

void Icosahedron::calculate( vector<vec3> *positions, vector<vec3> *normals, vector<vec3> *colors, vector<uint32_t> *indices ) const
{
	positions->reserve( 60 );
	normals->resize( 60 ); // needs to be resize rather than reserve
	colors->reserve( 60 );
	indices->reserve( 60 );

	for( size_t i = 0; i < 60; ++i ) {
		positions->emplace_back( *reinterpret_cast<const vec3*>(&sPositions[sIndices[i]*3]) );
		indices->push_back( (uint32_t)i );
	}

	// calculate the face normal for each triangle
	size_t numTriangles = indices->size() / 3;
	for( size_t i = 0; i < numTriangles; ++i ) {
		const uint32_t index0 = (*indices)[i*3+0];
		const uint32_t index1 = (*indices)[i*3+1];
		const uint32_t index2 = (*indices)[i*3+2];

		const vec3 &v0 = (*positions)[index0];
		const vec3 &v1 = (*positions)[index1];
		const vec3 &v2 = (*positions)[index2];

		vec3 e0 = v1 - v0;
		vec3 e1 = v2 - v0;

		(*normals)[index0] = (*normals)[index1] = (*normals)[index2] = normalize( cross( e0, e1 ) );
	}

	// color
	size_t numPositions = positions->size();
	for( size_t i = 0; i < numPositions; ++i )
		colors->emplace_back( (*positions)[i] * 0.5f + vec3( 0.5f ) );
}

uint8_t	Icosahedron::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::NORMAL: return 3;
		case Attrib::COLOR: return mHasColors ? 3 : 0;
		default:
			return 0;
	}
}

AttribSet Icosahedron::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::COLOR };
}

void Icosahedron::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	vector<vec3> positions, normals, colors;
	vector<uint32_t> indices;
	
	calculate( &positions, &normals, &colors, &indices );

	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *positions.data() ), positions.size() );
	target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *normals.data() ), normals.size() );
	target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *colors.data() ), colors.size() );

	target->copyIndices( Primitive::TRIANGLES, indices.data(), indices.size(), 1 );
}

///////////////////////////////////////////////////////////////////////////////////////
// Icosphere
Icosphere::Icosphere()
	: mSubdivision( 3 ), mCalculationsCached( false ), mHasColors( false )
{
}

void Icosphere::calculate() const
{
	if( mCalculationsCached )
		return;

	// start by copying the base icosahedron in its entirety (vertices are shared among faces)
	mPositions.assign( reinterpret_cast<vec3*>(Icosahedron::sPositions), reinterpret_cast<vec3*>(Icosahedron::sPositions) + 12 );
	mNormals.assign( reinterpret_cast<vec3*>(Icosahedron::sPositions), reinterpret_cast<vec3*>(Icosahedron::sPositions) + 12 );
	mIndices.assign( Icosahedron::sIndices, Icosahedron::sIndices + 60 );

	// subdivide all triangles
	subdivide();

	// spherize
	for( auto &pos : mPositions )
		pos = normalize( pos );
	for( auto &normal : mNormals )
		normal = normalize( normal );

	// add color if necessary
	size_t numPositions = mPositions.size();
	mColors.resize( numPositions );
	for( size_t i = 0; i < numPositions; ++i ) {
		mColors[i].x = mPositions[i].x * 0.5f + 0.5f;
		mColors[i].y = mPositions[i].y * 0.5f + 0.5f;
		mColors[i].z = mPositions[i].z * 0.5f + 0.5f;
	}

	// calculate texture coords based on equirectangular texture map
	calculateImplUV();

	mCalculationsCached = true;
}

void Icosphere::calculateImplUV() const
{
	// calculate texture coords
	mTexCoords.resize( mNormals.size(), vec2() );
	for( size_t i = 0; i < mNormals.size(); ++i ) {
		const vec3 &normal = mNormals[i];
		mTexCoords[i].x = (math<float>::atan2( normal.z, -normal.x ) / float(M_PI)) * 0.5f + 0.5f;
		mTexCoords[i].y = -normal.y * 0.5f + 0.5f;
	}

	// lambda closure to easily add a vertex with unique texture coordinate to our mesh
	auto addVertex = [&] ( size_t i, const vec2 &uv ) {
		const uint32_t index = mIndices[i];
		mIndices[i] = (uint32_t)mPositions.size();
		mPositions.push_back( mPositions[index] );
		mNormals.push_back( mNormals[index] );
		mTexCoords.push_back( uv );
		mColors.push_back( mColors[index] );
	};

	// fix texture seams (this is where the magic happens)
	size_t numTriangles = mIndices.size() / 3;
	for( size_t i = 0; i < numTriangles; ++i ) {
		const vec2 &uv0 = mTexCoords[ mIndices[i * 3 + 0] ];
		const vec2 &uv1 = mTexCoords[ mIndices[i * 3 + 1] ];
		const vec2 &uv2 = mTexCoords[ mIndices[i * 3 + 2] ];

		const float d1 = uv1.x - uv0.x;
		const float d2 = uv2.x - uv0.x;

		if( math<float>::abs(d1) > 0.5f && math<float>::abs(d2) > 0.5f )
			addVertex( i * 3 + 0, uv0 + vec2( (d1 > 0.0f) ? 1.0f : -1.0f, 0.0f ) );
		else if( math<float>::abs(d1) > 0.5f )
			addVertex( i * 3 + 1, uv1 + vec2( (d1 < 0.0f) ? 1.0f : -1.0f, 0.0f ) );
		else if( math<float>::abs(d2) > 0.5f )
			addVertex( i * 3 + 2, uv2 + vec2( (d2 < 0.0f) ? 1.0f : -1.0f, 0.0f ) );
	}
}

void Icosphere::subdivide() const
{
	for( int j = 0; j < mSubdivision; ++j ) {
		mPositions.reserve( mPositions.size() + mIndices.size() );
		mNormals.reserve( mNormals.size() + mIndices.size() );
		mIndices.reserve( mIndices.size() * 4 );

		const size_t numTriangles = mIndices.size() / 3;
		for( uint32_t i = 0; i < numTriangles; ++i ) {
			uint32_t index0 = mIndices[i * 3 + 0];
			uint32_t index1 = mIndices[i * 3 + 1];
			uint32_t index2 = mIndices[i * 3 + 2];

			uint32_t index3 = (uint32_t)mPositions.size();
			uint32_t index4 = index3 + 1;
			uint32_t index5 = index4 + 1;

			// add new triangles
			mIndices[i * 3 + 1] = index3;
			mIndices[i * 3 + 2] = index5;

			mIndices.push_back( index3 );
			mIndices.push_back( index1 );
			mIndices.push_back( index4 );

			mIndices.push_back( index5 );
			mIndices.push_back( index3 );
			mIndices.push_back( index4 );

			mIndices.push_back( index5 );
			mIndices.push_back( index4 );
			mIndices.push_back( index2 );

			// add new positions
			mPositions.push_back( 0.5f * (mPositions[index0] + mPositions[index1]) );
			mPositions.push_back( 0.5f * (mPositions[index1] + mPositions[index2]) );
			mPositions.push_back( 0.5f * (mPositions[index2] + mPositions[index0]) );

			// add new normals
			mNormals.push_back( 0.5f * (mNormals[index0] + mNormals[index1]) );
			mNormals.push_back( 0.5f * (mNormals[index1] + mNormals[index2]) );
			mNormals.push_back( 0.5f * (mNormals[index2] + mNormals[index0]) );
		}
	}
}

uint8_t Icosphere::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 2;
		case Attrib::COLOR: return mHasColors ? 3 : 0;
		default:
			return 0;
	}
}

AttribSet Icosphere::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::COLOR };
}

void Icosphere::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	calculate();

	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *mPositions.data() ), mPositions.size() );
	target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *mNormals.data() ), mNormals.size() );
	target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *mTexCoords.data() ), mTexCoords.size() );
	target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *mColors.data() ), mColors.size() );

	target->copyIndices( Primitive::TRIANGLES, mIndices.data(), mIndices.size(), 4 );
}

///////////////////////////////////////////////////////////////////////////////////////
// Teapot
const uint8_t Teapot::sPatchIndices[][16] = {
	// rim
	{102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
	// body
	{12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27}, {24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40},
	// lid
	{96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101, 101, 0, 1, 2, 3,}, {0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117},
	// bottom
	{118, 118, 118, 118, 124, 122, 119, 121, 123, 126, 125, 120, 40, 39, 38, 37},
	// handle
	{41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56}, {53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 28, 65, 66, 67},
	// spout
	{68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83}, {80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95}
};

#define LID 1.1f
#define LID_Z 1.0f
const float Teapot::sCurveData[127][3] =
{	{0.2f*LID, 0.f*LID, 2.7f*LID_Z}, {0.2f*LID, -0.112f*LID, 2.7f*LID_Z}, {0.112f*LID, -0.2f*LID, 2.7f*LID_Z}, {0.f*LID, -0.2f*LID, 2.7f*LID_Z},
	{1.3375f, 0.f, 2.53125f}, {1.3375f, -0.749f, 2.53125f}, {0.749f, -1.3375f, 2.53125f}, {0.f, -1.3375f, 2.53125f},
	{1.4375f, 0.f, 2.53125f}, {1.4375f, -0.805f, 2.53125f}, {0.805f, -1.4375f, 2.53125f}, {0.f, -1.4375f, 2.53125f},
	{1.5f, 0.f, 2.4f}, {1.5f, -0.84f, 2.4f}, {0.84f, -1.5f, 2.4f}, {0.f, -1.5f, 2.4f}, {1.75f, 0.f, 1.875f},
	{1.75f, -0.98f, 1.875f}, {0.98f, -1.75f, 1.875f}, {0.f, -1.75f, 1.875f}, {2.f, 0.f, 1.35f}, {2.f, -1.12f, 1.35f},
	{1.12f, -2.f, 1.35f}, {0.f, -2.f, 1.35f}, {2.f, 0.f, 0.9f}, {2.f, -1.12f, 0.9f}, {1.12f, -2.f, 0.9f},
	{0.f, -2.f, 0.9f}, {-2.f, 0.f, 0.9f}, {2.f, 0.f, 0.45f}, {2.f, -1.12f, 0.45f}, {1.12f, -2.f, 0.45f}, {0.f, -2.f, 0.45f},
	// 33
	{1.5f, 0.f, 0.225f},
	{1.5f, -0.84f, 0.225f}, {0.84f, -1.5f, 0.225f}, {0.f, -1.5f, 0.225f}, {1.5f, 0.f, 0.15f}, {1.5f,
	-0.84f, 0.15f}, {0.84f, -1.5f, 0.15f}, {0.f, -1.5f, 0.15f}, {-1.6f, 0.f, 2.025f}, {-1.6f, -0.3f,
	2.025f}, {-1.5f, -0.3f, 2.25f}, {-1.5f, 0.f, 2.25f}, {-2.3f, 0.f, 2.025f}, {-2.3f, -0.3f, 2.025f},
	{-2.5f, -0.3f, 2.25f}, {-2.5f, 0.f, 2.25f}, {-2.7f, 0.f, 2.025f}, {-2.7f, -0.3f, 2.025f}, {-3.f,
	-0.3f, 2.25f}, {-3.f, 0.f, 2.25f}, {-2.7f, 0.f, 1.8f}, {-2.7f, -0.3f, 1.8f}, {-3.f, -0.3f, 1.8f},
	{-3.f, 0.f, 1.8f}, {-2.7f, 0.f, 1.575f}, {-2.7f, -0.3f, 1.575f}, {-3.f, -0.3f, 1.35f}, {-3.f, 0.f,
	1.35f}, {-2.5f, 0.f, 1.125f}, {-2.5f, -0.3f, 1.125f}, {-2.65f, -0.3f, 0.9375f}, {-2.65f, 0.f,
	0.9375f}, {-2.f, -0.3f, 0.9f}, {-1.9f, -0.3f, 0.6f},
	// 67
	{-1.9f, 0.f, 0.6f}, {1.7f, 0.f, 1.425f}, {1.7f, -0.66f, 1.425f}, {1.7f, -0.66f, 0.6f}, {1.7f, 0.f, 0.6f}, {2.6f, 0.f, 1.425f}, {2.6f, -0.66f,
	1.425f}, {3.1f, -0.66f, 0.825f}, {3.1f, 0.f, 0.825f}, {2.3f, 0.f, 2.1f}, {2.3f, -0.25f, 2.1f},
	{2.4f, -0.25f, 2.025f}, {2.4f, 0.f, 2.025f}, {2.7f, 0.f, 2.4f}, {2.7f, -0.25f, 2.4f}, {3.3f, -0.25f,
	2.4f}, {3.3f, 0.f, 2.4f}, {2.8f, 0.f, 2.475f}, {2.8f, -0.25f, 2.475f}, {3.525f, -0.25f, 2.49375f},
	{3.525f, 0.f, 2.49375f}, {2.9f, 0.f, 2.475f}, {2.9f, -0.15f, 2.475f}, {3.45f, -0.15f, 2.5125f},
	{3.45f, 0.f, 2.5125f}, {2.8f, 0.f, 2.4f}, {2.8f, -0.15f, 2.4f}, {3.2f, -0.15f, 2.4f}, {3.2f, 0.f,
	2.4f},
	// 96:
	{0.f*LID, 0.f*LID, 3.15f*LID_Z}, {0.8f*LID, 0.f*LID, 3.15f*LID_Z}, {0.8f*LID, -0.45f*LID, 3.15f*LID_Z}, {0.45f*LID, -0.8f*LID, 3.15f*LID_Z},
	{0.f*LID, -0.8f*LID, 3.15f*LID_Z}, {0.f*LID, 0.f*LID, 2.85f*LID_Z},
	// 102:
	{1.4f, 0.f, 2.4f}, {1.4f, -0.784f, 2.4f}, {0.784f, -1.4f, 2.4f}, {0.f, -1.4f, 2.4f},
	// 106:
	{0.4f*LID, 0.f*LID, 2.55f*LID_Z}, {0.4f*LID, -0.224f*LID, 2.55f*LID_Z}, {0.224f*LID, -0.4f*LID, 2.55f*LID_Z},
	{0.f*LID, -0.4f*LID,2.55f*LID_Z}, {1.3f*LID, 0.f*LID, 2.55f*LID_Z}, {1.3f*LID, -0.728f*LID, 2.55f*LID_Z}, {0.728f*LID, -1.3f*LID, 2.55f*LID_Z}, {0.f*LID, -1.3f*LID, 2.55f*LID_Z},
	{1.3f*LID, 0.f*LID, 2.4f*LID_Z}, {1.3f*LID, -0.728f*LID, 2.4f*LID_Z}, {0.728f*LID, -1.3f*LID, 2.4f*LID_Z}, {0.f*LID, -1.3f*LID, 2.4f*LID_Z},
	// 118:
	{0.f, 0.f, 0.f}, {1.425f, -0.798f, 0.f}, {1.5f, 0.f, 0.075f}, {1.425f, 0.f, 0.f}, {0.798f, -1.425f, 0.f}, {0.f,
	-1.5f, 0.075f}, {0.f, -1.425f, 0.f}, {1.5f, -0.84f, 0.075f}, {0.84f, -1.5f, 0.075f} };

Teapot::Teapot()
	: mSubdivision( 6 )
{
	updateVertexCounts();
}

Teapot&	Teapot::subdivisions( int sub )
{
	mSubdivision = sub;
	updateVertexCounts();
	return *this;
}

uint8_t	Teapot::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 2;
		default:
			return 0;
	}
}

AttribSet Teapot::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0 };
}

void Teapot::updateVertexCounts()
{
	int numFaces = mSubdivision * mSubdivision * 32;
	mNumIndices = numFaces * 6;
	mNumVertices = 32 * (mSubdivision + 1) * (mSubdivision + 1);
}

void Teapot::calculate( vector<float> *positions, vector<float> *normals, vector<float> *texCoords, vector<uint32_t> *indices ) const
{
	positions->resize( mNumVertices * 3 );
	texCoords->resize( mNumVertices * 2 );
	normals->resize( mNumVertices * 3 );
	indices->resize( mNumIndices );

	generatePatches( positions->data(), normals->data(), texCoords->data(), indices->data(), mSubdivision );
}

void Teapot::generatePatches( float *v, float *n, float *tc, uint32_t *el, int grid )
{
	unique_ptr<float[]> B( new float[4*(grid+1)] );  // Pre-computed Bernstein basis functions
	unique_ptr<float[]> dB( new float[4*(grid+1)] ); // Pre-computed derivitives of basis functions
	int idx = 0, elIndex = 0, tcIndex = 0;

	// Pre-compute the basis functions  (Bernstein polynomials)
	// and their derivatives
	computeBasisFunctions( B.get(), dB.get(), grid );

	// Build each patch
	// The rim
	buildPatchReflect( 0, B.get(), dB.get(), v, n, tc, el, idx, elIndex, tcIndex, grid, true, true );
	// The body
	buildPatchReflect( 1, B.get(), dB.get(), v, n, tc, el, idx, elIndex, tcIndex, grid, true, true );
	buildPatchReflect( 2, B.get(), dB.get(), v, n, tc, el, idx, elIndex, tcIndex, grid, true, true );
	// The lid
	buildPatchReflect( 3, B.get(), dB.get(), v, n, tc, el, idx, elIndex, tcIndex, grid, true, true );
	buildPatchReflect( 4, B.get(), dB.get(), v, n, tc, el, idx, elIndex, tcIndex, grid, true, true );
	// The bottom
	buildPatchReflect( 5, B.get(), dB.get(), v, n, tc, el, idx, elIndex, tcIndex, grid, true, true );
	// The handle
	buildPatchReflect( 6, B.get(), dB.get(), v, n, tc, el, idx, elIndex, tcIndex, grid, false, true );
	buildPatchReflect( 7, B.get(), dB.get(), v, n, tc, el, idx, elIndex, tcIndex, grid, false, true );
	// The spout
	buildPatchReflect( 8, B.get(), dB.get(), v, n, tc, el, idx, elIndex, tcIndex, grid, false, true );
	buildPatchReflect( 9, B.get(), dB.get(), v, n, tc, el, idx, elIndex, tcIndex, grid, false, true );
}

void Teapot::buildPatchReflect( int patchNum, float *B, float *dB, float *v, float *n, float *tc, unsigned int *el,
								int &index, int &elIndex, int &tcIndex, int grid, bool reflectX, bool reflectY )
{
	vec3 patch[4][4];
	vec3 patchRevV[4][4];
	getPatch( patchNum, patch, false );
	getPatch( patchNum, patchRevV, true );

	// Patch without modification
	buildPatch( patchRevV, B, dB, v, n, tc, el, index, elIndex, tcIndex, grid, mat3(), false );

	// Patch reflected in x
	if( reflectX ) {
		mat3 reflect( glm::scale( vec3( -1, 1, 1 ) ) );
		buildPatch( patch, B, dB, v, n, tc, el, index, elIndex, tcIndex, grid, reflect, true );
	}

	// Patch reflected in y
	if( reflectY ) {
		mat3 reflect( glm::scale( vec3( 1, -1, 1 ) ) );
		buildPatch( patch, B, dB, v, n, tc, el, index, elIndex, tcIndex, grid, reflect, true );
	}

	// Patch reflected in x and y
	if( reflectX && reflectY ) {
		mat3 reflect( glm::scale( vec3( -1, -1, 1 ) ) );
		buildPatch( patchRevV, B, dB, v, n, tc, el, index, elIndex, tcIndex, grid, reflect, false );
	}
}

void Teapot::buildPatch( vec3 patch[][4], float *B, float *dB, float *v, float *n, float *tc,
						unsigned int *el, int &index, int &elIndex, int &tcIndex, int grid, mat3 reflect, bool invertNormal )
{
	int startIndex = index / 3;
	float tcFactor = 1.0f / grid;

	float scale = 2.0f / 6.42813f; // awful hack to keep it within unit cube

	for( int i = 0; i <= grid; i++ ) {
		for( int j = 0 ; j <= grid; j++) {
			vec3 pt = reflect * evaluate( i, j, B, patch );
			vec3 norm = reflect * evaluateNormal( i, j, B, dB, patch );
			if( invertNormal )
				norm = -norm;
			// awful hack due to normals discontinuity
			if( abs( pt.x ) < 0.01f && abs( pt.y ) < 0.01f )
				norm = ( pt.z < 1 ) ? vec3( 0, 0, -1 ) : vec3( 0, 0, 1 );

			v[index] = pt.x * scale; v[index+1] = pt.z * scale; v[index+2] = pt.y * scale;
			n[index] = norm.x; n[index+1] = norm.z; n[index+2] = norm.y;
			tc[tcIndex] = i * tcFactor; tc[tcIndex+1] = j * tcFactor;
			index += 3;
			tcIndex += 2;
		}
	}

	for( int i = 0; i < grid; i++ ) {
		int iStart = i * (grid+1) + startIndex;
		int nextiStart = (i+1) * (grid+1) + startIndex;
		for( int j = 0; j < grid; j++) {
			el[elIndex] = iStart + j;
			el[elIndex+1] = nextiStart + j + 1;
			el[elIndex+2] = nextiStart + j;

			el[elIndex+3] = iStart + j;
			el[elIndex+4] = iStart + j + 1;
			el[elIndex+5] = nextiStart + j + 1;

			elIndex += 6;
		}
	}
}

void Teapot::getPatch( int patchNum, vec3 patch[][4], bool reverseV )
{
	for( int u = 0; u < 4; u++) {          // Loop in u direction
		for( int v = 0; v < 4; v++ ) {     // Loop in v direction
			if( reverseV ) {
				patch[u][v] = vec3(
					sCurveData[sPatchIndices[patchNum][u*4+(3-v)]][0],
					sCurveData[sPatchIndices[patchNum][u*4+(3-v)]][1],
					sCurveData[sPatchIndices[patchNum][u*4+(3-v)]][2]
				);
			}
			else {
				patch[u][v] = vec3(
					sCurveData[sPatchIndices[patchNum][u*4+v]][0],
					sCurveData[sPatchIndices[patchNum][u*4+v]][1],
					sCurveData[sPatchIndices[patchNum][u*4+v]][2]
				);
			}
		}
	}
}

void Teapot::computeBasisFunctions( float *B, float *dB, int grid )
{
	float inc = 1.0f / grid;
	for( int i = 0; i <= grid; i++ ) {
		float t = i * inc;
		float tSqr = t * t;
		float oneMinusT = (1.0f - t);
		float oneMinusT2 = oneMinusT * oneMinusT;

		B[i*4 + 0] = oneMinusT * oneMinusT2;
		B[i*4 + 1] = 3.0f * oneMinusT2 * t;
		B[i*4 + 2] = 3.0f * oneMinusT * tSqr;
		B[i*4 + 3] = t * tSqr;

		dB[i*4 + 0] = -3.0f * oneMinusT2;
		dB[i*4 + 1] = -6.0f * t * oneMinusT + 3.0f * oneMinusT2;
		dB[i*4 + 2] = -3.0f * tSqr + 6.0f * t * oneMinusT;
		dB[i*4 + 3] = 3.0f * tSqr;
	}
}

vec3 Teapot::evaluate( int gridU, int gridV, const float *B, const vec3 patch[][4] )
{
	vec3 p;
	for( int i = 0; i < 4; i++) {
		for( int j = 0; j < 4; j++) {
			p += patch[i][j] * B[gridU*4+i] * B[gridV*4+j];
		}
	}

	return p;
}

vec3 Teapot::evaluateNormal( int gridU, int gridV, const float *B, const float *dB, const vec3 patch[][4] )
{
	vec3 du, dv;
	for( int i = 0; i < 4; i++ ) {
		for( int j = 0; j < 4; j++ ) {
			du += patch[i][j] * dB[gridU*4+i] * B[gridV*4+j];
			dv += patch[i][j] * B[gridU*4+i] * dB[gridV*4+j];
		}
	}

	return normalize( cross( du, dv ) );
}

void Teapot::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	vector<float> positions, normals, texCoords;
	vector<uint32_t> indices;
	
	calculate( &positions, &normals, &texCoords, &indices );

	target->copyAttrib( Attrib::POSITION, 3, 0, positions.data(), mNumVertices );
	target->copyAttrib( Attrib::NORMAL, 3, 0, normals.data(), mNumVertices );
	target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, texCoords.data(), mNumVertices );

	target->copyIndices( Primitive::TRIANGLES, indices.data(), mNumIndices, 4 );
}

///////////////////////////////////////////////////////////////////////////////////////
// Circle
Circle::Circle()
	: mRequestedSubdivisions( -1 ), mCenter( 0, 0 ), mRadius( 1.0f )
{
	updateVertexCounts();
}

Circle&	Circle::subdivisions( int subdivs )
{
	mRequestedSubdivisions = subdivs;
	updateVertexCounts();
	return *this;
}

Circle&	Circle::radius( float radius )
{
	mRadius = radius;
	updateVertexCounts();
	return *this;
}

// If numSegments<0, calculate based on radius
void Circle::updateVertexCounts()
{
	if( mRequestedSubdivisions <= 0 )
		mNumSubdivisions = (int)math<double>::floor( mRadius * float(M_PI * 2) );
	else
		mNumSubdivisions = mRequestedSubdivisions;

	if( mNumSubdivisions < 3 ) mNumSubdivisions = 3;
	mNumVertices = mNumSubdivisions + 1 + 1;
}

size_t Circle::getNumVertices() const
{
	return mNumVertices;
}

uint8_t	Circle::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 2;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 2;
		default:
			return 0;
	}
}

AttribSet Circle::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0 };
}

void Circle::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	std::vector<vec2>	positions, texCoords;
	std::vector<vec3>	normals;

	positions.reserve( mNumVertices );
	texCoords.reserve( mNumVertices );
	normals.reserve( mNumVertices );

	// center
	positions.emplace_back( mCenter );
	texCoords.emplace_back( 0.5f, 0.5f );
	normals.emplace_back( 0, 0, 1 );

	// iterate the segments
	const float tDelta = 1 / (float)mNumSubdivisions * 2.0f * 3.14159f;
	float t = 0;
	for( int s = 0; s <= mNumSubdivisions; s++ ) {
		vec2 unit( math<float>::cos( t ), math<float>::sin( t ) );
		positions.emplace_back( mCenter + unit * mRadius );
		texCoords.emplace_back( unit * 0.5f + vec2( 0.5f ) );
		normals.emplace_back( 0, 0, 1 );
		t += tDelta;
	}

	target->copyAttrib( Attrib::POSITION, 2, 0, (const float*)positions.data(), mNumVertices );
	target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*)normals.data(), mNumVertices );
	target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, (const float*)texCoords.data(), mNumVertices );
}

///////////////////////////////////////////////////////////////////////////////////////
// Sphere

Sphere::Sphere()
	: mSubdivisions( 18 ), mCenter( 0, 0, 0 ), mRadius( 1.0f ), mHasColors( false )
{
}

void Sphere::numRingsAndSegments( int *numRings, int *numSegments ) const
{
	*numSegments = mSubdivisions;
	if( *numSegments < 4 )
		*numSegments = std::max( 12, (int)math<double>::floor( mRadius * float(M_PI * 2) ) );

	*numRings = ( *numSegments >> 1 ) + 1;
	*numSegments += 1;
}

size_t Sphere::getNumVertices() const
{
	int numRings, numSegments;
	numRingsAndSegments( &numRings, &numSegments );
	return numSegments * numRings;
}

size_t Sphere::getNumIndices() const
{
	int numRings, numSegments;
	numRingsAndSegments( &numRings, &numSegments );
	return numSegments * numRings * 6;
}

uint8_t Sphere::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 2;
		case Attrib::COLOR: return mHasColors ? 3 : 0;
		default:
			return 0;
	}
}

AttribSet Sphere::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::COLOR };
}

void Sphere::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	int numRings, numSegments;
	numRingsAndSegments( &numRings, &numSegments );

	std::vector<vec3> positions;
	std::vector<vec2> texCoords;
	std::vector<vec3> normals;
	std::vector<vec3> colors;
	std::vector<uint32_t> indices;

	positions.resize( numSegments * numRings );
	normals.resize( numSegments * numRings );
	texCoords.resize( numSegments * numRings );
	colors.resize( numSegments * numRings );
	indices.resize( numSegments * numRings * 6 );

	float ringIncr = 1.0f / (float)( numRings - 1 );
	float segIncr = 1.0f / (float)( numSegments - 1 );
	float radius = mRadius;

	auto vertIt = positions.begin();
	auto normIt = normals.begin();
	auto texIt = texCoords.begin();
	auto colorIt = colors.begin();
	for( size_t r = 0; r < numRings; r++ ) {
		float v = r * ringIncr;
		for( size_t s = 0; s < numSegments; s++ ) {
			float u = 1.0f - s * segIncr;
			float x = math<float>::sin( float(M_PI * 2) * u ) * math<float>::sin( float(M_PI) * v );
			float y = math<float>::sin( float(M_PI) * (v - 0.5f) );
			float z = math<float>::cos( float(M_PI * 2) * u ) * math<float>::sin( float(M_PI) * v );

			*vertIt++ = vec3( x * radius + mCenter.x, y * radius + mCenter.y, z * radius + mCenter.z );

			*normIt++ = vec3( x, y, z );
			*texIt++ = vec2( u, v );
			*colorIt++ = vec3( x * 0.5f + 0.5f, y * 0.5f + 0.5f, z * 0.5f + 0.5f );
		}
	}

	auto indexIt = indices.begin();
	for( size_t r = 0; r < numRings - 1; r++ ) {
		for( size_t s = 0; s < numSegments - 1 ; s++ ) {
			*indexIt++ = (uint32_t)(r * numSegments + ( s + 1 ));
			*indexIt++ = (uint32_t)(r * numSegments + s);
			*indexIt++ = (uint32_t)(( r + 1 ) * numSegments + ( s + 1 ));

			*indexIt++ = (uint32_t)(( r + 1 ) * numSegments + s);
			*indexIt++ = (uint32_t)(( r + 1 ) * numSegments + ( s + 1 ));
			*indexIt++ = (uint32_t)(r * numSegments + s);
		}
	}
	
	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *positions.data() ), positions.size() );
	target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *normals.data() ), normals.size() );
	target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *texCoords.data() ), texCoords.size() );
	target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *colors.data() ), colors.size() );

	target->copyIndices( Primitive::TRIANGLES, indices.data(), indices.size(), 4 );
}

///////////////////////////////////////////////////////////////////////////////////////
// Capsule
Capsule::Capsule()
	: mDirection( 0, 1, 0 ), mLength( 1.0f ), mSubdivisionsAxis( 6 ), mHasColors( false )
{
	radius( 0.5f );
	subdivisionsHeight( 6 );
}

Capsule& Capsule::set( const vec3 &from, const vec3 &to )
{
	const vec3 axis = to - from;
	mLength = glm::length( axis );
	mDirection = normalize( axis );
	mCenter = from + 0.5f * axis;
	return *this;
}

void Capsule::updateCounts()
{
	mNumSegments = mSubdivisionsAxis;
	if( mNumSegments < 4 )
		mNumSegments = std::max( 12, (int)math<double>::floor( mRadius * float(M_PI * 2) ) );
	mSubdivisionsHeight = std::max( mSubdivisionsHeight, 2 );
}

void Capsule::calculate( vector<vec3> *positions, vector<vec3> *normals, vector<vec2> *texCoords, vector<vec3> *colors, vector<uint32_t> *indices ) const
{
	size_t ringsBody = mSubdivisionsHeight + 1;
	size_t ringsTotal = mSubdivisionsHeight + ringsBody;

	positions->reserve( mNumSegments * ringsTotal );
	normals->reserve( mNumSegments * ringsTotal );
	texCoords->reserve( mNumSegments * ringsTotal );
	colors->reserve( mNumSegments * ringsTotal );
	indices->reserve( mNumSegments * ringsTotal * 6 );

	float bodyIncr = 1.0f / (float)( ringsBody - 1 );
	float ringIncr = 1.0f / (float)( mSubdivisionsHeight - 1 );
	for( size_t r = 0; r < mSubdivisionsHeight / 2; r++ )
		calculateRing( mNumSegments, math<float>::sin( float(M_PI) * r * ringIncr), math<float>::sin( float(M_PI) * ( r * ringIncr - 0.5f ) ), -0.5f,
							positions, normals, texCoords, colors );
	for( size_t r = 0; r < ringsBody; r++ )
		calculateRing( mNumSegments, 1.0f, 0.0f, r * bodyIncr - 0.5f,
							positions, normals, texCoords, colors );
	for( size_t r = mSubdivisionsHeight / 2; r < mSubdivisionsHeight; r++ )
		calculateRing( mNumSegments, math<float>::sin( float(M_PI) * r * ringIncr), math<float>::sin( float(M_PI) * ( r * ringIncr - 0.5f ) ), +0.5f,
							positions, normals, texCoords, colors );

	for( size_t r = 0; r < ringsTotal - 1; r++ ) {
		for( size_t s = 0; s < mNumSegments - 1; s++ ) {
			indices->push_back( (uint32_t)(r * mNumSegments + ( s + 1 )) );
			indices->push_back( (uint32_t)(r * mNumSegments + ( s + 0 )) );
			indices->push_back( (uint32_t)(( r + 1 ) * mNumSegments + ( s + 1 )) );

			indices->push_back( (uint32_t)(( r + 1 ) * mNumSegments + ( s + 0 )) );
			indices->push_back( (uint32_t)(( r + 1 ) * mNumSegments + ( s + 1 )) );
			indices->push_back( (uint32_t)(r * mNumSegments + s) );
		}
	}
}

void Capsule::calculateRing( size_t segments, float radius, float y, float dy,
								vector<vec3> *positions, vector<vec3> *normals, vector<vec2> *texCoords, vector<vec3> *colors ) const
{
	const quat quaternion( vec3( 0, 1, 0 ), mDirection );

	float segIncr = 1.0f / (float)( segments - 1 );
	for( size_t s = 0; s < segments; s++ ) {
		float x = math<float>::cos( float(M_PI * 2) * s * segIncr ) * radius;
		float z = math<float>::sin( float(M_PI * 2) * s * segIncr ) * radius;

		positions->emplace_back( mCenter + ( quaternion * glm::vec3( mRadius * x, mRadius * y + mLength * dy, mRadius * z ) ) );

		normals->emplace_back( quaternion * glm::vec3( x, y, z ) );
		// perform cylindrical projection
		float u = 1.0f - (s * segIncr);
		float v = 0.5f - ((mRadius * y + mLength * dy) / (2.0f * mRadius + mLength));
		texCoords->emplace_back( u, v );

		float g = 0.5f + ((mRadius * y + mLength * dy) / (2.0f * mRadius + mLength));
		colors->emplace_back( x * 0.5f + 0.5f, g, z * 0.5f + 0.5f );
	}
}

size_t Capsule::getNumVertices() const
{
	return mNumSegments * ( mSubdivisionsHeight * 2 + 1 );
}

size_t Capsule::getNumIndices() const
{
	return mNumSegments * ( mSubdivisionsHeight * 2 + 1 ) * 6;
}

uint8_t Capsule::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 2;
		case Attrib::COLOR: return mHasColors ? 3 : 0;
		default:
			return 0;
	}
}

AttribSet Capsule::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::COLOR };
}

void Capsule::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	std::vector<vec3> positions, normals;
	std::vector<vec2> texCoords;
	std::vector<vec3> colors;
	std::vector<uint32_t> indices;

	calculate( &positions, &normals, &texCoords, &colors, &indices );
	
	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *positions.data() ), positions.size() );
	target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *normals.data() ), normals.size() );
	target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *texCoords.data() ), texCoords.size() );
	target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *colors.data() ), colors.size() );

	target->copyIndices( Primitive::TRIANGLES, indices.data(), indices.size(), 4 );
}


///////////////////////////////////////////////////////////////////////////////////////
// Torus
Torus::Torus()
	: mCenter( 0, 0, 0), mRadiusMajor( 1.0f ), mRadiusMinor( 0.75f ), mCoils( 1 ), mHeight( 0 ),
	mSubdivisionsAxis( 18 ), mSubdivisionsHeight( 18 ), mTwist( 0 ), mTwistOffset( 0 ), mHasColors( false )
{
	updateCounts();
}

void Torus::updateCounts()
{
	mNumAxis = (int) math<float>::ceil( mSubdivisionsAxis * mCoils );
	if( mNumAxis < 4 )
		mNumAxis = std::max( 12, (int)math<double>::floor( mRadiusMajor * float(M_PI * 2) ) );
	mNumAxis += 1;

	mNumRings = mSubdivisionsHeight;
	if( mNumRings < 3 )
		mNumRings = std::max( 12, (int)math<double>::floor( mRadiusMajor * float(M_PI * 2) ) ) + 1;
	mNumRings += 1;
}

size_t Torus::getNumVertices() const
{
	return mNumAxis * mNumRings;
}

size_t Torus::getNumIndices() const
{
	return (mNumAxis - 1) * (mNumRings - 1) * 6;
}

void Torus::calculate( vector<vec3> *positions, vector<vec3> *normals, vector<vec2> *texCoords, vector<vec3> *colors, vector<uint32_t> *indices ) const
{
	positions->reserve( mNumAxis * mNumRings );
	normals->reserve( mNumAxis * mNumRings );
	texCoords->reserve( mNumAxis * mNumRings );
	colors->reserve( mNumAxis * mNumRings );
	indices->reserve( (mNumAxis - 1) * (mNumRings - 1) * 6 );

	float majorIncr = 1.0f / (mNumAxis - 1);
	float minorIncr = 1.0f / (mNumRings - 1);
	float radiusDiff = mRadiusMajor - mRadiusMinor;
	float angle = float(M_PI * 2) * mCoils;
	float twist = angle * mTwist * minorIncr * majorIncr;

	// vertex, normal, tex coord and color buffers
	for( size_t i = 0; i < mNumAxis; ++i ) {
		float phi = i * majorIncr * angle;
		float cosPhi = -math<float>::cos( phi );
		float sinPhi =  math<float>::sin( phi );

		for( size_t j = 0; j < mNumRings; ++j ) {
			float theta = j * minorIncr * float(M_PI * 2) + i * twist + mTwistOffset;
			float cosTheta = -math<float>::cos( theta );
			float sinTheta =  math<float>::sin( theta );

			float r = mRadiusMinor + cosTheta * radiusDiff;
			float x = r * cosPhi;
			float y = i * majorIncr * mHeight + sinTheta * radiusDiff;
			float z = r * sinPhi;

			positions->emplace_back( mCenter + vec3( x, y, z ) );
			texCoords->emplace_back( i * majorIncr, j * minorIncr );
			normals->emplace_back( cosPhi * cosTheta, sinTheta, sinPhi * cosTheta );

			const vec3 &n = normals->back();
			colors->emplace_back( n.x * 0.5f + 0.5f, n.y * 0.5f + 0.5f, n.z * 0.5f + 0.5f );
		}
	}

	// index buffer
	for( size_t i = 0; i < mNumAxis - 1; ++i ) {
		for ( size_t j = 0; j < mNumRings - 1; ++j ) {
			indices->push_back( (uint32_t)((i + 0) * mNumRings + (j + 0)) );
			indices->push_back( (uint32_t)((i + 1) * mNumRings + (j + 1)) );
			indices->push_back( (uint32_t)((i + 1) * mNumRings + (j + 0)) );

			indices->push_back( (uint32_t)((i + 0) * mNumRings + (j + 0)) );
			indices->push_back( (uint32_t)((i + 0) * mNumRings + (j + 1)) );
			indices->push_back( (uint32_t)((i + 1) * mNumRings + (j + 1)) );
		}
	}
}

uint8_t Torus::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 2;
		case Attrib::COLOR: return mHasColors ? 3 : 0;
		default:
			return 0;
	}
}

AttribSet Torus::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::COLOR };
}

void Torus::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	std::vector<vec3> positions, normals;
	std::vector<vec2> texCoords;
	std::vector<vec3> colors;
	std::vector<uint32_t> indices;

	calculate( &positions, &normals, &texCoords, &colors, &indices );

	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *positions.data() ), positions.size() );
	target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *normals.data() ), normals.size() );
	target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *texCoords.data() ), texCoords.size() );
	target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *colors.data() ), colors.size() );

	target->copyIndices( Primitive::TRIANGLES, indices.data(), indices.size(), 4 );
}

///////////////////////////////////////////////////////////////////////////////////////
// Cylinder
Cylinder::Cylinder()
	: mOrigin( 0, 0, 0 ), mHeight( 2.0f ), mDirection( 0, 1, 0 ), mRadiusBase( 1.0f ), mRadiusApex( 1.0f ),
		mSubdivisionsAxis( 18 ), mSubdivisionsHeight( 1 ), mHasColors( false )
{
	updateCounts();
}

Cylinder& Cylinder::set( const vec3 &from, const vec3 &to )
{
	const vec3 axis = ( to - from );
	mHeight = length( axis );
	mDirection = normalize( axis );
	mOrigin = from;
	return *this;
}

void Cylinder::updateCounts()
{
	mNumSegments = mSubdivisionsAxis + 1;
	if( mNumSegments < 4 ) {
		float radius = math<float>::max( mRadiusBase, mRadiusApex );
		mNumSegments = std::max( 12, (int)math<double>::floor( radius * float(M_PI * 2) ) ) + 1;
	}
	mNumSlices = math<int>::max( mSubdivisionsHeight, 1 ) + 1;
}

size_t Cylinder::getNumVertices() const
{
	size_t result = mNumSegments * mNumSlices;
	if( mRadiusBase > 0 )
		result += mNumSegments * 2;
	if( mRadiusApex > 0.0f )
		result += mNumSegments * 2;
	return result;
}

size_t Cylinder::getNumIndices() const
{
	size_t result = (mNumSegments - 1) * (mNumSlices - 1) * 6;
	if( mRadiusBase > 0 )
		result += 3 * (mNumSegments - 1);
	if( mRadiusApex > 0 )
		result += 3 * (mNumSegments - 1);
	return result;
}

void Cylinder::calculate( vector<vec3> *positions, vector<vec3> *normals, vector<vec2> *texCoords, vector<vec3> *colors, vector<uint32_t> *indices ) const
{
	positions->reserve( mNumSegments * mNumSlices );
	normals->reserve( mNumSegments * mNumSlices );
	texCoords->reserve( mNumSegments * mNumSlices );
	indices->reserve( (mNumSegments - 1) * (mNumSlices - 1) * 6 );

	colors->reserve( mNumSegments * mNumSlices );

	const float segmentIncr = 1.0f / (mNumSegments - 1);
	const float ringIncr = 1.0f / (mNumSlices - 1);
	const quat axis( vec3( 0, 1, 0 ), mDirection );

	// vertex, normal, tex coord and color buffers
	for( size_t i = 0; i < mNumSegments; ++i ) {
		for( size_t j = 0; j < mNumSlices; ++j ) {
			float cosPhi = -math<float>::cos( i * segmentIncr * float(M_PI * 2) );
			float sinPhi =  math<float>::sin( i * segmentIncr * float(M_PI * 2) );

			float r = lerp<float>( mRadiusBase, mRadiusApex, j * ringIncr );
			float x = r * cosPhi;
			float y = mHeight * j * ringIncr;
			float z = r * sinPhi;
			const vec3 n = normalize( vec3( mHeight * cosPhi, mRadiusBase - mRadiusApex, mHeight * sinPhi ) );

			positions->emplace_back( mOrigin + axis * vec3( x, y, z ) );
			texCoords->emplace_back( i * segmentIncr, 1.0f - j * ringIncr );
			normals->emplace_back( axis * n );
			colors->emplace_back( n.x * 0.5f + 0.5f, n.y * 0.5f + 0.5f, n.z * 0.5f + 0.5f );
		}
	}

	// index buffer
	for ( size_t j = 0; j < mNumSlices - 1; ++j ) {
		for( size_t i = 0; i < mNumSegments - 1; ++i ) {
			indices->push_back( (uint32_t)((i + 0) * mNumSlices + (j + 0)) );
			indices->push_back( (uint32_t)((i + 1) * mNumSlices + (j + 0)) );
			indices->push_back( (uint32_t)((i + 1) * mNumSlices + (j + 1)) );

			indices->push_back( (uint32_t)((i + 0) * mNumSlices + (j + 0)) );
			indices->push_back( (uint32_t)((i + 1) * mNumSlices + (j + 1)) );
			indices->push_back( (uint32_t)((i + 0) * mNumSlices + (j + 1)) );
		}
	}

	// caps
	if( mRadiusBase > 0.0f )
		calculateCap( true, 0.0f, mRadiusBase, positions, normals, texCoords, colors, indices );

	if( mRadiusApex > 0.0f )
		calculateCap( false, mHeight, mRadiusApex, positions, normals, texCoords, colors, indices );
}

void Cylinder::calculateCap( bool flip, float height, float radius, vector<vec3> *positions, vector<vec3> *normals,
								vector<vec2> *texCoords, vector<vec3> *colors, vector<uint32_t> *indices ) const
{
	const size_t index = positions->size();
	const vec3 n = flip ? -mDirection : mDirection;
	normals->resize( index + mNumSegments * 2, n );
	colors->resize( index + mNumSegments * 2, vec3( n.x * 0.5f + 0.5f, n.y * 0.5f + 0.5f, n.z * 0.5f + 0.5f ) );

	const quat axis( vec3( 0, 1, 0 ), mDirection );

	// vertices
	const float segmentIncr = 1.0f / (mNumSegments - 1);
	for( size_t i = 0; i < mNumSegments; ++i ) {
		// center point
		positions->emplace_back( mOrigin + mDirection * height );
		texCoords->emplace_back( i * segmentIncr, 1.0f - height / mHeight );

		// edge point
		float cosPhi = -math<float>::cos( i * segmentIncr * float(M_PI * 2) );
		float sinPhi =  math<float>::sin( i * segmentIncr * float(M_PI * 2) );

		float x = radius * cosPhi;
		float y = height;
		float z = radius * sinPhi;

		positions->emplace_back( mOrigin + axis * vec3( x, y, z ) );
		texCoords->emplace_back( i * segmentIncr, 1.0f - height / mHeight );
	}

	// index buffer
	indices->reserve( indices->size() + 3 * (mNumSegments - 1) );

	for( size_t i = 0; i < mNumSegments - 1; ++i ) {
		if( flip ) {
			indices->push_back( (uint32_t)(index + i * 2 + 0) );
			indices->push_back( (uint32_t)(index + i * 2 + 3) );
			indices->push_back( (uint32_t)(index + i * 2 + 1) );
		}
		else {
			indices->push_back( (uint32_t)(index + i * 2 + 0) );
			indices->push_back( (uint32_t)(index + i * 2 + 1) );
			indices->push_back( (uint32_t)(index + i * 2 + 3) );
		}
	}
}

uint8_t Cylinder::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 2;
		case Attrib::COLOR: return mHasColors ? 3 : 0;
		default:
			return 0;
	}
}

AttribSet Cylinder::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::COLOR };
}

void Cylinder::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	vector<vec3> positions, normals, colors;
	vector<vec2> texCoords;
	vector<uint32_t> indices;

	calculate( &positions, &normals, &texCoords, &colors, &indices );

	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *positions.data() ), positions.size() );
	target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *normals.data() ), normals.size() );
	target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *texCoords.data() ), texCoords.size() );
	target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *colors.data() ), colors.size() );

	target->copyIndices( Primitive::TRIANGLES, indices.data(), indices.size(), 4 );
}

///////////////////////////////////////////////////////////////////////////////////////
// Plane
Plane::Plane()
	: mOrigin( 0, 0, 0 ), mAxisU( 1, 0, 0 ), mAxisV( 0, 0, 1 ), mSize( 2, 2 ), mSubdivisions( 1, 1 )
{
}

Plane& Plane::subdivisions( const ivec2 &subdivisions )
{
	mSubdivisions.x = std::max( subdivisions.x, 1 );
	mSubdivisions.y = std::max( subdivisions.y, 1 );
	return *this;
}

Plane& Plane::normal( const vec3 &normal )
{
	auto normalNormal = normalize( normal );
	float yAxisDot = dot( normalNormal, vec3( 0, 1, 0 ) );
	if( abs( yAxisDot ) < 0.999f ) {
		quat normalQuat( vec3( 0, 1, 0 ), normalNormal );
		mAxisU = normalQuat * vec3( 1, 0, 0 );
		mAxisV = normalQuat * vec3( 0, 0, 1 );
	}
	else {
		quat normalQuat( vec3( 0, 0, 1 ), normalNormal );
		mAxisU = normalQuat * vec3( 1, 0, 0 );
		mAxisV = normalQuat * vec3( 0, -1, 0 );
	}

	return *this;
}

Plane& Plane::axes( const vec3 &uAxis, const vec3 &vAxis )
{
	mAxisU = normalize( uAxis );
	mAxisV = normalize( vAxis );
	return *this;
}

uint8_t Plane::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 2;
		default:
			return 0;
	}
}

AttribSet Plane::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0 };
}

void Plane::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	std::vector<vec3> positions, normals;
	std::vector<vec2> texCoords;
	std::vector<uint32_t> indices;

	const size_t numVerts = ( mSubdivisions.x + 1 ) * ( mSubdivisions.y + 1 );
	positions.reserve( numVerts );
	normals.reserve( numVerts );
	texCoords.reserve( numVerts );

	const vec2 stepIncr = vec2( 1, 1 ) / vec2( mSubdivisions );
	const vec3 normal = cross( mAxisV, mAxisU );

	// fill vertex data
	for( int x = 0; x <= mSubdivisions.x; x++ ) {
		for( int y = 0; y <= mSubdivisions.y; y++ ) {
			float u = x * stepIncr.x;
			float v = y * stepIncr.y;
			positions.emplace_back( mOrigin + ( mSize.x * ( u - 0.5f ) ) * mAxisU + ( mSize.y * ( v - 0.5f ) ) * mAxisV );
			normals.emplace_back( normal );
			texCoords.emplace_back( u, v );
		}
	}

	// fill indices
	for( int x = 0; x < mSubdivisions.x; x++ ) {
		for( int y = 0; y < mSubdivisions.y; y++ ) {
			const uint32_t i = x * ( mSubdivisions.y + 1 ) + y;

			indices.push_back( i );
			indices.push_back( i + 1 );
			indices.push_back( i + mSubdivisions.y + 1 );

			indices.push_back( i + mSubdivisions.y + 1 );
			indices.push_back( i + 1 );
			indices.push_back( i + mSubdivisions.y + 2 );
		}
	}


	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *positions.data() ), positions.size() );
	target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *normals.data() ), normals.size() );
	target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *texCoords.data() ), texCoords.size() );
	
	target->copyIndices( Primitive::TRIANGLES, indices.data(), indices.size(), 4 );
}

///////////////////////////////////////////////////////////////////////////////////////
// Transform
uint8_t Transform::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return std::max<uint8_t>( 3, mSource.getAttribDims( Attrib::POSITION ) );
		default:
			return mSource.getAttribDims( attr );
	}
}

AttribSet Transform::getAvailableAttribs() const
{
	return mSource.getAvailableAttribs();
}

void Transform::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	// we want to capture and then modify both positions and normals
	map<Attrib,Modifier::Access> attribAccess;
	attribAccess[POSITION] = Modifier::READ_WRITE;
	attribAccess[NORMAL] = Modifier::READ_WRITE;
	Modifier modifier( mSource, target, attribAccess, Modifier::IGNORED );
	mSource.loadInto( &modifier, requestedAttribs );
	
	const size_t numVertices = mSource.getNumVertices();

	if( modifier.getReadAttribDims( POSITION ) == 2 ) {
		vec2* positions = reinterpret_cast<vec2*>( modifier.getReadAttribData( POSITION ) );
		for( size_t v = 0; v < numVertices; ++v )
			positions[v] = vec2( mTransform * vec4( positions[v], 0, 1 ) );
		target->copyAttrib( Attrib::POSITION, 2, 0, (const float*)positions, numVertices );
	}
	else if( modifier.getReadAttribDims( POSITION ) == 3 ) {
		vec3* positions = reinterpret_cast<vec3*>( modifier.getReadAttribData( POSITION ) );
		for( size_t v = 0; v < numVertices; ++v )
			positions[v] = vec3( mTransform * vec4( positions[v], 1 ) );
		target->copyAttrib( Attrib::POSITION, 3, 0, (const float*)positions, numVertices );
	}
	else if( modifier.getReadAttribDims( POSITION ) == 4 ) {
		vec4* positions = reinterpret_cast<vec4*>( modifier.getReadAttribData( POSITION ) );
		for( size_t v = 0; v < numVertices; ++v )
			positions[v] = mTransform * positions[v];
		target->copyAttrib( Attrib::POSITION, 4, 0, (const float*)positions, numVertices );
	}
	else if( modifier.getReadAttribDims( POSITION ) != 0 )
		CI_LOG_W( "Unsupported dimension for geom::POSITION passed to geom::Transform" );
	
	// and finally, we'll make the sort of modification to our normals (if they're present)
	// using the inverse transpose of 'mTransform'
	if( modifier.getReadAttribDims( NORMAL ) == 3 ) {
		vec3* normals = reinterpret_cast<vec3*>( modifier.getReadAttribData( NORMAL ) );
		mat3 normalsTransform = glm::transpose( inverse( mat3( mTransform ) ) );
		for( size_t v = 0; v < numVertices; ++v )
			normals[v] = normalsTransform * normals[v];
		target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*)normals, numVertices );
	}
	else if( modifier.getReadAttribDims( NORMAL ) != 0 )
		CI_LOG_W( "Unsupported dimension for geom::NORMAL passed to geom::Transform" );
}

///////////////////////////////////////////////////////////////////////////////////////
// Twist
uint8_t Twist::getAttribDims( Attrib attr ) const
{
	return mSource.getAttribDims( attr );
}

AttribSet Twist::getAvailableAttribs() const
{
	return mSource.getAvailableAttribs();
}

void Twist::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	// we want to capture and then modify both positions and normals
	map<Attrib,Modifier::Access> attribAccess;
	attribAccess[POSITION] = Modifier::READ_WRITE;
	attribAccess[NORMAL] = Modifier::READ_WRITE;
	Modifier modifier( mSource, target, attribAccess, Modifier::IGNORED );
	mSource.loadInto( &modifier, requestedAttribs );
	
	const size_t numVertices = mSource.getNumVertices();
	const float invAxisLength = 1.0f / distance( mAxisStart, mAxisEnd );
	const vec3 axisDir = ( mAxisEnd - mAxisStart ) * vec3( invAxisLength );

	if( modifier.getReadAttribDims( POSITION ) == 3 ) {
		vec3* positions = reinterpret_cast<vec3*>( modifier.getReadAttribData( POSITION ) );
		vec3* normals = nullptr;
		if( modifier.getReadAttribDims( NORMAL ) == 3 )
			normals = reinterpret_cast<vec3*>( modifier.getReadAttribData( NORMAL ) );
		
		for( size_t v = 0; v < numVertices; ++v ) {
			// find the 't' value of the point on the axis that inPosition is closest to
			float closestDist = dot( positions[v] - mAxisStart, axisDir );
			float tVal = glm::clamp<float>( closestDist * invAxisLength, 0, 1 );
			// 'pointOnAxis' is the actual point on the axis inPosition is closest to
			vec3 pointOnAxis = mAxisStart + axisDir * closestDist;
			// our rotation is around the axis, and the angle is a lerp between 'mStartAngle' and 'mEndAngle' based on 't'
			mat4 rotation = rotate( glm::mix( mStartAngle, mEndAngle, tVal ), axisDir );
			// now transform the point by rotating around 'pointOnAxis'
			mat4 transform = translate( pointOnAxis ) * rotation * translate( -pointOnAxis );
			vec3 outPos = vec3( transform * vec4( positions[v], 1 ) );
			positions[v] = outPos;
			// we need to transform the normal by rotating it by the same angle (but not around the point) we did the position
			if( normals ) {
				normals[v] = vec3( rotation * vec4( normals[v], 0 ) );
			}
		}
		target->copyAttrib( Attrib::POSITION, 3, 0, (const float*)positions, numVertices );
		if( normals )
			target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*)normals, numVertices );
	}
	else if( modifier.getReadAttribDims( POSITION ) != 0 )
		CI_LOG_W( "Unsupported dimension for geom::POSITION passed to geom::Twist" );
}

///////////////////////////////////////////////////////////////////////////////////////
// Lines
size_t Lines::getNumIndices() const
{
	switch( mSource.getPrimitive() ) {
		case LINES:
			return mSource.getNumIndices();
		break;
		case LINE_STRIP:
			return mSource.getNumIndices() ? ((mSource.getNumIndices() - 1) * 2 ) : ((mSource.getNumVertices() - 1) * 2 );
		break;
		case TRIANGLES:
			return mSource.getNumIndices() ? (mSource.getNumIndices() * 2) : (mSource.getNumVertices() * 2);
		break;
		case TRIANGLE_STRIP:
			return std::max<int>( 0, mSource.getNumIndices() ? (int)((mSource.getNumIndices() - 2) * 6 )
				: (int)(mSource.getNumVertices() - 2) * 6 );
		break;
		case TRIANGLE_FAN:
			return std::max<int>( 0, mSource.getNumIndices() ? (int)(mSource.getNumIndices() * 4 - 2 )
				: (int)(mSource.getNumVertices() * 4 - 2 ) );
		break;
	}
	return mSource.getNumIndices();
}

void Lines::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	// we are only interested in changing indices
	Modifier modifier( mSource, target, map<Attrib,Modifier::Access>(), Modifier::READ_WRITE );
	mSource.loadInto( &modifier, requestedAttribs );

	const size_t numInIndices = modifier.getNumIndices();
	const size_t numInVertices = mSource.getNumVertices();

	if( getNumIndices() < 2 ) { // early exit
		target->copyIndices( geom::LINES, modifier.getIndicesData(), modifier.getNumIndices(), 4 );
		return;
	}

	switch( mSource.getPrimitive() ) {
		case Primitive::LINES: // pass-through
			target->copyIndices( geom::LINES, modifier.getIndicesData(), modifier.getNumIndices(), 4 );
		break;
		case Primitive::LINE_STRIP: {
			vector<uint32_t> outIndices;
			outIndices.reserve( getNumIndices() );
			const uint32_t *indices = modifier.getIndicesData();
			if( indices ) {
				for( size_t i = 0; i < numInIndices - 1; i++ ) {
					outIndices.push_back( indices[i] ); outIndices.push_back( indices[i + 1] );
				}
			}
			else {
				for( size_t i = 0; i < numInVertices - 1; i++ ) {
					outIndices.push_back( (uint32_t)i ); outIndices.push_back( (uint32_t)(i + 1) );
				}
			}
		}
		break;
		case Primitive::TRIANGLE_FAN: {
			vector<uint32_t> outIndices;
			outIndices.reserve( getNumIndices() );
			const uint32_t *indices = modifier.getIndicesData();
			if( indices ) {
				for( size_t i = 1; i < numInIndices; i++ ) { // lines connecting first vertex ("hub") and all others
					outIndices.push_back( indices[0] ); outIndices.push_back( indices[i] );
				}
				for( size_t j = numInIndices-1, i = 0; i < numInIndices; j = i++ ) {// lines connecting adjacent vertices
					outIndices.push_back( indices[j] ); outIndices.push_back( indices[i] );
				}
			}
			else {
				for( size_t i = 1; i < numInVertices; i++ ) { // lines connecting first vertex ("hub") and all others
					outIndices.push_back( 0 ); outIndices.push_back( (uint32_t)i );
				}
				for( size_t j = numInVertices-1, i = 0; i < numInVertices; j = i++ ) {// lines connecting adjacent vertices
					outIndices.push_back( (uint32_t)j ); outIndices.push_back( (uint32_t)i );
				}
			}
			
			target->copyIndices( geom::LINES, outIndices.data(), outIndices.size(), 4 );
		}
		break;
		case Primitive::TRIANGLES: {
			vector<uint32_t> outIndices;
			outIndices.reserve( getNumIndices() );
			const uint32_t *indices = modifier.getIndicesData();
			if( indices ) {
				for( size_t i = 0; i < numInIndices; i += 3 ) {
					outIndices.push_back( indices[i + 0] ); outIndices.push_back( indices[i + 1] );
					outIndices.push_back( indices[i + 1] ); outIndices.push_back( indices[i + 2] );
					outIndices.push_back( indices[i + 2] ); outIndices.push_back( indices[i + 0] );
				}
			}
			else {
				for( uint32_t i = 0; i < numInVertices; i += 3 ) {
					outIndices.push_back( i + 0 ); outIndices.push_back( i + 1 );
					outIndices.push_back( i + 1 ); outIndices.push_back( i + 2 );
					outIndices.push_back( i + 2 ); outIndices.push_back( i + 0 );
				}
			}
			
			target->copyIndices( geom::LINES, outIndices.data(), outIndices.size(), 4 );
		}
		break;
		case Primitive::TRIANGLE_STRIP: {
			vector<uint32_t> outIndices;
			outIndices.reserve( getNumIndices() );
			const uint32_t *indices = modifier.getIndicesData();
			if( indices ) {
				for( size_t i = 0; i < numInIndices - 2; i++ ) {
					outIndices.push_back( indices[i + 0] ); outIndices.push_back( indices[i + 1] );
					outIndices.push_back( indices[i + 1] ); outIndices.push_back( indices[i + 2] );
					outIndices.push_back( indices[i + 2] ); outIndices.push_back( indices[i + 0] );
				}
			}
			else {
				for( uint32_t i = 0; i < numInVertices - 2; i++ ) {
					outIndices.push_back( i + 0 ); outIndices.push_back( i + 1 );
					outIndices.push_back( i + 1 ); outIndices.push_back( i + 2 );
					outIndices.push_back( i + 2 ); outIndices.push_back( i + 0 );
				}
			}
			
			target->copyIndices( geom::LINES, outIndices.data(), outIndices.size(), 4 );
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// ColorFromAttrib
uint8_t ColorFromAttrib::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::COLOR: return 3;
		default:
			return mSource.getAttribDims( attr );
	}
}

AttribSet ColorFromAttrib::getAvailableAttribs() const
{
	AttribSet result = mSource.getAvailableAttribs();
	result.insert( Attrib::COLOR );
	return result;
}

namespace {
template<typename I, typename IFD, typename O>
void processColorAttrib( const I* inputData, O *outputData, const std::function<O(IFD)> &fn, size_t numVertices )
{
	for( size_t v = 0; v < numVertices; ++v ) {
		IFD in( (IFD)inputData[v] );
		outputData[v] = fn( in );
	}
}

template<typename O>
void processColorAttrib2d( const vec2* inputData, O *outputData, const std::function<O(vec3)> &fn, size_t numVertices )
{
	for( size_t v = 0; v < numVertices; ++v ) {
		vec3 in( inputData[v], 0 );
		outputData[v] = fn( in );
	}
}
} // anonymous namespace

void ColorFromAttrib::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	if( (! mFnColor2) && (! mFnColor3) ) {
		mSource.loadInto( target, requestedAttribs );
		return;
	}

	// we want to capture 'mAttrib' and we want to write COLOR
	map<Attrib,Modifier::Access> attribAccess;
	attribAccess[mAttrib] = Modifier::READ;
	attribAccess[COLOR] = Modifier::WRITE;
	Modifier modifier( mSource, target, attribAccess, Modifier::IGNORED );
	mSource.loadInto( &modifier, requestedAttribs );

	if( modifier.getAttribDims( mAttrib ) == 0 ) {
		CI_LOG_W( "ColorFromAttrib called on geom::Source missing requested " << attribToString( mAttrib ) );
		mSource.loadInto( target, requestedAttribs );
		return;
	}

	const auto numVertices = mSource.getNumVertices();
	unique_ptr<float[]> mColorData( new float[numVertices * 3] );
	uint8_t inputAttribDims = modifier.getReadAttribDims( mAttrib );
	const float* inputAttribData = modifier.getReadAttribData( mAttrib );
	
	
	if( mFnColor2 ) {
		if( inputAttribDims == 2 )
			processColorAttrib( reinterpret_cast<const vec2*>( inputAttribData ), reinterpret_cast<Colorf*>( mColorData.get() ), mFnColor2, numVertices );
		else if( inputAttribDims == 3 )
			processColorAttrib( reinterpret_cast<const vec3*>( inputAttribData ), reinterpret_cast<Colorf*>( mColorData.get() ), mFnColor2, numVertices );
		else if( inputAttribDims == 4 )
			processColorAttrib( reinterpret_cast<const vec4*>( inputAttribData ), reinterpret_cast<Colorf*>( mColorData.get() ), mFnColor2, numVertices );
	}
	else if( mFnColor3 ) {
		if( inputAttribDims == 2 )
			processColorAttrib2d( reinterpret_cast<const vec2*>( inputAttribData ), reinterpret_cast<Colorf*>( mColorData.get() ), mFnColor3, numVertices );
		if( inputAttribDims == 3 )
			processColorAttrib( reinterpret_cast<const vec3*>( inputAttribData ), reinterpret_cast<Colorf*>( mColorData.get() ), mFnColor3, numVertices );
		else if( inputAttribDims == 4 )
			processColorAttrib( reinterpret_cast<const vec4*>( inputAttribData ), reinterpret_cast<Colorf*>( mColorData.get() ), mFnColor3, numVertices );
	}

	target->copyAttrib( Attrib::COLOR, 3, 0, mColorData.get(), numVertices );
}

///////////////////////////////////////////////////////////////////////////////////////
// AttribFn
template<typename S, typename D>
uint8_t AttribFn<S,D>::getAttribDims( Attrib attr ) const
{
	if( attr == mDstAttrib )
		return DSTDIM;
	else
		return mSource.getAttribDims( attr );
}

template<typename S, typename D>
geom::AttribSet geom::AttribFn<S,D>::getAvailableAttribs() const
{
	AttribSet result = mSource.getAvailableAttribs();
	result.insert( mDstAttrib );
	return result;
}

namespace {
template<typename S, typename D>
void processAttrib( const float *inputDataFloat, float *outputDataFloat, const std::function<D(S)> &fn, size_t numVertices )
{
	const S *inData = reinterpret_cast<const S*>( inputDataFloat );
	D *outData = reinterpret_cast<D*>( outputDataFloat );

	for( size_t v = 0; v < numVertices; ++v )
		outData[v] = fn( inData[v] );
}
} // anonymous namespace

template<typename S, typename D>
void geom::AttribFn<S,D>::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	// we want to capture 'mSrcAttrib' and we want to write 'mDstAttrib'
	std::map<Attrib,Modifier::Access> attribAccess;
	attribAccess[mSrcAttrib] = Modifier::READ;
	attribAccess[mDstAttrib] = Modifier::WRITE;
	Modifier modifier( mSource, target, attribAccess, Modifier::IGNORED );
	mSource.loadInto( &modifier, requestedAttribs );

	if( modifier.getAttribDims( mSrcAttrib ) == 0 ) {
		CI_LOG_W( "AttribFn called on geom::Source missing requested " << attribToString( mSrcAttrib ) );
		mSource.loadInto( target, requestedAttribs );
		return;
	}

	const auto numVertices = mSource.getNumVertices();
	std::unique_ptr<float[]> outData( new float[numVertices * DSTDIM] );
	std::unique_ptr<float[]> tempInData;
	const float *inputAttribData;
	const uint8_t inputAttribDims = modifier.getReadAttribDims( mSrcAttrib );
	// if the actual input dims of the attribute don't equal SRCDIMS, we'll need to temporarily copy it to a buffer
	if( inputAttribDims != SRCDIM ) {
		CI_LOG_W( "AttribFn source dimensions don't match for attrib " << attribToString( mSrcAttrib ) );
		tempInData = std::unique_ptr<float[]>( new float[numVertices * SRCDIM] );
		auto tempDataWrongDims = modifier.getReadAttribData( mSrcAttrib );
		geom::copyData( inputAttribDims, tempDataWrongDims, numVertices, SRCDIM, 0, tempInData.get() );
		inputAttribData = tempInData.get();
	}
	else
		inputAttribData = modifier.getReadAttribData( mSrcAttrib );
	
	processAttrib<S,D>( inputAttribData, outData.get(), mFn, numVertices );
	target->copyAttrib( Attrib::COLOR, DSTDIM, 0, outData.get(), numVertices );
}

///////////////////////////////////////////////////////////////////////////////////////
// Extrude
Extrude::Extrude( const Shape2d &shape, float distance, float approximationScale )
	: mDistance( distance ), mApproximationScale( approximationScale ), mFrontCap( true ), mBackCap( true ), mSubdivisions( 1 )
{
	for( const auto &contour : shape.getContours() )
		mPaths.push_back( contour );
	updatePathSubdivision();
}

void Extrude::updatePathSubdivision()
{
	mPathSubdivisionPositions.clear();
	mPathSubdivisionTangents.clear();

	// necessary for texcoord calculation
	bool capBoundsEmpty = true;

	// iterate all the paths of the shape and subdivide, calculating both positions and tangents
	for( const auto &path : mPaths ) {
		if( capBoundsEmpty ) {
			mCapBounds = path.calcPreciseBoundingBox();
			capBoundsEmpty = false;
		}
		else
			mCapBounds.include( path.calcPreciseBoundingBox() );
		mPathSubdivisionPositions.emplace_back( vector<vec2>() );
		mPathSubdivisionTangents.emplace_back( vector<vec2>() );
		path.subdivide( &mPathSubdivisionPositions.back(), &mPathSubdivisionTangents.back(), mApproximationScale );
		// normalize the tangents
		for( auto &tan : mPathSubdivisionTangents.back() )
			tan = normalize( tan );
	}

	// Each of the subdivided paths' positions constitute a new contour on our triangulation
	Triangulator triangulator;
	for( const auto &subdivision : mPathSubdivisionPositions )
		triangulator.addPolyLine( subdivision );
	
	mCap = triangulator.createMesh();
}

void Extrude::calculate( vector<vec3> *positions, vector<vec3> *normals, vector<vec3> *texCoords, vector<uint32_t> *indices ) const
{
	// CAPS VERTICES
	uint32_t numCapVertices = (uint32_t)mCap->getNumVertices();
	const vec2* capPositions = mCap->getPositions<2>();
	// front cap
	if( mFrontCap )
		for( size_t v = 0; v < numCapVertices; ++v ) {
			positions->emplace_back( vec3( capPositions[v], mDistance * 0.5f ) );
			normals->emplace_back( vec3( 0, 0, 1 ) );
			texCoords->emplace_back( vec3( ( positions->back().x - mCapBounds.x1 ) / mCapBounds.getWidth(),
											1.0f - ( positions->back().y - mCapBounds.y1 ) / mCapBounds.getHeight(),
											0 ) );
		}
	// back cap
	if( mBackCap )
		for( size_t v = 0; v < numCapVertices; ++v ) {
			positions->emplace_back( vec3( capPositions[v], -mDistance * 0.5f ) );
			normals->emplace_back( vec3( 0, 0, -1 ) );
			texCoords->emplace_back( vec3( ( positions->back().x - mCapBounds.x1 ) / mCapBounds.getWidth(),
											1.0f - ( positions->back().y - mCapBounds.y1 ) / mCapBounds.getHeight(),
											1 ) );
		}

	// CAP INDICES
	auto capIndices = mCap->getIndices();
	// front cap
	if( mFrontCap )
		for( size_t i = 0; i < capIndices.size(); ++i )
			indices->push_back( capIndices[i] );
	// back cap
	if( mBackCap ) {
		for( size_t i = 0; i < capIndices.size(); i += 3 ) { // we need to reverse the winding order for the back cap
			indices->push_back( capIndices[i+2] + numCapVertices );
			indices->push_back( capIndices[i+1] + numCapVertices );
			indices->push_back( capIndices[i+0] + numCapVertices );
		}
	}
	
	// EXTRUSION
	// we don't make use of the caps' vertices because their normals are wrong,
	// so we'll need to create verts unique to the extrusion
	for( size_t p = 0; p < mPathSubdivisionPositions.size(); ++p ) {
		for( int sub = 0; sub <= mSubdivisions; ++sub ) {
			const float t = sub / (float)mSubdivisions;
			const float distance = ( 0.5f - t ) * mDistance;
			const auto &pathPositions = mPathSubdivisionPositions[p];
			const auto &pathTangents = mPathSubdivisionTangents[p];
			// add all the positions & normals
			uint32_t baseIndex = (uint32_t)positions->size();
			for( size_t v = 0; v < pathPositions.size(); ++v ) {
				positions->emplace_back( pathPositions[v], distance );
				normals->emplace_back( vec2( pathTangents[v].y, -pathTangents[v].x ), 0 );
				texCoords->emplace_back( ( positions->back().x - mCapBounds.x1 ) / mCapBounds.getWidth(),
											1.0f - ( positions->back().y - mCapBounds.y1 ) / mCapBounds.getHeight(),
											t );
			}
			// add the indices
			if( sub != mSubdivisions ) {
				uint32_t numSubdivVerts = (uint32_t)pathPositions.size();
				for( uint32_t j = numSubdivVerts-1, i = 0; i < numSubdivVerts; j = i++ ) {
					indices->push_back( baseIndex + i );
					indices->push_back( baseIndex + j );
					indices->push_back( baseIndex + numSubdivVerts + j );
					indices->push_back( baseIndex + i );
					indices->push_back( baseIndex + numSubdivVerts + j );
					indices->push_back( baseIndex + numSubdivVerts + i );
				}
			}
		}
	}
}
	
size_t Extrude::getNumVertices() const
{
	size_t result = 0;
	for( size_t p = 0; p < mPathSubdivisionPositions.size(); ++p )
		result += mPathSubdivisionPositions[p].size() * (mSubdivisions + 1);
	if( mFrontCap )
		result += mCap->getNumVertices();
	if( mBackCap )
		result += mCap->getNumVertices();
	return result;
}

size_t Extrude::getNumIndices() const
{
	size_t result = 0;
	for( size_t p = 0; p < mPathSubdivisionPositions.size(); ++p )
		result += 6 * mPathSubdivisionPositions[p].size() * mSubdivisions;
	if( mFrontCap )
		result += mCap->getNumIndices();
	if( mBackCap )
		result += mCap->getNumIndices();
	return result;
}

uint8_t	Extrude::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 3;
		default:
			return 0;
	}
}

AttribSet Extrude::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0 };
}

void Extrude::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	vector<vec3> positions, normals, texCoords;
	vector<uint32_t> indices;

	calculate( &positions, &normals, &texCoords, &indices );

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*)positions.data(), positions.size() );
	target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*)normals.data(), normals.size() );
	target->copyAttrib( Attrib::TEX_COORD_0, 3, 0, (const float*)texCoords.data(), texCoords.size() );

	target->copyIndices( Primitive::TRIANGLES, indices.data(), indices.size(), calcIndicesRequiredBytes( indices.size() ) );
}

///////////////////////////////////////////////////////////////////////////////////////
// Extrude
ExtrudeSpline::ExtrudeSpline( const Shape2d &shape, const ci::BSpline<3,float> &spline, int splineSubdivisions, float approximationScale )
	: mApproximationScale( approximationScale ), mFrontCap( true ), mBackCap( true ), mSubdivisions( splineSubdivisions )
{
	for( const auto &contour : shape.getContours() )
		mPaths.push_back( contour );

	const float splineLength = spline.getLength( 0, 1 );
	vec3 prevPos = spline.getPosition( 0 );
	vec3 prevTangent = spline.getDerivative( 0 );
	mSplineFrames.emplace_back( firstFrame( prevPos, spline.getPosition( 0.1f ), spline.getPosition( 0.2f ) ) );
	mSplineTimes.push_back( 0 );
	for( int sub = 1; sub <= mSubdivisions; ++sub ) {
		const float t = spline.getTime( sub / (float)mSubdivisions * splineLength );
		const vec3 curPos = spline.getPosition( t );
		const vec3 curTangent = normalize( spline.getDerivative( t ) );
		mSplineFrames.emplace_back( nextFrame( mSplineFrames.back(), prevPos, curPos, prevTangent, curTangent ) );
		mSplineTimes.push_back( t );
		prevPos = curPos;
		prevTangent = curTangent;
	}
	
	updatePathSubdivision();
}

void ExtrudeSpline::updatePathSubdivision()
{
	bool capBoundsEmpty = true;
	
	// iterate all the paths of the shape and subdivide, calculating both positions and tangents
	for( const auto &path : mPaths ) {
		if( capBoundsEmpty ) {
			mCapBounds = path.calcPreciseBoundingBox();
			capBoundsEmpty = false;
		}
		else
			mCapBounds.include( path.calcPreciseBoundingBox() );
		mPathSubdivisionPositions.emplace_back( vector<vec2>() );
		mPathSubdivisionTangents.emplace_back( vector<vec2>() );
		path.subdivide( &mPathSubdivisionPositions.back(), &mPathSubdivisionTangents.back(), mApproximationScale );
		// normalize the tangents
		for( auto& tan : mPathSubdivisionTangents.back() )
			tan = normalize( tan );
	}

	// Each of the subdivided paths' positions constitute a new contour on our triangulation
	Triangulator triangulator;
	for( const auto &subdivision : mPathSubdivisionPositions )
		triangulator.addPolyLine( subdivision );
	
	mCap = triangulator.createMesh();
}

void ExtrudeSpline::calculate( vector<vec3> *positions, vector<vec3> *normals, vector<vec3> *texCoords, vector<uint32_t> *indices ) const
{
	auto capNumVertices = mCap->getNumVertices();
	
	// CAP VERTICES
	const vec2* capPositions = mCap->getPositions<2>();
	// front cap
	if( mFrontCap ) {
		const vec3 frontNormal = vec3( mSplineFrames.front() * vec4( 0, 0, -1, 0 ) );
		for( size_t v = 0; v < mCap->getNumVertices(); ++v ) {
			positions->emplace_back( mSplineFrames.front() * vec4( capPositions[v], 0, 1 ) );
			normals->emplace_back( frontNormal );
			texCoords->emplace_back( vec3( ( capPositions[v].x - mCapBounds.x1 ) / mCapBounds.getWidth(),
											1.0f - ( capPositions[v].y - mCapBounds.y1 ) / mCapBounds.getHeight(),
											0 ) );
		}
	}
	// back cap
	if( mBackCap ) {
		const vec3 backNormal = vec3( mSplineFrames.back() * vec4( 0, 0, 1, 0 ) );
		for( size_t v = 0; v < mCap->getNumVertices(); ++v ) {
			positions->emplace_back( mSplineFrames.back() * vec4( capPositions[v], 0, 1 ) );
			normals->emplace_back( backNormal );
			texCoords->emplace_back( vec3( ( capPositions[v].x - mCapBounds.x1 ) / mCapBounds.getWidth(),
											1.0f - ( capPositions[v].y - mCapBounds.y1 ) / mCapBounds.getHeight(),
											1 ) );
		}
	}
	
	// CAP INDICES
	auto capIndices = mCap->getIndices();
	// front cap
	if( mFrontCap )
		for( size_t i = 0; i < capIndices.size(); ++i )
			indices->push_back( capIndices[i] );
	// back cap
	if( mBackCap ) {
		for( size_t i = 0; i < capIndices.size(); i += 3 ) { // we need to reverse the winding order for the back cap
			indices->push_back( capIndices[i+2] + (uint32_t)capNumVertices );
			indices->push_back( capIndices[i+1] + (uint32_t)capNumVertices );
			indices->push_back( capIndices[i+0] + (uint32_t)capNumVertices );
		}
	}

	// EXTRUSION
	for( size_t p = 0; p < mPathSubdivisionPositions.size(); ++p ) {
		for( int sub = 0; sub <= mSubdivisions; ++sub ) {
			const mat4 &transform = mSplineFrames[sub];
			const auto &pathPositions = mPathSubdivisionPositions[p];
			const auto &pathTangents = mPathSubdivisionTangents[p];
			// add all the positions & normals
			uint32_t baseIndex = (uint32_t)positions->size();
			for( size_t v = 0; v < pathPositions.size(); ++v ) {
				positions->emplace_back( vec3( transform * vec4( pathPositions[v], 0, 1 ) ) );
				normals->emplace_back( transform * vec4( vec2( pathTangents[v].y, -pathTangents[v].x ), 0, 0 ) );
				texCoords->emplace_back( ( pathPositions[v].x - mCapBounds.x1 ) / mCapBounds.getWidth(),
											1.0f - ( pathPositions[v].y - mCapBounds.y1 ) / mCapBounds.getHeight(),
											mSplineTimes[sub] );
			}
			// add the indices
			if( sub != mSubdivisions ) {
				uint32_t numSubdivVerts = (uint32_t)pathPositions.size();
				for( uint32_t j = numSubdivVerts-1, i = 0; i < numSubdivVerts; j = i++ ) {
					indices->push_back( baseIndex + i );
					indices->push_back( baseIndex + j );
					indices->push_back( baseIndex + numSubdivVerts + j );
					indices->push_back( baseIndex + i );
					indices->push_back( baseIndex + numSubdivVerts + j );
					indices->push_back( baseIndex + numSubdivVerts + i );
				}
			}
		}
	}
}
	
size_t ExtrudeSpline::getNumVertices() const
{
	size_t result = 0;
	for( size_t p = 0; p < mPathSubdivisionPositions.size(); ++p )
		result += mPathSubdivisionPositions[p].size() * (mSubdivisions + 1);
	if( mFrontCap )
		result += mCap->getNumVertices();
	if( mBackCap )
		result += mCap->getNumVertices();
	return result;
}

size_t ExtrudeSpline::getNumIndices() const
{
	size_t result = 0;
	for( size_t p = 0; p < mPathSubdivisionPositions.size(); ++p )
		result += 6 * mPathSubdivisionPositions[p].size() * mSubdivisions;
	if( mFrontCap )
		result += mCap->getNumIndices();
	if( mBackCap )
		result += mCap->getNumIndices();
	return result;
}

uint8_t	ExtrudeSpline::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 3;
		default:
			return 0;
	}
}

AttribSet ExtrudeSpline::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0 };
}

void ExtrudeSpline::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	vector<vec3> positions, normals, texCoords;
	vector<uint32_t> indices;

	calculate( &positions, &normals, &texCoords, &indices );

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*)positions.data(), positions.size() );
	target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*)normals.data(), normals.size() );
	target->copyAttrib( Attrib::TEX_COORD_0, 3, 0, (const float*)texCoords.data(), texCoords.size() );

	target->copyIndices( Primitive::TRIANGLES, indices.data(), indices.size(), calcIndicesRequiredBytes( indices.size() ) );
}

///////////////////////////////////////////////////////////////////////////////////////
// VertexNormalLines
VertexNormalLines::VertexNormalLines( const geom::Source &source, float length )
	: mSource( source ), mLength( length )
{
}

size_t VertexNormalLines::getNumVertices() const
{
	if( mSource.getNumIndices() > 0 )
		return mSource.getNumIndices() * 2;
	else
		return mSource.getNumVertices() * 2;
}

uint8_t VertexNormalLines::getAttribDims( Attrib attr ) const
{
	if( attr == Attrib::POSITION )
		return 3;
	else if( attr == Attrib::CUSTOM_0 )
		return 1;
	else if( attr == Attrib::NORMAL || attr == Attrib::COLOR )
		return 0;
	else
		return mSource.getAttribDims( attr );
}

AttribSet VertexNormalLines::getAvailableAttribs() const
{
	AttribSet result = mSource.getAvailableAttribs();
	result.erase( Attrib::NORMAL );
	result.erase( Attrib::COLOR );
	result.insert( Attrib::POSITION );
	result.insert( Attrib::CUSTOM_0 );
	return result;
}

void VertexNormalLines::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	// we are interested in removing normals and colors and outputting positions
	map<Attrib,Modifier::Access> attribAccess;
	attribAccess[Attrib::POSITION] = Modifier::READ_WRITE;
	attribAccess[Attrib::NORMAL] = Modifier::READ_WRITE; // we actually won't ever write it but this prevents pass-through
	attribAccess[Attrib::TEX_COORD_0] = Modifier::READ_WRITE;
	attribAccess[Attrib::COLOR] = Modifier::WRITE; // we actually won't ever write it but this prevents pass-through as colors are often inconvenient
	Modifier modifier( mSource, target, attribAccess, Modifier::READ_WRITE );
	mSource.loadInto( &modifier, { Attrib::POSITION, Attrib::NORMAL } );

	const size_t numInIndices = modifier.getNumIndices();
	const size_t numInVertices = mSource.getNumVertices();

	if( modifier.getReadAttribDims( Attrib::POSITION ) != 3 ) {
		CI_LOG_W( "VertexNormalLines only works for 3D positions" );
		return;
	}
	if( modifier.getReadAttribDims( Attrib::NORMAL ) != 3 ) {
		if( modifier.getReadAttribDims( Attrib::NORMAL ) > 0 )
			CI_LOG_W( "VertexNormalLines requires 3D normals" );
		else
			CI_LOG_W( "VertexNormalLines requires normals" );
		return;
	}

	const uint32_t *indices = modifier.getIndicesData();
	const vec3 *positions = reinterpret_cast<const vec3*>( modifier.getReadAttribData( Attrib::POSITION ) );
	const vec3 *normals = reinterpret_cast<const vec3*>( modifier.getReadAttribData( Attrib::NORMAL ) );
	const float *texCoords = nullptr;
	size_t texCoordDims = modifier.getReadAttribDims( Attrib::TEX_COORD_0 );
	if( texCoordDims > 0 )
		texCoords = reinterpret_cast<const float*>( modifier.getReadAttribData( Attrib::TEX_COORD_0 ) );

	vector<vec3> outPositions;
	vector<float> outCustom0;
	vector<float> outTexCoord0;
	outPositions.reserve( getNumVertices() );
	outCustom0.reserve( getNumVertices() );
	outTexCoord0.reserve( getNumVertices() * texCoordDims );

	if( indices ) {
		for( size_t i = 0; i < numInIndices; i++ ) { // lines connecting first vertex ("hub") and all others
			outPositions.emplace_back( positions[indices[i]] ); outPositions.emplace_back( positions[indices[i]] + normals[indices[i]] * mLength );
			outCustom0.emplace_back( 0 ); outCustom0.emplace_back( 1 );
			if( texCoords ) {
				for( size_t d = 0; d < texCoordDims; ++d )
					outTexCoord0.push_back( texCoords[indices[i] * texCoordDims + d] );
				for( size_t d = 0; d < texCoordDims; ++d )
					outTexCoord0.push_back( texCoords[indices[i] * texCoordDims + d] );
			}
		}
	}
	else {
		for( size_t i = 0; i < numInVertices; i++ ) { // lines connecting first vertex ("hub") and all others
			outPositions.emplace_back( positions[i] ); outPositions.emplace_back( positions[i] + normals[i] * mLength );
			outCustom0.emplace_back( 0 ); outCustom0.emplace_back( 1 );
			if( texCoords ) {
				for( size_t d = 0; d < texCoordDims; ++d )
					outTexCoord0.push_back( texCoords[i * texCoordDims + d] );
				for( size_t d = 0; d < texCoordDims; ++d )
					outTexCoord0.push_back( texCoords[i * texCoordDims + d] );
			}
		}
	}
	
	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*)outPositions.data(), getNumVertices() );
	target->copyAttrib( Attrib::CUSTOM_0, 1, 0, (const float*)outCustom0.data(), getNumVertices() );
	if( texCoords )
		target->copyAttrib( Attrib::TEX_COORD_0, texCoordDims, 0, (const float*)outTexCoord0.data(), getNumVertices() );	
}

///////////////////////////////////////////////////////////////////////////////////////
// BSpline
template<int D, typename T>
BSpline::BSpline( const ci::BSpline<D,T> &spline, int subdivisions )
	: mPositionDims( D )
{
	CI_ASSERT( D >= 2 && D <= 4 );
	
	subdivisions = std::max( 2, subdivisions );
	mNumVertices = subdivisions;
	
	mPositions.reserve( mNumVertices * D );
	mNormals.reserve( mNumVertices );

	init( spline, subdivisions );
}

template<typename T>
void BSpline::init( const ci::BSpline<2,T> &spline, int subdivisions )
{
	const float tInc = 1.0f / ( subdivisions - 1 );

	for( size_t i = 0; i < (size_t)subdivisions; ++i ) {
		auto pos = spline.getPosition( i * tInc );
		auto deriv = spline.getDerivative( i * tInc );
		mPositions.push_back( pos.x ); mPositions.push_back( pos.y );
		mNormals.emplace_back( vec3( normalize( vec2( deriv.y, -deriv.x ) ), 0 ) );
	}
}

template<typename T>
void BSpline::init( const ci::BSpline<3,T> &spline, int subdivisions )
{
	const float tInc = 1.0f / ( subdivisions - 1 );

	for( size_t i = 0; i < (size_t)subdivisions; ++i ) {
		auto pos = spline.getPosition( i * tInc );
		auto deriv = spline.getDerivative( i * tInc );
		mPositions.push_back( pos.x ); mPositions.push_back( pos.y ); mPositions.push_back( pos.z );
		mNormals.emplace_back( normalize( vec3( deriv.y, -deriv.x, deriv.z ) ) );
	}
}

template<typename T>
void BSpline::init( const ci::BSpline<4,T> &spline, int subdivisions )
{
	const float tInc = 1.0f / ( subdivisions - 1 );

	for( size_t i = 0; i < (size_t)subdivisions; ++i ) {
		auto pos = spline.getPosition( i * tInc );
		auto deriv = spline.getDerivative( i * tInc );
		mPositions.push_back( pos.x ); mPositions.push_back( pos.y ); mPositions.push_back( pos.z ); mPositions.push_back( pos.w );
		mNormals.emplace_back( normalize( vec3( deriv.y, -deriv.x, deriv.z ) ) );
	}
}

uint8_t	BSpline::getAttribDims( Attrib attr ) const
{
	if( attr == Attrib::POSITION )
		return mPositionDims;
	else if( attr == Attrib::NORMAL )
		return 3;
	else
		return 0;
}

AttribSet BSpline::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL };
}

void BSpline::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	target->copyAttrib( Attrib::POSITION, mPositionDims, 0, mPositions.data(), mNumVertices );
	target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*)mNormals.data(), mNumVertices );
}

template BSpline::BSpline( const ci::BSpline<2,float>&, int );
template BSpline::BSpline( const ci::BSpline<3,float>&, int );

template class AttribFn<float,float>;	template class AttribFn<float,vec2>;	template class AttribFn<float,vec3>;	template class AttribFn<float,vec4>;
template class AttribFn<vec2,float>;	template class AttribFn<vec2,vec2>;		template class AttribFn<vec2,vec3>;		template class AttribFn<vec2,vec4>;
template class AttribFn<vec3,float>;	template class AttribFn<vec3,vec2>;		template class AttribFn<vec3,vec3>;		template class AttribFn<vec3,vec4>;
template class AttribFn<vec4,float>;	template class AttribFn<vec4,vec2>;		template class AttribFn<vec4,vec3>;		template class AttribFn<vec4,vec4>;

} } // namespace cinder::geom