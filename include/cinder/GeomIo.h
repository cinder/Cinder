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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Exception.h"
#include "cinder/Frustum.h"
#include "cinder/Vector.h"
#include "cinder/Matrix.h"
#include "cinder/Shape2d.h"
#include "cinder/Color.h"
#include "cinder/AxisAlignedBox.h"

#include <set>
#include <vector>
#include <map>
#include <algorithm>
#include <array>

// Forward declarations in cinder::
namespace cinder {
	class TriMesh;
	template<int D, typename T>
	class BSpline;
	class Sphere;
}

namespace cinder { namespace geom {

class Target;
class SourceModsBase;
class SourceModsContext;
typedef std::shared_ptr<class Source>	SourceRef;

// keep this incrementing by 1 only; some code relies on that for iterating; add corresponding entry to sAttribNames
enum Attrib { POSITION, COLOR, TEX_COORD_0, TEX_COORD_1, TEX_COORD_2, TEX_COORD_3,
	NORMAL, TANGENT, BITANGENT, BONE_INDEX, BONE_WEIGHT, 
	CUSTOM_0, CUSTOM_1, CUSTOM_2, CUSTOM_3, CUSTOM_4, CUSTOM_5, CUSTOM_6, CUSTOM_7, CUSTOM_8, CUSTOM_9,
	NUM_ATTRIBS, USER_DEFINED = NUM_ATTRIBS };
typedef	std::set<Attrib>	AttribSet;
extern std::string			sAttribNames[(int)Attrib::NUM_ATTRIBS];

enum Primitive { LINES, LINE_STRIP, TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN, NUM_PRIMITIVES };
enum DataType { FLOAT, INTEGER, DOUBLE };


//! Debug utility which returns the name of \a attrib as a std::string
std::string attribToString( Attrib attrib );
//! Debug utility which returns the name of \a primitive as a std::string
std::string primitiveToString( Primitive primitive );
//! Utility function for copying attribute data. Does the right thing to convert \a srcDimensions to \a dstDimensions. \a dstStrideBytes of \c 0 implies tightly packed data.
void copyData( uint8_t srcDimensions, const float *srcData, size_t numElements, uint8_t dstDimensions, size_t dstStrideBytes, float *dstData );
//! Utility function for copying attribute data. Does the right thing to convert \a srcDimensions to \a dstDimensions. Stride of \c 0 implies tightly packed data.
void copyData( uint8_t srcDimensions, size_t srcStrideBytes, const float *srcData, size_t numElements, uint8_t dstDimensions, size_t dstStrideBytes, float *dstData );
//! Utility function for calculating tangents and bitangents from indexed geometry. \a resultBitangents may be NULL if not needed.
void calculateTangents( size_t numIndices, const uint32_t *indices, size_t numVertices, const vec3 *positions, const vec3 *normals, const vec2 *texCoords, std::vector<vec3> *resultTangents, std::vector<vec3> *resultBitangents );
//! Utility function for calculating tangents and bitangents from indexed geometry and 3D texture coordinates. \a resultBitangents may be NULL if not needed.
void calculateTangents( size_t numIndices, const uint32_t *indices, size_t numVertices, const vec3 *positions, const vec3 *normals, const vec3 *texCoords, std::vector<vec3> *resultTangents, std::vector<vec3> *resultBitangents );

struct AttribInfo {
	AttribInfo( const Attrib &attrib, uint8_t dims, size_t stride, size_t offset, uint32_t instanceDivisor = 0 )
		: mAttrib( attrib ), mDims( dims ), mDataType( DataType::FLOAT ), mStride( stride ), mOffset( offset ), mInstanceDivisor( instanceDivisor )
	{}
	AttribInfo( const Attrib &attrib, DataType dataType, uint8_t dims, size_t stride, size_t offset, uint32_t instanceDivisor = 0 )
		: mAttrib( attrib ), mDims( dims ), mDataType( dataType ), mStride( stride ), mOffset( offset ), mInstanceDivisor( instanceDivisor )
	{}

	Attrib		getAttrib() const { return mAttrib; }
	uint8_t		getDims() const { return mDims; }
	void		setDims( uint8_t dims ) { mDims = dims; }
	DataType	getDataType() const { return mDataType; }
	void		setDataType( DataType dataType ) { mDataType = dataType; }
	size_t		getStride() const { return mStride; }
	void		setStride( size_t stride ) { mStride = stride; }
	size_t		getOffset() const { return mOffset;	}
	void		setOffset( size_t offset ) { mOffset = offset; }
	uint32_t	getInstanceDivisor() const { return mInstanceDivisor; }

	uint8_t		getByteSize() const { if( mDataType == geom::DataType::DOUBLE ) return mDims * 8; else return mDims * 4; }

  protected:
	Attrib		mAttrib;
	DataType	mDataType;
	int32_t		mDims;
	size_t		mStride;
	size_t		mOffset;
	uint32_t	mInstanceDivisor;
}; 


class BufferLayout {
  public:
	BufferLayout() {}
	BufferLayout( const std::vector<AttribInfo> &attribs )
		: mAttribs( attribs )
	{}

	void append( const Attrib &attrib, uint8_t dims, size_t stride, size_t offset, uint32_t instanceDivisor = 0 ) {
		mAttribs.push_back( AttribInfo( attrib, DataType::FLOAT, dims, stride, offset, instanceDivisor ) );
	}
	void append( const Attrib &attrib, DataType dataType, uint8_t dims, size_t stride, size_t offset, uint32_t instanceDivisor = 0 ) {
		mAttribs.push_back( AttribInfo( attrib, dataType, dims, stride, offset, instanceDivisor ) );
	}

	//! Returns the AttribInfo for a given Attrib, and throws ExcMissingAttrib if it is not available
	AttribInfo		getAttribInfo( Attrib attrib ) const;
	//! Returns whether a given Attrib is present in the BufferLayout
	bool			hasAttrib( Attrib attrib ) const;
	//! Returns the dimensions for a given Attrib, or 0 if it is not in the BufferLayout
	uint8_t			getAttribDims( Attrib attrib ) const;
	//! Returns a vector of all present Attribs
	const std::vector<AttribInfo>&	getAttribs() const { return mAttribs; }
	//! Returns the number of bytes necessary to store \a numVertices
	size_t			calcRequiredStorage( size_t numVertices ) const;
  protected:
	std::vector<AttribInfo>		mAttribs;
};

class Source {
  public:
	virtual ~Source() {}
	virtual size_t		getNumVertices() const = 0;
	virtual size_t		getNumIndices() const = 0;
	virtual Primitive	getPrimitive() const = 0;
	virtual uint8_t		getAttribDims( Attrib attr ) const = 0;

	virtual void		loadInto( Target *target, const AttribSet &requestedAttribs ) const = 0;

	virtual AttribSet	getAvailableAttribs() const = 0;

  protected:
	//! Builds a sequential list of vertices to simulate an indexed geometry when Source is non-indexed. Assumes \a dest contains storage for getNumVertices() entries
	void	copyIndicesNonIndexed( uint16_t *dest ) const;
	//! Builds a sequential list of vertices to simulate an indexed geometry when Source is non-indexed. Assumes \a dest contains storage for getNumVertices() entries
	void	copyIndicesNonIndexed( uint32_t *dest ) const;
	template<typename T>
	void forceCopyIndicesTrianglesImpl( T *dest ) const;
};

class Target {
  public:
	virtual uint8_t		getAttribDims( Attrib attr ) const = 0;	

	virtual void	copyAttrib( Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count ) = 0;
	virtual void	copyIndices( Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex ) = 0;

	//! For non-indexed geometry, this generates appropriate indices and then calls the copyIndices() virtual method.
	void	generateIndices( Primitive sourcePrimitive, size_t sourceNumIndices );

