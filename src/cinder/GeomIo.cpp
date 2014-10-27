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
			case 2:
				switch( dstDimensions ) {
					case 2: copyDataImpl<2,2>( srcData, numElements, dstStrideBytes, dstData ); break;
					case 3: copyDataImpl<2,3>( srcData, numElements, dstStrideBytes, dstData ); break;
					case 4: copyDataImpl<2,4>( srcData, numElements, dstStrideBytes, dstData ); break;
					default: throw ExcIllegalDestDimensions();
				}
				break;
			case 3:
				switch( dstDimensions ) {
					case 2: copyDataImpl<3,2>( srcData, numElements, dstStrideBytes, dstData ); break;
					case 3: copyDataImpl<3,3>( srcData, numElements, dstStrideBytes, dstData ); break;
					case 4: copyDataImpl<3,4>( srcData, numElements, dstStrideBytes, dstData ); break;
					default: throw ExcIllegalDestDimensions();
				}
				break;
		case 4:
				switch( dstDimensions ) {
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

bool Source::isEnabled( Attrib attrib ) const
{
	return mEnabledAttribs.count( attrib ) > 0;
}

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
	enable( Attrib::POSITION );	
	enable( Attrib::TEX_COORD_0 );
	enable( Attrib::NORMAL );
	
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
	enable( Attrib::POSITION );	
	enable( Attrib::TEX_COORD_0 );
	enable( Attrib::NORMAL );
	
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
	enable( Attrib::COLOR );
	mColors[0] = upperRight;
	mColors[1] = upperLeft;
	mColors[2] = lowerRight;
	mColors[3] = lowerLeft;
	return *this;
}

Rect& Rect::texCoords( const vec2 &upperLeft, const vec2 &upperRight, const vec2 &lowerRight, const vec2 &lowerLeft )
{
	enable( Attrib::TEX_COORD_0 );
	mTexCoords[0] = upperRight;
	mTexCoords[1] = upperLeft;
	mTexCoords[2] = lowerRight;
	mTexCoords[3] = lowerLeft;
	return *this;
}

void Rect::loadInto( Target *target ) const
{
	target->copyAttrib( Attrib::POSITION, 2, 0, (const float*)mPositions.data(), 4 );
	if( isEnabled( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 4, 0, (const float*)mColors.data(), 4 );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, (const float*)mTexCoords.data(), 4 );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, sNormals, 4 );
}

uint8_t	Rect::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 2;
		case Attrib::COLOR: return isEnabled( Attrib::COLOR ) ? 4 : 0;
		case Attrib::TEX_COORD_0: return isEnabled( Attrib::TEX_COORD_0 ) ? 2 : 0;
		case Attrib::NORMAL: return isEnabled( Attrib::NORMAL ) ? 3 : 0;
		default:
			return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Cube
Cube::Cube()
	: mSubdivisions( 1 ), mSize( 1 )
{
	enable( Attrib::POSITION );
	enable( Attrib::TEX_COORD_0 );
	enable( Attrib::NORMAL );
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
		case Attrib::COLOR: return isEnabled( Attrib::COLOR ) ? 3 : 0;
		case Attrib::TEX_COORD_0: return isEnabled( Attrib::TEX_COORD_0 ) ? 2 : 0;
		case Attrib::NORMAL: return isEnabled( Attrib::NORMAL ) ? 3 : 0;
		default:
			return 0;
	}	
}

void generateFace( const vec3 &faceCenter, const vec3 &uAxis, const vec3 &vAxis, int subdivU, int subdivV,
					vector<vec3> *positions, vector<vec3> *normals,
					const Color &color, vector<Color> *colors, vector<vec2> *texCoords,
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

void Cube::loadInto( Target *target ) const
{
	vector<vec3> positions;
	vector<uint32_t> indices;
	vector<vec3> normals;
	vector<Color> colors;
	vector<vec2> texCoords;
	vector<vec3> *normalsPtr = nullptr;
	vector<Color> *colorsPtr = nullptr;
	vector<vec2> *texCoordsPtr = nullptr;
	
	const size_t numVertices = getNumVertices();
	
	// reserve room in vectors and set pointers to non-null for normals, texcoords and colors as appropriate
	positions.reserve( numVertices );
	indices.reserve( getNumIndices() );
	if( isEnabled( Attrib::NORMAL ) ) {
		normals.reserve( numVertices );
		normalsPtr = &normals;
	}
	if( isEnabled( Attrib::COLOR ) ) {
		colors.reserve( numVertices );
		colorsPtr = &colors;
	}
	if( isEnabled( Attrib::TEX_COORD_0 ) ) {
		texCoords.reserve( numVertices );
		texCoordsPtr = &texCoords;
	}
	
	// +X
	generateFace( vec3(mSize.x,0,0), vec3(0,0,mSize.z), vec3(0,mSize.y,0), mSubdivisions.z, mSubdivisions.y, &positions,
		normalsPtr, Color(1,0,0), colorsPtr, texCoordsPtr, &indices );
	// +Y
	generateFace( vec3(0,mSize.y,0), vec3(mSize.x,0,0), vec3(0,0,mSize.z), mSubdivisions.x, mSubdivisions.z, &positions,
		normalsPtr, Color(0,1,0), colorsPtr, texCoordsPtr, &indices );
	// +Z
	generateFace( vec3(0,0,mSize.z), vec3(0,mSize.y,0), vec3(mSize.x,0,0), mSubdivisions.y, mSubdivisions.x, &positions,
		normalsPtr, Color(0,0,1), colorsPtr, texCoordsPtr, &indices );
	// -X
	generateFace( vec3(-mSize.x,0,0), vec3(0,mSize.y,0), vec3(0,0,mSize.z), mSubdivisions.y, mSubdivisions.z, &positions,
		normalsPtr, Color(0,1,1), colorsPtr, texCoordsPtr, &indices );
	// -Y
	generateFace( vec3(0,-mSize.y,0), vec3(0,0,mSize.z), vec3(mSize.x,0,0), mSubdivisions.z, mSubdivisions.x, &positions,
		normalsPtr, Color(1,0,1), colorsPtr, texCoordsPtr, &indices );
	// -Z
	generateFace( vec3(0,0,-mSize.z), vec3(mSize.x,0,0), vec3(0,mSize.y,0), mSubdivisions.x, mSubdivisions.y, &positions,
		normalsPtr, Color(1,1,0), colorsPtr, texCoordsPtr, &indices );

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*)positions.data(), numVertices );
	if( isEnabled( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, (const float*)colors.data(), numVertices );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, (const float*)texCoords.data(), numVertices );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*)normals.data(), numVertices );

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
	: mCalculationsCached( false )
{
	enable( Attrib::POSITION );
	enable( Attrib::NORMAL );
}

void Icosahedron::calculate() const
{
	if( mCalculationsCached )
		return;

	// instead of copying the positions, we create 3 unique vertices per face
	// to make sure the face is flat
	mPositions.resize( 60 );
	mIndices.resize( 60, 0 );
	mNormals.resize( 60 );

	for( size_t i = 0; i < 60; ++i ) {
		mPositions[i] = *reinterpret_cast<const vec3*>(&sPositions[sIndices[i]*3]);
		mIndices[i] = (uint32_t) i;
	}

	// calculate the face normal for each triangle
	size_t numTriangles = mIndices.size() / 3;
	for( size_t i = 0; i < numTriangles; ++i ) {
		const uint32_t index0 = mIndices[i*3+0];
		const uint32_t index1 = mIndices[i*3+1];
		const uint32_t index2 = mIndices[i*3+2];

		const vec3 &v0 = mPositions[index0];
		const vec3 &v1 = mPositions[index1];
		const vec3 &v2 = mPositions[index2];

		vec3 e0 = v1 - v0;
		vec3 e1 = v2 - v0;

		mNormals[index0] = mNormals[index1] = mNormals[index2] = normalize( cross( e0, e1 ) );
	}

	// add color if necessary
	if( isEnabled( Attrib::COLOR ) ) {
		size_t numPositions = mPositions.size();
		mColors.resize( numPositions );
		for( size_t i = 0; i < numPositions; ++i ) {
			mColors[i].x = mPositions[i].x * 0.5f + 0.5f;
			mColors[i].y = mPositions[i].y * 0.5f + 0.5f;
			mColors[i].z = mPositions[i].z * 0.5f + 0.5f;
		}
	}

	mCalculationsCached = true;
}

