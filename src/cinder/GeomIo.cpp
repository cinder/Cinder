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

#include "cinder/Camera.h"
#include "cinder/Frustum.h"
#include "cinder/GeomIo.h"
#include "cinder/Quaternion.h"
#include "cinder/Log.h"
#include "cinder/TriMesh.h"
#include "cinder/Triangulate.h"
#include "cinder/BSpline.h"
#include "cinder/Matrix.h"
#include "cinder/Sphere.h"
#include <algorithm>

#if defined( CINDER_ANDROID )
  #include "cinder/app/App.h"
#endif

using namespace std;

namespace cinder { namespace geom {

std::string sAttribNames[(int)Attrib::NUM_ATTRIBS] = {
	"POSITION", "COLOR", "TEX_COORD_0", "TEX_COORD_1", "TEX_COORD_2", "TEX_COORD_3",
	"NORMAL", "TANGENT", "BITANGENT", "BONE_INDEX", "BONE_WEIGHT",
	"CUSTOM_0", "CUSTOM_1", "CUSTOM_2", "CUSTOM_3", "CUSTOM_4", "CUSTOM_5", "CUSTOM_6", "CUSTOM_7", "CUSTOM_8", "CUSTOM_9"
};

std::string sPrimitiveNames[(int)Primitive::NUM_PRIMITIVES] = {
	"LINES", "LINE_STRIP", "TRIANGLES", "TRIANGLE_STRIP", "TRIANGLE_FAN"
};

std::string attribToString( Attrib attrib )
{
	if( attrib < Attrib::NUM_ATTRIBS )
		return sAttribNames[(int)attrib];
	else if( attrib == Attrib::USER_DEFINED )
		return "USER_DEFINED";
	else
		return "";
}

std::string primitiveToString( Primitive primitive )
{
	if( primitive < Primitive::NUM_PRIMITIVES )
		return sPrimitiveNames[(int)primitive];
	else
		return "";
}

///////////////////////////////////////////////////////////////////////////////////////
// Modifier
size_t Modifier::getNumVertices( const Modifier::Params &upstreamParams ) const
{
	return upstreamParams.getNumVertices();
}

size_t Modifier::getNumIndices( const Modifier::Params &upstreamParams ) const
{
	return upstreamParams.getNumIndices();
}

Primitive Modifier::getPrimitive( const Modifier::Params &upstreamParams ) const
{
	return upstreamParams.getPrimitive();
}

uint8_t	Modifier::getAttribDims( Attrib attr, uint8_t upstreamDims ) const
{
	return upstreamDims;
}

AttribSet Modifier::getAvailableAttribs( const Modifier::Params &upstreamParams ) const
{
	return upstreamParams.getAvailableAttribs();
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

// Assumes source is tightly packed
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

template<uint8_t SRCDIM, uint8_t DSTDIM>
void copyDataImpl( const float *srcData, size_t srcStrideBytes, size_t numElements, size_t dstStrideBytes, float *dstData )
{
	static const float sFillerData[4] = { 0, 0, 0, 1 };
	const uint8_t MINDIM = (SRCDIM < DSTDIM) ? SRCDIM : DSTDIM;

	if( dstStrideBytes == 0 )
		dstStrideBytes = DSTDIM * sizeof(float);
	if( srcStrideBytes == 0 )
		srcStrideBytes = SRCDIM * sizeof(float);

	for( size_t v = 0; v < numElements; ++v ) {
		uint8_t d;
		for( d = 0; d < MINDIM; ++d ) {
			dstData[d] = srcData[d];
		}
		for( ; d < DSTDIM; ++d ) {
			dstData[d] = sFillerData[d];
		}
		srcData = (float*)((uint8_t*)srcData + srcStrideBytes);
		dstData = (float*)((uint8_t*)dstData + dstStrideBytes);
	}
}
} // anonymous namespace

void copyData( uint8_t srcDimensions, size_t srcStrideBytes, const float *srcData, size_t numElements, uint8_t dstDimensions, size_t dstStrideBytes, float *dstData )
{
	if( srcStrideBytes == 0 )
		srcStrideBytes = srcDimensions * sizeof(float);
	if( dstStrideBytes == 0 )
		dstStrideBytes = dstDimensions * sizeof(float);

	if( srcDimensions == 0 ) { // fill with zeros
		for( size_t v = 0; v < numElements; ++v ) {
			for( uint8_t d = 0; d < dstDimensions; ++d )
				dstData[d] = 0;
			dstData = (float*)((uint8_t*)dstData + dstStrideBytes);	
		}
	}
	else if( srcStrideBytes == srcDimensions * sizeof(float) ) 	// call equivalent method that doesn't support srcStrideBytes
		copyData( srcDimensions, srcData, numElements, dstDimensions, dstStrideBytes, dstData );
	// we can get away with a memcpy
	else if( (srcDimensions == dstDimensions) && (dstStrideBytes == dstDimensions * sizeof(float)) && (srcStrideBytes == dstStrideBytes) ) {
		memcpy( dstData, srcData, numElements * srcDimensions * sizeof(float) );
	}
	else {
		switch( srcDimensions ) {
			case 1:
				switch( dstDimensions ) {
					case 1: copyDataImpl<1,1>( srcData, srcStrideBytes, numElements, dstStrideBytes, dstData ); break;
					case 2: copyDataImpl<1,2>( srcData, srcStrideBytes, numElements, dstStrideBytes, dstData ); break;
					case 3: copyDataImpl<1,3>( srcData, srcStrideBytes, numElements, dstStrideBytes, dstData ); break;
					case 4: copyDataImpl<1,4>( srcData, srcStrideBytes, numElements, dstStrideBytes, dstData ); break;
					default: throw ExcIllegalDestDimensions();
				}
			break;
			case 2:
				switch( dstDimensions ) {
					case 1: copyDataImpl<2,1>( srcData, srcStrideBytes, numElements, dstStrideBytes, dstData ); break;
					case 2: copyDataImpl<2,2>( srcData, srcStrideBytes, numElements, dstStrideBytes, dstData ); break;
					case 3: copyDataImpl<2,3>( srcData, srcStrideBytes, numElements, dstStrideBytes, dstData ); break;
					case 4: copyDataImpl<2,4>( srcData, srcStrideBytes, numElements, dstStrideBytes, dstData ); break;
					default: throw ExcIllegalDestDimensions();
				}
			break;
			case 3:
				switch( dstDimensions ) {
					case 1: copyDataImpl<3,1>( srcData, srcStrideBytes, numElements, dstStrideBytes, dstData ); break;
					case 2: copyDataImpl<3,2>( srcData, srcStrideBytes, numElements, dstStrideBytes, dstData ); break;
					case 3: copyDataImpl<3,3>( srcData, srcStrideBytes, numElements, dstStrideBytes, dstData ); break;
					case 4: copyDataImpl<3,4>( srcData, srcStrideBytes, numElements, dstStrideBytes, dstData ); break;
					default: throw ExcIllegalDestDimensions();
				}
			break;
			case 4:
				switch( dstDimensions ) {
					case 1: copyDataImpl<4,1>( srcData, srcStrideBytes, numElements, dstStrideBytes, dstData ); break;
					case 2: copyDataImpl<4,2>( srcData, srcStrideBytes, numElements, dstStrideBytes, dstData ); break;
					case 3: copyDataImpl<4,3>( srcData, srcStrideBytes, numElements, dstStrideBytes, dstData ); break;
					case 4: copyDataImpl<4,4>( srcData, srcStrideBytes, numElements, dstStrideBytes, dstData ); break;
					default: throw ExcIllegalDestDimensions();
				}
			break;
			default:
				throw ExcIllegalSourceDimensions();
		}
	}
}

void copyData( uint8_t srcDimensions, const float *srcData, size_t numElements, uint8_t dstDimensions, size_t dstStrideBytes, float *dstData )
{
	if( dstStrideBytes == 0 )
		dstStrideBytes = dstDimensions * sizeof(float);

	// we can get away with a memcpy
	if( srcDimensions == 0 ) { // fill with zeros
		for( size_t v = 0; v < numElements; ++v ) {
			for( uint8_t d = 0; d < dstDimensions; ++d )
				dstData[d] = 0;
			dstData = (float*)((uint8_t*)dstData + dstStrideBytes);	
		}
	}	
	else if( (srcDimensions == dstDimensions) && (dstStrideBytes == dstDimensions * sizeof(float)) ) {
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
void copyIndexDataForceTrianglesImpl( Primitive primitive, const uint32_t *source, size_t numIndices, T indexOffset, T *target )
{
	switch( primitive ) {
		case Primitive::LINES:
		case Primitive::LINE_STRIP:
		case Primitive::TRIANGLES:
			if( indexOffset == 0 ) {
				memcpy( target, source, sizeof(uint32_t) * numIndices );
			}
			else {
				for( size_t i = 0; i < numIndices; ++i )
					target[i] = source[i] + indexOffset;
			}
		break;
		case Primitive::TRIANGLE_STRIP: { // ABC, CBD, CDE, EDF, etc
			if( numIndices < 3 )
				return;
			size_t outIdx = 0; // (012, 213), (234, 435), etc : (odd,even), (odd,even), etc
			for( size_t i = 0; i < numIndices - 2; ++i ) {
				if( i & 1 ) { // odd
					target[outIdx++] = source[i+1];
					target[outIdx++] = source[i];
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
		break;
		default:
			throw ExcIllegalPrimitiveType();
	}
}

// Lengyel, Eric. "Computing Tangent Space Basis Vectors for an Arbitrary Mesh". 
// Terathon Software 3D Graphics Library, 2001.
// http://www.terathon.com/code/tangent.html
template<typename TEXTYPE>
void calculateTangentsImpl( size_t numIndices, const uint32_t *indices, size_t numVertices, const vec3 *positions, const vec3 *normals, const TEXTYPE *texCoords, vector<vec3> *resultTangents, vector<vec3> *resultBitangents )
{
	if( resultTangents )
		resultTangents->assign( numVertices, vec3( 0 ) );

	size_t numTriangles = numIndices / 3;
	for( size_t i = 0; i < numTriangles; ++i ) {
		uint32_t index0 = indices[i * 3];
		uint32_t index1 = indices[i * 3 + 1];
		uint32_t index2 = indices[i * 3 + 2];

		const vec3 &v0 = positions[index0];
		const vec3 &v1 = positions[index1];
		const vec3 &v2 = positions[index2];

		const vec2 &w0 = vec2( texCoords[index0] );
		const vec2 &w1 = vec2( texCoords[index1] );
		const vec2 &w2 = vec2( texCoords[index2] );

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

		float r = (s1 * t2 - s2 * t1);
		if( r != 0.0f ) r = 1.0f / r;

		vec3 tangent( (t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r );

		(*resultTangents)[index0] += tangent;
		(*resultTangents)[index1] += tangent;
		(*resultTangents)[index2] += tangent;
	}

	for( size_t i = 0; i < numVertices; ++i ) {
		vec3 normal = normals[i];
		vec3 tangent = (*resultTangents)[i];
		(*resultTangents)[i] = ( tangent - normal * dot( normal, tangent ) );

		float len = length2( (*resultTangents)[i] );
		if( len > 0.0f )
			(*resultTangents)[i] /= sqrt( len );
	}

	if( resultBitangents ) {
		resultBitangents->reserve( numVertices );
		for( size_t i = 0; i < numVertices; ++i )
			resultBitangents->emplace_back( normalize( cross( normals[i], (*resultTangents)[i] ) ) );
	}
}

} // anonymous namespace

void calculateTangents( size_t numIndices, const uint32_t *indices, size_t numVertices, const vec3 *positions, const vec3 *normals, const vec2 *texCoords, vector<vec3> *resultTangents, vector<vec3> *resultBitangents )
{
	calculateTangentsImpl( numIndices, indices, numVertices, positions, normals, texCoords, resultTangents, resultBitangents );
}

void calculateTangents( size_t numIndices, const uint32_t *indices, size_t numVertices, const vec3 *positions, const vec3 *normals, const vec3 *texCoords, vector<vec3> *resultTangents, vector<vec3> *resultBitangents )
{
	calculateTangentsImpl( numIndices, indices, numVertices, positions, normals, texCoords, resultTangents, resultBitangents );
}

///////////////////////////////////////////////////////////////////////////////////////
// Target
void Target::copyIndexDataForceTriangles( Primitive primitive, const uint32_t *source, size_t numIndices, uint32_t indexOffset, uint32_t *target )
{
	copyIndexDataForceTrianglesImpl<uint32_t>( primitive, source, numIndices, indexOffset, target );
}

void Target::copyIndexDataForceTriangles( Primitive primitive, const uint32_t *source, size_t numIndices, uint16_t indexOffset, uint16_t *target )
{
	copyIndexDataForceTrianglesImpl<uint16_t>( primitive, source, numIndices, indexOffset, target );
}

void Target::copyIndexDataForceLines( Primitive primitive, const uint32_t *source, size_t numIndices, uint32_t indexOffset, uint32_t *target )
{
	switch( primitive ) {
		case Primitive::LINES:
			for( size_t i = 0; i < numIndices; ++i )
				target[i] = (uint32_t)(source[i] + indexOffset);
		break;
		case Primitive::LINE_STRIP: {
			size_t outIdx = 0;
			for( size_t i = 0; i < numIndices - 1; ++i ) {
				target[outIdx++] = (uint32_t)(source[i + 0] + indexOffset);
				target[outIdx++] = (uint32_t)(source[i + 1] + indexOffset);
			}
		}
		break;
		default:
			throw ExcIllegalPrimitiveType();
	}
}

// Used for creating appropriate indices for non-indexed geometry
void Target::generateIndicesForceTriangles( Primitive primitive, size_t numInputIndices, uint32_t indexOffset, uint32_t *target )
{
	switch( primitive ) {
		case Primitive::TRIANGLES:
			for( size_t i = 0; i < numInputIndices; ++i )
				target[i] = (uint32_t)(indexOffset + i);
		break;
		case Primitive::TRIANGLE_FAN: {
			if( numInputIndices < 3 )
				return;
			size_t outIdx = 0;
			for( size_t i = 0; i < numInputIndices - 2; ++i ) {
				target[outIdx++] = (uint32_t)indexOffset;
				target[outIdx++] = (uint32_t)(i + 1 + indexOffset);
				target[outIdx++] = (uint32_t)(i + 2 + indexOffset);
			}
		}
		break;
		case Primitive::TRIANGLE_STRIP: {
			if( numInputIndices < 3 )
				return;
			size_t outIdx = 0; // (012, 213), (234, 435), etc : (odd,even), (odd,even), etc
			for( size_t i = 0; i < numInputIndices - 2; ++i ) {
				if( i & 1 ) { // odd
					target[outIdx++] = (uint32_t)(i + 1 + indexOffset);
					target[outIdx++] = (uint32_t)(i + 0 + indexOffset);
					target[outIdx++] = (uint32_t)(i + 2 + indexOffset);
				}
				else { // even
					target[outIdx++] = (uint32_t)(i + 0 + indexOffset);
					target[outIdx++] = (uint32_t)(i + 1 + indexOffset);
					target[outIdx++] = (uint32_t)(i + 2 + indexOffset);
				}
			}
		}
		break;
		default:
			throw ExcIllegalPrimitiveType();
	}
}

void Target::generateIndicesForceLines( Primitive primitive, size_t numInputIndices, uint32_t indexOffset, uint32_t *target )
{
	switch( primitive ) {
		case Primitive::LINES:
			for( size_t i = 0; i < numInputIndices; ++i )
				target[i] = (uint32_t)(indexOffset + i);
		break;
		case Primitive::LINE_STRIP: {
			size_t outIdx = 0;
			for( size_t i = 0; i < numInputIndices - 1; ++i ) {
				target[outIdx++] = (uint32_t)(indexOffset + i);
				target[outIdx++] = (uint32_t)(indexOffset + i + 1);
			}
		}
		break;
		default:
			throw ExcIllegalPrimitiveType();
	}
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

Primitive Target::determineCombinedPrimitive( Primitive a, Primitive b )
{
	auto isTriangle = []( Primitive p ) { return p == TRIANGLES || p == TRIANGLE_FAN || p == TRIANGLE_STRIP; };
	auto isLines = []( Primitive p ) { return p == LINES || p == LINE_STRIP; };

	if( isTriangle( a ) && isTriangle( b ) ) // triangles
		return Primitive::TRIANGLES;
	else if( isLines( a ) && isLines( b ) ) // lines
		return Primitive::LINES;
	else // unknown
		return Primitive::NUM_PRIMITIVES;
}

///////////////////////////////////////////////////////////////////////////////////////
// Rect
//float Rect::sPositions[4*2] = { 0.5f,-0.5f,	-0.5f,-0.5f,	0.5f,0.5f,	-0.5f,0.5f };
//float Rect::sColors[4*3] = { 1, 0, 1,	0, 0, 1,	1, 1, 1,	0, 1, 1 };
//float Rect::sTexCoords[4*2] = { 1, 1,		0, 1,		1, 0,		0, 0 };
const float Rect::sNormals[4*3] = {0, 0, 1,	0, 0, 1,	0, 0, 1,	0, 0, 1 };
const float Rect::sTangents[4*3] = {0.7071067f, 0.7071067f, 0,	0.7071067f, 0.7071067f, 0,	0.7071067f, 0.7071067f, 0,	0.7071067f, 0.7071067f, 0 };

Rect::Rect()
	: mHasColors( false )
{
	// upper-left, upper-right, lower-left, lower-right
	mPositions[0] = vec2( -0.5f, -0.5f );
	mPositions[1] = vec2( 0.5f, -0.5f );
	mPositions[2] = vec2( -0.5f, 0.5f );
	mPositions[3] = vec2( 0.5f, 0.5f );
	setDefaultColors();
	setDefaultTexCoords();
}

Rect::Rect( const Rectf &r )
	: mHasColors( false )
{
	rect( r );
	setDefaultColors();
	setDefaultTexCoords();
}

Rect& Rect::rect( const Rectf &r )
{
	mPositions[0] = r.getUpperLeft();
	mPositions[1] = r.getUpperRight();
	mPositions[2] = r.getLowerLeft();
	mPositions[3] = r.getLowerRight();
	return *this;
}

Rect& Rect::colors( const ColorAf &upperLeft, const ColorAf &upperRight, const ColorAf &lowerRight, const ColorAf &lowerLeft )
{
	mHasColors = true;
	mColors[0] = upperLeft;
	mColors[1] = upperRight;
	mColors[2] = lowerLeft;
	mColors[3] = lowerRight;
	return *this;
}

Rect& Rect::texCoords( const vec2 &upperLeft, const vec2 &upperRight, const vec2 &lowerRight, const vec2 &lowerLeft )
{
	mTexCoords[0] = upperLeft;
	mTexCoords[1] = upperRight;
	mTexCoords[2] = lowerLeft;
	mTexCoords[3] = lowerRight;
	return *this;
}

void Rect::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	if( requestedAttribs.count( Attrib::POSITION ) )
		target->copyAttrib( Attrib::POSITION, 2, 0, (const float*)mPositions.data(), 4 );
	if( requestedAttribs.count( Attrib::NORMAL ) )
		target->copyAttrib( Attrib::NORMAL, 3, 0, sNormals, 4 );
	if( requestedAttribs.count( Attrib::TEX_COORD_0 ) )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, (const float*)mTexCoords.data(), 4 );
	if( requestedAttribs.count( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 4, 0, (const float*)mColors.data(), 4 );
	if( requestedAttribs.count( Attrib::TANGENT ) )
		target->copyAttrib( Attrib::TANGENT, 3, 0, sTangents, 4 );
}

uint8_t	Rect::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 2;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 2;
		case Attrib::COLOR: return mHasColors ? 4 : 0;
		case Attrib::TANGENT: return 3;
		default:
			return 0;
	}
}

AttribSet Rect::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::COLOR, Attrib::TANGENT };
}

void Rect::setDefaultColors()
{
	// upper-left, upper-right, lower-left, lower-right
	mColors[0] = ColorAf( 1.0f, 0.0f, 0.0f, 1.0f );
	mColors[1] = ColorAf( 0.0f, 1.0f, 0.0f, 1.0f );
	mColors[2] = ColorAf( 1.0f, 1.0f, 0.0f, 1.0f );
	mColors[3] = ColorAf( 0.0f, 0.0f, 1.0f, 1.0f );
}

void Rect::setDefaultTexCoords()
{
	// upper-left, upper-right, lower-left, lower-right
	mTexCoords[0] = vec2( 0.0f, 1.0f );
	mTexCoords[1] = vec2( 1.0f, 1.0f );
	mTexCoords[2] = vec2( 0.0f, 0.0f );
	mTexCoords[3] = vec2( 1.0f, 0.0f );
}

///////////////////////////////////////////////////////////////////////////////////////
// RoundedRect
RoundedRect::RoundedRect()
	: mSubdivisions( -1 ), mCornerRadius( 0.1f ), mHasColors( false ), mNumVertices( 0 )
{
	rect( Rectf( -0.5f, -0.5f, 0.5f, 0.5f ) );
	updateVertexCount();
	setDefaultColors();
	setDefaultTexCoords();
}
	
RoundedRect::RoundedRect( const Rectf &r, float cornerRadius )
	: mSubdivisions( -1 ), mCornerRadius( cornerRadius ), mHasColors( false ), mNumVertices( 0 )
{
	rect( r );
	updateVertexCount();
	setDefaultColors();
	setDefaultTexCoords();
}
	
RoundedRect& RoundedRect::cornerSubdivisions( int cornerSubdivisions )
{
	mSubdivisions = cornerSubdivisions;
	updateVertexCount();
	return *this;
}

RoundedRect& RoundedRect::cornerRadius( float cornerRadius )
{
	mCornerRadius = cornerRadius;
	updateVertexCount();
	return *this;
}

RoundedRect& RoundedRect::texCoords( const vec2 &upperLeft, const vec2 &lowerRight )
{
	mRectTexCoords = Rectf( upperLeft.x, lowerRight.y, lowerRight.x, upperLeft.y );
	return *this;
}

RoundedRect& RoundedRect::colors( const ColorAf &upperLeft, const ColorAf &upperRight, const ColorAf &lowerRight, const ColorAf &lowerLeft )
{
	mHasColors = true;
	mColors[0] = lowerLeft;  
	mColors[1] = lowerRight; 
	mColors[2] = upperRight; 
	mColors[3] = upperLeft;
	return *this;
}
	
void RoundedRect::updateVertexCount()
{
	if( mSubdivisions <= 0 ) {
		mSubdivisions = (int)math<double>::floor( mCornerRadius * M_PI * 2 / 4 );
	}
	if( mSubdivisions < 2 ) mSubdivisions = 2;
	
	mNumVertices = ( mSubdivisions + 1 ) * 4 + 2;
}
	
uint8_t	RoundedRect::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 2;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 2;
		case Attrib::COLOR: return mHasColors ? 4 : 0;
		case Attrib::TANGENT: return 3;
		default:
			return 0;
	}
}

AttribSet RoundedRect::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::COLOR, Attrib::TANGENT };
}
	