  protected:
	void copyIndexData( const uint32_t *source, size_t numIndices, uint32_t *target );
	void copyIndexData( const uint32_t *source, size_t numIndices, uint16_t *target );
	void copyIndexDataForceTriangles( Primitive primitive, const uint32_t *source, size_t numIndices, uint32_t *target );
	void copyIndexDataForceTriangles( Primitive primitive, const uint32_t *source, size_t numIndices, uint16_t *target );
};

class Modifier {
  public:
	//! Expresses the upstream parameters for a Modifier such as # vertices
	class Params {
	  public:
		size_t		getNumVertices() const { return mNumVertices; }
		size_t		getNumIndices() const { return mNumIndices; }
		Primitive	getPrimitive() const { return mPrimitive; }
		AttribSet	getAvailableAttribs() const { return mAvaliableAttribs; }
		
		size_t		mNumVertices, mNumIndices;
		Primitive	mPrimitive;
		AttribSet	mAvaliableAttribs;
		
		friend class SourceModsBase;
	};
	
	virtual ~Modifier() {}
	
	virtual Modifier*	clone() const = 0;
	
	virtual size_t		getNumVertices( const Modifier::Params &upstreamParams ) const;
	virtual size_t		getNumIndices( const Modifier::Params &upstreamParams ) const;
	virtual Primitive	getPrimitive( const Modifier::Params &upstreamParams ) const;
	virtual uint8_t		getAttribDims( Attrib attr, uint8_t upstreamDims ) const;
	virtual AttribSet	getAvailableAttribs( const Modifier::Params &upstreamParams ) const;
	
	virtual void		process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const = 0;
};

class Rect : public Source {
  public:
	//! Equivalent to Rectf( -0.5, -0.5, 0.5, 0.5 )
	Rect();
	Rect( const Rectf &r );

	Rect&		rect( const Rectf &r );
	//! Enables default colors. Disabled by default.
	Rect&		colors( bool enable = true ) { mHasColors = enable; return *this; }
	//! Enables COLOR attrib and specifies corner values in clockwise order starting with the upper-left
	Rect&		colors( const ColorAf &upperLeft, const ColorAf &upperRight, const ColorAf &lowerRight, const ColorAf &lowerLeft );
	//! Enables TEX_COORD_0 attrib and specifies corner values in clockwise order starting with the upper-left
	Rect&		texCoords( const vec2 &upperLeft, const vec2 &upperRight, const vec2 &lowerRight, const vec2 &lowerLeft );

	size_t		getNumVertices() const override { return 4; }
	size_t		getNumIndices() const override { return 0; }
	Primitive	getPrimitive() const override { return Primitive::TRIANGLE_STRIP; }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  protected:
	void					setDefaultColors();
	void					setDefaultTexCoords();
	std::array<vec2,4>		mPositions, mTexCoords;
	std::array<ColorAf,4>	mColors;
	bool					mHasColors;
	static const float		sNormals[4*3], sTangents[4*3];
};
	
class RoundedRect : public Source {
  public:
	RoundedRect();
	RoundedRect( const Rectf &r, float cornerRadius = 1.0f );
	
	RoundedRect&	rect( const Rectf &r ) { mRectPositions = r; return *this; }
	RoundedRect&	colors( bool enable = true ) { mHasColors = enable; return *this; }
	RoundedRect&	cornerSubdivisions( int cornerSubdivisions );
	RoundedRect&	cornerRadius( float cornerRadius );
	RoundedRect&	texCoords( const vec2 &upperLeft, const vec2 &lowerRight );
	RoundedRect&	colors( const ColorAf &upperLeft, const ColorAf &upperRight, const ColorAf &lowerRight, const ColorAf &lowerLeft );
	
	size_t		getNumVertices() const override { return mNumVertices; }
	size_t		getNumIndices() const override { return 0; }
	Primitive	getPrimitive() const override { return Primitive::TRIANGLE_FAN; }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;
	
  protected:
	void updateVertexCount();
	void setDefaultColors();
	void setDefaultTexCoords();
	
	Rectf						mRectPositions, mRectTexCoords;
	std::array<vec4, 4>			mColors;
	bool						mHasColors;
	int							mSubdivisions, mNumVertices;
	float						mCornerRadius;
};

class Cube : public Source {
  public:
	Cube();

	//! Enables default colors. Disabled by default.
	Cube&			colors( bool enable = true ) { mHasColors = enable; return *this; }
	//! Enables per-face colors ordered { +X, -X, +Y, -Y, +Z, -Z }. Colors are disabled by default.
	Cube&			colors( const ColorAf &posX, const ColorAf &negX, const ColorAf &posY, const ColorAf &negY, const ColorAf &posZ, const ColorAf &negZ );

	Cube&			subdivisions( int sub ) { mSubdivisions = ivec3( std::max<int>( 1, sub ) ); return *this; }
	Cube&			subdivisionsX( int sub ) { mSubdivisions.x = std::max<int>( 1, sub ); return *this; }
	Cube&			subdivisionsY( int sub ) { mSubdivisions.y = std::max<int>( 1, sub ); return *this; }
	Cube&			subdivisionsZ( int sub ) { mSubdivisions.z = std::max<int>( 1, sub ); return *this; }
	Cube&			size( const vec3 &sz ) { mSize = sz; return *this; }
	Cube&			size( float x, float y, float z ) { mSize = vec3( x, y, z ); return *this; }

	size_t		getNumVertices() const override;
	size_t		getNumIndices() const override;
	Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  protected:
	ivec3					mSubdivisions;
	vec3					mSize;
	bool					mHasColors;
	std::array<ColorAf, 6>	mColors;
};

class Icosahedron : public Source {
  public:
	Icosahedron();

	// Enables colors. Disabled by default.
	Icosahedron&	colors( bool enable = true ) { mHasColors = enable; return *this; }
	
	size_t		getNumVertices() const override;
	size_t		getNumIndices() const override;
	Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  protected:
	void		calculate( std::vector<vec3> *positions, std::vector<vec3> *normals, std::vector<vec3> *colors, std::vector<vec2> *texcoords, std::vector<uint32_t> *indices ) const;

	bool			mHasColors;
	static float	sPositions[12*3];
	static float	sTexCoords[60*2];
	static uint32_t	sIndices[60];
	
	friend class Icosphere;
};

class Icosphere : public Source {
  public:
	Icosphere();

	// Enables colors. Disabled by default.
	Icosphere&	colors( bool enable = true ) { mHasColors = enable; return *this; }

	Icosphere&	subdivisions( int sub ) { mSubdivision = (sub > 0) ? (sub + 1) : 1; mCalculationsCached = false; return *this; }

	size_t		getNumVertices() const override { calculate(); return mPositions.size(); }
	size_t		getNumIndices() const override { calculate(); return mIndices.size(); }
	Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  protected:
	void	calculate() const;
	void	calculateImplUV() const;
	void	subdivide() const;

	int								mSubdivision;
	bool							mHasColors;
	mutable bool					mCalculationsCached;
	mutable std::vector<vec3>		mPositions, mNormals, mColors;
	mutable std::vector<vec2>		mTexCoords;
	mutable std::vector<uint32_t>	mIndices;
};

class Teapot : public Source {
  public:
	Teapot();

	Teapot&		subdivisions( int sub );

	size_t		getNumVertices() const override { return mNumVertices; }
	size_t		getNumIndices() const override { return mNumIndices; }
	Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  protected:
	void			calculate( std::vector<float> *positions, std::vector<float> *normals, std::vector<float> *texCoords, std::vector<uint32_t> *indices ) const;
	void			updateVertexCounts();