uint8_t	Icosahedron::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
	case Attrib::POSITION: return 3;
	case Attrib::NORMAL: return isEnabled( Attrib::NORMAL ) ? 3 : 0;
	case Attrib::COLOR: return isEnabled( Attrib::COLOR ) ? 3 : 0;
	default:
		return 0;
	}
}

void Icosahedron::loadInto( Target *target ) const
{
	calculate();

	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *mPositions.data() ), mPositions.size() );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *mNormals.data() ), mNormals.size() );
	if( isEnabled( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *mColors.data() ), mColors.size() );

	target->copyIndices( Primitive::TRIANGLES, mIndices.data(), mIndices.size(), 1 );
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

const float Teapot::sCurveData[][3] = 
{	{0.2f, 0.f, 2.7f}, {0.2f, -0.112f, 2.7f}, {0.112f, -0.2f, 2.7f}, {0.f, -0.2f, 2.7f}, {1.3375f, 0.f,
	2.53125f}, {1.3375f, -0.749f, 2.53125f}, {0.749f, -1.3375f, 2.53125f}, {0.f, -1.3375f, 2.53125f},
	{1.4375f, 0.f, 2.53125f}, {1.4375f, -0.805f, 2.53125f}, {0.805f, -1.4375f, 2.53125f}, {0.f,
	-1.4375f, 2.53125f}, {1.5f, 0.f, 2.4f}, {1.5f, -0.84f, 2.4f}, {0.84f, -1.5f, 2.4f}, {0.f, -1.5f,
	2.4f}, {1.75f, 0.f, 1.875f}, {1.75f, -0.98f, 1.875f}, {0.98f, -1.75f, 1.875f}, {0.f, -1.75f,
	1.875f}, {2.f, 0.f, 1.35f}, {2.f, -1.12f, 1.35f}, {1.12f, -2.f, 1.35f}, {0.f, -2.f, 1.35f}, {2.f,
	0.f, 0.9f}, {2.f, -1.12f, 0.9f}, {1.12f, -2.f, 0.9f}, {0.f, -2.f, 0.9f}, {-2.f, 0.f, 0.9f}, {2.f,
	0.f, 0.45f}, {2.f, -1.12f, 0.45f}, {1.12f, -2.f, 0.45f}, {0.f, -2.f, 0.45f}, {1.5f, 0.f, 0.225f},
	{1.5f, -0.84f, 0.225f}, {0.84f, -1.5f, 0.225f}, {0.f, -1.5f, 0.225f}, {1.5f, 0.f, 0.15f}, {1.5f,
	-0.84f, 0.15f}, {0.84f, -1.5f, 0.15f}, {0.f, -1.5f, 0.15f}, {-1.6f, 0.f, 2.025f}, {-1.6f, -0.3f,
	2.025f}, {-1.5f, -0.3f, 2.25f}, {-1.5f, 0.f, 2.25f}, {-2.3f, 0.f, 2.025f}, {-2.3f, -0.3f, 2.025f},
	{-2.5f, -0.3f, 2.25f}, {-2.5f, 0.f, 2.25f}, {-2.7f, 0.f, 2.025f}, {-2.7f, -0.3f, 2.025f}, {-3.f,
	-0.3f, 2.25f}, {-3.f, 0.f, 2.25f}, {-2.7f, 0.f, 1.8f}, {-2.7f, -0.3f, 1.8f}, {-3.f, -0.3f, 1.8f},
	{-3.f, 0.f, 1.8f}, {-2.7f, 0.f, 1.575f}, {-2.7f, -0.3f, 1.575f}, {-3.f, -0.3f, 1.35f}, {-3.f, 0.f,
	1.35f}, {-2.5f, 0.f, 1.125f}, {-2.5f, -0.3f, 1.125f}, {-2.65f, -0.3f, 0.9375f}, {-2.65f, 0.f,
	0.9375f}, {-2.f, -0.3f, 0.9f}, {-1.9f, -0.3f, 0.6f}, {-1.9f, 0.f, 0.6f}, {1.7f, 0.f, 1.425f}, {1.7f,
	-0.66f, 1.425f}, {1.7f, -0.66f, 0.6f}, {1.7f, 0.f, 0.6f}, {2.6f, 0.f, 1.425f}, {2.6f, -0.66f,
	1.425f}, {3.1f, -0.66f, 0.825f}, {3.1f, 0.f, 0.825f}, {2.3f, 0.f, 2.1f}, {2.3f, -0.25f, 2.1f},
	{2.4f, -0.25f, 2.025f}, {2.4f, 0.f, 2.025f}, {2.7f, 0.f, 2.4f}, {2.7f, -0.25f, 2.4f}, {3.3f, -0.25f,
	2.4f}, {3.3f, 0.f, 2.4f}, {2.8f, 0.f, 2.475f}, {2.8f, -0.25f, 2.475f}, {3.525f, -0.25f, 2.49375f},
	{3.525f, 0.f, 2.49375f}, {2.9f, 0.f, 2.475f}, {2.9f, -0.15f, 2.475f}, {3.45f, -0.15f, 2.5125f},
	{3.45f, 0.f, 2.5125f}, {2.8f, 0.f, 2.4f}, {2.8f, -0.15f, 2.4f}, {3.2f, -0.15f, 2.4f}, {3.2f, 0.f,
	2.4f}, {0.f, 0.f, 3.15f}, {0.8f, 0.f, 3.15f}, {0.8f, -0.45f, 3.15f}, {0.45f, -0.8f, 3.15f}, {0.f,
	-0.8f, 3.15f}, {0.f, 0.f, 2.85f}, {1.4f, 0.f, 2.4f}, {1.4f, -0.784f, 2.4f}, {0.784f, -1.4f, 2.4f},
	{0.f, -1.4f, 2.4f}, {0.4f, 0.f, 2.55f}, {0.4f, -0.224f, 2.55f}, {0.224f, -0.4f, 2.55f}, {0.f, -0.4f,
	2.55f}, {1.3f, 0.f, 2.55f}, {1.3f, -0.728f, 2.55f}, {0.728f, -1.3f, 2.55f}, {0.f, -1.3f, 2.55f},
	{1.3f, 0.f, 2.4f}, {1.3f, -0.728f, 2.4f}, {0.728f, -1.3f, 2.4f}, {0.f, -1.3f, 2.4f}, {0.f, 0.f,
	0.f}, {1.425f, -0.798f, 0.f}, {1.5f, 0.f, 0.075f}, {1.425f, 0.f, 0.f}, {0.798f, -1.425f, 0.f}, {0.f,
	-1.5f, 0.075f}, {0.f, -1.425f, 0.f}, {1.5f, -0.84f, 0.075f}, {0.84f, -1.5f, 0.075f} };

Teapot::Teapot()
	: mSubdivision( 6 ), mCalculationsCached( false )
{
	enable( Attrib::POSITION );
	enable( Attrib::TEX_COORD_0 );
	enable( Attrib::NORMAL );
	updateVertexCounts();
}

Teapot&	Teapot::subdivisions( int sub )
{
	mSubdivision = sub;
	updateVertexCounts();
	return *this;
}

size_t Teapot::getNumVertices() const
{
	return mNumVertices;
}