void RoundedRect::loadInto( cinder::geom::Target *target, const AttribSet &requestedAttribs ) const
{
	std::vector<vec2> positions;
	if( requestedAttribs.count( geom::Attrib::POSITION ) )
		positions.resize( mNumVertices );
	std::vector<vec2> texCoords;
	if( requestedAttribs.count( geom::Attrib::TEX_COORD_0 ) )
		texCoords.resize( mNumVertices );
	std::vector<vec4> colors;
	if( requestedAttribs.count( geom::Attrib::COLOR ) )
		colors.resize( mNumVertices );
	std::vector<vec3> normals;
	if( requestedAttribs.count( geom::Attrib::NORMAL ) )
		normals.resize( mNumVertices );
	std::vector<vec3> tangents;
	if( requestedAttribs.count( geom::Attrib::TANGENT ) )
		tangents.resize( mNumVertices );
	
	auto posCenter = mRectPositions.getCenter();
	auto texCenter = mRectTexCoords.getCenter();
	
	auto bufferPositions = ! positions.empty();
	auto bufferTexCoords = ! texCoords.empty();
	auto bufferNormals = ! normals.empty();
	auto bufferTangents = ! tangents.empty();
	auto bufferColors = ! colors.empty();
	
	if( bufferPositions )
		positions[0] = posCenter;
	if( bufferTexCoords )
		texCoords[0] = texCenter;
	if( bufferNormals )
		normals[0] = vec3( 0, 0, 1 );
	if( bufferTangents )
		tangents[0] = vec3( 0.7071067f, 0.7071067f, 0 );
	if( bufferColors )
		colors[0] = (mColors[0] / 4.0f) + (mColors[1] / 4.0f) + (mColors[2] / 4.0f) + (mColors[3] / 4.0f);

	float cornerRadius = glm::max( 0.0f, glm::min( mCornerRadius, 0.5f * glm::min( mRectPositions.getWidth(), mRectPositions.getHeight() ) ) );

	size_t tri = 1;
	const float angleDelta = (float)(1 / (float)mSubdivisions * M_PI / 2);
	const std::array<vec2, 4> cornerCenterVerts = {
		vec2( mRectPositions.x2 - cornerRadius, mRectPositions.y2 - cornerRadius ),
		vec2( mRectPositions.x1 + cornerRadius, mRectPositions.y2 - cornerRadius ),
		vec2( mRectPositions.x1 + cornerRadius, mRectPositions.y1 + cornerRadius ),
		vec2( mRectPositions.x2 - cornerRadius, mRectPositions.y1 + cornerRadius )
	};
	vec2 texCoordOffset = ( cornerRadius / mRectPositions.getSize() ) * mRectTexCoords.getSize();
	const std::array<vec2, 4> cornerCenterTexCoords = {
		vec2( mRectTexCoords.x2 - texCoordOffset.x, texCoordOffset.y + mRectTexCoords.y1 ), // lower right
		vec2( texCoordOffset.x + mRectTexCoords.x1, texCoordOffset.y + mRectTexCoords.y1 ), // lower left
		vec2( texCoordOffset.x + mRectTexCoords.x1, mRectTexCoords.y2 - texCoordOffset.y ), // upper left
		vec2( mRectTexCoords.x2 - texCoordOffset.x, mRectTexCoords.y2 - texCoordOffset.y )	// upper right
	};
	for( size_t corner = 0; corner < 4; ++corner ) {
		float angle = (float)(corner * M_PI / 2.0f);
		vec2 cornerCenter( cornerCenterVerts[corner] );
		vec2 cornerTexCoord( cornerCenterTexCoords[corner] );
		vec4 cornerColor( mColors[corner] );
		for( int s = 0; s <= mSubdivisions; s++ ) {
			auto cosVal = math<float>::cos( angle );
			auto sinVal = math<float>::sin( angle );
			// Need this calculation for texCoords and colors
			auto currentTexCoord = vec2( cornerTexCoord.x + cosVal * texCoordOffset.x,
										cornerTexCoord.y - sinVal * texCoordOffset.y );
			if( bufferPositions )
				positions[tri] = vec2( cornerCenter.x + cosVal * cornerRadius,
									   cornerCenter.y + sinVal * cornerRadius );
			if( bufferTexCoords )
				texCoords[tri] = currentTexCoord;
			if( bufferNormals )
				normals[tri] = vec3( 0, 0, 1 );
			if( bufferTangents )
				tangents[tri] = vec3( 0.7071067f, 0.7071067f, 0 );
			if( bufferColors ) {
				auto colorU0 = lerp( mColors[2], mColors[3], currentTexCoord.x / mRectTexCoords.getWidth() );
				auto colorU1 = lerp( mColors[0], mColors[1], currentTexCoord.x / mRectTexCoords.getWidth() );
				colors[tri] = lerp( colorU0, colorU1, currentTexCoord.y / mRectTexCoords.getHeight() );
			}
			++tri;
			angle += angleDelta;
		}
	}
	// Need this calculation for texCoords and colors
	auto currentTexCoord = vec2( mRectTexCoords.x2, texCoordOffset.y + mRectTexCoords.y1 );
	// close it off
	if( bufferPositions )
		positions[tri] = vec2( mRectPositions.x2, mRectPositions.y2 - cornerRadius );
	if( bufferTexCoords )
		texCoords[tri] = currentTexCoord;
	if( bufferNormals )
		normals[tri] = vec3( 0, 0, 1 );
	if( bufferTangents )
		tangents[tri] = vec3( 0.7071067f, 0.7071067f, 0 );
	if( bufferColors ) {
		auto colorU0 = lerp( mColors[2], mColors[3], currentTexCoord.x / mRectTexCoords.getWidth() );
		auto colorU1 = lerp( mColors[0], mColors[1], currentTexCoord.x / mRectTexCoords.getWidth() );
		colors[tri] = lerp( colorU0, colorU1, currentTexCoord.y / mRectTexCoords.getHeight() );
	}
	
	if( bufferPositions )
		target->copyAttrib( geom::Attrib::POSITION, 2, 0, value_ptr( *positions.data() ), positions.size() );
	if( bufferTexCoords )
		target->copyAttrib( geom::Attrib::TEX_COORD_0, 2, 0, value_ptr( *texCoords.data() ), texCoords.size() );
	if( bufferNormals )
		target->copyAttrib( geom::Attrib::NORMAL, 3, 0, value_ptr( *normals.data() ), normals.size() );
	if( bufferTangents )
		target->copyAttrib( geom::Attrib::TANGENT, 3, 0, value_ptr( *tangents.data() ), tangents.size() );
	if( bufferColors )
		target->copyAttrib( geom::Attrib::COLOR, 4, 0, value_ptr( *colors.data() ), colors.size() );
}
	
void RoundedRect::setDefaultColors()
{
	mColors[0] = ColorAf( 1.0f, 0.0f, 0.0f, 1.0f ); // lower-left
	mColors[1] = ColorAf( 0.0f, 1.0f, 0.0f, 1.0f ); // lower-right
	mColors[2] = ColorAf( 0.0f, 0.0f, 1.0f, 1.0f ); // upper-right
	mColors[3] = ColorAf( 1.0f, 1.0f, 1.0f, 1.0f ); // upper-left
}

void RoundedRect::setDefaultTexCoords()
{
	mRectTexCoords = Rectf( 0.0f, 0.0f, 1.0f, 1.0f );
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
		case Attrib::TANGENT: return 3;
		default:
			return 0;
	}	
}

AttribSet Cube::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::COLOR, Attrib::TANGENT };
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
	if( requestedAttribs.count( Attrib::NORMAL ) || requestedAttribs.count( Attrib::TANGENT ) ) {
		normals.reserve( numVertices );
		normalsPtr = &normals;
	}
	if( requestedAttribs.count( Attrib::COLOR ) > 0 ) {
		colors.reserve( numVertices );
		colorsPtr = &colors;
	}
	if( requestedAttribs.count( Attrib::TEX_COORD_0 ) || requestedAttribs.count( Attrib::TANGENT ) ) {
		texCoords.reserve( numVertices );
		texCoordsPtr = &texCoords;
	}

	vec3 sz = 0.5f * mSize;
	
	// +X
	generateFace( vec3(sz.x,0,0), vec3(0,0,sz.z), vec3(0,sz.y,0), mSubdivisions.z, mSubdivisions.y, &positions,
		normalsPtr, mColors[0], colorsPtr, texCoordsPtr, &indices );
	// +Y
	generateFace( vec3(0,sz.y,0), vec3(sz.x,0,0), vec3(0,0,sz.z), mSubdivisions.x, mSubdivisions.z, &positions,
		normalsPtr, mColors[2], colorsPtr, texCoordsPtr, &indices );
	// +Z
	generateFace( vec3(0,0,sz.z), vec3(0,sz.y,0), vec3(sz.x,0,0), mSubdivisions.y, mSubdivisions.x, &positions,
		normalsPtr, mColors[4], colorsPtr, texCoordsPtr, &indices );
	// -X
	generateFace( vec3(-sz.x,0,0), vec3(0,sz.y,0), vec3(0,0,sz.z), mSubdivisions.y, mSubdivisions.z, &positions,
		normalsPtr, mColors[1], colorsPtr, texCoordsPtr, &indices );
	// -Y
	generateFace( vec3(0,-sz.y,0), vec3(0,0,sz.z), vec3(sz.x,0,0), mSubdivisions.z, mSubdivisions.x, &positions,
		normalsPtr, mColors[3], colorsPtr, texCoordsPtr, &indices );
	// -Z
	generateFace( vec3(0,0,-sz.z), vec3(sz.x,0,0), vec3(0,sz.y,0), mSubdivisions.x, mSubdivisions.y, &positions,
		normalsPtr, mColors[5], colorsPtr, texCoordsPtr, &indices );

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*)positions.data(), numVertices );
	if( normalsPtr )
		target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*)normals.data(), numVertices );
	if( texCoordsPtr )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, (const float*)texCoords.data(), numVertices );
	if( colorsPtr )
		target->copyAttrib( Attrib::COLOR, 4, 0, (const float*)colors.data(), numVertices );

	// generate tangents
	if( requestedAttribs.count( geom::TANGENT ) ) {
		vector<vec3> tangents;
		calculateTangents( getNumIndices(), indices.data(), numVertices, positions.data(), normals.data(), texCoords.data(), &tangents, nullptr );
		
		if( requestedAttribs.count( geom::TANGENT ) )
			target->copyAttrib( Attrib::TANGENT, 3, 0, (const float*)tangents.data(), numVertices );
	}

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

float Icosahedron::sTexCoords[60 * 2] = {
	0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

	1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

	1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,

	1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };

uint32_t Icosahedron::sIndices[60] ={
	0,11, 5, 0, 5, 1, 0, 1, 7, 0, 7,10, 0,10,11,
	5,11, 4, 1, 5, 9, 7, 1, 8,10, 7, 6,11,10, 2,
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

void Icosahedron::calculate( vector<vec3> *positions, vector<vec3> *normals, vector<vec3> *colors, vector<vec2> *texcoords, vector<uint32_t> *indices ) const
{
	positions->reserve( 60 );
	normals->resize( 60 ); // needs to be resize rather than reserve
	colors->reserve( 60 );
	texcoords->reserve( 60 );
	indices->reserve( 60 );

	for( size_t i = 0; i < 60; ++i ) {
		positions->emplace_back( *reinterpret_cast<const vec3*>(&sPositions[sIndices[i]*3]) );
		texcoords->emplace_back( *reinterpret_cast<const vec2*>(&sTexCoords[i*2]) );
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
		case Attrib::TEX_COORD_0: return 2;
		default:
			return 0;
	}
}

AttribSet Icosahedron::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::COLOR, Attrib::TEX_COORD_0 };
}