	static void		generatePatches( float *v, float *n, float *tc, uint32_t *el, int grid );
	static void		buildPatchReflect( int patchNum, float *B, float *dB, float *v, float *n, float *tc, unsigned int *el,
										int &index, int &elIndex, int &tcIndex, int grid, bool reflectX, bool reflectY );
	static void		buildPatch( vec3 patch[][4], float *B, float *dB, float *v, float *n, float *tc, 
										unsigned int *el, int &index, int &elIndex, int &tcIndex, int grid, const mat3 reflect, bool invertNormal );
	static void		getPatch( int patchNum, vec3 patch[][4], bool reverseV );
	static void		computeBasisFunctions( float *B, float *dB, int grid );
	static vec3		evaluate( int gridU, int gridV, const float *B, const vec3 patch[][4] );
	static vec3		evaluateNormal( int gridU, int gridV, const float *B, const float *dB, const vec3 patch[][4] );

	int			mSubdivision;
	size_t		mNumVertices, mNumIndices;

	static const uint8_t	sPatchIndices[][16];
	static const float		sCurveData[][3];
};

class Circle : public Source {
  public:
	Circle();

	Circle&		center( const vec2 &center ) { mCenter = center; return *this; }
	Circle&		radius( float radius );
	Circle&		subdivisions( int subdivs );

	size_t		getNumVertices() const override;
	size_t		getNumIndices() const override { return 0; }
	Primitive	getPrimitive() const override { return Primitive::TRIANGLE_FAN; }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  private:
	void	updateVertexCounts();

	vec2		mCenter;
	float		mRadius;
	int			mRequestedSubdivisions, mNumSubdivisions;
	size_t		mNumVertices;
};

class Sphere : public Source {
  public:
	Sphere();
	Sphere( const ci::Sphere &sphere );

	Sphere&		colors( bool enable = true ) { mHasColors = enable; return *this; }
	Sphere&		center( const vec3 &center ) { mCenter = center; return *this; }
	Sphere&		radius( float radius ) { mRadius = radius; return *this; }
	//! Specifies the number of segments, which determines the roundness of the sphere.
	Sphere&		subdivisions( int subdiv ) { mSubdivisions = subdiv; return *this; }

	size_t		getNumVertices() const override;
	size_t		getNumIndices() const override;
	Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  protected:
	void		numRingsAndSegments( int *numRings, int *numSegments ) const;

	vec3		mCenter;
	float		mRadius;
	int			mSubdivisions;
	bool		mHasColors;
};

class Capsule : public Source {
  public:
	Capsule();

	//! Enables colors. Disabled by default.
	Capsule&		colors( bool enable = true ) { mHasColors = enable; return *this; }
	Capsule&		center( const vec3 &center ) { mCenter = center; return *this; }
	//! Specifies the number of radial subdivisions, which determines the roundness of the capsule. Defaults to \c 6.
	Capsule&		subdivisionsAxis( int subdiv ) { mSubdivisionsAxis = subdiv; updateCounts(); return *this; }
	//! Specifies the number of slices along the capsule's length. Defaults to \c 6. Add more subdivisions to improve texture mapping and lighting, or if you intend to bend the capsule.
	Capsule&		subdivisionsHeight( int subdiv ) { mSubdivisionsHeight = subdiv > 1 ? subdiv : 1; updateCounts(); return *this; }
	Capsule&		radius( float radius ) { mRadius = math<float>::max(0.f, radius); return *this; }
	Capsule&		length( float length ) { mLength = math<float>::max(0.f, length); return *this; }
	Capsule&		direction( const vec3 &direction ) { mDirection = normalize( direction ); return *this; }
	//! Conveniently sets center, length and direction
	Capsule&		set( const vec3 &from, const vec3 &to );

	size_t		getNumVertices() const override;
	size_t		getNumIndices() const override;
	Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  private:
	void	updateCounts();
	void	calculate( std::vector<vec3> *positions, std::vector<vec3> *normals, std::vector<vec2> *texCoords, std::vector<vec3> *colors, std::vector<uint32_t> *indices ) const;
	void	calculateRing( size_t segments, float radius, float y, float dy, std::vector<vec3> *positions,
							std::vector<vec3> *normals, std::vector<vec2> *texCoords, std::vector<vec3> *colors ) const;

	vec3		mDirection, mCenter;
	float		mLength, mRadius;
	int			mSubdivisionsHeight, mSubdivisionsAxis, mNumSegments;
	bool		mHasColors;
};

class Torus : public Source {
  public:
	Torus();

	// Enables colors. Disabled by default.
	Torus&	colors( bool enable = true ) { mHasColors = enable; return *this; }
	Torus&	center( const vec3 &center ) { mCenter = center; return *this; }
	Torus&	subdivisionsAxis( int subdiv ) { mSubdivisionsAxis = subdiv; updateCounts(); return *this; }
	Torus&	subdivisionsHeight( int subdiv ) { mSubdivisionsHeight = subdiv; updateCounts(); return *this; }
	//! Allows you to twist the torus along the ring.
	Torus&	twist( unsigned twist ) { mTwist = twist; return *this; }
	//! Allows you to twist the torus along the ring. The \a offset is in radians.
	Torus&	twist( unsigned twist, float offset ) { mTwist = twist; mTwistOffset = offset; return *this; }
	//! Specifies the major and minor radius as a ratio (minor : major). Resulting torus will fit unit cube.
	Torus&	ratio( float ratio ) { ratio = math<float>::clamp( ratio ); mRadiusMajor = 1; mRadiusMinor = 1 - ratio; return *this; }
	//! Specifies the major and minor radius separately.
	Torus&	radius( float major, float minor ) { mRadiusMajor = math<float>::max(0, major); mRadiusMinor = math<float>::max(0, minor); return *this; }

	size_t		getNumVertices() const override;
	size_t		getNumIndices() const override;
	Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  protected:
	void		updateCounts();
	void		calculate( std::vector<vec3> *positions, std::vector<vec3> *normals, std::vector<vec2> *texCoords, std::vector<vec3> *colors, std::vector<uint32_t> *indices ) const;

	vec3		mCenter;
	float		mRadiusMajor;
	float		mRadiusMinor;
	int			mSubdivisionsAxis;
	int			mSubdivisionsHeight;
	float		mHeight;
	float		mCoils;
	unsigned	mTwist;
	float		mTwistOffset;
	bool		mHasColors;
	int			mNumRings, mNumAxis;
};

class Helix : public Torus {
  public:
	Helix()
	{
		height( 2.0f );
		coils( 3.0f );
	}

	virtual Helix&	center( const vec3 &center ) { Torus::center( center ); return *this; }
	virtual Helix&	subdivisionsAxis( int subdiv ) { Torus::subdivisionsAxis( subdiv ); return *this; }
	virtual Helix&	subdivisionsHeight( int subdiv ) { Torus::subdivisionsHeight( subdiv ); return *this; }
	//! Specifies the height, measured from center to center.
	Helix&			height( float height ) { mHeight = height; return *this; }
	//! Specifies the number of coils.
	Helix&			coils( float coils ) { mCoils = math<float>::max(0.f, coils); return *this; }
	//! Allows you to twist the helix along the ring.
	Helix&			twist( unsigned twist ) { Torus::twist( twist ); return *this; }
	//! Allows you to twist the helix along the ring. The \a offset is in radians.
	Helix&			twist( unsigned twist, float offset ) { Torus::twist( twist, offset ); return *this; }
};

class Cylinder : public Source {
  public:
	Cylinder();

	//! Enables colors. Disabled by default.
	Cylinder&	colors( bool enable = true ) { mHasColors = enable; return *this; }

	//! Specifices the base of the Cylinder.
	Cylinder&	origin( const vec3 &origin ) { mOrigin = origin; updateCounts(); return *this; }
	//! Specifies the number of radial subdivisions, which determines the roundness of the Cylinder. Defaults to \c 18.
	Cylinder&	subdivisionsAxis( int subdiv ) { mSubdivisionsAxis = subdiv; updateCounts(); return *this; }
	//! Specifies the number of slices along the Cylinder's height. Defaults to \c 1.
	Cylinder&	subdivisionsHeight( int slices ) { mSubdivisionsHeight = slices; updateCounts(); return *this; }
	//! Specifies the height of the cylinder.
	Cylinder&	height( float height ) { mHeight = height; return *this; }
	//! Specifies the base and apex radius.
	Cylinder&	radius( float radius ) { mRadiusBase = mRadiusApex = math<float>::max(0.f, radius); updateCounts(); return *this; }
	//! Specifies the axis of the cylinder.
	Cylinder&	direction( const vec3 &direction ) { mDirection = normalize( direction ); return *this; }
	//! Conveniently sets origin, height and direction so that the center of the base is \a from and the center of the apex is \a to.
	Cylinder&	set( const vec3 &from, const vec3 &to );

