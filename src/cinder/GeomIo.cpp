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

template<uint8_t DSTDIM>
void copyDataMultAddImpl( const float *srcData, size_t numElements, size_t dstStrideBytes, float *dstData, const Vec2f &mult, const Vec2f &add )
{
	static const float sFillerData[4] = { 0, 0, 0, 1 };
	const uint8_t MINDIM = (2 < DSTDIM) ? 2 : DSTDIM;

	if( dstStrideBytes == 0 )
		dstStrideBytes = DSTDIM * sizeof(float);

	for( size_t v = 0; v < numElements; ++v ) {
		uint8_t d;
		for( d = 0; d < MINDIM; ++d ) {
			dstData[d] = srcData[d] * mult[d] + add[d];
		}
		for( ; d < DSTDIM; ++d ) {
			dstData[d] = sFillerData[d];
		}
		srcData += 2;
		dstData = (float*)((uint8_t*)dstData + dstStrideBytes);
	}
}

template<uint8_t DSTDIM>
void copyDataMultAddImpl( const float *srcData, size_t numElements, size_t dstStrideBytes, float *dstData, const Vec3f &mult, const Vec3f &add )
{
	static const float sFillerData[4] = { 0, 0, 0, 1 };
	const uint8_t MINDIM = (3 < DSTDIM) ? 3 : DSTDIM;

	if( dstStrideBytes == 0 )
		dstStrideBytes = DSTDIM * sizeof(float);

	for( size_t v = 0; v < numElements; ++v ) {
		uint8_t d;
		for( d = 0; d < MINDIM; ++d ) {
			dstData[d] = srcData[d] * mult[d] + add[d];
		}
		for( ; d < DSTDIM; ++d ) {
			dstData[d] = sFillerData[d];
		}
		srcData += 3;
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

void Source::copyDataMultAdd( const float *srcData, size_t numElements,
	uint8_t dstDimensions, size_t dstStrideBytes, float *dstData, const Vec3f &mult, const Vec3f &add )
{
	switch( dstDimensions) {
	case 2: copyDataMultAddImpl<2>( srcData, numElements, dstStrideBytes, dstData, mult, add ); break;
	case 3: copyDataMultAddImpl<3>( srcData, numElements, dstStrideBytes, dstData, mult, add ); break;
	case 4: copyDataMultAddImpl<4>( srcData, numElements, dstStrideBytes, dstData, mult, add ); break;
	default: throw ExcIllegalDestDimensions();
	}
}

void Source::copyDataMultAdd( const float *srcData, size_t numElements,
	uint8_t dstDimensions, size_t dstStrideBytes, float *dstData, const Vec2f &mult, const Vec2f &add )
{
	switch( dstDimensions) {
	case 2: copyDataMultAddImpl<2>( srcData, numElements, dstStrideBytes, dstData, mult, add ); break;
	case 3: copyDataMultAddImpl<3>( srcData, numElements, dstStrideBytes, dstData, mult, add ); break;
	case 4: copyDataMultAddImpl<4>( srcData, numElements, dstStrideBytes, dstData, mult, add ); break;
	default: throw ExcIllegalDestDimensions();
	}
}


namespace { 
template<typename T>
void copyIndexDataForceTrianglesImpl( Primitive primitive, const uint32_t *source, size_t numIndices, T *target )
{
	switch( primitive ) {
	case Primitive::LINES:
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

void Target::generateIndices( Primitive sourcePrimitive, size_t sourceNumIndices )
{
	unique_ptr<uint32_t[]> indices( new uint32_t[sourceNumIndices] );

	uint32_t count = 0;
	std::generate( indices.get(), indices.get() + sourceNumIndices, [&] { return count++; } );

	uint8_t requiredBytesPerIndex = 4;
	if( sourceNumIndices < 256 )
		requiredBytesPerIndex = 1;
	else if( sourceNumIndices < 65536 )
		requiredBytesPerIndex = 2;
	// now have the target copy these indices
	copyIndices( sourcePrimitive, indices.get(), sourceNumIndices, requiredBytesPerIndex );
}

///////////////////////////////////////////////////////////////////////////////////////
// Rect
float Rect::sPositions[4*2] = { 0.5f,-0.5f,	-0.5f,-0.5f,	0.5f,0.5f,	-0.5f,0.5f };
float Rect::sColors[4*3] = { 1, 0, 1,	0, 0, 1,	1, 1, 1,	0, 1, 1 };
float Rect::sTexCoords[4*2] = { 1, 1,	0, 1,		1, 0,		0, 0 };
float Rect::sNormals[4*3] = {0, 0, 1,	0, 0, 1,	0, 0, 1,	0, 0, 1 };

Rect::Rect()
	: mPos( Vec2f::zero() ), mScale( Vec2f::one() )
{
	enable( Attrib::POSITION );	
	enable( Attrib::TEX_COORD_0 );
	enable( Attrib::NORMAL );
}

void Rect::loadInto( Target *target ) const
{
	unique_ptr<Vec2f[]> positions( new Vec2f[4] );
	for( size_t p = 0; p < 4; ++p )
		positions.get()[p] = Vec2f( sPositions[p*2+0], sPositions[p*2+1] ) * mScale + mPos;

	target->copyAttrib( Attrib::POSITION, 2, 0, positions.get()->ptr(), 4 );
	if( isEnabled( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, sColors, 4 );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, sTexCoords, 4 );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, sNormals, 4 );
}

uint8_t	Rect::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
	case Attrib::POSITION: return 2;
	case Attrib::COLOR: return isEnabled( Attrib::COLOR ) ? 3 : 0;
	case Attrib::TEX_COORD_0: return isEnabled( Attrib::TEX_COORD_0 ) ? 2 : 0;
	case Attrib::NORMAL: return isEnabled( Attrib::NORMAL ) ? 3 : 0;
	default:
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Cube
float Cube::sPositions[24*3] = {  1.0f, 1.0f, 1.0f,   1.0f,-1.0f, 1.0f,	 1.0f,-1.0f,-1.0f,   1.0f, 1.0f,-1.0f,	// +X
	1.0f, 1.0f, 1.0f,   1.0f, 1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,  -1.0f, 1.0f, 1.0f,	// +Y
	1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,  -1.0f,-1.0f, 1.0f,   1.0f,-1.0f, 1.0f,	// +Z
	-1.0f, 1.0f, 1.0f,  -1.0f, 1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,  -1.0f,-1.0f, 1.0f,	// -X
	-1.0f,-1.0f,-1.0f,   1.0f,-1.0f,-1.0f,   1.0f,-1.0f, 1.0f,  -1.0f,-1.0f, 1.0f,	// -Y
	1.0f,-1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,   1.0f, 1.0f,-1.0f };// -Z

uint32_t Cube::sIndices[6*6] ={	0, 1, 2, 0, 2, 3,
	4, 5, 6, 4, 6, 7,
	8, 9,10, 8, 10,11,
	12,13,14,12,14,15,
	16,17,18,16,18,19,
	20,21,22,20,22,23 };

float Cube::sColors[24*3]	={  1,0,0,	1,0,0,	1,0,0,	1,0,0,		// +X = red
	0,1,0,	0,1,0,	0,1,0,	0,1,0,		// +Y = green
	0,0,1,	0,0,1,	0,0,1,	0,0,1,		// +Z = blue
	0,1,1,	0,1,1,	0,1,1,	0,1,1,		// -X = cyan
	1,0,1,	1,0,1,	1,0,1,	1,0,1,		// -Y = purple
	1,1,0,	1,1,0,	1,1,0,	1,1,0 };	// -Z = yellow

float Cube::sTexCoords[24*2]={	0,0,	1,0,	1,1,	0,1,
	1,0,	1,1,	0,1,	0,0,
	0,0,	1,0,	1,1,	0,1,
	1,0,	1,1,	0,1,	0,0,
	1,1,	0,1,	0,0,	1,0,
	1,1,	0,1,	0,0,	1,0 };

float Cube::sNormals[24*3]=	{	1,0,0,	1,0,0,	1,0,0,	1,0,0,
	0,1,0,	0,1,0,	0,1,0,	0,1,0,
	0,0,1,	0,0,1,	0,0,1,	0,0,1,
	-1,0,0,	-1,0,0,	-1,0,0,	-1,0,0,
	0,-1,0,	0,-1,0,  0,-1,0,0,-1,0,
	0,0,-1,	0,0,-1,	0,0,-1,	0,0,-1 };


Cube::Cube()
{
	enable( Attrib::POSITION );
	enable( Attrib::TEX_COORD_0 );
	enable( Attrib::NORMAL );
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

void Cube::loadInto( Target *target ) const
{
	target->copyAttrib( Attrib::POSITION, 3, 0, sPositions, 24 );
	if( isEnabled( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, sColors, 24 );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, sTexCoords, 24 );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, sNormals, 24 );

	target->copyIndices( Primitive::TRIANGLES, sIndices, 36, 1 );
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
	mPositions.resize( 60, Vec3f::zero() );
	mIndices.resize( 60, 0 );
	mNormals.resize( 60, Vec3f::zero() );

	for( size_t i = 0; i < 60; ++i ) {
		mPositions[i] = *reinterpret_cast<const Vec3f*>(&sPositions[sIndices[i]*3]);
		mIndices[i] = (uint32_t) i;
	}

	// calculate the face normal for each triangle
	size_t numTriangles = mIndices.size() / 3;
	for( size_t i = 0; i < numTriangles; ++i ) {
		const uint32_t index0 = mIndices[i*3+0];
		const uint32_t index1 = mIndices[i*3+1];
		const uint32_t index2 = mIndices[i*3+2];

		const Vec3f &v0 = mPositions[index0];
		const Vec3f &v1 = mPositions[index1];
		const Vec3f &v2 = mPositions[index2];

		Vec3f e0 = v1 - v0;
		Vec3f e1 = v2 - v0;

		mNormals[index0] = mNormals[index1] = mNormals[index2] = e0.cross(e1).normalized();
	}

	// add color if necessary
	if( isEnabled( Attrib::COLOR ) ) {
		size_t numPositions = mPositions.size();
		mColors.resize( numPositions, Vec3f::zero() );
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

	target->copyAttrib( Attrib::POSITION, 3, 0, mPositions.data()->ptr(), mPositions.size() );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, mNormals.data()->ptr(), mNormals.size() );
	if( isEnabled( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, mColors.data()->ptr(), mColors.size() );

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

Teapot&	Teapot::subdivision( int sub )
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
	Vec3f patch[4][4];
	Vec3f patchRevV[4][4];
	getPatch( patchNum, patch, false);
	getPatch( patchNum, patchRevV, true);

	// Patch without modification
	buildPatch( patchRevV, B, dB, v, n, tc, el, index, elIndex, tcIndex, grid, Matrix33f::identity(), false );

	// Patch reflected in x
	if( reflectX ) {
		buildPatch( patch, B, dB, v, n, tc, el, index, elIndex, tcIndex, grid, Matrix33f( Vec3f(-1.0f, 0.0f, 0.0f),
			Vec3f(0.0f, 1.0f, 0.0f),
			Vec3f(0.0f, 0.0f, 1.0f) ), true );
	}

	// Patch reflected in y
	if( reflectY ) {
		buildPatch( patch, B, dB, v, n, tc, el, index, elIndex, tcIndex, grid, Matrix33f( Vec3f(1.0f, 0.0f, 0.0f),
			Vec3f(0.0f, -1.0f, 0.0f),
			Vec3f(0.0f, 0.0f, 1.0f) ), true );
	}

	// Patch reflected in x and y
	if( reflectX && reflectY ) {
		buildPatch( patchRevV, B, dB, v, n, tc, el, index, elIndex, tcIndex, grid, Matrix33f( Vec3f(-1.0f, 0.0f, 0.0f),
			Vec3f(0.0f, -1.0f, 0.0f),
			Vec3f(0.0f, 0.0f, 1.0f) ), false );
	}
}

void Teapot::buildPatch( Vec3f patch[][4], float *B, float *dB, float *v, float *n, float *tc, 
	unsigned int *el, int &index, int &elIndex, int &tcIndex, int grid, Matrix33f reflect, bool invertNormal )
{
	int startIndex = index / 3;
	float tcFactor = 1.0f / grid;

	float scale = 2.0f / 6.42813f; // awful hack to keep it within unit cube

	for( int i = 0; i <= grid; i++ ) {
		for( int j = 0 ; j <= grid; j++) {
			Vec3f pt = reflect * evaluate( i, j, B, patch );
			Vec3f norm = reflect * evaluateNormal( i, j, B, dB, patch );
			if( invertNormal )
				norm = -norm;
			// awful hack due to normals discontinuity
			if( abs(pt.x) < 0.01f && abs(pt.y) < 0.01f )
				norm = ( pt.z < 1 ) ? -Vec3f::zAxis() : Vec3f::zAxis();

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

void Teapot::getPatch( int patchNum, Vec3f patch[][4], bool reverseV )
{
	for( int u = 0; u < 4; u++) {          // Loop in u direction
		for( int v = 0; v < 4; v++ ) {     // Loop in v direction
			if( reverseV ) {
				patch[u][v] = Vec3f(
					sCurveData[sPatchIndices[patchNum][u*4+(3-v)]][0],
					sCurveData[sPatchIndices[patchNum][u*4+(3-v)]][1],
					sCurveData[sPatchIndices[patchNum][u*4+(3-v)]][2]
				);
			}
			else {
				patch[u][v] = Vec3f(
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

Vec3f Teapot::evaluate( int gridU, int gridV, const float *B, const Vec3f patch[][4] )
{
	Vec3f p( Vec3f::zero() );
	for( int i = 0; i < 4; i++) {
		for( int j = 0; j < 4; j++) {
			p += patch[i][j] * B[gridU*4+i] * B[gridV*4+j];
		}
	}

	return p;
}

Vec3f Teapot::evaluateNormal( int gridU, int gridV, const float *B, const float *dB, const Vec3f patch[][4] )
{
	Vec3f du( Vec3f::zero() );
	Vec3f dv( Vec3f::zero() );

	for( int i = 0; i < 4; i++ ) {
		for( int j = 0; j < 4; j++ ) {
			du += patch[i][j] * dB[gridU*4+i] * B[gridV*4+j];
			dv += patch[i][j] * B[gridU*4+i] * dB[gridV*4+j];
		}
	}

	return du.cross( dv ).normalized();
}

///////////////////////////////////////////////////////////////////////////////////////
// Circle
Circle::Circle()
	: mRequestedSegments( -1 ), mCenter( 0, 0 ), mRadius( 1.0f )
{
	enable( Attrib::POSITION );
	enable( Attrib::TEX_COORD_0 );
	enable( Attrib::NORMAL );
	updateVertexCounts();
}

Circle&	Circle::segments( int segments )
{
	mRequestedSegments = segments;
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
	if( mRequestedSegments <= 0 )
		mNumSegments = (int)math<double>::floor( mRadius * float(M_PI * 2) );
	else
		mNumSegments = mRequestedSegments;

	if( mNumSegments < 3 ) mNumSegments = 3;
	mNumVertices = mNumSegments + 1 + 1;
}

void Circle::calculate() const
{
	mPositions.resize(mNumVertices); //  = unique_ptr<Vec2f[]>( new Vec2f[mNumVertices] );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		mTexCoords.resize(mNumVertices); // = unique_ptr<Vec2f[]>( new Vec2f[mNumVertices] );
	if( isEnabled( Attrib::NORMAL ) )		
		mNormals.resize(mNumVertices); // = unique_ptr<Vec3f[]>( new Vec3f[mNumVertices] );	

	// center
	mPositions[0] = mCenter;
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		mTexCoords[0] = Vec2f( 0.5f, 0.5f );
	if( isEnabled( Attrib::NORMAL ) )
		mNormals[0] = Vec3f( 0, 0, 1 );

	// iterate the segments
	const float tDelta = 1 / (float)mNumSegments * 2.0f * 3.14159f;
	float t = 0;
	for( int s = 0; s <= mNumSegments; s++ ) {
		Vec2f unit( math<float>::cos( t ), math<float>::sin( t ) );
		mPositions[s+1] = mCenter + unit * mRadius;
		if( isEnabled( Attrib::TEX_COORD_0 ) )
			mTexCoords[s+1] = unit * 0.5f + Vec2f( 0.5f, 0.5f );
		if( isEnabled( Attrib::NORMAL ) )
			mNormals[s+1] = Vec3f( 0, 0, 1 );
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
	: mNumSegments( 18 ), mNumSlices( 0 ), mCenter( 0, 0, 0 ), mRadius( 1.0f ), mCalculationsCached( false )
{
	enable( Attrib::POSITION );
	enable( Attrib::NORMAL );
	enable( Attrib::TEX_COORD_0 );
}

void Sphere::calculate() const
{
	if( mCalculationsCached )
		return;

	int numSegments = mNumSegments;
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

			vertIt->set( x * radius + mCenter.x, y * radius + mCenter.y, z * radius + mCenter.z );
			++vertIt;

			if( hasNormals ) {
				normIt->set( x, y, z );
				++normIt;
			}
			if( hasTexCoords ) {
				texIt->set( u, 1.0f - v );
				++texIt;
			}
			if( hasColors ) {
				colorIt->set( x * 0.5f + 0.5f, y * 0.5f + 0.5f, z * 0.5f + 0.5f );
				++colorIt;
			}
		}
	}

	auto indexIt = mIndices.begin();
	for( size_t r = 0; r < rings - 1; r++ ) {
		for( size_t s = 0; s < segments - 1 ; s++ ) {
			*indexIt++ = r * segments + ( s + 1 );
			*indexIt++ = r * segments + s;
			*indexIt++ = ( r + 1 ) * segments + ( s + 1 );

			*indexIt++ = ( r + 1 ) * segments + s;
			*indexIt++ = ( r + 1 ) * segments + ( s + 1 );
			*indexIt++ = r * segments + s;
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
		target->copyAttrib( Attrib::POSITION, 3, 0, mPositions.data()->ptr(), mPositions.size() );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, mTexCoords.data()->ptr(), mTexCoords.size() );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, mNormals.data()->ptr(), mNormals.size() );
	if( isEnabled( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, mColors.data()->ptr(), mColors.size() );

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
	mPositions.assign( reinterpret_cast<Vec3f*>(sPositions), reinterpret_cast<Vec3f*>(sPositions) + 12 );
	mNormals.assign( reinterpret_cast<Vec3f*>(sPositions), reinterpret_cast<Vec3f*>(sPositions) + 12 );
	mIndices.assign( sIndices, sIndices + 60 );

	// subdivide all triangles
	subdivide();

	// spherize
	std::for_each( mPositions.begin(), mPositions.end(), std::mem_fun_ref( &Vec3f::normalize ) );
	std::for_each( mNormals.begin(), mNormals.end(), std::mem_fun_ref( &Vec3f::normalize ) );

	// add color if necessary
	if( isEnabled( Attrib::COLOR ) ) {
		size_t numPositions = mPositions.size();
		mColors.resize( numPositions, Vec3f::zero() );
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
	mTexCoords.resize( mNormals.size(), Vec2f::zero() );
	for( size_t i = 0; i < mNormals.size(); ++i ) {
		const Vec3f &normal = mNormals[i];
		mTexCoords[i].x = (math<float>::atan2( normal.z, -normal.x ) / float(M_PI)) * 0.5f + 0.5f;
		mTexCoords[i].y = -normal.y * 0.5f + 0.5f;
	}

	// lambda closure to easily add a vertex with unique texture coordinate to our mesh
	auto addVertex = [&] ( size_t i, const Vec2f &uv ) {
		const uint32_t index = mIndices[i];
		mIndices[i] = mPositions.size();
		mPositions.push_back( mPositions[index] );
		mNormals.push_back( mNormals[index] );
		mTexCoords.push_back( uv );

		if( isEnabled( Attrib::COLOR ) )
			mColors.push_back( mColors[index] );
	};

	// fix texture seams (this is where the magic happens)
	size_t numTriangles = mIndices.size() / 3;
	for( size_t i = 0; i < numTriangles; ++i ) {
		const Vec2f &uv0 = mTexCoords[ mIndices[i * 3 + 0] ];
		const Vec2f &uv1 = mTexCoords[ mIndices[i * 3 + 1] ];
		const Vec2f &uv2 = mTexCoords[ mIndices[i * 3 + 2] ];

		const float d1 = uv1.x - uv0.x;
		const float d2 = uv2.x - uv0.x;

		if( math<float>::abs(d1) > 0.5f && math<float>::abs(d2) > 0.5f ) {
			addVertex( i * 3 + 0, uv0 + Vec2f( (d1 > 0.0f) ? 1.0f : -1.0f, 0.0f ) );
		}
		else if( math<float>::abs(d1) > 0.5f ) {
			addVertex( i * 3 + 1, uv1 + Vec2f( (d1 < 0.0f) ? 1.0f : -1.0f, 0.0f ) );
		}
		else if( math<float>::abs(d2) > 0.5f ) {
			addVertex( i * 3 + 2, uv2 + Vec2f( (d2 < 0.0f) ? 1.0f : -1.0f, 0.0f ) );
		}
	}
}

void Icosphere::subdivide() const
{
	for( int j = 0; j < mSubdivision; ++j ) {
		mPositions.reserve( mPositions.size() + mIndices.size() );
		mNormals.reserve( mNormals.size() + mIndices.size() );
		mIndices.reserve( mIndices.size() * 4 );

		const int numTriangles = mIndices.size() / 3;
		for( int i = 0; i < numTriangles; ++i ) {
			uint32_t index0 = mIndices[i * 3 + 0];
			uint32_t index1 = mIndices[i * 3 + 1];
			uint32_t index2 = mIndices[i * 3 + 2];

			uint32_t index3 = mPositions.size();
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

	target->copyAttrib( Attrib::POSITION, 3, 0, mPositions.data()->ptr(), mPositions.size() );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, mTexCoords.data()->ptr(), mTexCoords.size() );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, mNormals.data()->ptr(), mNormals.size() );
	if( isEnabled( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, mColors.data()->ptr(), mColors.size() );

	target->copyIndices( Primitive::TRIANGLES, mIndices.data(), mIndices.size(), 4 );
}

///////////////////////////////////////////////////////////////////////////////////////
// Capsule

Capsule::Capsule()
	: mDirection( 0, 1, 0 ), mLength( 1.0f )
{
	radius( 0.5f );
	slices( 6 );

	enable( Attrib::POSITION );
	enable( Attrib::NORMAL );
	enable( Attrib::TEX_COORD_0 );
}

Capsule& Capsule::set( const Vec3f &from, const Vec3f &to )
{
	const Vec3f axis = ( to - from );
	mLength = axis.length();
	mDirection = axis.normalized();
	mCenter = from + 0.5f * axis;
	mCalculationsCached = false;
	return *this;
}

void Capsule::calculate() const
{
	if( mCalculationsCached )
		return;

	int numSegments = mNumSegments;
	if( numSegments < 4 )
		numSegments = std::max( 12, (int)math<double>::floor( mRadius * float(M_PI * 2) ) );

	// numRings = numSegments / 2 and should always be an even number
	int numRings = ( numSegments >> 2 ) << 1;

	calculateImplUV( numSegments + 1, numRings + 1 );
	mCalculationsCached = true;
}

void Capsule::calculateImplUV( size_t segments, size_t rings ) const
{
	size_t ringsBody = mNumSlices + 1;
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
			mIndices.push_back( r * segments + ( s + 1 ) );
			mIndices.push_back( r * segments + ( s + 0 ) );
			mIndices.push_back( ( r + 1 ) * segments + ( s + 1 ) );

			mIndices.push_back( ( r + 1 ) * segments + ( s + 0 ) );
			mIndices.push_back( ( r + 1 ) * segments + ( s + 1 ) );
			mIndices.push_back( r * segments + s );
		}
	}
}

void Capsule::calculateRing( size_t segments, float radius, float y, float dy ) const
{
	const ci::Quatf quaternion( Vec3f::yAxis(), mDirection );

	bool hasNormals = isEnabled( Attrib::NORMAL );
	bool hasTexCoords = isEnabled( Attrib::TEX_COORD_0 );
	bool hasColors = isEnabled( Attrib::COLOR );

	float segIncr = 1.0f / (float)( segments - 1 );
	for( size_t s = 0; s < segments; s++ ) {
		float x = math<float>::cos( float(M_PI * 2) * s * segIncr ) * radius;
		float z = math<float>::sin( float(M_PI * 2) * s * segIncr ) * radius;

		mPositions.push_back( mCenter + ( quaternion * Vec3f( mRadius * x, mRadius * y + mLength * dy, mRadius * z ) ) );

		if( hasNormals ) {
			mNormals.push_back( quaternion * Vec3f( x, y, z ) );
		}
		if( hasTexCoords ) {
			// perform cylindrical projection
			float u = 1.0f - (s * segIncr);
			float v = 0.5f - ((mRadius * y + mLength * dy) / (2.0f * mRadius + mLength));
			mTexCoords.push_back( Vec2f( u, v ) );
		}
		if( hasColors ) {
			float g = 0.5f + ((mRadius * y + mLength * dy) / (2.0f * mRadius + mLength));
			mColors.push_back( Vec3f( x * 0.5f + 0.5f, g, z * 0.5f + 0.5f ) );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Torus

Torus::Torus()
	: mCenter( 0, 0, 0), mRadiusMajor( 1.0f ), mRadiusMinor( 0.75f ), mCoils( 1 ), mHeight( 0 )
	, mNumSegmentsAxis( 18 ), mNumSegmentsRing( 18 ), mTwist( 0 ), mTwistOffset( 0 )
{
	enable( Attrib::POSITION );
	enable( Attrib::NORMAL );
	enable( Attrib::TEX_COORD_0 );
}

void Torus::calculate() const
{
	if( mCalculationsCached )
		return;

	int numAxis = (int) math<float>::ceil( mNumSegmentsAxis * mCoils );
	if( numAxis < 4 )
		numAxis = std::max( 12, (int)math<double>::floor( mRadiusMajor * float(M_PI * 2) ) );

	int numRing = mNumSegmentsRing;
	if( numRing < 3 )
		numRing = std::max( 12, (int)math<double>::floor( mRadiusMajor * float(M_PI * 2) ) );

	calculateImplUV( numAxis + 1, numRing + 1 );
	mCalculationsCached = true;
}

void Torus::calculateImplUV( size_t segments, size_t rings ) const
{
	mPositions.resize( segments * rings, Vec3f::zero() );
	mNormals.resize( segments * rings, Vec3f::zero() );
	mTexCoords.resize( segments * rings, Vec2f::zero() );
	mIndices.resize( (segments - 1) * (rings - 1) * 6, 0 );

	if( isEnabled( Attrib::COLOR ) )
		mColors.resize( segments * rings, Vec3f::zero() );
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
			mPositions[k] = mCenter + Vec3f( x, y, z );
			mTexCoords[k] = Vec2f( i * majorIncr, j * minorIncr );
			mNormals[k] = Vec3f( cosPhi * cosTheta, sinTheta, sinPhi * cosTheta );

			if( isEnabled( Attrib::COLOR ) ) {
				const Vec3f &n = mNormals[k];
				mColors[k] = Vec3f( n.x * 0.5f + 0.5f, n.y * 0.5f + 0.5f, n.z * 0.5f + 0.5f );
			}
		}
	}

	// index buffer
	size_t k = 0;
	for( size_t i = 0; i < segments - 1; ++i ) {
		for ( size_t j = 0; j < rings - 1; ++j ) {
			mIndices[k++] = (i + 0) * rings + (j + 0);
			mIndices[k++] = (i + 1) * rings + (j + 1);
			mIndices[k++] = (i + 1) * rings + (j + 0);

			mIndices[k++] = (i + 0) * rings + (j + 0);
			mIndices[k++] = (i + 0) * rings + (j + 1);
			mIndices[k++] = (i + 1) * rings + (j + 1);
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

	target->copyAttrib( Attrib::POSITION, 3, 0, mPositions.data()->ptr(), mPositions.size() );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, mTexCoords.data()->ptr(), mTexCoords.size() );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, mNormals.data()->ptr(), mNormals.size() );
	if( isEnabled( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, mColors.data()->ptr(), mColors.size() );

	target->copyIndices( Primitive::TRIANGLES, mIndices.data(), mIndices.size(), 4 );
}

///////////////////////////////////////////////////////////////////////////////////////
// Cylinder

Cylinder::Cylinder()
	: mOrigin( 0, 0, 0 ), mHeight( 2.0f ), mDirection( 0, 1, 0 ), mRadiusBase( 1.0f ), mRadiusApex( 1.0f ), mNumSegments( 18 ), mNumSlices( 6 )
{
	enable( Attrib::POSITION );
	enable( Attrib::NORMAL );
	enable( Attrib::TEX_COORD_0 );
}

Cylinder& Cylinder::set( const Vec3f &from, const Vec3f &to )
{
	const Vec3f axis = ( to - from );
	mHeight = axis.length();
	mDirection = axis.normalized();
	mOrigin = from;
	mCalculationsCached = false;
	return *this;
}

void Cylinder::calculate() const
{
	if( mCalculationsCached )
		return;

	int numSegments = mNumSegments;
	if( numSegments < 4 ) {
		float radius = math<float>::max( mRadiusBase, mRadiusApex );
		numSegments = std::max( 12, (int)math<double>::floor( radius * float(M_PI * 2) ) );
	}
	int numSlices = math<int>::max( mNumSlices, 1 );

	calculateImplUV( numSegments + 1, numSlices + 1 );
	mCalculationsCached = true;
}

void Cylinder::calculateImplUV( size_t segments, size_t rings ) const
{
	mPositions.resize( segments * rings, Vec3f::zero() );
	mNormals.resize( segments * rings, Vec3f::zero() );
	mTexCoords.resize( segments * rings, Vec2f::zero() );
	mIndices.resize( (segments - 1) * (rings - 1) * 6, 0 );

	if( isEnabled( Attrib::COLOR ) )
		mColors.resize( segments * rings, Vec3f::zero() );
	else
		mColors.clear();

	const float segmentIncr = 1.0f / (segments - 1);
	const float ringIncr = 1.0f / (rings - 1);
	const Quatf axis( Vec3f::yAxis(), mDirection );

	// vertex, normal, tex coord and color buffers
	for( size_t j = 0; j < rings; ++j ) {
		for( size_t i = 0; i < segments; ++i ) {
			float cosPhi = -math<float>::cos( i * segmentIncr * float(M_PI * 2) );
			float sinPhi =  math<float>::sin( i * segmentIncr * float(M_PI * 2) );

			float r = lerp<float>( mRadiusBase, mRadiusApex, j * ringIncr );
			float x = r * cosPhi;
			float y = mHeight * j * ringIncr;
			float z = r * sinPhi;
			const Vec3f n = Vec3f( mHeight * cosPhi, mRadiusBase - mRadiusApex, mHeight * sinPhi ).normalized();

			const size_t k = i * rings + j;
			mPositions[k] = mOrigin + axis * Vec3f( x, y, z );
			mTexCoords[k] = Vec2f( i * segmentIncr, 1.0f - j * ringIncr );
			mNormals[k] = axis * n;

			if( isEnabled( Attrib::COLOR ) ) {
				mColors[k] = Vec3f( n.x * 0.5f + 0.5f, n.y * 0.5f + 0.5f, n.z * 0.5f + 0.5f );
			}
		}
	}

	// index buffer
	size_t k = 0;
	for ( size_t j = 0; j < rings - 1; ++j ) {
		for( size_t i = 0; i < segments - 1; ++i ) {
			mIndices[k++] = (i + 0) * rings + (j + 0);
			mIndices[k++] = (i + 1) * rings + (j + 0);
			mIndices[k++] = (i + 1) * rings + (j + 1);

			mIndices[k++] = (i + 0) * rings + (j + 0);
			mIndices[k++] = (i + 1) * rings + (j + 1);
			mIndices[k++] = (i + 0) * rings + (j + 1);
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

	mPositions.resize( index + segments * 2, Vec3f::zero() );
	mTexCoords.resize( index + segments * 2, Vec2f::zero() );
	mNormals.resize( index + segments * 2, flip ? -mDirection : mDirection );

	if( isEnabled( Attrib::COLOR ) ) {
		const Vec3f n = flip ? -mDirection : mDirection;
		mColors.resize( index + segments * 2, 
			Vec3f( n.x * 0.5f + 0.5f, n.y * 0.5f + 0.5f, n.z * 0.5f + 0.5f ) );
	}

	const Quatf axis( Vec3f::yAxis(), mDirection );

	// vertices
	const float segmentIncr = 1.0f / (segments - 1);
	for( size_t i = 0; i < segments; ++i ) {
		// center point
		mPositions[index + i * 2 + 0] = mOrigin + mDirection * height;
		mTexCoords[index + i * 2 + 0] = Vec2f( i * segmentIncr, 1.0f - height / mHeight );

		// edge point
		float cosPhi = -math<float>::cos( i * segmentIncr * float(M_PI * 2) );
		float sinPhi =  math<float>::sin( i * segmentIncr * float(M_PI * 2) );

		float x = radius * cosPhi;
		float y = height;
		float z = radius * sinPhi;

		mPositions[index + i * 2 + 1] = mOrigin + axis * Vec3f( x, y, z );
		mTexCoords[index + i * 2 + 1] = Vec2f( i * segmentIncr, 1.0f - height / mHeight );
	}

	// index buffer
	size_t k = mIndices.size();
	mIndices.resize( mIndices.size() + 3 * (segments - 1), 0 );

	for( size_t i = 0; i < segments - 1; ++i ) {
		if( flip ) {
			mIndices[k++] = index + i * 2 + 0;
			mIndices[k++] = index + i * 2 + 3;
			mIndices[k++] = index + i * 2 + 1;
		}
		else {
			mIndices[k++] = index + i * 2 + 0;
			mIndices[k++] = index + i * 2 + 1;
			mIndices[k++] = index + i * 2 + 3;
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

	target->copyAttrib( Attrib::POSITION, 3, 0, mPositions.data()->ptr(), mPositions.size() );
	if( isEnabled( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, mTexCoords.data()->ptr(), mTexCoords.size() );
	if( isEnabled( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, mNormals.data()->ptr(), mNormals.size() );
	if( isEnabled( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, mColors.data()->ptr(), mColors.size() );

	target->copyIndices( Primitive::TRIANGLES, mIndices.data(), mIndices.size(), 4 );
}


///////////////////////////////////////////////////////////////////////////////////////
// SplineExtrusion
#if 0
SplineExtrusion::SplineExtrusion( const std::function<Vec3f(float)> &pathCurve, int pathSegments, float radius, int radiusSegments )
	: mCalculationsCached( false ), mScale( 1 , 1, 1 ), mPos( Vec3f::zero() )
{
	calculateCurve( pathCurve, pathSegments, radius, radiusSegments );
}

void SplineExtrusion::calculateCurve( const std::function<Vec3f(float)> &pathCurve, int pathSegments, float radius, int radiusSegments ) const
{
	mNumVertices = pathSegments * radiusSegments;
	int numTriangles = ( pathSegments - 1 ) * radiusSegments * 2;
	mNumIndices = numTriangles * 3;

	mPositions = unique_ptr<float[]>( new float[mNumVertices * 3] );
	mTexCoords = unique_ptr<float[]>( new float[mNumVertices * 2] );	
	mNormals = unique_ptr<float[]>( new float[mNumVertices * 3] );
	mIndices = unique_ptr<uint32_t[]>( new uint32_t[mNumIndices] );

	for( int pathSeg = 0; pathSeg < pathSegments; ++pathSeg ) {

	}
}

void SplineExtrusion::calculate() const
{
	if( mCalculationsCached )
		return;

	mNumVertices = 32 * (mSubdivision + 1) * (mSubdivision + 1);
	int numFaces = mSubdivision * mSubdivision * 32;
	mNumIndices = numFaces * 6;
	mPositions = unique_ptr<float[]>( new float[mNumVertices * 3] );
	mTexCoords = unique_ptr<float[]>( new float[mNumVertices * 2] );	
	mNormals = unique_ptr<float[]>( new float[mNumVertices * 3] );
	mIndices = unique_ptr<uint32_t[]>( new uint32_t[mNumIndices] );

	generatePatches( mPositions.get(), mNormals.get(), mTexCoords.get(), mIndices.get(), mSubdivision );

	mCalculationsCached = true;
}


size_t SplineExtrusion::getNumVertices() const
{
	calculate();

	return mNumVertices;
}

bool SplineExtrusion::hasAttrib( Attrib attr ) const
{
	switch( attr ) {
	case Attrib::POSITION: return true;
	case Attrib::TEX_COORD_0: return mHasTexCoord0;
	case Attrib::NORMAL: return mHasNormals;
	default:
		return false;
	}
}

bool SplineExtrusion::canProvideAttrib( Attrib attr ) const
{
	switch( attr ) {
	case Attrib::POSITION:
	case Attrib::TEX_COORD_0:
	case Attrib::NORMAL:
		return true;
	default:
		return false;
	}
}

uint8_t	SplineExtrusion::getAttribDims( Attrib attr ) const
{
	if( ! canProvideAttrib( attr ) )
		return 0;

	switch( attr ) {
	case Attrib::POSITION: return 3;
	case Attrib::TEX_COORD_0: return 2;
	case Attrib::NORMAL: return 3;
	default:
		return 0;
	}
}

void SplineExtrusion::copyAttrib( Attrib attr, uint8_t dimensions, size_t stride, float *dest ) const
{
	calculate();

	switch( attr ) {
	case Attrib::POSITION:
		copyDataMultAdd( mPositions.get(), mNumVertices, dimensions, stride, dest, mScale, mPos );
		break;
	case Attrib::TEX_COORD_0:
		copyData( 2, mTexCoords.get(), mNumVertices, dimensions, stride, dest );
		break;
	case Attrib::NORMAL:
		copyData( 3, mNormals.get(), mNumVertices, dimensions, stride, dest );
		break;
	default:
		throw ExcMissingAttrib();
	}
}

size_t SplineExtrusion::getNumIndices() const
{
	calculate();

	return mNumIndices;
}

void SplineExtrusion::copyIndices( uint16_t *dest ) const
{
	calculate();

	for( int i = 0; i < mNumIndices; ++i )
		dest[i] = mIndices.get()[i];
}

void SplineExtrusion::copyIndices( uint32_t *dest ) const
{
	calculate();

	memcpy( dest, mIndices.get(), mNumIndices * sizeof(uint32_t) );		
}

#endif

} } // namespace cinder::geom