void Teapot::loadInto( Target *target ) const
{
	calculate();

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*)mPositions.data(), mNumVertices );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, (const float*)mTexCoords.data(), mNumVertices );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*)mNormals.data(), mNumVertices );

	target->copyIndices( Primitive::TRIANGLES, mIndices.data(), mNumIndices, 4 );
}

size_t Teapot::getNumIndices() const
{
	return mNumIndices;
}

uint8_t	Teapot::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
	case Attrib::POSITION: return 3;
	case Attrib::TEX_COORD_0: return isEnabled( Attrib::TEX_COORD_0 ) ? 2 : 0;
	case Attrib::NORMAL: return isEnabled( Attrib::NORMAL ) ? 3 : 0;
	default:
		return 0;
	}
}

void Teapot::updateVertexCounts() const
{
	int numFaces = mSubdivision * mSubdivision * 32;
	mNumIndices = numFaces * 6;
	mNumVertices = 32 * (mSubdivision + 1) * (mSubdivision + 1);
	mCalculationsCached = false;
}

void Teapot::calculate() const
{
	if(mCalculationsCached)
		return;

	updateVertexCounts();

	mPositions.resize(mNumVertices*3, 0.f); //= unique_ptr<float[]>( new float[mNumVertices * 3] );
	mTexCoords.resize(mNumVertices*2, 0.f); //= unique_ptr<float[]>( new float[mNumVertices * 2] );	
	mNormals.resize(mNumVertices*3, 0.f); //= unique_ptr<float[]>( new float[mNumVertices * 3] );
	mIndices.resize(mNumIndices, 0); //= unique_ptr<uint32_t[]>( new uint32_t[mNumIndices] );

	generatePatches( mPositions.data(), mNormals.data(), mTexCoords.data(), mIndices.data(), mSubdivision );

	mCalculationsCached = true;
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
	getPatch( patchNum, patch, false);
	getPatch( patchNum, patchRevV, true);

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

///////////////////////////////////////////////////////////////////////////////////////
// Circle
Circle::Circle()
	: mRequestedSubdivisions( -1 ), mCenter( 0, 0 ), mRadius( 1.0f )
{
	enable( Attrib::POSITION );
	enable( Attrib::TEX_COORD_0 );
	enable( Attrib::NORMAL );
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

void Circle::calculate() const
{
	mPositions.resize(mNumVertices); //  = unique_ptr<vec2[]>( new vec2[mNumVertices] );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		mTexCoords.resize(mNumVertices); // = unique_ptr<vec2[]>( new vec2[mNumVertices] );
	if( isEnabled( Attrib::NORMAL ) )		
		mNormals.resize(mNumVertices); // = unique_ptr<vec3[]>( new vec3[mNumVertices] );	

	// center
	mPositions[0] = mCenter;
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		mTexCoords[0] = vec2( 0.5f, 0.5f );
	if( isEnabled( Attrib::NORMAL ) )
		mNormals[0] = vec3( 0, 0, 1 );

	// iterate the segments
	const float tDelta = 1 / (float)mNumSubdivisions * 2.0f * 3.14159f;
	float t = 0;
	for( int s = 0; s <= mNumSubdivisions; s++ ) {
		vec2 unit( math<float>::cos( t ), math<float>::sin( t ) );
		mPositions[s+1] = mCenter + unit * mRadius;
		if( isEnabled( Attrib::TEX_COORD_0 ) )
			mTexCoords[s+1] = unit * 0.5f + vec2( 0.5f, 0.5f );
		if( isEnabled( Attrib::NORMAL ) )
			mNormals[s+1] = vec3( 0, 0, 1 );
		t += tDelta;
	}
}

size_t Circle::getNumVertices() const
{
	return mNumVertices;
}

void Circle::loadInto( Target *target ) const
{
	calculate();

	target->copyAttrib( Attrib::POSITION, 2, 0, (const float*)mPositions.data(), mNumVertices );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, (const float*)mTexCoords.data(), mNumVertices );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*)mNormals.data(), mNumVertices );
}

uint8_t	Circle::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
	case Attrib::POSITION: return 2;
	case Attrib::TEX_COORD_0: return isEnabled( Attrib::TEX_COORD_0 ) ? 2 : 0;
	case Attrib::NORMAL: return isEnabled( Attrib::NORMAL ) ? 3 : 0;
	default:
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Sphere

Sphere::Sphere()
	: mSubdivisions( 18 ), mCenter( 0, 0, 0 ), mRadius( 1.0f ), mCalculationsCached( false )
{
	enable( Attrib::POSITION );
	enable( Attrib::NORMAL );
	enable( Attrib::TEX_COORD_0 );
}

void Sphere::calculate() const
{
	if( mCalculationsCached )
		return;

	int numSegments = mSubdivisions;
	if( numSegments < 4 )
		numSegments = std::max( 12, (int)math<double>::floor( mRadius * float(M_PI * 2) ) );

	// numRings = numSegments / 2
	int numRings = ( numSegments >> 1 );

	calculateImplUV( numSegments + 1, numRings + 1 );
	mCalculationsCached = true;
}

void Sphere::calculateImplUV( size_t segments, size_t rings ) const
{
	mPositions.resize( segments * rings );
	mNormals.resize( segments * rings );
	mTexCoords.resize( segments * rings );
	mColors.resize( segments * rings );
	mIndices.resize( segments * rings * 6 );

	float ringIncr = 1.0f / (float)( rings - 1 );
	float segIncr = 1.0f / (float)( segments - 1 );
	float radius = mRadius;

	bool hasNormals = isEnabled( Attrib::NORMAL );
	bool hasTexCoords = isEnabled( Attrib::TEX_COORD_0 );
	bool hasColors = isEnabled( Attrib::COLOR );

	auto vertIt = mPositions.begin();
	auto normIt = mNormals.begin();
	auto texIt = mTexCoords.begin();
	auto colorIt = mColors.begin();
	for( size_t r = 0; r < rings; r++ ) {
		float v = r * ringIncr;
		for( size_t s = 0; s < segments; s++ ) {
			float u = 1.0f - s * segIncr;
			float x = math<float>::sin( float(M_PI * 2) * u ) * math<float>::sin( float(M_PI) * v );
			float y = math<float>::sin( float(M_PI) * (v - 0.5f) );
			float z = math<float>::cos( float(M_PI * 2) * u ) * math<float>::sin( float(M_PI) * v );

			*vertIt++ = vec3( x * radius + mCenter.x, y * radius + mCenter.y, z * radius + mCenter.z );

			if( hasNormals )
				*normIt++ = vec3( x, y, z );
			if( hasTexCoords )
				*texIt++ = vec2( u, v );
			if( hasColors )
				*colorIt++ = vec3( x * 0.5f + 0.5f, y * 0.5f + 0.5f, z * 0.5f + 0.5f );
		}
	}

	auto indexIt = mIndices.begin();
	for( size_t r = 0; r < rings - 1; r++ ) {
		for( size_t s = 0; s < segments - 1 ; s++ ) {
			*indexIt++ = (uint32_t)(r * segments + ( s + 1 ));
			*indexIt++ = (uint32_t)(r * segments + s);
			*indexIt++ = (uint32_t)(( r + 1 ) * segments + ( s + 1 ));

			*indexIt++ = (uint32_t)(( r + 1 ) * segments + s);
			*indexIt++ = (uint32_t)(( r + 1 ) * segments + ( s + 1 ));
			*indexIt++ = (uint32_t)(r * segments + s);
		}
	}
}

size_t Sphere::getNumVertices() const
{
	calculate();
	return mPositions.size();
}

size_t Sphere::getNumIndices() const
{
	calculate();
	return mIndices.size();
}

uint8_t Sphere::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
	case Attrib::POSITION: return 3;
	case Attrib::TEX_COORD_0: return isEnabled( Attrib::TEX_COORD_0 ) ? 2 : 0;
	case Attrib::NORMAL: return isEnabled( Attrib::NORMAL ) ? 3 : 0;
	case Attrib::COLOR: return isEnabled( Attrib::COLOR ) ? 3 : 0;
	default:
		return 0;
	}
}