	size_t		getNumVertices() const override;
	size_t		getNumIndices() const override;
	Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  protected:
	void	updateCounts();
	void	calculate( std::vector<vec3> *positions, std::vector<vec3> *normals, std::vector<vec2> *texCoords, std::vector<vec3> *colors, std::vector<uint32_t> *indices ) const;
	void	calculateCap( bool flip, float height, float radius, std::vector<vec3> *positions, std::vector<vec3> *normals,
								std::vector<vec2> *texCoords, std::vector<vec3> *colors, std::vector<uint32_t> *indices ) const;

	vec3		mOrigin;
	float		mHeight;
	vec3		mDirection;
	float		mRadiusBase;
	float		mRadiusApex;
	int			mSubdivisionsAxis;
	int			mSubdivisionsHeight;
	bool		mHasColors;
	int			mNumSegments, mNumSlices;
};

class Cone : public Cylinder {
  public:
	Cone()
	{ radius( 1.0f, 0.0f ); }

	//! Enables colors. Disabled by default.
	Cone&	colors( bool enable = true ) { mHasColors = enable; return *this; }

	Cone&	origin( const vec3 &origin ) { Cylinder::origin( origin ); return *this; }
	//! Specifies the number of radial subdivisions, which determines the roundness of the Cone. Defaults to \c 18.
	Cone&	subdivisionsAxis( int subdiv ) { mSubdivisionsAxis = subdiv; updateCounts(); return *this; }
	//! Specifies the number of subdivisions along the Cone's height. Defaults to \c 1.
	Cone&	subdivisionsHeight( int subdiv ) { mSubdivisionsHeight = subdiv; updateCounts(); return *this; }
	Cone&	height( float height ) { Cylinder::height( height ); return *this; }
	//! Specifies the base and apex radius.
	Cone&	radius( float radius ) {  Cylinder::radius( radius ); return *this; }
	//! Specifies the base radius.
	Cone&	base( float base ) { mRadiusBase = math<float>::max( base, 0.f ); return *this; }
	//! Specifies the apex radius.
	Cone&	apex( float apex ) { mRadiusApex = math<float>::max( apex, 0.f ); return *this; }
	//! Specifies the base radius as a \a ratio of the height. A value of 1.0f yields a cone angle of 45 degrees.
	Cone&	ratio( float ratio ) { mRadiusBase = math<float>::max( mRadiusApex + ratio * mHeight, 0.f ); return *this; }
	//! Specifies the base and apex radius separately.
	Cone&	radius( float base, float apex ) { mRadiusBase = math<float>::max(0.f, base); mRadiusApex = math<float>::max(0.f, apex); return *this; }
	Cone&	direction( const vec3 &direction ) { Cylinder::direction( direction ); return *this; }
	//! Conveniently sets origin, height and direction.
	Cone&	set( const vec3 &from, const vec3 &to ) { Cylinder::set( from, to ); return *this; }
};

//! Defaults to a plane on the z axis, origin = [0, 0, 0], normal = [0, 1, 0]
class Plane : public Source {
  public:
	Plane();

	// Specifies the number of times each side is subdivided, ex [2,2] means 4 quads in total. Defaults to [1, 1].
	virtual Plane&	subdivisions( const ivec2 &subdivisions );
	//! Specifies the size in each axis. Defaults to [2, 2], or 1 in each direction
	virtual Plane&	size( const vec2 &size ) { mSize = size; return *this; }
	virtual Plane&	axes( const vec3 &uAxis, const vec3 &vAxis );

	Plane& origin( const vec3 &origin )	{ mOrigin = origin; return *this; }
	Plane& normal( const vec3 &normal );

	size_t		getNumVertices() const override		{ return ( mSubdivisions.x + 1 ) * ( mSubdivisions.y + 1 ); }
	size_t		getNumIndices() const override		{ return mSubdivisions.x * mSubdivisions.y * 6; }
	Primitive	getPrimitive() const override		{ return Primitive::TRIANGLES; }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  protected:
	ivec2		mSubdivisions;
	vec2		mSize;
	vec3		mOrigin, mAxisU, mAxisV;
};

class Extrude : public Source {
  public:
	Extrude( const Shape2d &shape, float distance, float approximationScale = 1.0f );
	
	//! Sets the distance of extrusion along the axis.
	Extrude&	distance( float dist ) { mDistance = dist; return *this; }
	//! Enables or disables front and back caps. Enabled by default.
	Extrude&	caps( bool caps ) { mFrontCap = mBackCap = caps; return *this; }
	//! Enables or disables front cap. Enabled by default.
	Extrude&	frontCap( bool cap ) { mFrontCap = cap; return *this; }
	//! Enables or disables back cap. Enabled by default.
	Extrude&	backCap( bool cap ) { mBackCap = cap; return *this; }
	//! Sets the number of subdivisions along the axis of extrusion
	Extrude&	subdivisions( int sub ) { mSubdivisions = std::max<int>( 1, sub ); updatePathSubdivision(); return *this; }

	size_t		getNumVertices() const override;
	size_t		getNumIndices() const override;
	Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;
	
  protected:
	void		updatePathSubdivision();
	void		calculate( std::vector<vec3> *positions, std::vector<vec3> *normals, std::vector<vec3> *texCoords, std::vector<uint32_t> *indices ) const;
  
	std::vector<Path2d>				mPaths;
	float							mApproximationScale;
	float							mDistance;
	bool							mFrontCap, mBackCap;
	int								mSubdivisions;
	std::shared_ptr<TriMesh>		mCap;
	Rectf							mCapBounds;
	
	std::vector<std::vector<vec2>>	mPathSubdivisionPositions, mPathSubdivisionTangents;
};

class ExtrudeSpline : public Source {
  public:
	ExtrudeSpline( const Shape2d &shape, const ci::BSpline<3,float> &spline, int splineSubdivisions = 10, float approximationScale = 1.0f );
	
	//! Enables or disables front and back caps. Enabled by default.
	ExtrudeSpline&		caps( bool caps ) { mFrontCap = mBackCap = caps; return *this; }
	//! Enables or disables front cap. Enabled by default.
	ExtrudeSpline&		frontCap( bool cap ) { mFrontCap = cap; return *this; }
	//! Enables or disables back cap. Enabled by default.
	ExtrudeSpline&		backCap( bool cap ) { mBackCap = cap; return *this; }
	//! Sets the number of subdivisions along the axis of extrusion
	ExtrudeSpline&		subdivisions( int sub ) { mSubdivisions = std::max<int>( 1, sub ); updatePathSubdivision(); return *this; }

	size_t		getNumVertices() const override;
	size_t		getNumIndices() const override;
	Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;
	
  protected:
	void updatePathSubdivision();
	void calculate( std::vector<vec3> *positions, std::vector<vec3> *normals, std::vector<vec3> *texCoords, std::vector<uint32_t> *indices ) const;
	
	std::vector<Path2d>				mPaths;
	std::vector<mat4>				mSplineFrames;
	std::vector<float>				mSplineTimes;
	float							mApproximationScale;
	bool							mFrontCap, mBackCap;
	int								mSubdivisions;
	std::shared_ptr<TriMesh>		mCap;
	Rectf							mCapBounds;