void Icosahedron::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	vector<vec3> positions, normals, colors;
	vector<vec2> texcoords;
	vector<uint32_t> indices;
	
	calculate( &positions, &normals, &colors, &texcoords, &indices );

	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *positions.data() ), positions.size() );
	target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *normals.data() ), normals.size() );
	target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *colors.data() ), colors.size() );
	target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *texcoords.data() ), texcoords.size() );

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
		mTexCoords[i].x = 0.5f - 0.5f * glm::atan( normal.x, -normal.z ) / float( M_PI );
		mTexCoords[i].y = 1.0f - glm::acos( normal.y ) / float( M_PI );
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
		case Attrib::TANGENT: return 3;
		case Attrib::TEX_COORD_0: return 2;
		case Attrib::COLOR: return mHasColors ? 3 : 0;
		default:
			return 0;
	}
}

AttribSet Icosphere::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::COLOR, Attrib::TANGENT };
}

void Icosphere::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	calculate();

	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *mPositions.data() ), mPositions.size() );
	target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *mNormals.data() ), mNormals.size() );
	target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *mTexCoords.data() ), mTexCoords.size() );
	target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *mColors.data() ), mColors.size() );

	if( requestedAttribs.count( Attrib::TANGENT ) ) {
		vector<vec3> tangents;
		calculateTangents( mIndices.size(), mIndices.data(), mPositions.size(), mPositions.data(), mNormals.data(), mTexCoords.data(), &tangents, nullptr );
		target->copyAttrib( Attrib::TANGENT, 3, 0, value_ptr( *tangents.data() ), tangents.size() );
	}

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
		case Attrib::TANGENT: return 3;
		default:
			return 0;
	}
}

AttribSet Teapot::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::TANGENT };
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

	if( requestedAttribs.count( Attrib::TANGENT ) ) {
		vector<vec3> tangents;
		calculateTangents( indices.size(), indices.data(), positions.size() / 3, (const vec3*)positions.data(), (const vec3*)normals.data(), (const vec2*)texCoords.data(), &tangents, nullptr );
		target->copyAttrib( Attrib::TANGENT, 3, 0, value_ptr( *tangents.data() ), tangents.size() );
	}

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
	std::vector<vec2> positions, texCoords;
	std::vector<vec3> normals;

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
// Ring
Ring::Ring()
	: mRequestedSubdivisions( -1 ), mCenter( 0, 0 ), mRadius( 1.0f ), mWidth( 0.5f )
{
	updateVertexCounts();
}

Ring&	Ring::subdivisions( int subdivs )
{
	mRequestedSubdivisions = subdivs;
	updateVertexCounts();
	return *this;
}

Ring&	Ring::radius( float radius )
{
	mRadius = radius;
	updateVertexCounts();
	return *this;
}

Ring&	Ring::width( float width )
{
	mWidth = width;
	return *this;
}

// If numSegments<0, calculate based on radius
void Ring::updateVertexCounts()
{
	if( mRequestedSubdivisions <= 0 )
		mNumSubdivisions = (int) math<double>::floor( mRadius * float( M_PI * 2 ) );
	else
		mNumSubdivisions = mRequestedSubdivisions;

	if( mNumSubdivisions < 3 ) mNumSubdivisions = 3;
	mNumVertices = ( mNumSubdivisions + 1 ) * 2;
}

size_t Ring::getNumVertices() const
{
	return mNumVertices;
}

uint8_t	Ring::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 2;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 2;
		default:
			return 0;
	}
}

AttribSet Ring::getAvailableAttribs() const
{
	return{ Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0 };
}

void Ring::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	std::vector<vec2> positions, texCoords;
	std::vector<vec3> normals;

	positions.reserve( mNumVertices );
	texCoords.reserve( mNumVertices );
	normals.reserve( mNumVertices );

	float innerRadius = mRadius - 0.5f * mWidth;
	float outerRadius = mRadius + 0.5f * mWidth;

	// iterate the segments
	const float tDelta = 1 / (float) mNumSubdivisions * 2.0f * 3.14159f;
	float t = 0;
	for( int s = 0; s <= mNumSubdivisions; s++ ) {
		vec2 unit( math<float>::cos( t ), math<float>::sin( t ) );
		positions.emplace_back( mCenter + unit * innerRadius );
		positions.emplace_back( mCenter + unit * outerRadius );
		texCoords.emplace_back( vec2( 1, s / (float) mNumSubdivisions ) );
		texCoords.emplace_back( vec2( 0, s / (float) mNumSubdivisions ) );
		normals.emplace_back( 0, 0, 1 );
		normals.emplace_back( 0, 0, 1 );
		t += tDelta;
	}

	target->copyAttrib( Attrib::POSITION, 2, 0, (const float*) positions.data(), mNumVertices );
	target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*) normals.data(), mNumVertices );
	target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, (const float*) texCoords.data(), mNumVertices );
}

///////////////////////////////////////////////////////////////////////////////////////
// Sphere

Sphere::Sphere()
	: mSubdivisions( 18 ), mCenter( 0, 0, 0 ), mRadius( 1.0f ), mHasColors( false )
{
}

Sphere::Sphere( const ci::Sphere &sphere )
	: mSubdivisions( 18 ), mCenter( sphere.getCenter() ), mRadius( sphere.getRadius() ), mHasColors( false )
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
		case Attrib::TANGENT: return 3;
		default:
			return 0;
	}
}

AttribSet Sphere::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::COLOR, Attrib::TANGENT };
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
	for( int r = 0; r < numRings; r++ ) {
		float v = r * ringIncr;
		for( int s = 0; s < numSegments; s++ ) {
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
	for( int r = 0; r < numRings - 1; r++ ) {
		for( int s = 0; s < numSegments - 1 ; s++ ) {
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

	if( requestedAttribs.count( geom::TANGENT ) ) {
		vector<vec3> tangents;
		calculateTangents( indices.size(), indices.data(), positions.size(), positions.data(), normals.data(), texCoords.data(), &tangents, nullptr );
		
		if( requestedAttribs.count( geom::TANGENT ) )
			target->copyAttrib( Attrib::TANGENT, 3, 0, (const float*)tangents.data(), tangents.size() );
	}

	target->copyIndices( Primitive::TRIANGLES, indices.data(), indices.size(), 4 );
}

///////////////////////////////////////////////////////////////////////////////////////
// Capsule
Capsule::Capsule()
	: mDirection( 0, 1, 0 ), mLength( 1.0f ), mSubdivisionsAxis( 12 ), mHasColors( false )
{
	radius( 0.5f );
	subdivisionsHeight( 12 );
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
	if( normals )
		normals->reserve( mNumSegments * ringsTotal );
	if( texCoords )
		texCoords->reserve( mNumSegments * ringsTotal );
	if( colors )
		colors->reserve( mNumSegments * ringsTotal );
	indices->reserve( ( mNumSegments - 1 ) * ( ringsTotal - 1 ) * 6 );

	float bodyIncr = 1.0f / (float)( ringsBody - 1 );
	float ringIncr = 1.0f / (float)( mSubdivisionsHeight - 1 );
	for( int r = 0; r < mSubdivisionsHeight / 2; r++ )
		calculateRing( mNumSegments, math<float>::sin( float(M_PI) * r * ringIncr), math<float>::sin( float(M_PI) * ( r * ringIncr - 0.5f ) ), -0.5f,
							positions, normals, texCoords, colors );
	for( size_t r = 0; r < ringsBody; r++ )
		calculateRing( mNumSegments, 1.0f, 0.0f, r * bodyIncr - 0.5f,
							positions, normals, texCoords, colors );
	for( int r = mSubdivisionsHeight / 2; r < mSubdivisionsHeight; r++ )
		calculateRing( mNumSegments, math<float>::sin( float(M_PI) * r * ringIncr), math<float>::sin( float(M_PI) * ( r * ringIncr - 0.5f ) ), +0.5f,
							positions, normals, texCoords, colors );

	for( size_t r = 0; r < ringsTotal - 1; r++ ) {
		for( int s = 0; s < mNumSegments - 1; s++ ) {
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

		if( normals )
			normals->emplace_back( quaternion * glm::vec3( x, y, z ) );
		// perform cylindrical projection
		if( texCoords ) {
			float u = 1.0f - (s * segIncr);
			float v = 0.5f - ((mRadius * y + mLength * dy) / (2.0f * mRadius + mLength));
			texCoords->emplace_back( u, v );
		}
		
		if( colors ) {
			float g = 0.5f + ((mRadius * y + mLength * dy) / (2.0f * mRadius + mLength));
			colors->emplace_back( x * 0.5f + 0.5f, g, z * 0.5f + 0.5f );
		}
	}
}

size_t Capsule::getNumVertices() const
{
	return mNumSegments * ( mSubdivisionsHeight * 2 + 1 );
}

size_t Capsule::getNumIndices() const
{
	return ( mNumSegments - 1 ) * ( mSubdivisionsHeight + mSubdivisionsHeight ) * 6;
}

uint8_t Capsule::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 2;
		case Attrib::COLOR: return mHasColors ? 3 : 0;
		case Attrib::TANGENT: return 3;
		default:
			return 0;
	}
}

AttribSet Capsule::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::COLOR, Attrib::TANGENT };
}

void Capsule::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	std::vector<vec3> positions, normals;
	std::vector<vec2> texCoords;
	std::vector<vec3> colors;
	std::vector<uint32_t> indices;

	vector<vec3> *normalsPtr = ( requestedAttribs.count( Attrib::NORMAL ) || requestedAttribs.count( Attrib::TANGENT ) ) ? &normals : nullptr;
	vector<vec2> *texCoordsPtr = ( requestedAttribs.count( Attrib::TEX_COORD_0 ) || requestedAttribs.count( Attrib::TANGENT ) ) ? &texCoords : nullptr;
	vector<vec3> *colorsPtr = ( requestedAttribs.count( Attrib::COLOR ) ) ? &colors : nullptr;

	calculate( &positions, normalsPtr, texCoordsPtr, colorsPtr, &indices );

	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *positions.data() ), positions.size() );
	if( normalsPtr )
		target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *normals.data() ), normals.size() );
	if( texCoordsPtr )
		target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *texCoords.data() ), texCoords.size() );
	if( colorsPtr )
		target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *colors.data() ), colors.size() );

	if( requestedAttribs.count( geom::TANGENT ) ) {
		vector<vec3> tangents;
		calculateTangents( indices.size(), indices.data(), positions.size(), positions.data(), normals.data(), texCoords.data(), &tangents, nullptr );
		
		if( requestedAttribs.count( geom::TANGENT ) )
			target->copyAttrib( Attrib::TANGENT, 3, 0, (const float*)tangents.data(), tangents.size() );
	}

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
	if( colors )
		colors->reserve( mNumAxis * mNumRings );
	indices->reserve( (mNumAxis - 1) * (mNumRings - 1) * 6 );

	float majorIncr = 1.0f / (mNumAxis - 1);
	float minorIncr = 1.0f / (mNumRings - 1);
	float radiusDiff = mRadiusMajor - mRadiusMinor;
	float angle = float(M_PI * 2) * mCoils;
	float twist = angle * mTwist * minorIncr * majorIncr;

	// vertex, normal, tex coord and color buffers
	for( int i = 0; i < mNumAxis; ++i ) {
		float phi = i * majorIncr * angle;
		float cosPhi = -math<float>::cos( phi );
		float sinPhi =  math<float>::sin( phi );

		for( int j = 0; j < mNumRings; ++j ) {
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
			if( colors )
				colors->emplace_back( n.x * 0.5f + 0.5f, n.y * 0.5f + 0.5f, n.z * 0.5f + 0.5f );
		}
	}

	// index buffer
	for( int i = 0; i < mNumAxis - 1; ++i ) {
		for ( int j = 0; j < mNumRings - 1; ++j ) {
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
		case Attrib::TANGENT: return 3;
		default:
			return 0;
	}
}

AttribSet Torus::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::COLOR, Attrib::TANGENT };
}

void Torus::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	std::vector<vec3> positions, normals;
	std::vector<vec2> texCoords;
	std::vector<vec3> colors;
	std::vector<uint32_t> indices;

	vector<vec3> *colorsPtr = ( requestedAttribs.count( Attrib::COLOR ) ) ? &colors : nullptr;

	calculate( &positions, &normals, &texCoords, colorsPtr, &indices );

	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *positions.data() ), positions.size() );
	target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *normals.data() ), normals.size() );
	target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *texCoords.data() ), texCoords.size() );
	if( requestedAttribs.count( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *colors.data() ), colors.size() );

	if( requestedAttribs.count( geom::TANGENT ) ) {
		vector<vec3> tangents;
		calculateTangents( indices.size(), indices.data(), positions.size(), positions.data(), normals.data(), texCoords.data(), &tangents, nullptr );
		
		if( requestedAttribs.count( geom::TANGENT ) )
			target->copyAttrib( Attrib::TANGENT, 3, 0, (const float*)tangents.data(), tangents.size() );
	}

	target->copyIndices( Primitive::TRIANGLES, indices.data(), indices.size(), 4 );
}

///////////////////////////////////////////////////////////////////////////////////////
// TorusKnot
TorusKnot::TorusKnot()
	: mP( 2 ), mQ( 3 ), mRadius( 0.2f ), mSubdivisionsAxis( 12 ), mSubdivisionsHeight( 128 ), mScale( 1 ), mHasColors( false )
{
}

size_t TorusKnot::getNumVertices() const
{
	return size_t( ( mSubdivisionsHeight + 1 ) * ( mSubdivisionsAxis + 1 ) );
}

size_t TorusKnot::getNumIndices() const
{
	return mSubdivisionsAxis * mSubdivisionsHeight * 6;
}

uint8_t TorusKnot::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::NORMAL: return 3;
		case Attrib::TEX_COORD_0: return 2;
		case Attrib::COLOR: return mHasColors ? 3 : 0;
		case Attrib::TANGENT: return 3;
		default:
			return 0;
	}
}

AttribSet TorusKnot::getAvailableAttribs() const
{
	return{ Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::COLOR, Attrib::TANGENT };
}

void TorusKnot::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	auto numVertices = getNumVertices();
	auto numIndices = getNumIndices();

	std::vector<vec3> positions( numVertices ), normals( numVertices );
	std::vector<vec2> texCoords( numVertices );
	std::vector<vec3> colors( numVertices );
	std::vector<vec3> tangents( numVertices );
	std::vector<uint32_t> indices( numIndices );

	std::vector<vec3> *colorsPtr = ( requestedAttribs.count( Attrib::COLOR ) ) ? &colors : nullptr;
	std::vector<vec3> *tangentsPtr = ( requestedAttribs.count( Attrib::TANGENT ) ) ? &tangents : nullptr;

	calculate( &positions, &normals, &texCoords, colorsPtr, tangentsPtr, &indices );

	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *positions.data() ), positions.size() );
	target->copyAttrib( Attrib::NORMAL, 3, 0, value_ptr( *normals.data() ), normals.size() );
	target->copyAttrib( Attrib::TEX_COORD_0, 2, 0, value_ptr( *texCoords.data() ), texCoords.size() );

	if( requestedAttribs.count( Attrib::COLOR ) )
		target->copyAttrib( Attrib::COLOR, 3, 0, value_ptr( *colors.data() ), colors.size() );
	if( requestedAttribs.count( geom::TANGENT ) )
		target->copyAttrib( Attrib::TANGENT, 3, 0, value_ptr( *tangents.data() ), tangents.size() );

	target->copyIndices( Primitive::TRIANGLES, indices.data(), indices.size(), 4 );
}