void Sphere::loadInto( Target *target ) const
{
	calculate();
	if( isEnabled( Attrib::POSITION ) )
		target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *mPositions.data() ), mPositions.size() );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *mTexCoords.data() ), mTexCoords.size() );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *mNormals.data() ), mNormals.size() );
	if( isEnabled( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *mColors.data() ), mColors.size() );

	target->copyIndices( Primitive::TRIANGLES, mIndices.data(), mIndices.size(), 4 );
}

///////////////////////////////////////////////////////////////////////////////////////
// Icosphere

Icosphere::Icosphere()
	: mSubdivision( 3 ), mCalculationsCached( false )
{
	enable( Attrib::POSITION );
	enable( Attrib::TEX_COORD_0 );
	enable( Attrib::NORMAL );
}

void Icosphere::calculate() const
{
	if( mCalculationsCached )
		return;

	// start by copying the base icosahedron in its entirety (vertices are shared among faces)
	mPositions.assign( reinterpret_cast<vec3*>(sPositions), reinterpret_cast<vec3*>(sPositions) + 12 );
	mNormals.assign( reinterpret_cast<vec3*>(sPositions), reinterpret_cast<vec3*>(sPositions) + 12 );
	mIndices.assign( sIndices, sIndices + 60 );

	// subdivide all triangles
	subdivide();

	// spherize
	for( auto &pos : mPositions ) {
		pos = normalize( pos );
	}
	for( auto &normal : mNormals ) {
		normal = normalize( normal );
	}

	// add color if necessary
	if( isEnabled( Attrib::COLOR ) ) {
		size_t numPositions = mPositions.size();
		mColors.resize( numPositions );
		for( size_t i = 0; i < numPositions; ++i ) {
			mColors[i].x = mPositions[i].x * 0.5f + 0.5f;
			mColors[i].y = mPositions[i].y * 0.5f + 0.5f;
			mColors[i].z = mPositions[i].z * 0.5f + 0.5f;
		}
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

		if( isEnabled( Attrib::COLOR ) )
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

		if( math<float>::abs(d1) > 0.5f && math<float>::abs(d2) > 0.5f ) {
			addVertex( i * 3 + 0, uv0 + vec2( (d1 > 0.0f) ? 1.0f : -1.0f, 0.0f ) );
		}
		else if( math<float>::abs(d1) > 0.5f ) {
			addVertex( i * 3 + 1, uv1 + vec2( (d1 < 0.0f) ? 1.0f : -1.0f, 0.0f ) );
		}
		else if( math<float>::abs(d2) > 0.5f ) {
			addVertex( i * 3 + 2, uv2 + vec2( (d2 < 0.0f) ? 1.0f : -1.0f, 0.0f ) );
		}
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
	case Attrib::TEX_COORD_0: return isEnabled( Attrib::TEX_COORD_0 ) ? 2 : 0;
	case Attrib::NORMAL: return isEnabled( Attrib::NORMAL ) ? 3 : 0;
	case Attrib::COLOR: return isEnabled( Attrib::COLOR ) ? 3 : 0;
	default:
		return 0;
	}
}

void Icosphere::loadInto( Target *target ) const
{
	calculate();

	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *mPositions.data() ), mPositions.size() );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *mTexCoords.data() ), mTexCoords.size() );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *mNormals.data() ), mNormals.size() );
	if( isEnabled( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *mColors.data() ), mColors.size() );

	target->copyIndices( Primitive::TRIANGLES, mIndices.data(), mIndices.size(), 4 );
}

///////////////////////////////////////////////////////////////////////////////////////
// Capsule

Capsule::Capsule()
	: mDirection( 0, 1, 0 ), mLength( 1.0f ), mSubdivisionsAxis( 6 )
{
	radius( 0.5f );
	subdivisionsHeight( 6 );

	enable( Attrib::POSITION );
	enable( Attrib::NORMAL );
	enable( Attrib::TEX_COORD_0 );
}

Capsule& Capsule::set( const vec3 &from, const vec3 &to )
{
	const vec3 axis = to - from;
	mLength = glm::length( axis );
	mDirection = normalize( axis );
	mCenter = from + 0.5f * axis;
	mCalculationsCached = false;
	return *this;
}

void Capsule::calculate() const
{
	if( mCalculationsCached )
		return;

	int numSegments = mSubdivisionsAxis;
	if( numSegments < 4 )
		numSegments = std::max( 12, (int)math<double>::floor( mRadius * float(M_PI * 2) ) );

	calculateImplUV( numSegments, std::max( mSubdivisionsHeight, 2 ) );
	mCalculationsCached = true;
}

void Capsule::calculateImplUV( size_t segments, size_t rings ) const
{
	size_t ringsBody = mSubdivisionsHeight + 1;
	size_t ringsTotal = rings + ringsBody;

	mPositions.clear();
	mNormals.clear();
	mTexCoords.clear();
	mColors.clear();
	mIndices.clear();

	mPositions.reserve( segments * ringsTotal );
	mNormals.reserve( segments * ringsTotal );
	mTexCoords.reserve( segments * ringsTotal );
	mColors.reserve( segments * ringsTotal );
	mIndices.reserve( segments * ringsTotal * 6 );

	float bodyIncr = 1.0f / (float)( ringsBody - 1 );
	float ringIncr = 1.0f / (float)( rings - 1 );
	for( size_t r = 0; r < rings / 2; r++ ) {
		calculateRing( segments, math<float>::sin( float(M_PI) * r * ringIncr),
			math<float>::sin( float(M_PI) * ( r * ringIncr - 0.5f ) ), -0.5f );
	}
	for( size_t r = 0; r < ringsBody; r++ ) {
		calculateRing( segments, 1.0f, 0.0f, r * bodyIncr - 0.5f );
	}
	for( size_t r = rings / 2; r < rings; r++ ) {
		calculateRing( segments, math<float>::sin( float(M_PI) * r * ringIncr),
			math<float>::sin( float(M_PI) * ( r * ringIncr - 0.5f ) ), +0.5f );
	}

	for( size_t r = 0; r < ringsTotal - 1; r++ ) {
		for( size_t s = 0; s < segments - 1; s++ ) {
			mIndices.push_back( (uint32_t)(r * segments + ( s + 1 )) );
			mIndices.push_back( (uint32_t)(r * segments + ( s + 0 )) );
			mIndices.push_back( (uint32_t)(( r + 1 ) * segments + ( s + 1 )) );

			mIndices.push_back( (uint32_t)(( r + 1 ) * segments + ( s + 0 )) );
			mIndices.push_back( (uint32_t)(( r + 1 ) * segments + ( s + 1 )) );
			mIndices.push_back( (uint32_t)(r * segments + s) );
		}
	}
}

void Capsule::calculateRing( size_t segments, float radius, float y, float dy ) const
{
	const quat quaternion( vec3( 0, 1, 0 ), mDirection );

	bool hasNormals = isEnabled( Attrib::NORMAL );
	bool hasTexCoords = isEnabled( Attrib::TEX_COORD_0 );
	bool hasColors = isEnabled( Attrib::COLOR );

	float segIncr = 1.0f / (float)( segments - 1 );
	for( size_t s = 0; s < segments; s++ ) {
		float x = math<float>::cos( float(M_PI * 2) * s * segIncr ) * radius;
		float z = math<float>::sin( float(M_PI * 2) * s * segIncr ) * radius;

		mPositions.push_back( mCenter + ( quaternion * glm::vec3( mRadius * x, mRadius * y + mLength * dy, mRadius * z ) ) );

		if( hasNormals ) {
			mNormals.push_back( quaternion * glm::vec3( x, y, z ) );
		}
		if( hasTexCoords ) {
			// perform cylindrical projection
			float u = 1.0f - (s * segIncr);
			float v = 0.5f - ((mRadius * y + mLength * dy) / (2.0f * mRadius + mLength));
			mTexCoords.push_back( vec2( u, v ) );
		}
		if( hasColors ) {
			float g = 0.5f + ((mRadius * y + mLength * dy) / (2.0f * mRadius + mLength));
			mColors.push_back( vec3( x * 0.5f + 0.5f, g, z * 0.5f + 0.5f ) );
		}
	}
}