	std::vector<std::vector<vec2>>	mPathSubdivisionPositions, mPathSubdivisionTangents;
};

//! Converts a BSpline into a \c LINE_STRIP
class BSpline : public Source {
  public:
	template<int D, typename T>
	BSpline( const ci::BSpline<D,T> &spline, int subdivisions );

	size_t		getNumVertices() const override				{ return mNumVertices; }
	size_t		getNumIndices() const override				{ return 0; }
	Primitive	getPrimitive() const override				{ return geom::LINE_STRIP; }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;
	
  protected:
	template<typename T>
	void init( const ci::BSpline<2,T> &spline, int subdivisions );
	template<typename T>
	void init( const ci::BSpline<3,T> &spline, int subdivisions );
	template<typename T>
	void init( const ci::BSpline<4,T> &spline, int subdivisions );

	int						mPositionDims;
	size_t					mNumVertices;
	std::vector<float>		mPositions;
	std::vector<vec3>		mNormals;
};

//////////////////////////////////////////////////////////////////////////////////////
// Wireframe primitives
class WireSource : public Source {
  public:
	size_t			getNumIndices() const override { return 0; }
	Primitive		getPrimitive() const override { return geom::LINES; }
	uint8_t			getAttribDims( Attrib attr ) const override	{ return ( attr == Attrib::POSITION ) ? 3 : 0; }
	AttribSet		getAvailableAttribs() const override { return{ Attrib::POSITION }; }

  protected:
	WireSource() {}
	virtual ~WireSource() {}  
};


class WireCircle : public WireSource {
  public:
	WireCircle();

	//!
	WireCircle&	center( const vec2 &center ) { mCenter = vec3( center, 0 ); return *this; }
	//!
	WireCircle&	center( const vec3 &center ) { mCenter = center; return *this; }
	//!
	WireCircle&	radius( float radius ) { mRadius = radius; return *this; }
	//! Specifies the number of segments that make up each circle. Defaults to \c 72.
	WireCircle&	subdivisions( int subdiv ) { mNumSegments = math<int>::max( 3, subdiv ); return *this; }

	size_t		getNumVertices() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  private:
	vec3		mCenter;
	float		mRadius;
	int			mNumSegments;
	size_t		mNumVertices;
};
	
class WireRoundedRect : public WireSource {
  public:
	WireRoundedRect();
	WireRoundedRect( const Rectf &r, float cornerRadius = 1.0f );
	
	WireRoundedRect&	rect( const Rectf &r ) { mRectPositions = r; return *this; }
	WireRoundedRect&	cornerSubdivisions( int cornerSubdivisions );
	WireRoundedRect&	cornerRadius( float cornerRadius );
	
	size_t		getNumVertices() const override { return mNumVertices; }
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;
	
  protected:
	void updateVertexCount();
	
	Rectf						mRectPositions;
	int							mCornerSubdivisions, mNumVertices;
	float						mCornerRadius;
};

class WireCube : public WireSource {
  public:
	WireCube() : WireCube( vec3( 1 ) ) {}
	WireCube( const vec3 &size, const ivec3 &subdivisions = ivec3( 1 ) )
		: mSize( size )
	{
		mSubdivisions = glm::max( ivec3( 1 ), subdivisions );
	}
	
	WireCube&	subdivisions( int sub ) { mSubdivisions = ivec3( std::max<int>( 1, sub ) ); return *this; }
	WireCube&	subdivisionsX( int sub ) { mSubdivisions.x = std::max<int>( 1, sub ); return *this; }
	WireCube&	subdivisionsY( int sub ) { mSubdivisions.y = std::max<int>( 1, sub ); return *this; }
	WireCube&	subdivisionsZ( int sub ) { mSubdivisions.z = std::max<int>( 1, sub ); return *this; }
	WireCube&	size( const vec3 &sz ) { mSize = sz; return *this; }
	WireCube&	size( float x, float y, float z ) { mSize = vec3( x, y, z ); return *this; }

	size_t		getNumVertices() const override { return ( mSubdivisions.x - 1 ) * 8 + ( mSubdivisions.y - 1 ) * 8 + ( mSubdivisions.z - 1 ) * 8 + 24; }
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  protected:
	ivec3					mSubdivisions;
	vec3					mSize;
};


class WireCylinder : public WireSource {
  public:
	WireCylinder()
		: mOrigin( 0, 0, 0 ), mHeight( 2.0f ), mDirection( 0, 1, 0 ), mRadiusBase( 1.0f ), mRadiusApex( 1.0f ),
		mSubdivisionsAxis( 6 ), mSubdivisionsHeight( 1 ), mNumSegments( 72 )
	{
	}

	//! Specifices the base of the Cylinder.
	WireCylinder&	origin( const vec3 &origin ) { mOrigin = origin; return *this; }
	//! Specifies the number of radial subdivisions, which determines the roundness of the Cylinder. Defaults to \c 18.
	WireCylinder&	subdivisionsAxis( int subdiv ) { mSubdivisionsAxis = subdiv; return *this; }
	//! Specifies the number of slices along the Cylinder's height. Defaults to \c 1.
	WireCylinder&	subdivisionsHeight( int slices ) { mSubdivisionsHeight = slices; return *this; }
	//! Specifies the number of segments that make up each circle. Defaults to \c 72.
	WireCylinder&	subdivisionsCircle( int subdiv ) { mNumSegments = math<int>::max( 3, subdiv ); return *this; }
	//! Specifies the height of the cylinder.
	WireCylinder&	height( float height ) { mHeight = height; return *this; }
	//! Specifies the base and apex radius.
	WireCylinder&	radius( float radius ) { mRadiusBase = mRadiusApex = math<float>::max( 0.f, radius ); return *this; }
	//! Specifies the axis of the cylinder.
	WireCylinder&	direction( const vec3 &direction ) { mDirection = normalize( direction ); return *this; }
	//! Conveniently sets origin, height and direction so that the center of the base is \a from and the center of the apex is \a to.
	WireCylinder&	set( const vec3 &from, const vec3 &to );

	size_t		getNumVertices() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  protected:
	vec3		mOrigin;
	float		mHeight;
	vec3		mDirection;
	float		mRadiusBase;
	float		mRadiusApex;
	int			mSubdivisionsAxis;
	int			mSubdivisionsHeight;
	int			mNumSegments, mNumSlices;
};


class WireCone : public WireCylinder {
  public:
	WireCone()
	{ radius( 1.0f, 0.0f ); }

	WireCone&	origin( const vec3 &origin ) { WireCylinder::origin( origin ); return *this; }
	//! Specifies the number of radial subdivisions, which determines the roundness of the WireCone. Defaults to \c 18.
	WireCone&	subdivisionsAxis( int subdiv ) { mSubdivisionsAxis = subdiv; return *this; }
	//! Specifies the number of subdivisions along the WireCone's height. Defaults to \c 1.
	WireCone&	subdivisionsHeight( int subdiv ) { mSubdivisionsHeight = subdiv; return *this; }
	//! Specifies the number of segments that make up each circle. Defaults to \c 72.
	WireCone&	subdivisionsCircle( int subdiv ) { mNumSegments = math<int>::max( 3, subdiv ); return *this; }
	//!
	WireCone&	height( float height ) { WireCylinder::height( height ); return *this; }
	//! Specifies the base and apex radius.
	WireCone&	radius( float radius ) { WireCylinder::radius( radius ); return *this; }
	//! Specifies the base radius.
	WireCone&	base( float base ) { mRadiusBase = math<float>::max( base, 0.f ); return *this; }
	//! Specifies the apex radius.
	WireCone&	apex( float apex ) { mRadiusApex = math<float>::max( apex, 0.f ); return *this; }
	//! Specifies the apex radius as a \a ratio of the height. A value of 1.0f yields a cone angle of 45 degrees.
	WireCone&	ratio( float ratio ) { mRadiusApex = math<float>::max( mRadiusBase + ratio * mHeight, 0.f ); return *this; }
	//! Specifies the base and apex radius separately.
	WireCone&	radius( float base, float apex ) { mRadiusBase = math<float>::max( 0.f, base ); mRadiusApex = math<float>::max( 0.f, apex ); return *this; }
	WireCone&	direction( const vec3 &direction ) { WireCylinder::direction( direction ); return *this; }
	//! Conveniently sets origin, height and direction.
	WireCone&	set( const vec3 &from, const vec3 &to ) { WireCylinder::set( from, to ); return *this; }
};

class WireFrustum : public WireSource {
  public:
	WireFrustum( const CameraPersp &cam );