void TorusKnot::calculate( std::vector<vec3> *positions, std::vector<vec3> *normals, std::vector<vec2> *texCoords, std::vector<vec3> *colors, std::vector<vec3> *tangents, std::vector<uint32_t> *indices ) const
{
	float stepHeight = float( 2.0 * M_PI ) / mSubdivisionsHeight;
	float stepAxis = float( 2.0 * M_PI ) / mSubdivisionsAxis;

	// Prevent geometry from penetrating itself.
	int divider = gcd( mP, mQ );
	int _p = ( divider != 0 ) ? mP / divider : 1;
	int _q = ( divider != 0 ) ? mQ / divider : 0;

	for( int i = 0; i <= mSubdivisionsHeight; ++i ) {
		float p = _p * i * stepHeight;
		float q = _q * i * stepHeight;
		float r = 0.5f * ( 2.0f + glm::cos( q ) );
		vec3 center( r * glm::sin( p ) * mScale.x, r * glm::sin( q ) * mScale.y, r * glm::cos( p ) * mScale.z );

		p = _p * ( i + 1 ) * stepHeight;
		q = _q * ( i + 1 ) * stepHeight;
		r = 0.5f * ( 2.0f + glm::cos( q ) );
		vec3 next( r * glm::sin( p ) * mScale.x, r * glm::sin( q ) * mScale.y, r * glm::cos( p ) * mScale.z );

		vec3 T = normalize( next - center );
		vec3 B = normalize( cross( T, next + center ) );
		vec3 N = normalize( cross( B, T ) );

		for( int j = 0; j <= mSubdivisionsAxis; ++j ) {
			float x = glm::cos( j * stepAxis ) * mRadius;
			float y = glm::sin( j * stepAxis ) * mRadius;

			int idx = i * ( mSubdivisionsAxis + 1 ) + j;
			( *normals )[idx] = B * x + N * y;
			( *positions )[idx] = ( *normals )[idx] + center;
			( *normals )[idx] = glm::normalize( ( *normals )[idx] );
			( *texCoords )[idx].y = float( j ) / mSubdivisionsAxis;
			( *texCoords )[idx].x = float( i ) / mSubdivisionsHeight;

			if( tangents )
				( *tangents )[idx] = T; 
			
			if( colors )
				( *colors )[idx] = ( *normals )[idx] * 0.5f + 0.5f;
		}
	}

	int nAxis = mSubdivisionsAxis + 1;
	for( int j = 0; j < mSubdivisionsAxis; j++ ) {
		for( int i = 0; i < mSubdivisionsHeight; i++ ) {
			int idx = 6 * ( j * mSubdivisionsHeight + i );
			( *indices )[idx + 0] = ( j + i * nAxis );
			( *indices )[idx + 1] = ( j + ( i + 1 ) * nAxis );
			( *indices )[idx + 2] = ( ( j + 1 ) + i * nAxis );
			( *indices )[idx + 3] = ( ( j + 1 ) + i * nAxis );
			( *indices )[idx + 4] = ( j + ( i + 1 ) * nAxis );
			( *indices )[idx + 5] = ( ( j + 1 ) + ( i + 1 ) * nAxis );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Cylinder
Cylinder::Cylinder()
	: mOrigin( 0, 0, 0 ), mHeight( 2.0f ), mDirection( 0, 1, 0 ), mRadiusBase( 1.0f ), mRadiusApex( 1.0f ),
		mSubdivisionsAxis( 18 ), mSubdivisionsHeight( 1 ), mSubdivisionsCap( 3 ), mHasColors( false )
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
		result += mNumSegments * mSubdivisionsCap * 2;
	if( mRadiusApex > 0.0f )
		result += mNumSegments * mSubdivisionsCap * 2;
	return result;
}

size_t Cylinder::getNumIndices() const
{
	size_t result = ( mNumSegments - 1 ) * ( mNumSlices - 1 ) * 6;
	if( mRadiusBase > 0 )
		result += 6 * ( mNumSegments - 1 ) * mSubdivisionsCap;
	if( mRadiusApex > 0 )
		result += 6 * ( mNumSegments - 1 ) * mSubdivisionsCap;
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
	const quat axis = glm::rotation( vec3( 0, 1, 0 ), mDirection );

	// vertex, normal, tex coord and color buffers
	for( int i = 0; i < mNumSegments; ++i ) {
		for( int j = 0; j < mNumSlices; ++j ) {
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
	for ( int j = 0; j < mNumSlices - 1; ++j ) {
		for( int i = 0; i < mNumSegments - 1; ++i ) {
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
	normals->resize( index + mSubdivisionsCap * mNumSegments * 2, n );
	colors->resize( index + mSubdivisionsCap *mNumSegments * 2, vec3( n.x * 0.5f + 0.5f, n.y * 0.5f + 0.5f, n.z * 0.5f + 0.5f ) );

	const quat axis = glm::rotation( vec3( 0, 1, 0 ), mDirection );

	// vertices
	const float segmentIncr = 1.0f / ( mNumSegments - 1 );
	for( int r = 0; r < mSubdivisionsCap; ++r ) {
		for( int i = 0; i < mNumSegments; ++i ) {
			float cosPhi = -math<float>::cos( i * segmentIncr * float( M_PI * 2 ) );
			float sinPhi = math<float>::sin( i * segmentIncr * float( M_PI * 2 ) );

			// inner point
			float x = ( radius * cosPhi * float( r ) ) / mSubdivisionsCap;
			float y = height;
			float z = ( radius * sinPhi * float( r ) ) / mSubdivisionsCap;

			positions->emplace_back( mOrigin + axis * vec3( x, y, z ) );
			texCoords->emplace_back( i * segmentIncr, 1.0f - height / mHeight );

			// outer point
			x = ( radius * cosPhi * float( r + 1 ) ) / mSubdivisionsCap;
			y = height;
			z = ( radius * sinPhi * float( r + 1 ) ) / mSubdivisionsCap;

			positions->emplace_back( mOrigin + axis * vec3( x, y, z ) );
			texCoords->emplace_back( i * segmentIncr, 1.0f - height / mHeight );
		}
	}

	// index buffer
	indices->reserve( indices->size() + 6 * mNumSegments * mSubdivisionsCap );

	for( int r = 0; r < mSubdivisionsCap; ++r ) {
		for( int i = 0; i < ( mNumSegments - 1 ); ++i ) {
			if( flip ) {
				indices->push_back( (uint32_t)( index + r * mNumSegments * 2 + i * 2 + 0 ) );
				indices->push_back( (uint32_t)( index + r * mNumSegments * 2 + i * 2 + 2 ) );
				indices->push_back( (uint32_t)( index + r * mNumSegments * 2 + i * 2 + 3 ) );

				indices->push_back( (uint32_t)( index + r * mNumSegments * 2 + i * 2 + 0 ) );
				indices->push_back( (uint32_t)( index + r * mNumSegments * 2 + i * 2 + 3 ) );
				indices->push_back( (uint32_t)( index + r * mNumSegments * 2 + i * 2 + 1 ) );
			}
			else {
				indices->push_back( (uint32_t)( index + r * mNumSegments * 2 + i * 2 + 0 ) );
				indices->push_back( (uint32_t)( index + r * mNumSegments * 2 + i * 2 + 3 ) );
				indices->push_back( (uint32_t)( index + r * mNumSegments * 2 + i * 2 + 2 ) );

				indices->push_back( (uint32_t)( index + r * mNumSegments * 2 + i * 2 + 0 ) );
				indices->push_back( (uint32_t)( index + r * mNumSegments * 2 + i * 2 + 1 ) );
				indices->push_back( (uint32_t)( index + r * mNumSegments * 2 + i * 2 + 3 ) );
			}
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
		case Attrib::TANGENT: return 3;
		default:
			return 0;
	}
}

AttribSet Cylinder::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::COLOR, Attrib::TANGENT };
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

	if( requestedAttribs.count( geom::TANGENT ) ) {
		vector<vec3> tangents;
		calculateTangents( indices.size(), indices.data(), positions.size(), positions.data(), normals.data(), texCoords.data(), &tangents, nullptr );
		
		if( requestedAttribs.count( geom::TANGENT ) )
			target->copyAttrib( Attrib::TANGENT, 3, 0, (const float*)tangents.data(), tangents.size() );
	}

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
		quat normalQuat = glm::rotation( vec3( 0, 1, 0 ), normalNormal );
		mAxisU = normalQuat * vec3( 1, 0, 0 );
		mAxisV = normalQuat * vec3( 0, 0, 1 );
	}
	else {
		quat normalQuat = glm::rotation( vec3( 0, 0, 1 ), normalNormal );
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
		case Attrib::TANGENT: return 3;
		default:
			return 0;
	}
}

AttribSet Plane::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::TANGENT };
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

	// generate tangents
	if( requestedAttribs.count( geom::TANGENT ) ) {
		vector<vec3> tangents;
		calculateTangents( getNumIndices(), indices.data(), positions.size(), positions.data(), normals.data(), texCoords.data(), &tangents, nullptr );
		
		if( requestedAttribs.count( geom::TANGENT ) )
			target->copyAttrib( Attrib::TANGENT, 3, 0, (const float*)tangents.data(), tangents.size() );
	}
	
	target->copyIndices( Primitive::TRIANGLES, indices.data(), indices.size(), 4 );
}

///////////////////////////////////////////////////////////////////////////////////////
// Transform
uint8_t	Transform::getAttribDims( Attrib attr, uint8_t upstreamDims ) const
{
	if( attr == Attrib::POSITION )
		return 3;
	else
		return upstreamDims;
}

void Transform::process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const
{
	ctx->processUpstream( requestedAttribs );
	
	const size_t numVertices = ctx->getNumVertices();

	if( ctx->getAttribDims( POSITION ) == 2 ) {
		const vec2* inPositions = reinterpret_cast<vec2*>( ctx->getAttribData( POSITION ) );
		vector<vec3> outPositions;
		outPositions.reserve( numVertices );
		for( size_t v = 0; v < numVertices; ++v )
			outPositions.push_back( vec3( mTransform * vec4( inPositions[v], 0, 1 ) ) );
		ctx->copyAttrib( POSITION, 3, 0, (const float*)outPositions.data(), numVertices );
	}
	else if( ctx->getAttribDims( POSITION ) == 3 ) {
		vec3* positions = reinterpret_cast<vec3*>( ctx->getAttribData( POSITION ) );
		for( size_t v = 0; v < numVertices; ++v )
			positions[v] = vec3( mTransform * vec4( positions[v], 1 ) );
	}
	else if( ctx->getAttribDims( POSITION ) == 4 ) {
		vec4* positions = reinterpret_cast<vec4*>( ctx->getAttribData( POSITION ) );
		for( size_t v = 0; v < numVertices; ++v )
			positions[v] = mTransform * positions[v];
	}
	else if( ctx->getAttribDims( POSITION ) != 0 )
		CI_LOG_W( "Unsupported dimension for geom::POSITION passed to geom::Transform" );
	
	// we'll make the sort of modification to our normals (if they're present)
	// using the inverse transpose of 'mTransform'
	if( ctx->getAttribDims( NORMAL ) == 3 ) {
		vec3* normals = reinterpret_cast<vec3*>( ctx->getAttribData( NORMAL ) );
		mat3 normalsTransform = glm::transpose( inverse( mat3( mTransform ) ) );
		for( size_t v = 0; v < numVertices; ++v )
			normals[v] = normalize( normalsTransform * normals[v] );
	}
	else if( ctx->getAttribDims( NORMAL ) != 0 )
		CI_LOG_W( "Unsupported dimension for geom::NORMAL passed to geom::Transform" );

	// and finally, we'll make the sort of modification to our tangents (if they're present)
	// using the inverse transpose of 'mTransform'
	if( ctx->getAttribDims( TANGENT ) == 3 ) {
		vec3* tangents = reinterpret_cast<vec3*>( ctx->getAttribData( TANGENT ) );
		mat3 tangentsTransform = glm::transpose( inverse( mat3( mTransform ) ) );
		for( size_t v = 0; v < numVertices; ++v )
			tangents[v] = normalize( tangentsTransform * tangents[v] );
	}
	else if( ctx->getAttribDims( TANGENT ) != 0 )
		CI_LOG_W( "Unsupported dimension for geom::TANGENT passed to geom::Transform" );
}

///////////////////////////////////////////////////////////////////////////////////////
// Twist
void Twist::process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const
{
	ctx->processUpstream( requestedAttribs );
	
	const size_t numVertices = ctx->getNumVertices();
	const float invAxisLength = 1.0f / distance( mAxisStart, mAxisEnd );
	const vec3 axisDir = ( mAxisEnd - mAxisStart ) * vec3( invAxisLength );

	if( ctx->getAttribDims( POSITION ) == 3 ) {
		vec3* positions = reinterpret_cast<vec3*>( ctx->getAttribData( POSITION ) );
		vec3* normals = nullptr, *tangents = nullptr;
		if( ctx->getAttribDims( NORMAL ) == 3 )
			normals = reinterpret_cast<vec3*>( ctx->getAttribData( NORMAL ) );
		if( ctx->getAttribDims( TANGENT ) == 3 )
			tangents = reinterpret_cast<vec3*>( ctx->getAttribData( TANGENT ) );
		
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
			if( normals )
				normals[v] = vec3( rotation * vec4( normals[v], 0 ) );
			// we need to transform the tangent by rotating it by the same angle (but not around the point) we did the position
			if( tangents )
				tangents[v] = vec3( rotation * vec4( tangents[v], 0 ) );
		}
	}
	else if( ctx->getAttribDims( POSITION ) != 0 )
		CI_LOG_W( "Unsupported dimension for geom::POSITION passed to geom::Twist" );
}

///////////////////////////////////////////////////////////////////////////////////////
// Lines
size_t Lines::getNumIndices( const Modifier::Params &upstreamParams ) const
{
	return calcNumIndices( upstreamParams.getPrimitive(), upstreamParams.getNumIndices(), upstreamParams.getNumVertices() );
}

size_t Lines::calcNumIndices( Primitive primitive, size_t upstreamNumIndices, size_t upstreamNumVertices )
{
	switch( primitive ) {
		case LINES:
			return upstreamNumIndices;
		break;
		case LINE_STRIP:
			return upstreamNumIndices ? ((upstreamNumIndices - 1) * 2 ) : ((upstreamNumVertices - 1) * 2 );
		break;
		case TRIANGLES:
			return upstreamNumIndices ? (upstreamNumIndices * 2) : (upstreamNumVertices * 2);
		break;
		case TRIANGLE_STRIP:
			return std::max<int>( 0, upstreamNumIndices ? (int)((upstreamNumVertices - 2) * 6 )
				: (int)(upstreamNumIndices - 2) * 6 );
		break;
		case TRIANGLE_FAN:
			return std::max<int>( 0, upstreamNumIndices ? (int)(upstreamNumVertices * 4 - 2 )
				: (int)(upstreamNumIndices * 4 - 2 ) );
		break;
		default:
			return upstreamNumIndices;
	}
}

void Lines::process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const
{
	ctx->processUpstream( requestedAttribs );
	
	const size_t numInIndices = ctx->getNumIndices();
	const size_t numInVertices = ctx->getNumVertices();
	const size_t numOutIndices = calcNumIndices( ctx->getPrimitive(), numInIndices, numInVertices );

	if( numOutIndices < 2 ) // early exit
		return;

	switch( ctx->getPrimitive() ) {
		case Primitive::LINES: // pass-through
		break;
		case Primitive::LINE_STRIP: {
			vector<uint32_t> outIndices;
			outIndices.reserve( numOutIndices );
			const uint32_t *indices = ctx->getIndicesData();
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
			
			ctx->copyIndices( geom::LINES, outIndices.data(), outIndices.size(), 4 );
		}
		break;
		case Primitive::TRIANGLE_FAN: {
			vector<uint32_t> outIndices;
			outIndices.reserve( numOutIndices );
			const uint32_t *indices = ctx->getIndicesData();
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
			
			ctx->copyIndices( geom::LINES, outIndices.data(), outIndices.size(), 4 );
		}
		break;
		case Primitive::TRIANGLES: {
			vector<uint32_t> outIndices;
			outIndices.reserve( numOutIndices );
			const uint32_t *indices = ctx->getIndicesData();
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
			
			ctx->copyIndices( geom::LINES, outIndices.data(), outIndices.size(), 4 );
		}
		break;
		case Primitive::TRIANGLE_STRIP: {
			vector<uint32_t> outIndices;
			outIndices.reserve( numOutIndices );
			const uint32_t *indices = ctx->getIndicesData();
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
			
			ctx->copyIndices( geom::LINES, outIndices.data(), outIndices.size(), 4 );
		}
		break;
		default:
			CI_LOG_E( "geom::Lines unsupported primitive: " << primitiveToString( ctx->getPrimitive() ) );
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// ColorFromAttrib
uint8_t ColorFromAttrib::getAttribDims( Attrib attr, uint8_t upstreamDims ) const
{
	switch( attr ) {
		case Attrib::COLOR: return 3;
		default:
			return upstreamDims;
	}
}

AttribSet ColorFromAttrib::getAvailableAttribs( const Modifier::Params &upstreamParams ) const
{
	AttribSet result = upstreamParams.getAvailableAttribs();
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

void ColorFromAttrib::process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const
{
	AttribSet request = requestedAttribs;
	request.insert( mAttrib );
	ctx->processUpstream( request );

	// if we have no function to apply just continue
	if( ( ! mFnColor2 ) && ( ! mFnColor3 ) )
		return;

	if( ctx->getAttribDims( mAttrib ) == 0 ) {
		CI_LOG_W( "ColorFromAttrib called on geom::Source missing requested " << attribToString( mAttrib ) );
		return;
	}

	const auto numVertices = ctx->getNumVertices();
	unique_ptr<float[]> mColorData( new float[numVertices * 3] );
	uint8_t inputAttribDims = ctx->getAttribDims( mAttrib );
	const float* inputAttribData = ctx->getAttribData( mAttrib );
	
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

	ctx->copyAttrib( Attrib::COLOR, 3, 0, mColorData.get(), numVertices );
}

///////////////////////////////////////////////////////////////////////////////////////
// Color
uint8_t Constant::getAttribDims( Attrib attr, uint8_t upstreamDims ) const
{
	if( attr == mAttrib )
		return mDims;

	return upstreamDims;
}

AttribSet Constant::getAvailableAttribs( const Modifier::Params &upstreamParams ) const
{
	AttribSet result = upstreamParams.getAvailableAttribs();
	result.insert( mAttrib );
	return result;
}

void Constant::process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const
{
	ctx->processUpstream( requestedAttribs );

	const size_t numVertices = ctx->getNumVertices();
	
	switch( mDims ) {
		case 1: {
			unique_ptr<float[]> mColorData( new float[numVertices] );
			for( size_t v = 0; v < numVertices; ++v )
				mColorData[v] = mValue.x;
			ctx->copyAttrib( mAttrib, 1, 0, (const float*)mColorData.get(), numVertices );
		}
		break;
		case 2: {
			unique_ptr<vec2[]> mColorData( new vec2[numVertices] );
			for( size_t v = 0; v < numVertices; ++v )
				mColorData[v] = vec2( mValue.x, mValue.y );
			ctx->copyAttrib( mAttrib, 2, 0, (const float*)mColorData.get(), numVertices );
		}
		break;
		case 3: {
			unique_ptr<vec3[]> mColorData( new vec3[numVertices] );
			for( size_t v = 0; v < numVertices; ++v )
				mColorData[v] = vec3( mValue.x, mValue.y, mValue.z );
			ctx->copyAttrib( mAttrib, 3, 0, (const float*)mColorData.get(), numVertices );
		}
		break;
		case 4: {
			unique_ptr<vec4[]> mColorData( new vec4[numVertices] );
			for( size_t v = 0; v < numVertices; ++v )
				mColorData[v] = mValue;
			ctx->copyAttrib( mAttrib, 4, 0, (const float*)mColorData.get(), numVertices );
		}
		break;
		default:
			CI_LOG_E( "Illegal dimensions." );
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// AttribFn
template<typename S, typename D>
uint8_t AttribFn<S,D>::getAttribDims( Attrib attr, uint8_t upstreamDims ) const
{
	if( attr == mDstAttrib )
		return DSTDIM;
	else
		return upstreamDims;
}

template<typename S, typename D>
AttribSet AttribFn<S,D>::getAvailableAttribs( const Modifier::Params &upstreamParams ) const
{
	AttribSet result = upstreamParams.getAvailableAttribs();
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
void geom::AttribFn<S,D>::process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const
{
	AttribSet request = requestedAttribs;
	request.insert( mSrcAttrib );
	ctx->processUpstream( request );

	if( ctx->getAttribDims( mSrcAttrib ) == 0 ) {
		CI_LOG_W( "AttribFn called on geom::Source missing requested " << attribToString( mSrcAttrib ) );
		return;
	}

	const auto numVertices = ctx->getNumVertices();
	std::unique_ptr<float[]> outData( new float[numVertices * DSTDIM] );
	std::unique_ptr<float[]> tempInData;
	const float *inputAttribData;
	const uint8_t inputAttribDims = ctx->getAttribDims( mSrcAttrib );
	// if the actual input dims of the attribute don't equal SRCDIMS, we'll need to temporarily copy it to a buffer
	if( inputAttribDims != SRCDIM ) {
		CI_LOG_W( "AttribFn source dimensions don't match for attrib " << attribToString( mSrcAttrib ) );
		tempInData = std::unique_ptr<float[]>( new float[numVertices * SRCDIM] );
		auto tempDataWrongDims = ctx->getAttribData( mSrcAttrib );
		geom::copyData( inputAttribDims, tempDataWrongDims, numVertices, SRCDIM, 0, tempInData.get() );
		inputAttribData = tempInData.get();
	}
	else
		inputAttribData = ctx->getAttribData( mSrcAttrib );
	
	processAttrib<S,D>( inputAttribData, outData.get(), mFn, numVertices );
	ctx->copyAttrib( mDstAttrib, DSTDIM, 0, outData.get(), numVertices );
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
		case Attrib::TANGENT: return 3;
		default:
			return 0;
	}
}

AttribSet Extrude::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::TANGENT };
}

void Extrude::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	vector<vec3> positions, normals, texCoords;
	vector<uint32_t> indices;

	calculate( &positions, &normals, &texCoords, &indices );

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*)positions.data(), positions.size() );
	target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*)normals.data(), normals.size() );
	target->copyAttrib( Attrib::TEX_COORD_0, 3, 0, (const float*)texCoords.data(), texCoords.size() );

	// generate tangents
	if( requestedAttribs.count( geom::TANGENT ) ) {
		vector<vec3> tangents;
		calculateTangents( getNumIndices(), indices.data(), positions.size(), positions.data(), normals.data(), texCoords.data(), &tangents, nullptr );
		
		if( requestedAttribs.count( geom::TANGENT ) )
			target->copyAttrib( Attrib::TANGENT, 3, 0, (const float*)tangents.data(), tangents.size() );
	}

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
		case Attrib::TANGENT: return 3;
		default:
			return 0;
	}
}