size_t Capsule::getNumVertices() const
{
	calculate();
	return mPositions.size();
}

size_t Capsule::getNumIndices() const
{
	calculate();
	return mIndices.size();
}

uint8_t Capsule::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
	case Attrib::POSITION: return 3;
	case Attrib::TEX_COORD_0: return isEnabled( Attrib::TEX_COORD_0 ) ? 2 : 0;
	case Attrib::NORMAL: return isEnabled( Attrib::NORMAL ) ? 3 : 0;
	case Attrib::COLOR: return isEnabled( Attrib::COLOR ) ? 3 : 0;
	default:
		return 0;
	}
}

void Capsule::loadInto( Target *target ) const
{
	calculate();
	if( isEnabled( Attrib::POSITION ) )
		target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *mPositions.data() ), mPositions.size() );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *mTexCoords.data() ), mTexCoords.size() );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *mNormals.data() ), mNormals.size() );
	if( isEnabled( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *mColors.data() ), mColors.size() );

	target->copyIndices( Primitive::TRIANGLES, mIndices.data(), mIndices.size(), 4 );
}


///////////////////////////////////////////////////////////////////////////////////////
// Torus

Torus::Torus()
	: mCenter( 0, 0, 0), mRadiusMajor( 1.0f ), mRadiusMinor( 0.75f ), mCoils( 1 ), mHeight( 0 )
	, mSubdivisionsAxis( 18 ), mSubdivisionsHeight( 18 ), mTwist( 0 ), mTwistOffset( 0 )
{
	enable( Attrib::POSITION );
	enable( Attrib::NORMAL );
	enable( Attrib::TEX_COORD_0 );
}

void Torus::calculate() const
{
	if( mCalculationsCached )
		return;

	int numAxis = (int) math<float>::ceil( mSubdivisionsAxis * mCoils );
	if( numAxis < 4 )
		numAxis = std::max( 12, (int)math<double>::floor( mRadiusMajor * float(M_PI * 2) ) );

	int numRing = mSubdivisionsHeight;
	if( numRing < 3 )
		numRing = std::max( 12, (int)math<double>::floor( mRadiusMajor * float(M_PI * 2) ) );

	calculateImplUV( numAxis + 1, numRing + 1 );
	mCalculationsCached = true;
}

void Torus::calculateImplUV( size_t segments, size_t rings ) const
{
	mPositions.resize( segments * rings );
	mNormals.resize( segments * rings );
	mTexCoords.resize( segments * rings, vec2() );
	mIndices.resize( (segments - 1) * (rings - 1) * 6, 0 );

	if( isEnabled( Attrib::COLOR ) )
		mColors.resize( segments * rings );
	else
		mColors.clear();

	float majorIncr = 1.0f / (segments - 1);
	float minorIncr = 1.0f / (rings - 1);
	float radiusDiff = mRadiusMajor - mRadiusMinor;
	float angle = float(M_PI * 2) * mCoils;
	float twist = angle * mTwist * minorIncr * majorIncr;

	// vertex, normal, tex coord and color buffers
	for( size_t i = 0; i < segments; ++i ) {
		float phi = i * majorIncr * angle;
		float cosPhi = -math<float>::cos( phi );
		float sinPhi =  math<float>::sin( phi );

		for( size_t j = 0; j < rings; ++j ) {
			float theta = j * minorIncr * float(M_PI * 2) + i * twist + mTwistOffset;
			float cosTheta = -math<float>::cos( theta );
			float sinTheta =  math<float>::sin( theta );

			float r = mRadiusMinor + cosTheta * radiusDiff;
			float x = r * cosPhi;
			float y = i * majorIncr * mHeight + sinTheta * radiusDiff;
			float z = r * sinPhi;

			const size_t k = i * rings + j;
			mPositions[k] = mCenter + vec3( x, y, z );
			mTexCoords[k] = vec2( i * majorIncr, j * minorIncr );
			mNormals[k] = vec3( cosPhi * cosTheta, sinTheta, sinPhi * cosTheta );

			if( isEnabled( Attrib::COLOR ) ) {
				const vec3 &n = mNormals[k];
				mColors[k] = vec3( n.x * 0.5f + 0.5f, n.y * 0.5f + 0.5f, n.z * 0.5f + 0.5f );
			}
		}
	}

	// index buffer
	size_t k = 0;
	for( size_t i = 0; i < segments - 1; ++i ) {
		for ( size_t j = 0; j < rings - 1; ++j ) {
			mIndices[k++] = (uint32_t)((i + 0) * rings + (j + 0));
			mIndices[k++] = (uint32_t)((i + 1) * rings + (j + 1));
			mIndices[k++] = (uint32_t)((i + 1) * rings + (j + 0));

			mIndices[k++] = (uint32_t)((i + 0) * rings + (j + 0));
			mIndices[k++] = (uint32_t)((i + 0) * rings + (j + 1));
			mIndices[k++] = (uint32_t)((i + 1) * rings + (j + 1));
		}
	}
}

uint8_t Torus::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
	case Attrib::POSITION: return 3;
	case Attrib::TEX_COORD_0: return isEnabled( Attrib::TEX_COORD_0 ) ? 2 : 0;
	case Attrib::NORMAL: return isEnabled( Attrib::NORMAL ) ? 3 : 0;
	case Attrib::COLOR: return isEnabled( Attrib::COLOR ) ? 3 : 0;
	default:
		return 0;
	}
}

void Torus::loadInto( Target *target ) const
{
	calculate();

	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *mPositions.data() ), mPositions.size() );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *mTexCoords.data() ), mTexCoords.size() );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *mNormals.data() ), mNormals.size() );
	if( isEnabled( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *mColors.data() ), mColors.size() );

	target->copyIndices( Primitive::TRIANGLES, mIndices.data(), mIndices.size(), 4 );
}

///////////////////////////////////////////////////////////////////////////////////////
// Cylinder

Cylinder::Cylinder()
	: mOrigin( 0, 0, 0 ), mHeight( 2.0f ), mDirection( 0, 1, 0 ), mRadiusBase( 1.0f ), mRadiusApex( 1.0f ), mSubdivisionsAxis( 18 ), mSubdivisionsHeight( 1 )
{
	enable( Attrib::POSITION );
	enable( Attrib::NORMAL );
	enable( Attrib::TEX_COORD_0 );
}

Cylinder& Cylinder::set( const vec3 &from, const vec3 &to )
{
	const vec3 axis = ( to - from );
	mHeight = length( axis );
	mDirection = normalize( axis );
	mOrigin = from;
	mCalculationsCached = false;
	return *this;
}

void Cylinder::calculate() const
{
	if( mCalculationsCached )
		return;

	int numSegments = mSubdivisionsAxis;
	if( numSegments < 4 ) {
		float radius = math<float>::max( mRadiusBase, mRadiusApex );
		numSegments = std::max( 12, (int)math<double>::floor( radius * float(M_PI * 2) ) );
	}
	int numSlices = math<int>::max( mSubdivisionsHeight, 1 );

	calculateImplUV( numSegments + 1, numSlices + 1 );
	mCalculationsCached = true;
}