	//template<typename T>
	//WireFrustum( const Frustum<T> &frustum );

	size_t		getNumVertices() const override { return 24; }
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  private:
	vec3 ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr;
};

//! Defaults to a plane on the z axis, origin = [0, 0, 0], normal = [0, 1, 0]
class WirePlane : public WireSource {
  public:
	WirePlane()
		: mSubdivisions( 1, 1 ), mSize( 2, 2 ), mOrigin( 0 ), mAxisU( 1, 0, 0 ), mAxisV( 0, 0, 1 ) {}

	// Specifies the number of times each side is subdivided, ex [2,2] means 4 quads in total. Defaults to [1, 1].
	WirePlane&	subdivisions( const ivec2 &subdivisions );
	//! Specifies the size in each axis. Defaults to [2, 2], or 1 in each direction
	WirePlane&	size( const vec2 &size ) { mSize = size; return *this; }
	//!
	WirePlane&	axes( const vec3 &uAxis, const vec3 &vAxis );
	//!
	WirePlane&	origin( const vec3 &origin ) { mOrigin = origin; return *this; }
	//!
	WirePlane&	normal( const vec3 &normal );

	size_t		getNumVertices() const override { return ( mSubdivisions.x + 1 ) * 2 + ( mSubdivisions.y + 1 ) * 2; }
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  protected:
	ivec2		mSubdivisions;
	vec2		mSize;
	vec3		mOrigin, mAxisU, mAxisV;
};


class WireSphere : public WireSource {
  public:
	WireSphere()
		: mCenter(0), mRadius(1.0f), mSubdivisionsAxis(6), mSubdivisionsHeight(4), mNumSegments(72) {}

	WireSphere&	center( const vec3 &center ) { mCenter = center; return *this; }
	WireSphere&	radius( float radius ) { mRadius = radius; return *this; }
	//! Specifies the number of segments that make up each circle. Defaults to \c 72.
	WireSphere&	subdivisionsCircle( int subdiv ) { mNumSegments = math<int>::max( 3, subdiv ); return *this; }
	WireSphere&	subdivisionsHeight( int subdiv ) { mSubdivisionsHeight = glm::max( 1, subdiv ); return *this; }
	WireSphere&	subdivisionsAxis( int subdiv ) { mSubdivisionsAxis = glm::max( 1, subdiv ); return *this; }

	size_t		getNumVertices() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  protected:
	vec3		mCenter;
	float		mRadius;
	int			mSubdivisionsAxis;
	int			mSubdivisionsHeight;
	int			mNumSegments;
};


class WireTorus : public WireSource {
  public:
	WireTorus()
		: mCenter( 0 ), mRadiusMajor( 1.0f ), mRadiusMinor( 0.75f ), mSubdivisionsAxis( 18 ), mSubdivisionsHeight( 18 ), mNumSegments( 72 ) {}

	WireTorus&	center( const vec3 &center ) { mCenter = center; return *this; }
	WireTorus&	subdivisionsAxis( int subdiv ) { mSubdivisionsAxis = subdiv; return *this; }
	WireTorus&	subdivisionsHeight( int subdiv ) { mSubdivisionsHeight = subdiv; return *this; }
	//! Specifies the number of segments that make up each circle. Defaults to \c 72.
	WireTorus&	subdivisionsCircle( int subdiv ) { mNumSegments = math<int>::max( 3, subdiv ); return *this; }
	//! Specifies the major and minor radius as a ratio (minor : major). Resulting torus will fit unit cube.
	WireTorus&	ratio( float ratio ) { ratio = math<float>::clamp( ratio ); mRadiusMajor = 1; mRadiusMinor = 1 - ratio; return *this; }
	//! Specifies the major and minor radius separately.
	WireTorus&	radius( float major, float minor ) { mRadiusMajor = math<float>::max( 0, major ); mRadiusMinor = math<float>::max( 0, minor ); return *this; }

	size_t		getNumVertices() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;

  protected:
	vec3		mCenter;
	float		mRadiusMajor;
	float		mRadiusMinor;
	int			mSubdivisionsAxis;
	int			mSubdivisionsHeight;
	float		mHeight;
	float		mCoils;
	int			mNumRings, mNumAxis, mNumSegments;
};


//////////////////////////////////////////////////////////////////////////////////////
// Modifiers
//! "Bakes" a mat4 transformation into the positions, normals and tangents of a geom::Source
class Transform : public Modifier {
  public:
	//! Does not currently support a projection matrix (i.e. doesn't divide by 'w' )
	Transform( const mat4 &transform )
		: mTransform( transform )
	{}

	const mat4&			getMatrix() const { return mTransform; }
	void				setMatrix( const mat4 &transform ) { mTransform = transform; }
	
	// Inherited from Modifier
	Modifier*			clone() const override { return new Transform( mTransform ); }
	uint8_t				getAttribDims( Attrib attr, uint8_t upstreamDims ) const override;
	void				process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const override;
	
	mat4		mTransform;
};

//! "Bakes" a translation into the positions, normals and tangents of a geom::Source
class Translate : public Transform {
  public:
	Translate( const vec3 &v ) : Transform( glm::translate( v ) ) {}
	Translate( float x, float y, float z ) : Transform( glm::translate( vec3( x, y, z ) ) ) {}
	Translate( const vec2 &v ) : Transform( glm::translate( vec3( v, 0 ) ) ) {}
	Translate( float x, float y ) : Transform( glm::translate( vec3( x, y, 0 ) ) ) {}
};

//! "Bakes" a scale into the positions, normals and tangents of a geom::Source
class Scale : public Transform {
  public:
	Scale( const vec3 &v ) : Transform( glm::scale( v ) ) {}
	Scale( float x, float y, float z ) : Transform( glm::scale( vec3( x, y, z ) ) ) {}
	Scale( float s ) : Transform( glm::scale( vec3( s ) ) ) {}
};

//! "Bakes" a rotation into the positions, normals and tangents of a geom::Source
class Rotate : public Transform {
  public:
	Rotate( const glm::quat &q ) : Transform( glm::toMat4( q ) ) {}
};

//! Twists a geom::Source around a given axis
class Twist : public Modifier {
  public:
	Twist()
		: mAxisStart( 0, -1, 0 ), mAxisEnd( 0, 1, 0 ), mStartAngle( (float)-M_PI ), mEndAngle( (float)M_PI )
	{}

	Twist&		axisStart( const vec3 &start ) { mAxisStart = start; return *this; }
	Twist&		axisEnd( const vec3 &end ) { mAxisEnd = end; return *this; }
	Twist&		axis( const vec3 &start, const vec3 &end ) { mAxisStart = start; mAxisEnd = end; return *this; }
	Twist&		startAngle( float radians ) { mStartAngle = radians; return *this; }
	Twist&		endAngle( float radians ) { mEndAngle = radians; return *this; }

	Modifier*	clone() const override { return new Twist( *this ); }
	void		process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const override;
	
  protected:
	vec3					mAxisStart, mAxisEnd;
	float					mStartAngle, mEndAngle;
};

//! Converts any geom::Source to equivalent vertices connected by lines. Output primitive type is always geom::Primitive::LINES.
class Lines : public Modifier {
  public:
	Modifier*	clone() const override { return new Lines(); }
	
	size_t		getNumIndices( const Modifier::Params &upstreamParams ) const override;
	Primitive	getPrimitive( const Modifier::Params &upstreamParams ) const override { return geom::LINES; }
	void		process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const override;
	
