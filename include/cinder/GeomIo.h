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
#include "cinder/Vector.h"
#include "cinder/Matrix.h"
#include "cinder/Shape2d.h"
#include "cinder/Color.h"

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
}

namespace cinder { namespace geom {

class Target;
typedef std::shared_ptr<class Source>	SourceRef;

// keep this incrementing by 1 only; some code relies on that for iterating; add corresponding entry to sAttribNames
enum Attrib { POSITION, COLOR, TEX_COORD_0, TEX_COORD_1, TEX_COORD_2, TEX_COORD_3,
	NORMAL, TANGENT, BITANGENT, BONE_INDEX, BONE_WEIGHT, 
	CUSTOM_0, CUSTOM_1, CUSTOM_2, CUSTOM_3, CUSTOM_4, CUSTOM_5, CUSTOM_6, CUSTOM_7, CUSTOM_8, CUSTOM_9,
	NUM_ATTRIBS };
typedef	std::set<Attrib>	AttribSet;
extern std::string			sAttribNames[(int)Attrib::NUM_ATTRIBS];

enum Primitive { LINES, LINE_STRIP, TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN };
enum DataType { FLOAT, INTEGER, DOUBLE };


//! Debug utility which returns the name of \a attrib as a std::string
std::string attribToString( Attrib attrib );
//! Utility function for copying attribute data. Does the right thing to convert \a srcDimensions to \a dstDimensions. \a dstStrideBytes of \c 0 implies tightly packed data.
void copyData( uint8_t srcDimensions, const float *srcData, size_t numElements, uint8_t dstDimensions, size_t dstStrideBytes, float *dstData );

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

class Rect : public Source {
  public:
	//! Equivalent to Rectf( -0.5, -0.5, 0.5, 0.5 )
	Rect();
	Rect( const Rectf &r );

	Rect&		rect( const Rectf &r );
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
	std::array<vec2,4>		mPositions, mTexCoords;
	std::array<ColorAf,4>	mColors;

	static float	sNormals[4*3];
};

class Cube : public Source {
  public:
	Cube();

	//! Enables default colors. Disabled by default.
	Cube&			colors();
	//! Enables per-face colors ordered { +X, -X, +Y, -Y, +Z, -Z }. Colors are disabled by default.
	Cube&			colors( const ColorAf &posX, const ColorAf &negX, const ColorAf &posY, const ColorAf &negY, const ColorAf &posZ, const ColorAf &negZ );
	//! Disables colors. Disabled by default.
	Cube&			disableColors();

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
	ivec3		mSubdivisions;
	vec3		mSize;
	bool		mHasColors;
	ColorAf		mColors[6];
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
	void		calculate( std::vector<vec3> *positions, std::vector<vec3> *normals, std::vector<vec3> *colors, std::vector<uint32_t> *indices ) const;

	bool			mHasColors;
	static float	sPositions[12*3];
	static uint32_t	sIndices[60];
	
	friend class Icosphere;
};

class Icosphere : public Source {
  public:
	Icosphere();

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

	Sphere&		colors() { mHasColors = true; return *this; }
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
	void			numRingsAndSegments( int *numRings, int *numSegments ) const;

	vec3		mCenter;
	float		mRadius;
	int			mSubdivisions;
	bool		mHasColors;
};

class Capsule : public Source {
  public:
	Capsule();