void Cylinder::calculateImplUV( size_t segments, size_t rings ) const
{
	mPositions.resize( segments * rings );
	mNormals.resize( segments * rings );
	mTexCoords.resize( segments * rings, vec2() );
	mIndices.resize( (segments - 1) * (rings - 1) * 6, 0 );

	if( isEnabled( Attrib::COLOR ) )
		mColors.resize( segments * rings );
	else
		mColors.clear();

	const float segmentIncr = 1.0f / (segments - 1);
	const float ringIncr = 1.0f / (rings - 1);
	const quat axis( vec3( 0, 1, 0 ), mDirection );

	// vertex, normal, tex coord and color buffers
	for( size_t j = 0; j < rings; ++j ) {
		for( size_t i = 0; i < segments; ++i ) {
			float cosPhi = -math<float>::cos( i * segmentIncr * float(M_PI * 2) );
			float sinPhi =  math<float>::sin( i * segmentIncr * float(M_PI * 2) );

			float r = lerp<float>( mRadiusBase, mRadiusApex, j * ringIncr );
			float x = r * cosPhi;
			float y = mHeight * j * ringIncr;
			float z = r * sinPhi;
			const vec3 n = normalize( vec3( mHeight * cosPhi, mRadiusBase - mRadiusApex, mHeight * sinPhi ) );

			const size_t k = i * rings + j;
			mPositions[k] = mOrigin + axis * vec3( x, y, z );
			mTexCoords[k] = vec2( i * segmentIncr, 1.0f - j * ringIncr );
			mNormals[k] = axis * n;

			if( isEnabled( Attrib::COLOR ) ) {
				mColors[k] = vec3( n.x * 0.5f + 0.5f, n.y * 0.5f + 0.5f, n.z * 0.5f + 0.5f );
			}
		}
	}

	// index buffer
	size_t k = 0;
	for ( size_t j = 0; j < rings - 1; ++j ) {
		for( size_t i = 0; i < segments - 1; ++i ) {
			mIndices[k++] = (uint32_t)((i + 0) * rings + (j + 0));
			mIndices[k++] = (uint32_t)((i + 1) * rings + (j + 0));
			mIndices[k++] = (uint32_t)((i + 1) * rings + (j + 1));

			mIndices[k++] = (uint32_t)((i + 0) * rings + (j + 0));
			mIndices[k++] = (uint32_t)((i + 1) * rings + (j + 1));
			mIndices[k++] = (uint32_t)((i + 0) * rings + (j + 1));
		}
	}

	// caps
	if( mRadiusBase > 0.0f ) {
		calculateCap( true, 0.0f, mRadiusBase, segments );
	}

	if( mRadiusApex > 0.0f ) {
		calculateCap( false, mHeight, mRadiusApex, segments );
	}
}

void Cylinder::calculateCap( bool flip, float height, float radius, size_t segments ) const
{
	const size_t index = mPositions.size();

	mPositions.resize( index + segments * 2 );
	mTexCoords.resize( index + segments * 2 );
	mNormals.resize( index + segments * 2, flip ? -mDirection : mDirection );

	if( isEnabled( Attrib::COLOR ) ) {
		const vec3 n = flip ? -mDirection : mDirection;
		mColors.resize( index + segments * 2, 
			vec3( n.x * 0.5f + 0.5f, n.y * 0.5f + 0.5f, n.z * 0.5f + 0.5f ) );
	}

	const quat axis( vec3( 0, 1, 0 ), mDirection );

	// vertices
	const float segmentIncr = 1.0f / (segments - 1);
	for( size_t i = 0; i < segments; ++i ) {
		// center point
		mPositions[index + i * 2 + 0] = mOrigin + mDirection * height;
		mTexCoords[index + i * 2 + 0] = vec2( i * segmentIncr, 1.0f - height / mHeight );

		// edge point
		float cosPhi = -math<float>::cos( i * segmentIncr * float(M_PI * 2) );
		float sinPhi =  math<float>::sin( i * segmentIncr * float(M_PI * 2) );

		float x = radius * cosPhi;
		float y = height;
		float z = radius * sinPhi;

		mPositions[index + i * 2 + 1] = mOrigin + axis * vec3( x, y, z );
		mTexCoords[index + i * 2 + 1] = vec2( i * segmentIncr, 1.0f - height / mHeight );
	}

	// index buffer
	size_t k = mIndices.size();
	mIndices.resize( mIndices.size() + 3 * (segments - 1), 0 );

	for( size_t i = 0; i < segments - 1; ++i ) {
		if( flip ) {
			mIndices[k++] = (uint32_t)(index + i * 2 + 0);
			mIndices[k++] = (uint32_t)(index + i * 2 + 3);
			mIndices[k++] = (uint32_t)(index + i * 2 + 1);
		}
		else {
			mIndices[k++] = (uint32_t)(index + i * 2 + 0);
			mIndices[k++] = (uint32_t)(index + i * 2 + 1);
			mIndices[k++] = (uint32_t)(index + i * 2 + 3);
		}
	}
}

uint8_t Cylinder::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
	case Attrib::POSITION: return 3;
	case Attrib::TEX_COORD_0: return isEnabled( Attrib::TEX_COORD_0 ) ? 2 : 0;
	case Attrib::NORMAL: return isEnabled( Attrib::NORMAL ) ? 3 : 0;
	case Attrib::COLOR: return isEnabled( Attrib::COLOR ) ? 3 : 0;
	default:
		return 0;
	}
}

void Cylinder::loadInto( Target *target ) const
{
	calculate();

	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *mPositions.data() ), mPositions.size() );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *mTexCoords.data() ), mTexCoords.size() );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *mNormals.data() ), mNormals.size() );
	if( isEnabled( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *mColors.data() ), mColors.size() );

	target->copyIndices( Primitive::TRIANGLES, mIndices.data(), mIndices.size(), 4 );
}

///////////////////////////////////////////////////////////////////////////////////////
// Plane

Plane::Plane()
	: mOrigin( 0, 0, 0 ), mAxisU( 1, 0, 0 ), mAxisV( 0, 0, 1 ), mSize( 2, 2 ), mSubdivisions( 1, 1 )
{
	enable( Attrib::POSITION );
	enable( Attrib::NORMAL );
	enable( Attrib::TEX_COORD_0 );
}

Plane& Plane::subdivisions( const ivec2 &subdivisions )
{
	mSubdivisions.x = std::max( subdivisions.x, 1 );
	mSubdivisions.y = std::max( subdivisions.y, 1 );
	mCalculationsCached = false;
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

	mCalculationsCached = false;
	return *this;
}

Plane& Plane::axes( const vec3 &uAxis, const vec3 &vAxis )
{
	mAxisU = normalize( uAxis );
	mAxisV = normalize( vAxis );
	mCalculationsCached = false;
	return *this;
}

void Plane::calculate() const
{
	if( mCalculationsCached )
		return;

	const size_t numVerts = ( mSubdivisions.x + 1 ) * ( mSubdivisions.y + 1 );
	mPositions.resize( numVerts );
	if( isEnabled( Attrib::NORMAL ) )
		mNormals.resize( numVerts );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		mTexCoords.resize( numVerts );

	const vec2 stepIncr = vec2( 1, 1 ) / vec2( mSubdivisions );
	const vec3 normal = cross( mAxisV, mAxisU );

	// fill vertex data
	for( int x = 0; x <= mSubdivisions.x; x++ ) {
		for( int y = 0; y <= mSubdivisions.y; y++ ) {
			float u = x * stepIncr.x;
			float v = y * stepIncr.y;

			vec3 pos = mOrigin + ( mSize.x * ( u - 0.5f ) ) * mAxisU + ( mSize.y * ( v - 0.5f ) ) * mAxisV;

			size_t i = x * ( mSubdivisions.y + 1 ) + y;
			mPositions[i] = pos;

			if( isEnabled( Attrib::NORMAL ) )
				mNormals[i] = normal;
			if( isEnabled( Attrib::TEX_COORD_0 ) )
				mTexCoords[i] = vec2( u, v );
		}
	}

	// fill indices. TODO: this could be optimized by moving it to above loop, though last row of vertices would need to be done outside of loop
	mIndices.clear();
	for( int x = 0; x < mSubdivisions.x; x++ ) {
		for( int y = 0; y < mSubdivisions.y; y++ ) {
			uint32_t i = x * ( mSubdivisions.y + 1 ) + y;

			mIndices.push_back( i );
			mIndices.push_back( i + 1 );
			mIndices.push_back( i + mSubdivisions.y + 1 );

			mIndices.push_back( i + mSubdivisions.y + 1 );
			mIndices.push_back( i + 1 );
			mIndices.push_back( i + mSubdivisions.y + 2 );
		}
	}

	mCalculationsCached = true;
}