  protected:
	static size_t	calcNumIndices( Primitive primitive, size_t upstreamNumIndices, size_t upstreamNumVertices );
};

//! Modifies the color of a geom::Source as a function of a 2D or 3D input attribute
class ColorFromAttrib : public Modifier {
  public:
	ColorFromAttrib( Attrib attrib, const std::function<Colorf(vec2)> &fn )
		: mAttrib( attrib ), mFnColor2( fn )
	{}
	ColorFromAttrib( Attrib attrib, const std::function<Colorf(vec3)> &fn )
		: mAttrib( attrib ), mFnColor3( fn )
	{}
	
	Attrib				getAttrib() const { return mAttrib; }
	ColorFromAttrib&	attrib( Attrib attrib ) { mAttrib = attrib; return *this; }

	Modifier*	clone() const override { return new ColorFromAttrib( mAttrib, mFnColor2, mFnColor3 ); }
	uint8_t		getAttribDims( Attrib attr, uint8_t upstreamDims ) const override;
	AttribSet	getAvailableAttribs( const Modifier::Params &upstreamParams ) const override;
	
	void		process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const override;
	
  protected:
	ColorFromAttrib( Attrib attrib, const std::function<Colorf(vec2)> &fn2, const std::function<Colorf(vec3)> &fn3 )
		: mAttrib( attrib ), mFnColor2( fn2 ), mFnColor3( fn3 )
	{}

	Attrib							mAttrib;
	std::function<Colorf(vec2)>		mFnColor2;
	std::function<Colorf(vec3)>		mFnColor3;
};

//! Maps an attribute as a function of another attribute. Valid types are: float, vec2, vec3, vec4
template<typename S, typename D>
class AttribFn : public Modifier {
  public:
	typedef typename std::function<D(S)> FN;
	static const int SRCDIM = sizeof(S)/ sizeof(float);
	static const int DSTDIM = sizeof(D)/ sizeof(float);
	
	AttribFn( Attrib src, Attrib dst, const FN &fn )
		: mSrcAttrib( src ), mDstAttrib( dst ), mFn( fn )
	{}

	AttribFn( Attrib attrib, const FN &fn )
		: mSrcAttrib( attrib ), mDstAttrib( attrib ), mFn( fn )
	{}
	
	Modifier*	clone() const override { return new AttribFn( mSrcAttrib, mDstAttrib, mFn ); }
	uint8_t		getAttribDims( Attrib attr, uint8_t upstreamDims ) const override;
	AttribSet	getAvailableAttribs( const Modifier::Params &upstreamParams ) const override;
	
	void		process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const override;
	
  protected:
	geom::Attrib		mSrcAttrib, mDstAttrib;
	FN					mFn;
};

//! Draws lines representing the Attrib::NORMALs for a geom::Source. Encodes 0 for base and 1 for normal into CUSTOM_0
class VertexNormalLines : public Modifier {
  public:
	VertexNormalLines( float length, Attrib attrib = Attrib::NORMAL );

	VertexNormalLines&	length( float len ) { mLength = len; return *this; }

	size_t		getNumVertices( const Modifier::Params &upstreamParams ) const override;
	size_t		getNumIndices( const Modifier::Params &upstreamParams ) const override				{ return 0; }
	Primitive	getPrimitive( const Modifier::Params &upstreamParams ) const override				{ return geom::LINES; }
	uint8_t		getAttribDims( Attrib attr, uint8_t upstreamDims ) const override;
	AttribSet	getAvailableAttribs( const Modifier::Params &upstreamParams ) const override;
	
	Modifier*	clone() const override { return new VertexNormalLines( mLength, mAttrib ); }
	void		process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const override;
	
  protected:
	float					mLength;
	Attrib					mAttrib;
};

//! Creates TANGENT and BITANGENT attributes based on POSITIONS, NORMALS and TEX_COORD_0. Requires indexed geometry.
class Tangents : public Modifier {
  public:
	Tangents() {}

	uint8_t		getAttribDims( Attrib attr, uint8_t upstreamDims ) const override;
	AttribSet	getAvailableAttribs( const Modifier::Params &upstreamParams ) const override;
	
	Modifier*	clone() const override { return new Tangents; }
	void		process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const override;
};

//! Inverts the value of an attribute. Works for any dimension.
class Invert : public Modifier {
  public:
	Invert( Attrib attrib )
		: mAttrib( attrib )
	{}

	Modifier*	clone() const override { return new Invert( mAttrib ); }
	void		process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const override;

  protected:
	Attrib		mAttrib;
};

//! Removes an attribute entirely
class Remove : public Modifier {
  public:
	Remove( Attrib attrib )
		: mAttrib( attrib )
	{}
	
	uint8_t		getAttribDims( Attrib attr, uint8_t upstreamDims ) const override;
	AttribSet	getAvailableAttribs( const Modifier::Params &upstreamParams ) const override;	
	
	Modifier*	clone() const override { return new Remove( mAttrib ); }
	void		process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const override;
	
  protected:
	Attrib		mAttrib;
};

//! Combines an additional Source. Requires the Primitive types to match. Attributes not available on Source will be filled with \c 0
class Combine : public Modifier {
  public:
	Combine( const Source *source )
		: mSource( source )
	{}
	
	Modifier*	clone() const override { return new Combine( mSource ); }
	
	size_t		getNumVertices( const Modifier::Params &upstreamParams ) const override;
	size_t		getNumIndices( const Modifier::Params &upstreamParams ) const override;
	
	void		process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const override;
	
  protected:
	const Source		*mSource;
};

//! Calculates the 3D bounding box of the geometry.
class Bounds : public Modifier {
  public:
	Bounds( AxisAlignedBox *result, Attrib attrib = POSITION )
		: mResult( result ), mAttrib( attrib )
	{}
	
	
	Modifier*	clone() const override { return new Bounds( mResult, mAttrib ); }
	void		process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const override;
	
  protected:
	AxisAlignedBox	*mResult;
	Attrib				mAttrib;
};

//! Calculates a single level of subdivision of triangles by inserting a single vertex in the center of each triangle.
//! Interpolates all attributes and normalizes 3D NORMAL, TANGENT and BITANGENT attributes.
class Subdivide : public Modifier {
  public:
	Subdivide()
	{}
	
	size_t		getNumVertices( const Modifier::Params &upstreamParams ) const override;
	size_t		getNumIndices( const Modifier::Params &upstreamParams ) const override;
	
	Modifier*	clone() const override { return new Subdivide(); }
	void		process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const override;
};


////////////////////////////////////////////////////////////////////////////////
//! Base class for SourceMods<> and SourceModsPtr<>
//! Used by Modifiers to process Source -> Target
class SourceModsContext : public Target {
  public:
	SourceModsContext( const SourceModsBase *sourceMods );
	//! Can be used to capture a Source. Calling loadInto() in this case is an error.
	SourceModsContext();

	// called by SourceModsBase::loadInto()
	void			loadInto( Target *target, const AttribSet &requestedAttribs );
	
	// Target virtuals; also used by Modifiers
	uint8_t			getAttribDims( Attrib attr ) const override;
	void			copyAttrib( Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count ) override;
	void			copyIndices( Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex ) override;
	
	//! Appends vertex data to existing data for \a attr. \a dims must match existing data.
	void			appendAttrib( Attrib attr, uint8_t dims, const float *srcData, size_t count );
	void			clearAttrib( Attrib attr );
	//! Appends index data to existing index data. \a primitive must match existing data.
	void			appendIndices( Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytes );
	void			clearIndices();

	size_t			getNumVertices() const;
	size_t			getNumIndices() const;
	Primitive		getPrimitive() const;
	AttribSet		getAvailableAttribs() const;
	
	void			processUpstream( const AttribSet &requestedAttribs );