	//! Enables colors. Disabled by default.
	Capsule&		colors() { mHasColors = true; return *this; }
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
	//! Enables colors. Disabled by default.
	Cylinder&	colors() { mHasColors = true; return *this; }

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
	Cone() { radius( 1.0f, 0.0f ); }

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
	//! Specifies the apex radius as a \a ratio of the height. A value of 1.0f yields a cone angle of 45 degrees.
	Cone&	ratio( float ratio ) { mRadiusApex = math<float>::max( mRadiusBase + ratio * mHeight, 0.f ); return *this; }
	//! Specifies the base and apex radius separately.
	Cone&	radius( float base, float apex ) { mRadiusBase = math<float>::max(0.f, base); mRadiusApex = math<float>::max(0.f, apex); return *this; }
	Cone&	direction( const vec3 &direction ) { Cylinder::direction( direction ); return *this; }
	//! Conveniently sets origin, height and direction.
	Cone&	set( const vec3 &from, const vec3 &to ) { Cylinder::set( from, to ); return *this; }
	//! Enables colors. Disabled by default.
	Cone&	colors() { mHasColors = true; return *this; }
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

//////////////////////////////////////////////////////////////////////////////////////
// Modifiers

// By default, attributes pass through the Modifier from the input source -> target
// READ attributes values are captured from mSource, typically to derive other attributes from, and then are passed through
// WRITE attributes prevent the passing of the attribute data from source -> target, to allow the owner of the Modifier to write it later
// READ_WRITE attributes are captured but not passed through to the target
class Modifier : public geom::Target {
  public:
	typedef enum { READ, WRITE, READ_WRITE, IGNORED } Access;

	Modifier( const geom::Source &source, geom::Target *target, const std::map<Attrib,Access> &attribs, Access indicesAccess )
		: mSource( source ), mTarget( target ), mAttribs( attribs ), mIndicesAccess( indicesAccess ), mNumIndices( 0 )
	{}

	uint8_t		getAttribDims( geom::Attrib attr ) const override;
	
	void copyAttrib( Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count ) override;
	void copyIndices( Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex ) override;
	
	uint8_t	getReadAttribDims( Attrib attr ) const;
	// not const because consumer is allowed to overwrite this data
	float* getReadAttribData( Attrib attr ) const;

	size_t			getNumIndices() const { return mNumIndices; }
	const uint32_t*	getIndicesData() const { return mIndices.get(); }
		
  protected:
	const geom::Source		&mSource;
	geom::Target			*mTarget;
	
	std::map<Attrib,Access>						mAttribs;
	std::map<Attrib,std::unique_ptr<float[]>>	mAttribData;
	std::map<Attrib,uint8_t>					mAttribDims;
	
	Access									mIndicesAccess;
	std::unique_ptr<uint32_t[]>				mIndices;
	size_t									mNumIndices;
	geom::Primitive							mPrimitive;
};

//! "Bakes" a mat4 transformation into the positions and normals of a geom::Source
class Transform : public Source {
  public:
	//! Does not currently support a projection matrix (i.e. doesn't divide by 'w' )
	Transform( const geom::Source &source, const mat4 &transform )
		: mSource( source ), mTransform( transform )
	{}

	const mat4&			getMatrix() const { return mTransform; }
	void				setMatrix( const mat4 &transform ) { mTransform = transform; }
  
  	size_t		getNumVertices() const override		{ return mSource.getNumVertices(); }
	size_t		getNumIndices() const override		{ return mSource.getNumIndices(); }
	Primitive	getPrimitive() const override		{ return mSource.getPrimitive(); }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;
	
	const geom::Source&		mSource;
	mat4					mTransform;
};

//! Twists a geom::Source around a given axis
class Twist : public Source {
  public:
	Twist( const geom::Source &source )
		: mSource( source ), mAxisStart( 0, -1, 0 ), mAxisEnd( 0, 1, 0 ),
			mStartAngle( (float)-M_PI ), mEndAngle( (float)M_PI )
	{}

	Twist&		axisStart( const vec3 &start ) { mAxisStart = start; return *this; }
	Twist&		axisEnd( const vec3 &end ) { mAxisEnd = end; return *this; }
	Twist&		axis( const vec3 &start, const vec3 &end ) { mAxisStart = start; mAxisEnd = end; return *this; }
	Twist&		startAngle( float radians ) { mStartAngle = radians; return *this; }
	Twist&		endAngle( float radians ) { mEndAngle = radians; return *this; }
  
  	size_t		getNumVertices() const override		{ return mSource.getNumVertices(); }
	size_t		getNumIndices() const override		{ return mSource.getNumIndices(); }
	Primitive	getPrimitive() const override		{ return mSource.getPrimitive(); }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;
	