uint8_t Plane::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::TEX_COORD_0: return isEnabled( Attrib::TEX_COORD_0 ) ? 2 : 0;
		case Attrib::NORMAL: return isEnabled( Attrib::NORMAL ) ? 3 : 0;
		case Attrib::COLOR: return isEnabled( Attrib::COLOR ) ? 3 : 0;
		default:
			return 0;
	}
}

void Plane::loadInto( Target *target ) const
{
	calculate();

	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *mPositions.data() ), mPositions.size() );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *mTexCoords.data() ), mTexCoords.size() );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *mNormals.data() ), mNormals.size() );
	if( isEnabled( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *mColors.data() ), mColors.size() );
	
	target->copyIndices( Primitive::TRIANGLES, mIndices.data(), mIndices.size(), 4 );
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

void Transform::loadInto( Target *target ) const
{
	// we want to capture and then modify both positions and normals
	map<Attrib,Modifier::Access> attribAccess;
	attribAccess[POSITION] = Modifier::READ_WRITE;
	attribAccess[NORMAL] = Modifier::READ_WRITE;
	Modifier modifier( mSource, target, attribAccess, Modifier::IGNORED );
	mSource.loadInto( &modifier );
	
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

void Twist::loadInto( Target *target ) const
{
	// we want to capture and then modify both positions and normals
	map<Attrib,Modifier::Access> attribAccess;
	attribAccess[POSITION] = Modifier::READ_WRITE;
	attribAccess[NORMAL] = Modifier::READ_WRITE;
	Modifier modifier( mSource, target, attribAccess, Modifier::IGNORED );
	mSource.loadInto( &modifier );
	
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

void Lines::loadInto( Target *target ) const
{
	// we are only interested in changing indices
	Modifier modifier( mSource, target, map<Attrib,Modifier::Access>(), Modifier::READ_WRITE );
	mSource.loadInto( &modifier );

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
}

void ColorFromAttrib::loadInto( Target *target ) const
{
	if( (! mFnColor2) && (! mFnColor3) ) {
		mSource.loadInto( target );
		return;
	}

	// we want to capture 'mAttrib' and we want to write COLOR
	map<Attrib,Modifier::Access> attribAccess;
	attribAccess[mAttrib] = Modifier::READ;
	attribAccess[COLOR] = Modifier::WRITE;
	Modifier modifier( mSource, target, attribAccess, Modifier::IGNORED );
	mSource.loadInto( &modifier );

	if( modifier.getAttribDims( mAttrib ) == 0 ) {
		CI_LOG_W( "ColorFromAttrib called on geom::Source missing requested " << attribToString( mAttrib ) );
		mSource.loadInto( target );
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
// Extrude
Extrude::Extrude( const Shape2d &shape, float distance, float approximationScale )
	: mCalculationsCached( false ), mDistance( distance ), mApproximationScale( approximationScale ), mFrontCap( true ), mBackCap( true ), mSubdivisions( 1 )
{
	enable( Attrib::POSITION );
	enable( Attrib::NORMAL );
	enable( Attrib::TEX_COORD_0 );
	
	for( const auto &contour : shape.getContours() )
		mPaths.push_back( contour );
}

void Extrude::calculate() const
{
	if( mCalculationsCached )
		return;
	
	mPathSubdivisionPositions.clear();
	mPathSubdivisionTangents.clear();
	mPositions.clear();
	mNormals.clear();
	mTexCoords.clear();
	mIndices.clear();
	
	// necessary for texcoord calculation
	bool capBoundsEmpty = true;
	Rectf capBounds;
	
	// iterate all the paths of the shape and subdivide, calculating both positions and tangents
	for( const auto &path : mPaths ) {
		if( capBoundsEmpty ) {
			capBounds = path.calcPreciseBoundingBox();
			capBoundsEmpty = false;
		}
		else
			capBounds.include( path.calcPreciseBoundingBox() );
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
	
	TriMesh cap = triangulator.calcMesh();

	// CAPS VERTICES
	const vec2* capPositions = cap.getPositions<2>();
	// front cap
	if( mFrontCap )
		for( size_t v = 0; v < cap.getNumVertices(); ++v ) {
			mPositions.emplace_back( vec3( capPositions[v], mDistance * 0.5f ) );
			mNormals.emplace_back( vec3( 0, 0, 1 ) );
			mTexCoords.emplace_back( vec3( ( mPositions.back().x - capBounds.x1 ) / capBounds.getWidth(),
											1.0f - ( mPositions.back().y - capBounds.y1 ) / capBounds.getHeight(),
											0 ) );
		}
	// back cap
	if( mBackCap )
		for( size_t v = 0; v < cap.getNumVertices(); ++v ) {
			mPositions.emplace_back( vec3( capPositions[v], -mDistance * 0.5f ) );
			mNormals.emplace_back( vec3( 0, 0, -1 ) );
			mTexCoords.emplace_back( vec3( ( mPositions.back().x - capBounds.x1 ) / capBounds.getWidth(),
											1.0f - ( mPositions.back().y - capBounds.y1 ) / capBounds.getHeight(),
											1 ) );
		}
	
	// CAP INDICES
	auto capIndices = cap.getIndices();
	// front cap
	if( mFrontCap )
		for( size_t i = 0; i < capIndices.size(); ++i )
			mIndices.push_back( capIndices[i] );
	// back cap
	if( mBackCap ) {
		for( size_t i = 0; i < capIndices.size(); i += 3 ) { // we need to reverse the winding order for the back cap
			mIndices.push_back( capIndices[i+2] + (uint32_t)cap.getNumVertices() );
			mIndices.push_back( capIndices[i+1] + (uint32_t)cap.getNumVertices() );
			mIndices.push_back( capIndices[i+0] + (uint32_t)cap.getNumVertices() );
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
			uint32_t baseIndex = (uint32_t)mPositions.size();
			for( size_t v = 0; v < pathPositions.size(); ++v ) {
				mPositions.push_back( vec3( pathPositions[v], distance ) );
				mNormals.push_back( vec3( vec2( pathTangents[v].y, -pathTangents[v].x ), 0 ) );
				mTexCoords.emplace_back( vec3( ( mPositions.back().x - capBounds.x1 ) / capBounds.getWidth(),
											1.0f - ( mPositions.back().y - capBounds.y1 ) / capBounds.getHeight(),
											t ) );
			}
			// add the indices
			if( sub != mSubdivisions ) {
				uint32_t numSubdivVerts = (uint32_t)pathPositions.size();
				for( uint32_t j = numSubdivVerts-1, i = 0; i < numSubdivVerts; j = i++ ) {
					mIndices.push_back( baseIndex + i );
					mIndices.push_back( baseIndex + j );
					mIndices.push_back( baseIndex + numSubdivVerts + j );
					mIndices.push_back( baseIndex + i );
					mIndices.push_back( baseIndex + numSubdivVerts + j );
					mIndices.push_back( baseIndex + numSubdivVerts + i );
				}
			}
		}
	}

	mCalculationsCached = true;
}
	
size_t Extrude::getNumVertices() const
{
	calculate();
	return mPositions.size();
}

size_t Extrude::getNumIndices() const
{
	calculate();
	return mIndices.size();
}

uint8_t	Extrude::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::NORMAL: return isEnabled( Attrib::NORMAL ) ? 3 : 0;
		case Attrib::TEX_COORD_0: return isEnabled( Attrib::TEX_COORD_0 ) ? 3 : 0;
		default:
			return 0;
	}
}

void Extrude::loadInto( Target *target ) const
{
	calculate();

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*)mPositions.data(), mPositions.size() );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*)mNormals.data(), mNormals.size() );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 3, 0, (const float*)mTexCoords.data(), mTexCoords.size() );

	target->copyIndices( Primitive::TRIANGLES, mIndices.data(), mIndices.size(), calcIndicesRequiredBytes( mIndices.size() ) );
}