	float*			getAttribData( Attrib attr );
	uint32_t*		getIndicesData();
	
  private:
	const Source					*mSource;
	std::vector<Modifier*>			mModiferStack;
	
	const AttribSet					*mAttribMask;
	
	size_t										mNumVertices;
	std::map<Attrib,AttribInfo>					mAttribInfo;
	std::map<Attrib,std::unique_ptr<float[]>>	mAttribData;
	std::map<Attrib,size_t>						mAttribCount;
	
	std::unique_ptr<uint32_t[]>				mIndices;
	size_t									mNumIndices;
	uint8_t									mIndicesRequiredBytes;
	geom::Primitive							mPrimitive;
};

class SourceModsBase : public Source {
  public:
	SourceModsBase()
		: mVariablesCached( false )
	{}

	// geom::Source methods
	size_t		getNumVertices() const override;
	size_t		getNumIndices() const override;
	Primitive	getPrimitive() const override;
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;
	
	void											addModifier( const Modifier &modifier );
	const std::vector<std::unique_ptr<Modifier>>&	getModifiers() const { return mModifiers; }
	
	virtual const Source*		getSource() const = 0;

  protected:
	void		cacheVariables() const;
	
	const Source* 							mSourceBase;
	std::vector<std::unique_ptr<Modifier>>	mModifiers;
	
	mutable bool							mVariablesCached;
	mutable std::vector<Modifier::Params>	mParamsStack;
	
	friend class SourceModsContext;
};

template<typename T>
class SourceMods;

//! In general you should not return this as the result of a function or even instantiate it directly
//! Similar to SourceMods<> but stores a pointer to the SOURCE rather than a copy of it
template<typename SOURCE>
class SourceModsPtr : public SourceModsBase {
  public:
	SourceModsPtr( const SOURCE *srcPtr )
		: SourceModsBase(), mSrcPtr( srcPtr )
	{}

	SourceModsPtr( const SourceModsPtr<SOURCE> &rhs )
		: SourceModsBase(), mSrcPtr( rhs.mSrcPtr )
	{
		for( const auto &rhsMod : rhs.mModifiers )
			mModifiers.push_back( std::unique_ptr<Modifier>( rhsMod->clone() ) );
	}

	SourceModsPtr( const SOURCE* srcPtr, const std::vector<std::unique_ptr<Modifier>>& srcModifiers )
		: SourceModsBase(), mSrcPtr( srcPtr )
	{
		for( const auto &rhsMod : srcModifiers )
			mModifiers.push_back( std::unique_ptr<Modifier>( rhsMod->clone() ) );
	}
	
	SourceModsPtr( SourceModsPtr<SOURCE> &&rhs )
		: SourceModsBase(), mSrcPtr( rhs.mSrcPtr )
	{
		for( auto &rhsMod : rhs.mModifiers )
			mModifiers.push_back( std::move( rhsMod ) );
	}
	
	SourceModsPtr& operator=( const SourceModsPtr<SOURCE> &rhs )
	{
		mSourceBase = rhs.mSrcPtr;
		mSrcPtr = rhs.mSrcPtr;
		
		for( const auto &rhsMod : rhs.mModifiers )
			mModifiers.push_back( std::unique_ptr<Modifier>( rhsMod->clone() ) );

		return *this;
	}
	
	SourceModsPtr& operator=( SourceModsPtr<SOURCE> &&rhs )
	{
		mSourceBase = rhs.mSrcPtr;
		mSrcPtr = std::move( rhs.mSrc );
		
		for( auto &rhsMod : rhs.mModifiers )
			mModifiers.push_back( std::move( rhsMod ) );

		return *this;
	}
	
	const Source*		getSource() const override { return mSrcPtr; }
	
	const SOURCE		*mSrcPtr;
};

template<typename SOURCE>
class SourceMods : public SourceModsBase {
  public:
	SourceMods( const SOURCE &src )
		: SourceModsBase(), mSrc( src )
	{}
	
	SourceMods( SOURCE &&src )
		: SourceModsBase(), mSrc( std::move( src ) )
	{}

	SourceMods( const SourceMods<SOURCE> &rhs )
		: SourceModsBase(), mSrc( rhs.mSrc )
	{
		for( const auto &rhsMod : rhs.mModifiers )
			mModifiers.push_back( std::unique_ptr<Modifier>( rhsMod->clone() ) );
	}

	SourceMods( SourceMods<SOURCE> &&rhs )
		: SourceModsBase(), mSrc( std::move( rhs.mSrc ) )
	{
		for( auto &rhsMod : rhs.mModifiers )
			mModifiers.push_back( std::move( rhsMod ) );
	}

	SourceMods( const SourceModsPtr<SOURCE> &rhs )
		: SourceModsBase(), mSrc( *rhs.mSrcPtr )
	{
		for( const auto &rhsMod : rhs.mModifiers )
			mModifiers.push_back( std::unique_ptr<Modifier>( rhsMod->clone() ) );
	}
	
	SourceMods& operator=( const SourceMods<SOURCE> &rhs )
	{
		mSourceBase = &mSrc;
		mSrc = rhs.mSrc;
		
		for( const auto &rhsMod : rhs.mModifiers )
			mModifiers.push_back( std::unique_ptr<Modifier>( rhsMod->clone() ) );

		return *this;
	}
	
	SourceMods& operator=( SourceMods<SOURCE> &&rhs )
	{
		mSourceBase = &mSrc;
		mSrc = std::move( rhs.mSrc );
		
		for( auto &rhsMod : rhs.mModifiers )
			mModifiers.push_back( std::move( rhsMod ) );

		return *this;
	}
	
	const Source*		getSource() const override { return &mSrc; }
	
	SOURCE		mSrc;
};

////////////////////////////////////////////////////////////////////////////////
// Source
template<typename SOURCE>
SourceModsPtr<SOURCE> operator>>( SourceMods<SOURCE> &sourceMod, const Modifier &modifier )
{
	SourceModsPtr<SOURCE> result( &sourceMod.mSrc, sourceMod.getModifiers() );
	result.addModifier( modifier );
	return result;
}

template<typename SOURCE>
SourceMods<SOURCE>&& operator>>( SourceMods<SOURCE> &&sourceMod, const Modifier &modifier )
{
	sourceMod.addModifier( modifier );
	return std::move(sourceMod);
}

template<typename SOURCE>
SourceModsPtr<SOURCE> operator>>( SourceModsPtr<SOURCE> &sourceMod, const Modifier &modifier )
{
	SourceModsPtr<SOURCE> result( sourceMod );
	result.addModifier( modifier );
	return result;
}

template<typename SOURCE>
SourceModsPtr<SOURCE>&& operator>>( SourceModsPtr<SOURCE> &&sourceMod, const Modifier &modifier )
{
	sourceMod.addModifier( modifier );
	return std::move(sourceMod);
}

template<typename SOURCE>
SourceMods<SOURCE> operator>>( const SOURCE &source, const Modifier &modifier )
{
	SourceMods<SOURCE> result( source );
	result.addModifier( modifier );
	return result;
}

template<typename SOURCE>
typename std::enable_if<std::is_base_of<Source,SOURCE>::value, SourceMods<SOURCE>>::type operator>>( SOURCE &&source, const Modifier &modifier )
{
	SourceMods<SOURCE> result( std::forward<SOURCE>( source ) );
	result.addModifier( modifier );
	return result;
}

////////////////////////////////////////////////////////////////////////////////

class Exc : public Exception {
};

class ExcMissingAttrib : public Exception {
};

class ExcIllegalSourceDimensions : public Exception {
};

class ExcIllegalDestDimensions : public Exception {
};

class ExcIllegalPrimitiveType : public Exception {
};

class ExcNoIndices : public Exception {
};

class ExcIllegalIndexType : public Exception {
};

// Attempt to store >65535 indices into a uint16_t
class ExcInadequateIndexStorage : public Exception {
};

} } // namespace cinder::geom