AttribSet ExtrudeSpline::getAvailableAttribs() const
{
	return { Attrib::POSITION, Attrib::NORMAL, Attrib::TEX_COORD_0, Attrib::TANGENT };
}

void ExtrudeSpline::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	vector<vec3> positions, normals, texCoords;
	vector<uint32_t> indices;

	calculate( &positions, &normals, &texCoords, &indices );

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*)positions.data(), positions.size() );
	target->copyAttrib( Attrib::NORMAL, 3, 0, (const float*)normals.data(), normals.size() );
	target->copyAttrib( Attrib::TEX_COORD_0, 3, 0, (const float*)texCoords.data(), texCoords.size() );

	// generate tangents
	if( requestedAttribs.count( geom::TANGENT ) ) {
		vector<vec3> tangents;
		calculateTangents( getNumIndices(), indices.data(), positions.size(), positions.data(), normals.data(), texCoords.data(), &tangents, nullptr );
		
		if( requestedAttribs.count( geom::TANGENT ) )
			target->copyAttrib( Attrib::TANGENT, 3, 0, (const float*)tangents.data(), tangents.size() );
	}

	target->copyIndices( Primitive::TRIANGLES, indices.data(), indices.size(), calcIndicesRequiredBytes( indices.size() ) );
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
template BSpline::BSpline( const ci::BSpline<3, float>&, int );

///////////////////////////////////////////////////////////////////////////////////////
// WireCapsule
WireCapsule::WireCapsule()
	: mDirection( 0, 1, 0 ), mLength( 1.0f ), mSubdivisionsAxis( 6 ), mNumSegments( 72 ), mSubdivisionsHeight( 1 ), mRadius( 0.5f )
{
}

WireCapsule& WireCapsule::set( const vec3 &from, const vec3 &to )
{
	const vec3 axis = to - from;
	mLength = glm::length( axis );
	mDirection = normalize( axis );
	mCenter = from + 0.5f * axis;
	return *this;
}

void WireCapsule::calculate( vector<vec3> *positions ) const
{
	positions->reserve( getNumVertices() );

	float bodyIncr = 1.0f / (float) ( mSubdivisionsHeight );
	for( int r = 1; r < mSubdivisionsHeight; ++r ) {
		float t = r * bodyIncr - 0.5f;
		float h = ( mLength + 2 * mRadius ) * t;
		float radius = mRadius;
		if( math<float>::abs( h ) > 0.5f * mLength ) {
			float y = math<float>::abs( h ) - 0.5f * mLength;
			radius = y / math<float>::tan( math<float>::asin( y / mRadius ) );
		}
		calculateRing( radius, t, positions );
	}

	const quat quaternion = glm::rotation( vec3( 0, 1, 0 ), mDirection );

	int subdivisionsAxis = mSubdivisionsAxis > 1 ? mSubdivisionsAxis : 0;
	float axisIncr = 1.0f / (float) ( mSubdivisionsAxis );
	int numSegments = mNumSegments / 2;
	float segIncr = 1.0f / (float) ( numSegments );
	for( int i = 0; i < subdivisionsAxis; ++i ) {
		// Straight lines.
		float a = float( M_PI * 2 ) * i * axisIncr;
		float x = math<float>::cos( a );
		float z = math<float>::sin( a );
		positions->emplace_back( mCenter + quaternion * vec3( x * mRadius, -0.5f * mLength, z * mRadius ) );
		positions->emplace_back( mCenter + quaternion * vec3( x * mRadius, +0.5f * mLength, z * mRadius ) );

		// Caps.
		for( int j = 0; j < numSegments; ++j ) {
			float a1 = float( M_PI / 2 ) * j * segIncr;
			float a2 = float( M_PI / 2 ) * ( j + 1 ) * segIncr;
			float r1 = math<float>::cos( a1 ) * mRadius;
			float r2 = math<float>::cos( a2 ) * mRadius;
			float y1 = math<float>::sin( a1 ) * mRadius;
			float y2 = math<float>::sin( a2 ) * mRadius;
			positions->emplace_back( mCenter + quaternion * vec3( x * r1, -0.5f * mLength - y1, z * r1 ) );
			positions->emplace_back( mCenter + quaternion * vec3( x * r2, -0.5f * mLength - y2, z * r2 ) );
			positions->emplace_back( mCenter + quaternion * vec3( x * r1, +0.5f * mLength + y1, z * r1 ) );
			positions->emplace_back( mCenter + quaternion * vec3( x * r2, +0.5f * mLength + y2, z * r2 ) );
		}
	}
}

void WireCapsule::calculateRing( float radius, float d, vector<vec3> *positions ) const
{
	const quat quaternion = glm::rotation( vec3( 0, 1, 0 ), mDirection );

	float length = mLength + 2 * mRadius;
	float segIncr = 1.0f / (float) ( mNumSegments );
	positions->emplace_back( mCenter + ( quaternion * glm::vec3( radius, d * length, 0 ) ) );
	for( int s = 1; s < mNumSegments; s++ ) {
		float a = float( M_PI * 2 ) * s * segIncr;
		float x = math<float>::cos( a ) * radius;
		float z = math<float>::sin( a ) * radius;

		vec3 p = mCenter + ( quaternion * glm::vec3( x, d * length, z ) );
		positions->emplace_back( p );
		positions->emplace_back( p );
	}
	positions->emplace_back( mCenter + ( quaternion * glm::vec3( radius, d * length, 0 ) ) );
}

size_t WireCapsule::getNumVertices() const
{
	int numSegments = mNumSegments / 2;
	int subdivisionsAxis = mSubdivisionsAxis > 1 ? mSubdivisionsAxis : 0;
	return ( mNumSegments * ( mSubdivisionsHeight - 1 ) + subdivisionsAxis * ( 1 + 2 * numSegments ) ) * 2;
}

void WireCapsule::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	std::vector<vec3> positions;

	calculate( &positions );

	target->copyAttrib( Attrib::POSITION, 3, 0, value_ptr( *positions.data() ), positions.size() );
}


///////////////////////////////////////////////////////////////////////////////////////
// WireCircle
WireCircle::WireCircle()
	: mRadius( 1 ), mNumSegments( 12 )
{
}

size_t WireCircle::getNumVertices() const
{
	return mNumSegments + 1;
}

void WireCircle::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	size_t numVertices = getNumVertices();

	std::vector<vec3> positions;
	positions.resize( numVertices );

	vec3 *ptr = positions.data();

	float angle = float( 2.0 * M_PI / mNumSegments );

	for( int i = 0; i < mNumSegments; ++i ) {
		vec3 v = mCenter + mRadius * vec3( glm::cos( i * angle ), glm::sin( i * angle ), 0 );
		*ptr++ = v;
	}
	*ptr++ = mCenter + mRadius * vec3( 1, 0, 0 );

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*) positions.data(), numVertices );
}
	
///////////////////////////////////////////////////////////////////////////////////////
// WireRoundedRect
WireRoundedRect::WireRoundedRect()
	: mCornerSubdivisions( -1 ), mCornerRadius( 0.1f ), mNumVertices( 0 )
{
	rect( Rectf( -0.5f, -0.5f, 0.5f, 0.5f ) );
	updateVertexCount();
}
	
WireRoundedRect::WireRoundedRect( const Rectf &r, float cornerRadius )
	: mCornerSubdivisions( -1 ), mCornerRadius( cornerRadius ), mNumVertices( 0 )
{
	rect( r );
	updateVertexCount();
}
	
WireRoundedRect& WireRoundedRect::cornerSubdivisions( int cornerSubdivisions )
{
	mCornerSubdivisions = cornerSubdivisions;
	updateVertexCount();
	return *this;
}

WireRoundedRect& WireRoundedRect::cornerRadius( float cornerRadius )
{
	mCornerRadius = cornerRadius;
	updateVertexCount();
	return *this;
}

void WireRoundedRect::updateVertexCount()
{
	if( mCornerSubdivisions <= 0 ) {
		mCornerSubdivisions = (int)math<double>::floor( mCornerRadius * M_PI * 2 / 4 );
	}
	if( mCornerSubdivisions < 2 ) mCornerSubdivisions = 2;
	
	mNumVertices = (2 * ( mCornerSubdivisions + 1 ) * 4) + 1;
}
	
void WireRoundedRect::loadInto( cinder::geom::Target *target, const AttribSet &requestedAttribs ) const
{
	float cornerRadius = glm::max( 0.0f, glm::min( mCornerRadius, 0.5f * glm::min( mRectPositions.getWidth(), mRectPositions.getHeight() ) ) );

	std::vector<vec2> verts( mNumVertices );
	vec2* ptr = verts.data();
	const float angleDelta = (float)(1 / (float)mCornerSubdivisions * M_PI / 2);
	const std::array<vec2, 4> cornerCenterVerts = {
		vec2( mRectPositions.x2 - cornerRadius, mRectPositions.y2 - cornerRadius ),
		vec2( mRectPositions.x1 + cornerRadius, mRectPositions.y2 - cornerRadius ),
		vec2( mRectPositions.x1 + cornerRadius, mRectPositions.y1 + cornerRadius ),
		vec2( mRectPositions.x2 - cornerRadius, mRectPositions.y1 + cornerRadius )
	};
	// provide the last vert as the starting point to loop around to.
	*ptr++ = vec2( cornerCenterVerts[3].x + math<float>::cos( ( 3 * (float)M_PI / 2 ) + ( angleDelta * mCornerSubdivisions ) ) * mCornerRadius,
				 cornerCenterVerts[3].y + math<float>::sin( ( 3 * (float)M_PI / 2 ) + ( angleDelta * mCornerSubdivisions ) ) * mCornerRadius );
	for( size_t corner = 0; corner < 4; ++corner ) {
		float angle = corner * (float)M_PI / 2;
		vec2 cornerCenter( cornerCenterVerts[corner] );
		for( int s = 0; s <= mCornerSubdivisions; s++ ) {
			// This is the ending point of the first line
			*ptr++ = vec2( cornerCenter.x + math<float>::cos( angle ) * cornerRadius,
						   cornerCenter.y + math<float>::sin( angle ) * cornerRadius );
			 // This is the starting point of the next line
			*ptr++ = vec2( cornerCenter.x + math<float>::cos( angle ) * cornerRadius,
						   cornerCenter.y + math<float>::sin( angle ) * cornerRadius );
			angle += angleDelta;
		}
	}
	
	target->copyAttrib( geom::Attrib::POSITION, 2, 0, value_ptr( *verts.data() ), verts.size() );
}

///////////////////////////////////////////////////////////////////////////////////////
// WireCube
void WireCube::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	size_t numVertices = getNumVertices();

	std::vector<vec3> positions;
	positions.resize( numVertices );

	vec3 d = -0.5f * mSize;
	vec3 s = mSize / vec3( mSubdivisions );

	vec3 *ptr = positions.data();
	*ptr++ = vec3( -d.x, d.y, d.z );
	*ptr++ = vec3( d.x, d.y, d.z );
	*ptr++ = vec3( -d.x, -d.y, d.z );
	*ptr++ = vec3( d.x, -d.y, d.z );
	*ptr++ = vec3( -d.x, d.y, -d.z );
	*ptr++ = vec3( d.x, d.y, -d.z );
	*ptr++ = vec3( -d.x, -d.y, -d.z );
	*ptr++ = vec3( d.x, -d.y, -d.z );

	*ptr++ = vec3( d.x, -d.y, d.z );
	*ptr++ = vec3( d.x, d.y, d.z );
	*ptr++ = vec3( -d.x, -d.y, d.z );
	*ptr++ = vec3( -d.x, d.y, d.z );
	*ptr++ = vec3( d.x, -d.y, -d.z );
	*ptr++ = vec3( d.x, d.y, -d.z );
	*ptr++ = vec3( -d.x, -d.y, -d.z );
	*ptr++ = vec3( -d.x, d.y, -d.z );

	*ptr++ = vec3( d.x, d.y, -d.z );
	*ptr++ = vec3( d.x, d.y, d.z );
	*ptr++ = vec3( -d.x, d.y, -d.z );
	*ptr++ = vec3( -d.x, d.y, d.z );
	*ptr++ = vec3( d.x, -d.y, -d.z );
	*ptr++ = vec3( d.x, -d.y, d.z );
	*ptr++ = vec3( -d.x, -d.y, -d.z );
	*ptr++ = vec3( -d.x, -d.y, d.z );

	for( int i = 1; i < mSubdivisions.x; ++i ) {
		float x = d.x + i * s.x;
		*ptr++ = vec3( x, d.y, d.z );
		*ptr++ = vec3( x, -d.y, d.z );
		*ptr++ = vec3( x, -d.y, d.z );
		*ptr++ = vec3( x, -d.y, -d.z );
		*ptr++ = vec3( x, -d.y, -d.z );
		*ptr++ = vec3( x, d.y, -d.z );
		*ptr++ = vec3( x, d.y, -d.z );
		*ptr++ = vec3( x, d.y, d.z );
	}

	for( int i = 1; i < mSubdivisions.y; ++i ) {
		float y = d.y + i * s.y;
		*ptr++ = vec3( d.x, y, d.z );
		*ptr++ = vec3( -d.x, y, d.z );
		*ptr++ = vec3( -d.x, y, d.z );
		*ptr++ = vec3( -d.x, y, -d.z );
		*ptr++ = vec3( -d.x, y, -d.z );
		*ptr++ = vec3( d.x, y, -d.z );
		*ptr++ = vec3( d.x, y, -d.z );
		*ptr++ = vec3( d.x, y, d.z );
	}

	for( int i = 1; i < mSubdivisions.z; ++i ) {
		float z = d.z + i * s.z;
		*ptr++ = vec3( d.x, d.y, z );
		*ptr++ = vec3( -d.x, d.y, z );
		*ptr++ = vec3( -d.x, d.y, z );
		*ptr++ = vec3( -d.x, -d.y, z );
		*ptr++ = vec3( -d.x, -d.y, z );
		*ptr++ = vec3( d.x, -d.y, z );
		*ptr++ = vec3( d.x, -d.y, z );
		*ptr++ = vec3( d.x, d.y, z );
	}

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*) positions.data(), numVertices );
}