///////////////////////////////////////////////////////////////////////////////////////
// Extrude
ExtrudeSpline::ExtrudeSpline( const Shape2d &shape, const ci::BSpline<3,float> &spline, int splineSubdivisions, float approximationScale )
	: mCalculationsCached( false ), mApproximationScale( approximationScale ), mFrontCap( true ), mBackCap( true ), mSubdivisions( splineSubdivisions )
{
	enable( Attrib::POSITION );
	enable( Attrib::NORMAL );
	enable( Attrib::TEX_COORD_0 );
	
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
}

void ExtrudeSpline::calculate() const
{
	if( mCalculationsCached )
		return;
	
	mPathSubdivisionPositions.clear();
	mPathSubdivisionTangents.clear();
	mPositions.clear();
	mNormals.clear();
	mTexCoords.clear();
	mIndices.clear();

	// necessary for texcoord calculation
	bool capBoundsEmpty = true;
	Rectf capBounds;
	
	// iterate all the paths of the shape and subdivide, calculating both positions and tangents
	for( const auto &path : mPaths ) {
		if( capBoundsEmpty ) {
			capBounds = path.calcPreciseBoundingBox();
			capBoundsEmpty = false;
		}
		else
			capBounds.include( path.calcPreciseBoundingBox() );
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
	
	TriMesh cap = triangulator.calcMesh();

	// CAP VERTICES
	const vec2* capPositions = cap.getPositions<2>();
	// front cap
	if( mFrontCap ) {
		const vec3 frontNormal = vec3( mSplineFrames.front() * vec4( 0, 0, -1, 0 ) );
		for( size_t v = 0; v < cap.getNumVertices(); ++v ) {
			mPositions.emplace_back( vec3( mSplineFrames.front() * vec4( capPositions[v], 0, 1 ) ) );
			mNormals.emplace_back( frontNormal );
			mTexCoords.emplace_back( vec3( ( capPositions[v].x - capBounds.x1 ) / capBounds.getWidth(),
											1.0f - ( capPositions[v].y - capBounds.y1 ) / capBounds.getHeight(),
											0 ) );
		}
	}
	// back cap
	if( mBackCap ) {
		const vec3 backNormal = vec3( mSplineFrames.back() * vec4( 0, 0, 1, 0 ) );
		for( size_t v = 0; v < cap.getNumVertices(); ++v ) {
			mPositions.emplace_back( vec3( mSplineFrames.back() * vec4( capPositions[v], 0, 1 ) ) );
			mNormals.emplace_back( backNormal );
			mTexCoords.emplace_back( vec3( ( capPositions[v].x - capBounds.x1 ) / capBounds.getWidth(),
											1.0f - ( capPositions[v].y - capBounds.y1 ) / capBounds.getHeight(),
											1 ) );
		}
	}
	
	// CAP INDICES
	auto capIndices = cap.getIndices();
	// front cap
	if( mFrontCap )
		for( size_t i = 0; i < capIndices.size(); ++i )
			mIndices.push_back( capIndices[i] );
	// back cap
	if( mBackCap ) {
		for( size_t i = 0; i < capIndices.size(); i += 3 ) { // we need to reverse the winding order for the back cap
			mIndices.push_back( capIndices[i+2] + (uint32_t)cap.getNumVertices() );
			mIndices.push_back( capIndices[i+1] + (uint32_t)cap.getNumVertices() );
			mIndices.push_back( capIndices[i+0] + (uint32_t)cap.getNumVertices() );
		}
	}

	// EXTRUSION
	for( size_t p = 0; p < mPathSubdivisionPositions.size(); ++p ) {
		for( int sub = 0; sub <= mSubdivisions; ++sub ) {
			const mat4 &transform = mSplineFrames[sub];
			const auto &pathPositions = mPathSubdivisionPositions[p];
			const auto &pathTangents = mPathSubdivisionTangents[p];
			// add all the positions & normals
			uint32_t baseIndex = (uint32_t)mPositions.size();
			for( size_t v = 0; v < pathPositions.size(); ++v ) {
				mPositions.push_back( vec3( transform * vec4( pathPositions[v], 0, 1 ) ) );
				mNormals.push_back( vec3( transform * vec4( vec2( pathTangents[v].y, -pathTangents[v].x ), 0, 0 ) ) );
				mTexCoords.emplace_back( vec3( ( pathPositions[v].x - capBounds.x1 ) / capBounds.getWidth(),
											1.0f - ( pathPositions[v].y - capBounds.y1 ) / capBounds.getHeight(),
											mSplineTimes[sub] ) );
			}
			// add the indices
			if( sub != mSubdivisions ) {
				uint32_t numSubdivVerts = (uint32_t)pathPositions.size();
				for( uint32_t j = numSubdivVerts-1, i = 0; i < numSubdivVerts; j = i++ ) {
					mIndices.push_back( baseIndex + i );
					mIndices.push_back( baseIndex + j );
					mIndices.push_back( baseIndex + numSubdivVerts + j );
					mIndices.push_back( baseIndex + i );
					mIndices.push_back( baseIndex + numSubdivVerts + j );
					mIndices.push_back( baseIndex + numSubdivVerts + i );
				}
			}
		}
	}

	mCalculationsCached = true;
}
	
size_t ExtrudeSpline::getNumVertices() const
{
	calculate();
	return mPositions.size();
}

size_t ExtrudeSpline::getNumIndices() const
{
	calculate();
	return mIndices.size();
}

uint8_t	ExtrudeSpline::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::NORMAL: return isEnabled( Attrib::NORMAL ) ? 3 : 0;
		case Attrib::TEX_COORD_0: return isEnabled( Attrib::TEX_COORD_0 ) ? 3 : 0;		
		default:
			return 0;
	}
}

void ExtrudeSpline::loadInto( Target *target ) const
{
	calculate();

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*)mPositions.data(), mPositions.size() );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*)mNormals.data(), mNormals.size() );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 3, 0, (const float*)mTexCoords.data(), mTexCoords.size() );

	target->copyIndices( Primitive::TRIANGLES, mIndices.data(), mIndices.size(), calcIndicesRequiredBytes( mIndices.size() ) );
}

///////////////////////////////////////////////////////////////////////////////////////
// VertexNormalLines
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

void VertexNormalLines::loadInto( Target *target ) const
{
	// we are interested in removing normals and colors and outputting positions
	map<Attrib,Modifier::Access> attribAccess;
	attribAccess[Attrib::POSITION] = Modifier::READ_WRITE;
	attribAccess[Attrib::NORMAL] = Modifier::READ_WRITE; // we actually won't ever write it but this prevents pass-through
	attribAccess[Attrib::TEX_COORD_0] = Modifier::READ_WRITE;
	attribAccess[Attrib::COLOR] = Modifier::WRITE; // we actually won't ever write it but this prevents pass-through as colors are often inconvenient
	Modifier modifier( mSource, target, attribAccess, Modifier::READ_WRITE );
	mSource.loadInto( &modifier );

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

void BSpline::loadInto( Target *target ) const
{
	target->copyAttrib( Attrib::POSITION, mPositionDims, 0, mPositions.data(), mNumVertices );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*)mNormals.data(), mNumVertices );
}

template BSpline::BSpline( const ci::BSpline<2,float>&, int );
template BSpline::BSpline( const ci::BSpline<3,float>&, int );

} } // namespace cinder::geom