  protected:
	const geom::Source&		mSource;
	vec3					mAxisStart, mAxisEnd;
	float					mStartAngle, mEndAngle;
};

//! Converts any geom::Source to equivalent vertices connected by lines. Output primitive type is always geom::Primitive::LINES.
class Lines : public Source {
  public:
	Lines( const geom::Source &source )
		: mSource( source )
	{}

	size_t		getNumVertices() const override				{ return mSource.getNumVertices(); }
	size_t		getNumIndices() const override;
	Primitive	getPrimitive() const override				{ return geom::LINES; }
	uint8_t		getAttribDims( Attrib attr ) const override	{ return mSource.getAttribDims( attr ); }
	AttribSet	getAvailableAttribs() const override		{ return mSource.getAvailableAttribs(); }
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;
	
  protected:
	const geom::Source&		mSource;
};

//! Modifiers the color of a geom::Source as a function of a 2D or 3D input attribute
class ColorFromAttrib : public Source {
  public:
	ColorFromAttrib( const geom::Source &source, Attrib attrib, const std::function<Colorf(vec2)> &fn )
		: mSource( source ), mAttrib( attrib ), mFnColor2( fn )
	{}
	ColorFromAttrib( const geom::Source &source, Attrib attrib, const std::function<Colorf(vec3)> &fn )
		: mSource( source ), mAttrib( attrib ), mFnColor3( fn )
	{}
	
	Attrib				getAttrib() const { return mAttrib; }
	ColorFromAttrib&	attrib( Attrib attrib ) { mAttrib = attrib; return *this; }

	size_t		getNumVertices() const override				{ return mSource.getNumVertices(); }
	size_t		getNumIndices() const override				{ return mSource.getNumIndices(); }
	Primitive	getPrimitive() const override				{ return mSource.getPrimitive(); }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;
	
  protected:
	const geom::Source&				mSource;
	Attrib							mAttrib;
	std::function<Colorf(vec2)>		mFnColor2;
	std::function<Colorf(vec3)>		mFnColor3;
};

//! Maps an attribute as a function of another attribute. Valid types are: float, vec2, vec3, vec4
template<typename S, typename D>
class AttribFn : public Source {
  public:
	typedef typename std::function<D(S)> FN;
	static const int SRCDIM = sizeof(S)/ sizeof(float);
	static const int DSTDIM = sizeof(D)/ sizeof(float);
	
	AttribFn( const Source &source, Attrib src, Attrib dst, const FN &fn )
		: mSource( source ), mSrcAttrib( src ), mDstAttrib( dst ), mFn( fn )
	{}
	
	size_t		getNumVertices() const override				{ return mSource.getNumVertices(); }
	size_t		getNumIndices() const override				{ return mSource.getNumIndices(); }
	Primitive	getPrimitive() const override				{ return mSource.getPrimitive(); }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;
	
  protected:
	const geom::Source	&mSource;
	geom::Attrib		mSrcAttrib, mDstAttrib;
	FN					mFn;
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

//! Draws lines representing the Attrib::NORMALs for a geom::Source. Encodes 0 for base and 1 for normal into CUSTOM_0. Prevents pass-through of NORMAL and COLOR. Passes CI_TEX_COORD_0
class VertexNormalLines : public Source {
  public:
	VertexNormalLines( const geom::Source &source, float length );

	VertexNormalLines&	length( float len ) { mLength = len; return *this; }

	size_t		getNumVertices() const override;
	size_t		getNumIndices() const override				{ return 0; }
	Primitive	getPrimitive() const override				{ return geom::LINES; }
	uint8_t		getAttribDims( Attrib attr ) const override;
	AttribSet	getAvailableAttribs() const override;
	void		loadInto( Target *target, const AttribSet &requestedAttribs ) const override;
	
  protected:
	const geom::Source&		mSource;
	float					mLength;
};

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