///////////////////////////////////////////////////////////////////////////////////////
// WireCylinder
size_t WireCylinder::getNumVertices() const
{
	int subdivisionAxis = ( mSubdivisionsAxis > 1 ) ? mSubdivisionsAxis : 0;
	int subdivisionHeight = mSubdivisionsHeight + 1;

	if( mRadiusApex <= 0.0f && mRadiusBase <= 0.0f )
		subdivisionHeight = 0;
	else if( mRadiusApex <= 0.0f || mRadiusBase <= 0.0f )
		subdivisionHeight--;

	return ( subdivisionAxis + subdivisionHeight * mNumSegments ) * 2;
}

void WireCylinder::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	size_t numVertices = getNumVertices();

	std::vector<vec3> positions;
	positions.resize( numVertices );

	vec3 *ptr = positions.data();

	glm::mat3 m = glm::toMat3( glm::rotation( vec3( 0, 1, 0 ), mDirection ) );

	if ( mSubdivisionsAxis > 1 ) {
		float angle = float( 2.0 * M_PI / mSubdivisionsAxis );
		for( int i = 0; i < mSubdivisionsAxis; ++i ) {
			float c = glm::cos( i * angle );
			float s = glm::sin( i * angle );
			*ptr++ = mOrigin + m * vec3( mRadiusBase * c, 0, mRadiusBase * s );
			*ptr++ = mOrigin + m * vec3( mRadiusApex * c, mHeight, mRadiusApex * s );
		}
	}
	
	float angle = float( 2.0 * M_PI / mNumSegments );
	for( int i = 0; i <= mSubdivisionsHeight; ++i ) {
		float height = i * mHeight / mSubdivisionsHeight;
		float radius = lerp<float>( mRadiusBase, mRadiusApex, float( i ) / mSubdivisionsHeight );
		if( radius <= 0.0f )
			continue;

		*ptr++ = mOrigin + m * vec3( radius, height, 0 );
		for( int j = 1; j < mNumSegments; ++j ) {
			vec3 v = mOrigin + m * vec3( radius * glm::cos( j * angle ), height, radius * glm::sin( j * angle ) );
			*ptr++ = v;
			*ptr++ = v;
		}
		*ptr++ = mOrigin + m * vec3( radius, height, 0 );
	}

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*) positions.data(), numVertices );
}

std::vector<vec3> WireIcosahedron::sPositions;

size_t WireIcosahedron::getNumVertices() const
{
	return 120;
}

void WireIcosahedron::loadInto( Target * target, const AttribSet & requestedAttribs ) const
{
	calculate();
	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*) sPositions.data(), 120 );
}

void WireIcosahedron::calculate() const
{
	if( sPositions.empty() ) {
		sPositions.reserve( 60 );

		for( size_t i = 0; i < 20; ++i ) {
			sPositions.emplace_back( *reinterpret_cast<const vec3*>( &Icosahedron::sPositions[Icosahedron::sIndices[i * 3 + 0] * 3] ) );
			sPositions.emplace_back( *reinterpret_cast<const vec3*>( &Icosahedron::sPositions[Icosahedron::sIndices[i * 3 + 1] * 3] ) );
			sPositions.emplace_back( *reinterpret_cast<const vec3*>( &Icosahedron::sPositions[Icosahedron::sIndices[i * 3 + 1] * 3] ) );
			sPositions.emplace_back( *reinterpret_cast<const vec3*>( &Icosahedron::sPositions[Icosahedron::sIndices[i * 3 + 2] * 3] ) );
			sPositions.emplace_back( *reinterpret_cast<const vec3*>( &Icosahedron::sPositions[Icosahedron::sIndices[i * 3 + 2] * 3] ) );
			sPositions.emplace_back( *reinterpret_cast<const vec3*>( &Icosahedron::sPositions[Icosahedron::sIndices[i * 3 + 0] * 3] ) );
		}
	}
}

WireFrustum::WireFrustum( const CameraPersp &cam )
{
	cam.getNearClipCoordinates( &ntl, &ntr, &nbl, &nbr );
	cam.getFarClipCoordinates( &ftl, &ftr, &fbl, &fbr );
}

void WireFrustum::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	/*// extract camera position from view matrix, so that it will work with CameraStereo as well
	//  see: http://www.gamedev.net/topic/397751-how-to-get-camera-position/page__p__3638207#entry3638207
	mat4 view = cam.getViewMatrix();
	vec3 eye;
	eye.x = -( view[0][0] * view[3][0] + view[0][1] * view[3][1] + view[0][2] * view[3][2] );
	eye.y = -( view[1][0] * view[3][0] + view[1][1] * view[3][1] + view[1][2] * view[3][2] );
	eye.z = -( view[2][0] * view[3][0] + view[2][1] * view[3][1] + view[2][2] * view[3][2] );
	//*/

	size_t numVertices = getNumVertices();

	std::vector<vec3> positions;
	positions.resize( numVertices );

	vec3 *ptr = positions.data();
	*ptr++ = ntl;	*ptr++ = ntr;
	*ptr++ = ntr;	*ptr++ = nbr;
	*ptr++ = nbr;	*ptr++ = nbl;
	*ptr++ = nbl;	*ptr++ = ntl;

	*ptr++ = ftl;	*ptr++ = ftr;
	*ptr++ = ftr;	*ptr++ = fbr;
	*ptr++ = fbr;	*ptr++ = fbl;
	*ptr++ = fbl;	*ptr++ = ftl;

	*ptr++ = ftl;	*ptr++ = ntl;
	*ptr++ = ftr;	*ptr++ = ntr;
	*ptr++ = fbr;	*ptr++ = nbr;
	*ptr++ = fbl;	*ptr++ = nbl;

	//gl::ScopedColor color( 0.25f * gl::context()->getCurrentColor() );
	//implDrawLine( eye, nearTopLeft );
	//implDrawLine( eye, nearTopRight );
	//implDrawLine( eye, nearBottomRight );
	//implDrawLine( eye, nearBottomLeft );

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*) positions.data(), numVertices );
}

///////////////////////////////////////////////////////////////////////////////////////
// WirePlane
WirePlane& WirePlane::subdivisions( const ivec2 &subdivisions )
{
	mSubdivisions.x = std::max( subdivisions.x, 1 );
	mSubdivisions.y = std::max( subdivisions.y, 1 );
	return *this;
}

WirePlane& WirePlane::normal( const vec3 &normal )
{
	auto normalNormal = normalize( normal );
	float yAxisDot = dot( normalNormal, vec3( 0, 1, 0 ) );
	if( abs( yAxisDot ) < 0.999f ) {
		quat normalQuat = glm::rotation( vec3( 0, 1, 0 ), normalNormal );
		mAxisU = normalQuat * vec3( 1, 0, 0 );
		mAxisV = normalQuat * vec3( 0, 0, 1 );
	}
	else {
		quat normalQuat = glm::rotation( vec3( 0, 0, 1 ), normalNormal );
		mAxisU = normalQuat * vec3( 1, 0, 0 );
		mAxisV = normalQuat * vec3( 0, -1, 0 );
	}

	return *this;
}

WirePlane& WirePlane::axes( const vec3 &uAxis, const vec3 &vAxis )
{
	mAxisU = normalize( uAxis );
	mAxisV = normalize( vAxis );
	return *this;
}

void WirePlane::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	size_t numVertices = getNumVertices();

	std::vector<vec3> positions;
	positions.resize( numVertices );

	vec3 *ptr = positions.data();

	const vec2 stepIncr = vec2( 1, 1 ) / vec2( mSubdivisions );

	for( int x = 0; x <= mSubdivisions.x; ++x ) {
		float u = x * stepIncr.x - 0.5f;
		*ptr++ = mOrigin + ( mSize.x *  u ) * mAxisU + ( mSize.y * -0.5f ) * mAxisV;
		*ptr++ = mOrigin + ( mSize.x *  u ) * mAxisU + ( mSize.y * +0.5f ) * mAxisV;
	}

	for( int y = 0; y <= mSubdivisions.y; ++y ) {
		float v = y * stepIncr.y - 0.5f;
		*ptr++ = mOrigin + ( mSize.x * -0.5f ) * mAxisU + ( mSize.y * v ) * mAxisV;
		*ptr++ = mOrigin + ( mSize.x * +0.5f ) * mAxisU + ( mSize.y * v ) * mAxisV;
	}

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*) positions.data(), numVertices );
}


///////////////////////////////////////////////////////////////////////////////////////
// WireSphere
size_t WireSphere::getNumVertices() const
{
	int subdivisionAxis = ( mSubdivisionsAxis > 1 ) ? mSubdivisionsAxis : 0;
	return ( mSubdivisionsHeight - 1 ) * mNumSegments * 2 + ( ( mNumSegments + 1 ) / 2 ) * subdivisionAxis * 2;
}

void WireSphere::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	size_t numVertices = getNumVertices();

	std::vector<vec3> positions;
	positions.resize( numVertices );

	vec3 *ptr = positions.data();

	float angle = float( 2.0 * M_PI / mNumSegments );
	for( int i = 1; i < mSubdivisionsHeight; ++i ) {
		float f = float( i ) / mSubdivisionsHeight * 2.0f - 1.0f;
		float radius = mRadius * glm::cos( f * float( M_PI / 2.0 ) );
		vec3 center = mCenter + mRadius * vec3( 0, glm::sin( f * float( M_PI / 2.0 ) ), 0 );

		*ptr++ = center + vec3( 0, 0, 1 ) * radius;
		for( int j = 1; j < mNumSegments; ++j ) {
			vec3 v = center + vec3( glm::sin( j * angle ), 0, glm::cos( j * angle ) ) * radius;
			*ptr++ = v;
			*ptr++ = v;
		}
		*ptr++ = center + vec3( 0, 0, 1 ) * radius;
	}

	if( mSubdivisionsAxis > 1 ) {
		int semidiv = ( mNumSegments + 1 ) / 2;
		float semi = float( M_PI / semidiv );
		float angle = float( 2.0 * M_PI / mSubdivisionsAxis );

		for( int i = 0; i < mSubdivisionsAxis; ++i ) {
			*ptr++ = mCenter + vec3( 0, 1, 0 ) * mRadius;
			for( int j = 1; j < semidiv; ++j ) {
				vec3 v = mCenter + vec3( glm::sin( j * semi ) * glm::sin( i * angle ), glm::cos( j * semi ), glm::sin( j * semi ) * glm::cos( i * angle ) ) * mRadius;
				*ptr++ = v;
				*ptr++ = v;
			}
			*ptr++ = mCenter + vec3( 0, -1, 0 ) * mRadius;
		}
	}

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*) positions.data(), numVertices );
}


///////////////////////////////////////////////////////////////////////////////////////
// WireTorus
size_t WireTorus::getNumVertices() const
{
	int subdivisionAxis = ( mSubdivisionsAxis > 1 ) ? mSubdivisionsAxis : 0;
	int subdivisionHeight = ( mSubdivisionsHeight > 1 ) ? mSubdivisionsHeight : 0;
	return ( subdivisionHeight + subdivisionAxis ) * mNumSegments * 2;
}

void WireTorus::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	size_t numVertices = getNumVertices();

	std::vector<vec3> positions;
	positions.resize( numVertices );

	vec3 *ptr = positions.data();

	if( mSubdivisionsHeight > 1 ) {
		float angle = float( 2.0 * M_PI / mSubdivisionsHeight );
		float step = float( 2.0 * M_PI / mNumSegments );
		for( int i = 0; i < mSubdivisionsHeight; ++i ) {
			float radius = mRadiusMinor + ( mRadiusMajor - mRadiusMinor ) * glm::cos( i * angle );
			vec3 center = mCenter + vec3( 0, ( mRadiusMajor - mRadiusMinor ) * glm::sin( i*angle ), 0 );

			*ptr++ = center + radius * vec3( 0, 0, 1 );
			for( int j = 1; j < mNumSegments; ++j ) {
				vec3 v = center + radius * vec3( glm::sin( j * step ), 0, glm::cos( j * step ) );
				*ptr++ = v;
				*ptr++ = v;
			}
			*ptr++ = center + radius * vec3( 0, 0, 1 );
		}
	}

	if( mSubdivisionsAxis > 1 ) {
		float angle = float( 2.0 * M_PI / mSubdivisionsAxis );
		float step = float( 2.0 * M_PI / mNumSegments );
		for( int i = 0; i < mSubdivisionsAxis; ++i ) {
			float radius = mRadiusMinor;
			float c = glm::cos( i * angle );
			float s = glm::sin( i * angle );
			vec3 center = mCenter + vec3( radius * c, 0, radius * s );

			radius = ( mRadiusMajor - mRadiusMinor );
			*ptr++ = center + vec3( radius * c, 0, radius * s );
			for( int j = 1; j < mNumSegments; ++j ) {
				vec3 v = center + radius * vec3( glm::cos( j * step ) * c, glm::sin( j * step ), glm::cos( j * step ) * s );
				*ptr++ = v;
				*ptr++ = v;
			}
			*ptr++ = center + vec3( radius * c, 0, radius * s );
		}
	}

	target->copyAttrib( Attrib::POSITION, 3, 0, (const float*) positions.data(), numVertices );
}

///////////////////////////////////////////////////////////////////////////////////////
// VertexNormalLines
VertexNormalLines::VertexNormalLines( float length, Attrib attrib )
	: mLength( length ), mAttrib( attrib )
{
}

size_t VertexNormalLines::getNumVertices( const Modifier::Params &upstreamParams ) const
{
	if( upstreamParams.getNumIndices() > 0 )
		return upstreamParams.getNumIndices() * 2;
	else
		return upstreamParams.getNumVertices() * 2;
}

uint8_t VertexNormalLines::getAttribDims( Attrib attr, uint8_t upstreamDims ) const
{
	if( attr == Attrib::POSITION )
		return 3;
	else if( attr == Attrib::CUSTOM_0 )
		return 1;
	else if( attr == mAttrib || attr == Attrib::COLOR )
		return 0;
	else
		return upstreamDims;
}

AttribSet VertexNormalLines::getAvailableAttribs( const Modifier::Params &upstreamParams ) const
{
	AttribSet result = upstreamParams.getAvailableAttribs();
	result.erase( mAttrib );
	result.erase( Attrib::COLOR );
	result.insert( Attrib::POSITION );
	result.insert( Attrib::CUSTOM_0 );
	return result;
}

void VertexNormalLines::process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const
{
	AttribSet request = requestedAttribs;
	request.insert( mAttrib );
	request.insert( Attrib::POSITION );
	ctx->processUpstream( request );

	const size_t numInIndices = ctx->getNumIndices();
	const size_t numInVertices = ctx->getNumVertices();

	if( ctx->getAttribDims( Attrib::POSITION ) != 3 ) {
		CI_LOG_W( "VertexNormalLines only works for 3D positions" );
		return;
	}
	if( ctx->getAttribDims( mAttrib ) != 3 ) {
		if( ctx->getAttribDims( mAttrib ) > 0 )
			CI_LOG_W( "VertexNormalLines requires 3D " << attribToString( mAttrib ) );
		else
			CI_LOG_W( "VertexNormalLines requires " << attribToString( mAttrib ) );
		return;
	}

	const uint32_t *indices = ctx->getIndicesData();
	const vec3 *positions = reinterpret_cast<const vec3*>( ctx->getAttribData( Attrib::POSITION ) );
	const vec3 *attrib = reinterpret_cast<const vec3*>( ctx->getAttribData( mAttrib ) );
	const float *texCoords = nullptr;
	size_t texCoordDims = ctx->getAttribDims( Attrib::TEX_COORD_0 );
	if( texCoordDims > 0 )
		texCoords = reinterpret_cast<const float*>( ctx->getAttribData( Attrib::TEX_COORD_0 ) );

	vector<vec3> outPositions;
	vector<float> outCustom0;
	vector<float> outTexCoord0;
	size_t numVertices = ctx->getNumIndices() ? ( ctx->getNumIndices() * 2 ) : ( ctx->getNumVertices() * 2 );
	outPositions.reserve( numVertices );
	outCustom0.reserve( numVertices );
	if( texCoords )
		outTexCoord0.reserve( numVertices * texCoordDims );

	if( indices ) {
		for( size_t i = 0; i < numInIndices; i++ ) { // lines connecting first vertex ("hub") and all others
			outPositions.emplace_back( positions[indices[i]] ); outPositions.emplace_back( positions[indices[i]] + attrib[indices[i]] * mLength );
			outCustom0.emplace_back( 0.0f ); outCustom0.emplace_back( 1.0f );
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
			outPositions.emplace_back( positions[i] ); outPositions.emplace_back( positions[i] + attrib[i] * mLength );
			outCustom0.emplace_back( 0.0f ); outCustom0.emplace_back( 1.0f );
			if( texCoords ) {
				for( size_t d = 0; d < texCoordDims; ++d )
					outTexCoord0.push_back( texCoords[i * texCoordDims + d] );
				for( size_t d = 0; d < texCoordDims; ++d )
					outTexCoord0.push_back( texCoords[i * texCoordDims + d] );
			}
		}
	}
	
	ctx->copyAttrib( Attrib::POSITION, 3, 0, (const float*)outPositions.data(), numVertices );
	ctx->copyAttrib( Attrib::CUSTOM_0, 1, 0, (const float*)outCustom0.data(), numVertices );
	if( texCoords )
		ctx->copyAttrib( Attrib::TEX_COORD_0, texCoordDims, 0, (const float*)outTexCoord0.data(), numVertices );

	// if the upstream has mAttrib, we should clear that out too; otherwise we are likely to get an error about incorrect count if the
	// consumer were expecting that attribute. For example, geom::Cube >> geom::VertexNormalLines() rendered with a Lambert would error
	ctx->clearAttrib( mAttrib );

	// if the upstream was indexed, we need to clear that out
	ctx->clearIndices();
}

///////////////////////////////////////////////////////////////////////////////////////
// Tangents
uint8_t Tangents::getAttribDims( Attrib attr, uint8_t upstreamDims ) const
{
	if( attr == Attrib::TANGENT || attr == Attrib::BITANGENT )
		return 3;
	else
		return upstreamDims;
}

AttribSet Tangents::getAvailableAttribs( const Modifier::Params &upstreamParams ) const
{
	AttribSet result = upstreamParams.getAvailableAttribs();
	result.insert( Attrib::TANGENT );
	result.insert( Attrib::BITANGENT );
	return result;
}

void Tangents::process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const
{
	AttribSet request = requestedAttribs;
	request.insert( Attrib::POSITION );
	request.insert( Attrib::NORMAL );
	request.insert( Attrib::TEX_COORD_0 );
	ctx->processUpstream( request );

	const size_t numIndices = ctx->getNumIndices();
	const size_t numVertices = ctx->getNumVertices();

	if( numIndices == 0 ) {
		CI_LOG_W( "geom::Tangents requires indexed geometry" );
		return;
	}
	if( ctx->getAttribDims( Attrib::POSITION ) != 3 ) {
		CI_LOG_W( "geom::Tangents requires 3D positions" );
		return;
	}
	if( ctx->getAttribDims( Attrib::NORMAL ) != 3 ) {
		CI_LOG_W( "geom::Tangents requires 3D normals" );
		return;
	}
	if( ctx->getAttribDims( Attrib::TEX_COORD_0 ) != 2 ) {
		CI_LOG_W( "geom::Tangents requires 2D texture coordinates" );
		return;
	}

	const vec3 *positions = (const vec3*)ctx->getAttribData( geom::POSITION );
	const vec3 *normals = (const vec3*)ctx->getAttribData( geom::NORMAL );
	const vec2 *texCoords = (const vec2*)ctx->getAttribData( geom::TEX_COORD_0 );
	
	if( requestedAttribs.count( geom::TANGENT ) || requestedAttribs.count( geom::BITANGENT ) ) {
		vector<vec3> tangents, bitangents;
		vector<vec3> *bitangentPtr = ( requestedAttribs.count( geom::BITANGENT ) ) ? &bitangents : nullptr;
		calculateTangents( numIndices, ctx->getIndicesData(), numVertices, positions, normals, texCoords, &tangents, bitangentPtr );
		
		if( requestedAttribs.count( geom::TANGENT ) )
			ctx->copyAttrib( Attrib::TANGENT, 3, 0, (const float*)tangents.data(), numVertices );
		if( bitangentPtr )
			ctx->copyAttrib( Attrib::BITANGENT, 3, 0, (const float*)bitangentPtr, numVertices );
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Invert
void Invert::process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const
{
	ctx->processUpstream( requestedAttribs );

	if( ctx->getAttribDims( mAttrib ) == 0 ) {
		CI_LOG_W( "geom::Invert missing attrib: " << attribToString( mAttrib ) );
		return;
	}
	
	float *d = ctx->getAttribData( mAttrib );
	size_t maxIdx = ctx->getAttribDims( mAttrib ) * ctx->getNumVertices();
	for( size_t i = 0; i < maxIdx; ++i )
		d[i] = -d[i];
	
	// we don't need to copyAttrib() because we processed in place
}

///////////////////////////////////////////////////////////////////////////////////////
// Remove
uint8_t Remove::getAttribDims( Attrib attr, uint8_t upstreamDims ) const
{
	if( attr == mAttrib )
		return 0;
	else
		return upstreamDims;
}

AttribSet Remove::getAvailableAttribs( const Modifier::Params &upstreamParams ) const
{
	AttribSet result = upstreamParams.getAvailableAttribs();
	result.erase( mAttrib );
	return result;
}

void Remove::process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const
{
	ctx->processUpstream( requestedAttribs );

	ctx->clearAttrib( mAttrib );
}

///////////////////////////////////////////////////////////////////////////////////////
// Assume indexed geometry will result
namespace {
size_t determineRequiredIndices( Primitive sourcePrimitive, Primitive destPrimitive, size_t numIndices )
{
	switch( destPrimitive ) {
		case Primitive::TRIANGLES:
			switch( sourcePrimitive ) {
				case Primitive::TRIANGLES:
					return numIndices;
				break;
				case Primitive::TRIANGLE_STRIP: // ABC, CBD, CDE, EDF, etc
					return ( numIndices - 2 ) * 3; // Triangles: 3:1  4:2  5:3;
				break;
				case Primitive::TRIANGLE_FAN: // ABC, ACD, ADE, etc
					return ( numIndices - 2 ) * 3; // Triangles: 3:1  4:2  5:3;
				break;
				default:
					return 0;
			}
		break;
		case Primitive::LINES:
			switch( sourcePrimitive ) {
				case Primitive::LINES:
					return numIndices;
				break;
				case Primitive::LINE_STRIP:
					return (numIndices - 1) * 2;
				break;
				default:
					return 0;
			}
		break;
		default:
			return 0;
	}
}
}

//////////////////////////////////////////////////////////////////////////////////////
// Bounds
void Bounds::process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const
{
	AttribSet request = requestedAttribs;
	request.insert( mAttrib );
	ctx->processUpstream( request );
	
	uint8_t dims = ctx->getAttribDims( mAttrib );
	if( dims == 0 ) {
		CI_LOG_W( "geom::Bounds requested attribute " << attribToString( mAttrib ) << " missing." );
		return;
	}
	
	size_t numVertices = ctx->getNumVertices();
	const float *data = ctx->getAttribData( mAttrib );
	vec3 minResult( FLT_MAX ), maxResult( FLT_MIN );
	for( size_t v = 0; v < numVertices; ++v ) {
		if( dims == 1 ) {
			minResult.x = std::min<float>( minResult.x, data[0] );
			maxResult.x = std::max<float>( maxResult.x, data[0] );
		}
		else if( dims == 2 ) {
			minResult.x = std::min<float>( minResult.x, data[0] );
			maxResult.x = std::max<float>( maxResult.x, data[0] );
			minResult.y = std::min<float>( minResult.y, data[1] );
			maxResult.y = std::max<float>( maxResult.y, data[1] );
		}
		else {
			minResult.x = std::min<float>( minResult.x, data[0] );
			maxResult.x = std::max<float>( maxResult.x, data[0] );
			minResult.y = std::min<float>( minResult.y, data[1] );
			maxResult.y = std::max<float>( maxResult.y, data[1] );
			minResult.z = std::min<float>( minResult.z, data[2] );
			maxResult.z = std::max<float>( maxResult.z, data[2] );
		}
		data += dims;
	}
	
	if( mResult )
		*mResult = AxisAlignedBox( minResult, maxResult );
}

//////////////////////////////////////////////////////////////////////////////////////
// Subdivide
size_t Subdivide::getNumVertices( const Modifier::Params &upstreamParams ) const
{
	if( upstreamParams.getPrimitive() == Primitive::TRIANGLES ) {
		size_t numTriangles = upstreamParams.getNumIndices() / 3;
		// we add one new vertex for every input triangle
		return upstreamParams.getNumVertices() + numTriangles;
	}
	else
		return upstreamParams.getNumVertices();
}

size_t Subdivide::getNumIndices( const Modifier::Params &upstreamParams ) const
{
	// We create 3 triangles per every input triangle
	if( upstreamParams.getPrimitive() == Primitive::TRIANGLES ) {
		size_t numTriangles = upstreamParams.getNumIndices() / 3;
		return numTriangles * 3 /*output triangles per input*/ * 3 /*vertices per triangle*/;
	}
	else
		return upstreamParams.getNumIndices();
}

void Subdivide::process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const
{
	AttribSet request = requestedAttribs;
	request.insert( POSITION );
	ctx->processUpstream( request );
	
	if( ctx->getPrimitive() != Primitive::TRIANGLES ) {
		CI_LOG_E( "geom::PhongTessellate only supports TRIANGLES primitive." );
		return;
	}

	if( ctx->getAttribDims( POSITION ) != 3 ) {
		CI_LOG_E( "geom::Subdivide requires 3D POSITION." );
		return;
	}
	
	const size_t numInVertices = ctx->getNumVertices();
	const size_t numInIndices = ctx->getNumIndices();
	
	const uint32_t *inIndices = ctx->getIndicesData();
	const vec3 *inPositions = reinterpret_cast<const vec3*>( ctx->getAttribData( POSITION ) );
	
	vector<vec3> outPositions;
	vector<uint32_t> outIndices;
	
	for( size_t idx = 0; idx < numInIndices; idx += 3 ) {
		vec3 sumPos = inPositions[inIndices[idx+0]] + inPositions[inIndices[idx+1]] +
						inPositions[inIndices[idx+2]];
		outPositions.push_back( sumPos / 3.0f );
		
		uint32_t newIdx = (uint32_t)(outPositions.size() + numInVertices - 1);
		// 0-new-2
		outIndices.push_back( inIndices[idx+0] ); outIndices.push_back( newIdx ); outIndices.push_back( inIndices[idx+2] );
		// 0-1-new
		outIndices.push_back( inIndices[idx+0] ); outIndices.push_back( inIndices[idx+1] ); outIndices.push_back( newIdx );
		// new-1-2
		outIndices.push_back( newIdx ); outIndices.push_back( inIndices[idx+1] ); outIndices.push_back( inIndices[idx+2] );
	}
	
	// iterate the attributes and lerp
	for( const auto &attr : ctx->getAvailableAttribs() ) {
		// we processed POSITION in the previous loop
		if( attr == POSITION )
			continue;
	
		vector<float> outData;
		const float *inData = ctx->getAttribData( attr );
		uint8_t dims = ctx->getAttribDims( attr );
		outData.reserve( numInIndices / 3 * dims );
		for( size_t idx = 0; idx < numInIndices; idx += 3 ) {
			for( uint8_t dim = 0; dim < dims; ++dim )
				outData.push_back( (inData[inIndices[idx+0]*dims + dim] +
									inData[inIndices[idx+1]*dims + dim] +
									inData[inIndices[idx+2]*dims + dim]) / 3.0f );
		}
		
		// normalize 3D NORMAL, TANGENT or BITANGENT
		if( ( (attr == NORMAL) || (attr == TANGENT) || (attr == BITANGENT) ) && ( dims == 3 ) ) {
			size_t numVerts = outData.size() / 3;
			for( size_t v = 0; v < numVerts; ++v ) {
				vec3 *d = reinterpret_cast<vec3*>( &outData[v * 3] );
				*d = normalize( *d );
			}
		}

		ctx->appendAttrib( attr, dims, outData.data(), outData.size() / dims );
	}
	
	ctx->appendAttrib( POSITION, 3, (const float*)outPositions.data(), outPositions.size() );
	ctx->copyIndices( ctx->getPrimitive(), outIndices.data(), outIndices.size(), 4 );
}

//////////////////////////////////////////////////////////////////////////////////////
// SourceMods
void SourceMods::copyImpl( const SourceMods &rhs )
{
	mVariablesCached = false;
	mChildren.clear();
	mModifiers.clear();
	mSourceStorage.reset();

	if( rhs.mSourcePtr ) {
		mSourceStorage = std::unique_ptr<Source>( rhs.mSourcePtr->clone() );
		mSourcePtr = mSourceStorage.get();
	}
	else
		mSourcePtr = nullptr;

	for( auto &modifier : rhs.mModifiers )
		mModifiers.push_back( std::unique_ptr<Modifier>( modifier->clone() ) );
	for( auto &child : rhs.mChildren )
		mChildren.push_back( std::unique_ptr<SourceMods>( child->clone() ) );
}

size_t SourceMods::getNumVertices() const
{
	cacheVariables();
	if( mSourcePtr ) {
		return mParamsStack.back().getNumVertices();
	}
	else {
		size_t result = 0;
		for( auto& child : mChildren )
			result += child->getNumVertices();
		return result;
	}
}

size_t SourceMods::getNumIndices() const
{
	cacheVariables();
	if( mSourcePtr ) {
		return mParamsStack.back().getNumIndices();
	}
	else {
		size_t result = 0;
		for( auto& child : mChildren )
			result += child->getNumIndices();
		return result;
	}
}

Primitive SourceMods::getPrimitive() const
{
	cacheVariables();
	if( mSourcePtr ) {
		return mParamsStack.back().getPrimitive();
	}
	else {
		if( ! mChildren.empty() ) {
			Primitive result = mChildren.front()->getPrimitive();
			for( auto childIt = ++mChildren.begin(); childIt != mChildren.end(); ++childIt )
				result = Target::determineCombinedPrimitive( result, (*childIt)->getPrimitive() );
			return result;
		}
		else
			return Primitive::NUM_PRIMITIVES;
	}
}

uint8_t	SourceMods::getAttribDims( Attrib attr ) const
{
	cacheVariables();
	
	if( mSourcePtr ) {
		uint8_t result = mSourcePtr->getAttribDims( attr );
		for( auto &mod : mModifiers )
			result = mod->getAttribDims( attr, result );
		
		return result;
	}
	else {
		if( ! mChildren.empty() )
			return mChildren.front()->getAttribDims( attr );
		else
			return 0;
	}
}

AttribSet SourceMods::getAvailableAttribs() const
{
	cacheVariables();
	if( mSourcePtr )
		return mParamsStack.back().getAvailableAttribs();
	else if( ! mChildren.empty() )
		return mChildren.front()->getAvailableAttribs();
	else
		return AttribSet();
}

// Caches out 'mCachedNumVertices', 'mCachedNumIndices', 'mCachedPrimitive' & 'mCachedAvailableAttribs'
//
// First we store the Source's values for the above variables; then we iterate the modifiers, updating all variables in turn
// A Modifier's various get*() methods (getNumVertices() for example) will call back into 'this' in some instances.
// For example, the geom::Lines modifier must call getPrimitive() in order to calculate the numIndices
// In this example, the SourceMods::getPrimitive() method will reflect whatever the primitive is as of the previous modifier in the iteration,
// or the Source if this is the first modifier, because we are setting 'mCachedPrimitive' in the loop
void SourceMods::cacheVariables() const
{
	if( mVariablesCached || ( ! mSourcePtr ) )
		return;

	// this is important to set first; modifiers' get*() methods might call into one of our get*() methods
	// in which case we need cacheVariables() to return immediately
	mVariablesCached = true;
	
	mParamsStack.push_back( Modifier::Params() );
	mParamsStack.back().mNumVertices = mSourcePtr->getNumVertices();
	mParamsStack.back().mNumIndices = mSourcePtr->getNumIndices();
	mParamsStack.back().mPrimitive = mSourcePtr->getPrimitive();
	mParamsStack.back().mAvaliableAttribs = mSourcePtr->getAvailableAttribs();
	for( auto &mod : mModifiers ) {
		// we store these values in temporaries so that they aren't yet returned by get*()
		auto numVertices = mod->getNumVertices( mParamsStack.back() );
		auto numIndices = mod->getNumIndices( mParamsStack.back() );
		auto primitive = mod->getPrimitive( mParamsStack.back() );
		auto availableAttribs = mod->getAvailableAttribs( mParamsStack.back() );

		mParamsStack.push_back( Modifier::Params() );
		mParamsStack.back().mNumVertices = numVertices;
		mParamsStack.back().mNumIndices = numIndices;
		mParamsStack.back().mPrimitive = primitive;
		mParamsStack.back().mAvaliableAttribs = availableAttribs;
	}
}

void SourceMods::loadInto( Target *target, const AttribSet &requestedAttribs ) const
{
	if( mSourcePtr ) { // normal, no children
		if( mModifiers.empty() ) {
			mSourcePtr->loadInto( target, requestedAttribs );
		}
		else {
			SourceModsContext context( this );
			context.loadInto( target, requestedAttribs );
		}	
	}
	else if( ! mChildren.empty() ) { // children
		SourceModsContext context( this );
		for( auto& child : mChildren ) {
			SourceModsContext siblingContext( child.get() );
			siblingContext.preload( requestedAttribs );
			context.combine( siblingContext );
		}
	
		context.complete( target, requestedAttribs );	
	}
}

void SourceMods::append( const Modifier &modifier )
{
	mModifiers.emplace_back( modifier.clone() );
	mVariablesCached = false;
}

void SourceMods::append( const Source &source )
{
	append( SourceMods( source ) );
}

void SourceMods::append( const SourceMods &sourceMods )
{
	// if we don't have a Source (ie we're not a "bachelor"), this is no problem; just add 'sourceMods' as another child
	if( ! mSourcePtr ) {
		mChildren.emplace_back( sourceMods.clone() );
	}
	else { // we're a "bachelor"; now we need to make a child of ourselves and then add 'sourceMods' as the second child
		CI_ASSERT( mChildren.empty() );
		mChildren.emplace_back( clone() );
		// clear traces of when we owned our Source & Modifiers
		mSourcePtr = nullptr;
		mSourceStorage.reset();
		mModifiers.clear();
		// add the original SourceMods we were combining with
		mChildren.emplace_back( sourceMods.clone() );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SourceModsContext
SourceModsContext::SourceModsContext( const SourceMods *sourceMods )
	: mNumIndices( 0 ), mNumVertices( 0 ), mAttribMask( nullptr ), mPrimitive( NUM_PRIMITIVES )
{
	mSource = sourceMods->getSource();
	
	if( ! sourceMods->mParamsStack.empty() ) // this allows for a non-indexed Source to have never specified the primitive via copyIndices()
		mPrimitive = sourceMods->mParamsStack.front().mPrimitive;
	
	for( auto &modifier : sourceMods->mModifiers )
		mModiferStack.push_back( modifier.get() );
}

SourceModsContext::SourceModsContext()
	: mNumIndices( 0 ), mNumVertices( 0 ), mSource( nullptr ), mAttribMask( nullptr ), mPrimitive( NUM_PRIMITIVES )
{
}

void SourceModsContext::preload( const AttribSet &requestedAttribs )
{
	if( ! mSource ) {
		CI_LOG_E( "SourceModsContext::preload() called with a NULL source." );
		return;
	}

	// If we have modifiers, initiate the chain by calling the last modifier's process() method.
	// This in turn will call processUpstream(), which will call the next modifier's process(), until there
	// are no remaining modifiers. Finally processUpstream() will call loadInto() on the Source
	if( ! mModiferStack.empty() ) {
		auto modifier = mModiferStack.back();
		mModiferStack.pop_back();
		modifier->process( this, requestedAttribs );
	}
	else { // no modifiers; just loadInto on the soucre directly
		mSource->loadInto( this, requestedAttribs );
	}
}

// Expects preload() to have been called on 'rhs'
void SourceModsContext::combine( const SourceModsContext &rhs )
{
	// This is our first SourceModsContext to be combined with; just copy its contents and return
	if( mPrimitive == NUM_PRIMITIVES ) {
		mPrimitive = rhs.getPrimitive();
		this->copyIndices( rhs.getPrimitive(), rhs.getIndicesData(), rhs.getNumIndices(), 4 );
		for( auto &attribInfo : rhs.mAttribInfo ) {
			const auto &data = rhs.mAttribData.find( attribInfo.first )->second;
			const auto &count = rhs.mAttribCount.find( attribInfo.first )->second;
			this->copyAttrib( attribInfo.first, attribInfo.second.getDims(), attribInfo.second.getStride(), data.get(), count );
		}
			
		return;
	}

	Primitive rhsPrimitive = rhs.getPrimitive();
	Primitive combinedPrimitive = determineCombinedPrimitive( getPrimitive(), rhsPrimitive );
	if( (combinedPrimitive != Primitive::LINES) && (combinedPrimitive != Primitive::TRIANGLES) ) {
		CI_LOG_E( "Can't combine Primitives: " << primitiveToString( getPrimitive() ) << " with "
					<< primitiveToString( rhsPrimitive ) );
		return;
	}

	// Handle indices
	size_t numIndices = getNumIndices(); // upstream indices
	size_t rhsNumIndices = rhs.getNumIndices(); // rhs indices
	size_t numVertices = getNumVertices(); // upstream vertices
	size_t rhsNumVertices = rhs.getNumVertices(); // rhs vertices
	
	size_t numOutIndices = determineRequiredIndices( getPrimitive(), combinedPrimitive, numIndices ? numIndices : numVertices );
	size_t rhsNumOutIndices = determineRequiredIndices( rhsPrimitive, combinedPrimitive, rhsNumIndices ? rhsNumIndices : rhsNumVertices );
	size_t totalOutIndices = numOutIndices + rhsNumOutIndices;
	
	std::vector<uint32_t> outIndices;
	outIndices.resize( totalOutIndices );
	
	if( combinedPrimitive == Primitive::TRIANGLES ) {
		if( getNumIndices() ) // 'this is indexed
			Target::copyIndexDataForceTriangles( getPrimitive(), getIndicesData(), numIndices, 0, outIndices.data() );
		else // 'this' wasn't previously indexed but needs to be
			Target::generateIndicesForceTriangles( getPrimitive(), numVertices, 0, outIndices.data() );

		// rhs
		if( rhs.getNumIndices() ) // 'rhs' is indexed
			Target::copyIndexDataForceTriangles( rhsPrimitive, rhs.getIndicesData(),
				rhsNumIndices, (uint32_t)numVertices, outIndices.data() + numOutIndices );
		else // 'rhs' wasn't previously indexed but needs to be
			Target::generateIndicesForceTriangles( rhsPrimitive, rhsNumVertices, (uint32_t)numVertices, outIndices.data() + numOutIndices );
	}
	else if( combinedPrimitive == Primitive::LINES ) {
		if( getNumIndices() ) // 'this' is indexed
			Target::copyIndexDataForceLines( getPrimitive(), getIndicesData(), numIndices, 0, outIndices.data() );
		else // 'this' wasn't previously indexed but needs to be
			Target::generateIndicesForceLines( getPrimitive(), numVertices, 0, outIndices.data() );

		// rhs
		if( rhs.getNumIndices() ) // 'rhs' is indexed
			Target::copyIndexDataForceLines( rhsPrimitive, rhs.getIndicesData(),
				rhsNumIndices, (uint32_t)numVertices, outIndices.data() + numOutIndices );
		else // 'rhs' wasn't previously indexed but needs to be
			Target::generateIndicesForceLines( rhsPrimitive, rhsNumVertices, (uint32_t)numVertices, outIndices.data() + numOutIndices );
	}

	this->copyIndices( combinedPrimitive, outIndices.data(), outIndices.size(), 4 );
	
	// Handle Attributes
	for( const auto &attribInfo : mAttribInfo ) {
		uint8_t dims = getAttribDims( attribInfo.first );
		if( dims > 0 ) {
			uint8_t rhsDims = rhs.getAttribDims( attribInfo.first );
			// if 'rhs' has data for the attribute, copy that
			const float *rhsAttribData = rhs.getAttribData( attribInfo.first );
			this->appendAttrib( attribInfo.first, rhsDims, rhsAttribData, rhsNumVertices );
		}
	}
}

// Expects preload() to have been called, with 0 or more combine()s preceding
void SourceModsContext::complete( Target *target, const AttribSet &requestedAttribs )
{
	if( ( mSource == nullptr ) && ( ! mModiferStack.empty() ) ) {
		// If we have modifiers, initiate the chain by calling the last modifier's process() method.
		// This in turn will call processUpstream(), which will call the next modifier's process(), until there
		// are no remaining modifiers. Finally processUpstream() will call loadInto() on the Source
		auto modifier = mModiferStack.back();
		mModiferStack.pop_back();
		modifier->process( this, requestedAttribs );
	}

	// first let's verify that all counts on our requested attributes are the same. If not, we'll continue to process but with an error
	for( const auto &attribCount : mAttribCount )
		if( attribCount.second != mNumVertices && ( requestedAttribs.count( attribCount.first ) > 0 ) )
			CI_LOG_E( "Attribute " << attribToString( attribCount.first ) << " count is " << attribCount.first << " instead of " << mNumVertices );
	
	for( const auto &attribInfoPair : mAttribInfo ) {
		Attrib attrib = attribInfoPair.first;
		const AttribInfo &attribInfo = attribInfoPair.second;
		target->copyAttrib( attrib, attribInfo.getDims(), attribInfo.getStride(), mAttribData[attrib].get(), mAttribCount[attrib] );
	}

	target->copyIndices( mPrimitive, mIndices.get(), mNumIndices, calcIndicesRequiredBytes( mNumIndices ) );	
}

void SourceModsContext::loadInto( Target *target, const AttribSet &requestedAttribs )
{
	if( ! mSource ) {
		CI_LOG_E( "SourceModsContext::loadInto() called with a NULL source." );
		return;
	}

	// initiate the chain by calling the last modifier's process() method.
	// This in turn will call processUpstream(), which will call the next modifier's process(), until there
	// are no remaining modifiers. Finally processUpstream() will call loadInto() on the Source
	if( ! mModiferStack.empty() ) {
		auto modifier = mModiferStack.back();
		mModiferStack.pop_back();
		modifier->process( this, requestedAttribs );

		// We've finished processing all Modifiers and the Source. Now iterate all the attribute data and the indices
		// and copy them to the target.
		
		// first let's verify that all counts on our requested attributes are the same. If not, we'll continue to process but with an error
		for( const auto &attribCount : mAttribCount )
			if( attribCount.second != mNumVertices && ( requestedAttribs.count( attribCount.first ) > 0 ) )
				CI_LOG_E( "Attribute " << attribToString( attribCount.first ) << " count is " << attribCount.first << " instead of " << mNumVertices );
		
		for( const auto &attribInfoPair : mAttribInfo ) {
			Attrib attrib = attribInfoPair.first;
			const AttribInfo &attribInfo = attribInfoPair.second;
			target->copyAttrib( attrib, attribInfo.getDims(), attribInfo.getStride(), mAttribData[attrib].get(), mAttribCount[attrib] );
		}

		target->copyIndices( mPrimitive, mIndices.get(), mNumIndices, calcIndicesRequiredBytes( mNumIndices ) );
	}
	else {
		// no modifiers; in this case just call loadInto()
		mSource->loadInto( target, requestedAttribs );
	}
}

void SourceModsContext::processUpstream( const AttribSet &requestedAttribs )
{
	// next 'modifier' is actually the Source, because we're at the end of the stack of modifiers
	if( mModiferStack.empty() ) {
		mAttribMask = &requestedAttribs;
		if( mSource )
			mSource->loadInto( this, requestedAttribs );
		mAttribMask = nullptr;
	}
	else {
		// we want the Params to reflect upstream from the current Modifier
		auto modifier = mModiferStack.back();
		mModiferStack.pop_back();
		modifier->process( this, requestedAttribs );
	}
}
	
uint8_t	SourceModsContext::getAttribDims( Attrib attr ) const
{
	auto attrInfoIt = mAttribInfo.find( attr );
	if( attrInfoIt != mAttribInfo.end() )
		return attrInfoIt->second.getDims();
	else
		return 0;
}

size_t SourceModsContext::getNumVertices() const
{
	return mNumVertices;
}

size_t SourceModsContext::getNumIndices() const
{
	return mNumIndices;
}

Primitive SourceModsContext::getPrimitive() const
{
	return mPrimitive;
}

float* SourceModsContext::getAttribData( Attrib attr )
{
	auto attrDataIt = mAttribData.find( attr );
	if( attrDataIt != mAttribData.end() ) {
		return attrDataIt->second.get();
	}
	else
		return nullptr;
}

AttribSet SourceModsContext::getAvailableAttribs() const
{
	AttribSet result;
	for( const auto &attribInfo : mAttribInfo )
		result.insert( attribInfo.first );
	return result;
}

uint32_t* SourceModsContext::getIndicesData()
{
	return mIndices.get();
}

void SourceModsContext::copyAttrib( Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count )
{
	// The attribMask is used to ignore attributes coming from the source which were not directly requested
	// A Source is allowed to supply attributes that weren't requested; this allows us to ignore them; without it,
	// a chain like: sphere1 >> geom::Combine( &sphere2 ) >> geom::Combine( &sphere3 ) >> geom::Translate( ... )
	// can crash, because geom::Translate could be processing residual attributes from further up the chain
	if( mAttribMask && mAttribMask->count( attr ) == 0 )
		return;

	// theoretically this should be the same for all calls to copyAttrib from a given modifier. If it's not at loadInto(), we'll log an error
	mNumVertices = count;

	// we definitely need allocation if we haven't encountered this attrib before
	bool needsAllocation = ( mAttribCount.count( attr ) == 0 ) || ( mAttribData.count( attr ) == 0 ) || ( mAttribInfo.count( attr ) == 0 );
	// we need allocation if we have this attrib but with different parameters
	if( ! needsAllocation ) {
		const AttribInfo &attribInfo = mAttribInfo.at( attr );
		needsAllocation = ( attribInfo.getStride() != strideBytes ) || ( attribInfo.getDims() != dims ) || ( count != mAttribCount[attr] );
	}
	
	if( needsAllocation ) {
		mAttribData[attr] = unique_ptr<float[]>( new float[dims * count] );
		mAttribCount[attr] = count;
		// oddly elaborate logic necessary to replace set contents w/o a default-constructible type
		// equivalent to mAttribInfo[attr] = AttribInfo( ... )
		auto it = mAttribInfo.insert( make_pair( attr, AttribInfo( attr, dims, dims * sizeof(float), (size_t)0 ) ) ).first;
		it->second = AttribInfo( attr, dims, dims * sizeof(float), (size_t)0 ); // only necessary if the key already exists
	}
	
	copyData( dims, strideBytes, srcData, count, dims, 0, mAttribData.at( attr ).get() );
}

void SourceModsContext::appendAttrib( Attrib attr, uint8_t dims, const float *srcData, size_t count )
{
	// if we don't have any data for this attribute, just call copyAttrib
	if( mAttribInfo.count( attr ) == 0 ) {
		copyAttrib( attr, dims, 0, srcData, count );
		return;
	}
	auto attribInfoIt = mAttribInfo.at( attr );
	uint8_t existingDims = attribInfoIt.getDims();
	size_t existingCount = mAttribCount[attr];
	const float *existingData = mAttribData[attr].get();

	auto newData = unique_ptr<float[]>( new float[(existingCount + count) * existingDims] );
	// copy old data
	memcpy( newData.get(), existingData, sizeof(float) * existingCount * existingDims );
	// append new data
	copyData( dims, 0, srcData, count, existingDims, 0, newData.get() + existingCount * existingDims );
	// reassign data
	mAttribData[attr] = std::move( newData );
	mAttribCount[attr] = existingCount + count;

	mNumVertices = existingCount + count;
}

void SourceModsContext::copyIndices( Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytes )
{
	mPrimitive = primitive;
	mIndicesRequiredBytes = requiredBytes;
	// need to reallocate storage only if this is a different number of indices
	if( mNumIndices != numIndices ) {
		mNumIndices = numIndices;
		mIndices = unique_ptr<uint32_t[]>( new uint32_t[numIndices] );
	}
	memcpy( mIndices.get(), source, sizeof(uint32_t) * numIndices );
}

void SourceModsContext::appendIndices( Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytes )
{
	if( mPrimitive != primitive )
		CI_LOG_E( "Primitive types don't match" );
	
	auto newIndices = unique_ptr<uint32_t[]>( new uint32_t[numIndices + mNumIndices] );
	mIndicesRequiredBytes = std::max( mIndicesRequiredBytes, requiredBytes );

	// copy old index data
	if( mNumIndices && mIndices.get() )
		memcpy( newIndices.get(), mIndices.get(), sizeof(uint32_t) * mNumIndices );
	// and append new index data
	memcpy( newIndices.get() + mNumIndices, source, sizeof(uint32_t) * numIndices );
	// total indices += new number of indices
	mNumIndices = mNumIndices + numIndices;
	
	mIndices = std::move( newIndices );
}

void SourceModsContext::clearAttrib( Attrib attr )
{
	mAttribInfo.erase( attr );
	mAttribData.erase( attr );
	mAttribCount.erase( attr );
}

void SourceModsContext::clearIndices()
{
	mNumIndices = 0;
	mIndices.reset();
}

///////////////////////////////////////////////////////////////////////////////////////
// Modifier

template class AttribFn<float,float>;	template class AttribFn<float,vec2>;	template class AttribFn<float,vec3>;	template class AttribFn<float,vec4>;
template class AttribFn<vec2,float>;	template class AttribFn<vec2,vec2>;		template class AttribFn<vec2,vec3>;		template class AttribFn<vec2,vec4>;
template class AttribFn<vec3,float>;	template class AttribFn<vec3,vec2>;		template class AttribFn<vec3,vec3>;		template class AttribFn<vec3,vec4>;
template class AttribFn<vec4,float>;	template class AttribFn<vec4,vec2>;		template class AttribFn<vec4,vec3>;		template class AttribFn<vec4,vec4>;

} } // namespace cinder::geom
