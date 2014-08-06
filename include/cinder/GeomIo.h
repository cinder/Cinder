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
#include "cinder/BSpline.h"

#include <set>

namespace cinder { namespace geom {

class Target;
typedef std::shared_ptr<class Source>	SourceRef;

// keep this incrementing by 1 only; some code relies on that for iterating; add corresponding entry to sAttribNames
enum Attrib { POSITION, COLOR, TEX_COORD_0, TEX_COORD_1, TEX_COORD_2, TEX_COORD_3,
	NORMAL, TANGENT, BITANGENT, BONE_INDEX, BONE_WEIGHT, 
	CUSTOM_0, CUSTOM_1, CUSTOM_2, CUSTOM_3, CUSTOM_4, CUSTOM_5, CUSTOM_6, CUSTOM_7, CUSTOM_8, CUSTOM_9,
	NUM_ATTRIBS };
extern std::string sAttribNames[(int)Attrib::NUM_ATTRIBS];
enum Primitive { LINES, TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN }; 
enum DataType { FLOAT, INTEGER, DOUBLE };

//! Debug utility which returns the name of \a attrib as a std::string
std::string attribToString( Attrib attrib );

struct AttribInfo {
	AttribInfo( const Attrib &attrib, uint8_t dims, size_t stride, size_t offset, uint32_t instanceDivisor = 0 )
		: mAttrib( attrib ), mDims( dims ), mDataType( DataType::FLOAT ), mStride( stride ), mOffset( offset ), mInstanceDivisor( instanceDivisor )
	{}
	AttribInfo( const Attrib &attrib, DataType dataType, uint8_t dims, size_t stride, size_t offset, uint32_t instanceDivisor = 0 )
		: mAttrib( attrib ), mDims( dims ), mDataType( dataType ), mStride( stride ), mOffset( offset ), mInstanceDivisor( instanceDivisor )
	{}

	Attrib		getAttrib() const { return mAttrib; }
	uint8_t		getDims() const { return mDims; }
	DataType	getDataType() const { return mDataType; }
	size_t		getStride() const { return mStride; }
	size_t		getOffset() const { return mOffset;	}
	uint32_t	getInstanceDivisor() const { return mInstanceDivisor; }

	void		setStride( size_t stride ) { mStride = stride; }
	void		setOffset( size_t offset ) { mOffset = offset; }

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
protected:
	std::vector<AttribInfo>		mAttribs;
};

void copyData( uint8_t srcDimensions, const float *srcData, size_t numElements, uint8_t dstDimensions, size_t dstStrideBytes, float *dstData );

class Source {
public:
	virtual size_t		getNumVertices() const = 0;
	virtual size_t		getNumIndices() const = 0;
	virtual Primitive	getPrimitive() const = 0;
	virtual uint8_t		getAttribDims( Attrib attr ) const = 0;

	virtual void		loadInto( Target *target ) const = 0;

	virtual void		clearAttribs() { mEnabledAttribs.clear(); }
	virtual Source&		enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); return *this; }
	virtual Source&		disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); return *this; }
	virtual bool		isEnabled( Attrib attrib ) const;

protected:  
	static void	copyDataMultAdd( const float *srcData, size_t numElements, uint8_t dstDimensions, size_t dstStrideBytes, float *dstData, const Vec2f &mult, const Vec2f &add );
	static void	copyDataMultAdd( const float *srcData, size_t numElements, uint8_t dstDimensions, size_t dstStrideBytes, float *dstData, const Vec3f &mult, const Vec3f &add );

	//! Builds a sequential list of vertices to simulate an indexed geometry when Source is non-indexed. Assumes \a dest contains storage for getNumVertices() entries
	void	copyIndicesNonIndexed( uint16_t *dest ) const;
	//! Builds a sequential list of vertices to simulate an indexed geometry when Source is non-indexed. Assumes \a dest contains storage for getNumVertices() entries
	void	copyIndicesNonIndexed( uint32_t *dest ) const;
	template<typename T>
	void forceCopyIndicesTrianglesImpl( T *dest ) const;

	std::set<Attrib>	mEnabledAttribs;
};

class Target {
public:
	virtual Primitive	getPrimitive() const = 0;
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
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL
	Rect();

	virtual Rect&		enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); return *this; }
	virtual Rect&		disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); return *this; }	
	Rect&		position( const Vec2f &pos ) { mPos = pos; return *this; }
	Rect&		scale( const Vec2f &scale ) { mScale = scale; return *this; }
	Rect&		scale( float s ) { mScale = Vec2f( s, s ); return *this; }

	virtual size_t		getNumVertices() const override { return 4; }
	virtual size_t		getNumIndices() const override { return 0; }
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLE_STRIP; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;

	Vec2f		mPos, mScale;

	static float	sPositions[4*2];
	static float	sColors[4*3];
	static float	sTexCoords[4*2];
	static float	sNormals[4*3];
};

class Cube : public Source {
public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL
	Cube();

	virtual Cube&	enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); return *this; }
	virtual Cube&	disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); return *this; }

	virtual size_t		getNumVertices() const override { return 24; }
	virtual size_t		getNumIndices() const override { return 36; }
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;

protected:
	static float	sPositions[24*3];
	static float	sColors[24*3];
	static float	sTexCoords[24*2];
	static float	sNormals[24*3];

	static uint32_t	sIndices[36];
};

class Icosahedron : public Source {
public:
	//! Defaults to having POSITION and NORMAL. Supports COLOR
	Icosahedron();
	virtual ~Icosahedron() {}

	virtual Icosahedron&	enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Icosahedron&	disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }

	virtual size_t		getNumVertices() const override { calculate(); return mPositions.size(); }
	virtual size_t		getNumIndices() const override { calculate(); return mIndices.size(); }
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;

protected:
	virtual void		calculate() const;

	mutable bool						mCalculationsCached;
	mutable std::vector<Vec3f>			mPositions;
	mutable std::vector<Vec3f>			mNormals;
	mutable std::vector<Vec3f>			mColors;
	mutable std::vector<uint32_t>		mIndices;

	static float	sPositions[12*3];

	static uint32_t	sIndices[60];
};

class Teapot : public Source {
public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL
	Teapot();

	virtual Teapot&		enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Teapot&		disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }
	Teapot&				subdivision( int sub );

	virtual size_t		getNumVertices() const override;
	virtual size_t		getNumIndices() const override;
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	virtual void		loadInto( Target *target ) const override;
	virtual uint8_t		getAttribDims( Attrib attr ) const override;

protected:
	void			calculate() const;
	void			updateVertexCounts() const;

	static void		generatePatches( float *v, float *n, float *tc, uint32_t *el, int grid );
	static void		buildPatchReflect( int patchNum, float *B, float *dB, float *v, float *n, float *tc, unsigned int *el,
		int &index, int &elIndex, int &tcIndex, int grid, bool reflectX, bool reflectY );
	static void		buildPatch( Vec3f patch[][4], float *B, float *dB, float *v, float *n, float *tc, 
		unsigned int *el, int &index, int &elIndex, int &tcIndex, int grid, const Matrix33f reflect, bool invertNormal );
	static void		getPatch( int patchNum, Vec3f patch[][4], bool reverseV );
	static void		computeBasisFunctions( float *B, float *dB, int grid );
	static Vec3f	evaluate( int gridU, int gridV, const float *B, const Vec3f patch[][4] );
	static Vec3f	evaluateNormal( int gridU, int gridV, const float *B, const float *dB, const Vec3f patch[][4] );

	int			mSubdivision;

	mutable bool					mCalculationsCached;
	mutable	size_t					mNumVertices;
	mutable size_t					mNumIndices;
	mutable std::vector<float>		mPositions;
	mutable std::vector<float>		mTexCoords;
	mutable std::vector<float>		mNormals;
	mutable std::vector<uint32_t>	mIndices;

	static const uint8_t	sPatchIndices[][16];
	static const float		sCurveData[][3];
};

class Circle : public Source {
public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL
	Circle();

	virtual Circle&	enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); return *this; }
	virtual Circle&	disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); return *this; }
	Circle&		center( const Vec2f &center ) { mCenter = center; return *this; }
	Circle&		radius( float radius );
	Circle&		segments( int segments );

	virtual void		loadInto( Target *target ) const override;
	virtual size_t		getNumVertices() const override;
	virtual size_t		getNumIndices() const override { return 0; }
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLE_FAN; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;

private:
	void	updateVertexCounts();
	void	calculate() const;

	Vec2f		mCenter;
	float		mRadius;
	int			mRequestedSegments, mNumSegments;

	size_t						mNumVertices;
	mutable std::vector<Vec2f>	mPositions;
	mutable std::vector<Vec2f>	mTexCoords;
	mutable std::vector<Vec3f>	mNormals;
};

class Sphere : public Source {
public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL. Supports COLOR
	Sphere();
	virtual ~Sphere() {}

	virtual Sphere&	enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Sphere&	disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }
	Sphere&		center( const Vec3f &center ) { mCenter = center; mCalculationsCached = false; return *this; }
	Sphere&		radius( float radius ) { mRadius = radius; mCalculationsCached = false; return *this; }
	//! Specifies the number of segments, which determines the roundness of the sphere.
	Sphere&		segments( int segments ) { mNumSegments = segments; mCalculationsCached = false; return *this; }

	virtual size_t		getNumVertices() const override;
	virtual size_t		getNumIndices() const override;
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;

protected:
	virtual void		calculate() const;
	virtual void		calculateImplUV( size_t segments, size_t rings ) const;

	Vec3f		mCenter;
	float		mRadius;
	int			mNumSegments;
	int			mNumSlices;

	mutable bool						mCalculationsCached;
	mutable std::vector<Vec3f>			mPositions;
	mutable std::vector<Vec2f>			mTexCoords;
	mutable std::vector<Vec3f>			mNormals;
	mutable std::vector<Vec3f>			mColors;
	mutable std::vector<uint32_t>		mIndices;
};

class Icosphere : public Icosahedron {
public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL. Supports COLOR
	Icosphere();

	virtual Icosphere&	enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Icosphere&	disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }
	Icosphere&			subdivision( int sub ) { mSubdivision = (sub > 0) ? (sub + 1) : 1; mCalculationsCached = false; return *this; }

	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;

protected:
	virtual void		calculate() const;
	virtual void		calculateImplUV() const;
	virtual void		subdivide() const;

	int									mSubdivision;

	mutable bool						mCalculationsCached;
	mutable std::vector<Vec2f>			mTexCoords;
};

class Capsule : public Sphere {
public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL. Supports COLOR
	Capsule();

	virtual Capsule&	enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Capsule&	disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }
	Capsule&		center( const Vec3f &center ) { mCenter = center; mCalculationsCached = false; return *this; }
	//! Specifies the number of segments, which determines the roundness of the capsule.
	Capsule&		segments( int segments ) { mNumSegments = segments; mCalculationsCached = false; return *this; }
	//! Specifies the number of slices between the caps. Defaults to 6. Add more slices to improve texture mapping and lighting, or if you intend to bend the capsule.
	Capsule&		slices( int slices ) { mNumSlices = slices > 1 ? slices : 1; mCalculationsCached = false; return *this; }
	Capsule&		radius( float radius ) { mRadius = math<float>::max(0.f, radius); mCalculationsCached = false; return *this; }
	Capsule&		length( float length ) { mLength = math<float>::max(0.f, length); mCalculationsCached = false; return *this; }
	Capsule&		direction( const Vec3f &direction ) { mDirection = direction.normalized(); mCalculationsCached = false; return *this; }
	//! Conveniently sets center, length and direction
	Capsule&		set( const Vec3f &from, const Vec3f &to );

private:
	virtual void	calculate() const override;
	virtual void	calculateImplUV( size_t segments, size_t rings ) const override;
	void			calculateRing( size_t segments, float radius, float y, float dy ) const;

	Vec3f		mDirection;
	float		mLength;
};

class Torus : public Source {
public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL. Supports COLOR
	Torus();
	virtual ~Torus() {}

	virtual Torus&	enable( Attrib attrib ) override { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Torus&	disable( Attrib attrib ) override { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }
	virtual Torus&	center( const Vec3f &center ) { mCenter = center; mCalculationsCached = false; return *this; }
	virtual Torus&	segmentsAxis( int value ) { mNumSegmentsAxis = value; mCalculationsCached = false; return *this; }
	virtual Torus&	segmentsRing( int value ) { mNumSegmentsRing = value; mCalculationsCached = false; return *this; }
	//! Allows you to twist the torus along the ring.
	virtual Torus&	twist( unsigned twist ) { mTwist = twist; mCalculationsCached = false; return *this; }
	//! Allows you to twist the torus along the ring. The \a offset is in radians.
	virtual Torus&	twist( unsigned twist, float offset ) { mTwist = twist; mTwistOffset = offset; mCalculationsCached = false; return *this; }
	//! Specifies the major and minor radius as a ratio (minor : major). Resulting torus will fit unit cube.
	virtual Torus&	ratio( float ratio ) {
		ratio = math<float>::clamp( ratio );
		mRadiusMajor = 1.f;
		mRadiusMinor = 1.f - ratio;
		mCalculationsCached = false; return *this; }
	//! Specifies the major and minor radius separately.
	virtual Torus&	radius( float major, float minor ) { 
		mRadiusMajor = math<float>::max(0.f, major); 
		mRadiusMinor = math<float>::max(0.f, minor); 
		mCalculationsCached = false; return *this; }

	virtual size_t		getNumVertices() const override { calculate(); return mPositions.size(); }
	virtual size_t		getNumIndices() const override { calculate(); return mIndices.size(); }
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;

protected:
	void			calculate() const;
	void			calculateImplUV( size_t segments, size_t rings ) const;

	Vec3f		mCenter;
	float		mRadiusMajor;
	float		mRadiusMinor;
	int			mNumSegmentsAxis;
	int			mNumSegmentsRing;
	float		mHeight;
	float		mCoils;
	unsigned	mTwist;
	float		mTwistOffset;

	mutable bool						mCalculationsCached;
	mutable std::vector<Vec3f>			mPositions;
	mutable std::vector<Vec2f>			mTexCoords;
	mutable std::vector<Vec3f>			mNormals;
	mutable std::vector<Vec3f>			mColors;
	mutable std::vector<uint32_t>		mIndices;
};

class Helix : public Torus {
public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL. Supports COLOR
	Helix()
	{
		height(2.0f);
		coils(3.0f);
	}

	virtual Helix&	enable( Attrib attrib ) override { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Helix&	disable( Attrib attrib ) override { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }
	virtual Helix&	center( const Vec3f &center ) override { Torus::center( center ); return *this; }
	virtual Helix&	segmentsAxis( int value ) override { Torus::segmentsAxis( value ); return *this; }
	virtual Helix&	segmentsRing( int value ) override { Torus::segmentsRing( value ); return *this; }
	//! Specifies the height, measured from center to center.
	Helix&			height( float height ) { mHeight = height; mCalculationsCached = false; return *this; }
	//! Specifies the number of coils.
	Helix&			coils( float coils ) { mCoils = math<float>::max(0.f, coils); mCalculationsCached = false; return *this; }
	//! Allows you to twist the helix along the ring.
	virtual Helix&	twist( unsigned twist ) override { Torus::twist( twist ); return *this; }
	//! Allows you to twist the helix along the ring. The \a offset is in radians.
	virtual Helix&	twist( unsigned twist, float offset ) override { Torus::twist( twist, offset ); return *this; }
};

class Cylinder : public Source {
public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL. Supports COLOR
	Cylinder();
	virtual ~Cylinder() {}

	virtual Cylinder&	enable( Attrib attrib ) override { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Cylinder&	disable( Attrib attrib ) override { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }
	virtual Cylinder&	origin( const Vec3f &origin ) { mOrigin = origin; mCalculationsCached = false; return *this; }
	//! Specifies the number of segments, which determines the roundness of the cylinder.
	virtual Cylinder&	segments( int segments ) { mNumSegments = segments; mCalculationsCached = false; return *this; }
	//! Specifies the number of slices. Defaults to 6. Add more slices to improve texture mapping and lighting, or if you intend to bend the cylinder.
	virtual Cylinder&	slices( int slices ) { mNumSlices = slices; mCalculationsCached = false; return *this; }
	//! Specifies the height of the cylinder.
	virtual Cylinder&	height( float height ) { mHeight = height; mCalculationsCached = false; return *this; }
	//! Specifies the base and apex radius.
	virtual Cylinder&	radius( float radius ) { mRadiusBase = mRadiusApex = math<float>::max(0.f, radius); mCalculationsCached = false; return *this; }
	//! Specifies the axis of the cylinder.
	virtual Cylinder&	direction( const Vec3f &direction ) { mDirection = direction.normalized();  mCalculationsCached = false; return *this; }
	//! Conveniently sets origin, height and direction.
	virtual Cylinder&	set( const Vec3f &from, const Vec3f &to );

	virtual size_t		getNumVertices() const override { calculate(); return mPositions.size(); }
	virtual size_t		getNumIndices() const override { calculate(); return mIndices.size(); }
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;

protected:
	virtual void	calculate() const;
	virtual void	calculateImplUV( size_t segments, size_t rings ) const;
	virtual void	calculateCap( bool flip, float height, float radius, size_t segments ) const;

	Vec3f		mOrigin;
	float		mHeight;
	Vec3f		mDirection;
	float		mRadiusBase;
	float		mRadiusApex;
	int			mNumSegments;
	int			mNumSlices;

	mutable bool						mCalculationsCached;
	mutable std::vector<Vec3f>			mPositions;
	mutable std::vector<Vec2f>			mTexCoords;
	mutable std::vector<Vec3f>			mNormals;
	mutable std::vector<Vec3f>			mColors;
	mutable std::vector<uint32_t>		mIndices;
};

class Cone : public Cylinder {
public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL. Supports COLOR
	Cone() { radius( 1.0f, 0.0f ); }
	virtual ~Cone() {}

	virtual Cone&	enable( Attrib attrib ) override { Cylinder::enable( attrib ); return *this; }
	virtual Cone&	disable( Attrib attrib ) override { Cylinder::disable( attrib ); return *this; }
	virtual Cone&	origin( const Vec3f &origin ) override { Cylinder::origin( origin ); return *this; }
	//! Specifies the number of segments, which determines the roundness of the cone.
	virtual Cone&	segments( int segments ) override { Cylinder::segments( segments ); return *this; }
	//! Specifies the number of slices. Defaults to 6. Add more slices to improve texture mapping and lighting, or if you intend to bend the cone.
	virtual Cone&	slices( int slices ) override { Cylinder::slices( slices ); return *this; }
	virtual Cone&	height( float height ) override { Cylinder::height( height ); return *this; }
	//! Specifies the base and apex radius.
	virtual Cone&	radius( float radius ) override {  Cylinder::radius( radius ); return *this; }
	//! Specifies the base radius.
	Cone&	base( float base ) { mRadiusBase = math<float>::max( base, 0.f ); mCalculationsCached = false; return *this; }
	//! Specifies the apex radius.
	Cone&	apex( float apex ) { mRadiusApex = math<float>::max( apex, 0.f ); mCalculationsCached = false; return *this; }
	//! Specifies the apex radius as a \a ratio of the height. A value of 1.0f yields a cone angle of 45 degrees.
	Cone&	ratio( float ratio ) { mRadiusApex = math<float>::max( mRadiusBase + ratio * mHeight, 0.f ); mCalculationsCached = false; return *this; }
	//! Specifies the base and apex radius separately.
	Cone&	radius( float base, float apex ) { 
		mRadiusBase = math<float>::max(0.f, base); 
		mRadiusApex = math<float>::max(0.f, apex); 
		mCalculationsCached = false; return *this; }
	//!
	virtual Cone&	direction( const Vec3f &direction ) override { Cylinder::direction( direction ); return *this; }
	//! Conveniently sets origin, height and direction.
	virtual Cone&	set( const Vec3f &from, const Vec3f &to ) override { Cylinder::set( from, to ); return *this; }
};

#if 0
class SplineExtrusion : public Source {
public:
	SplineExtrusion( const std::function<Vec3f(float)> &pathCurve, int pathSegments, float radius, int radiusSegments );

	SplineExtrusion&		texCoords() { mHasTexCoord0 = true; return *this; }
	SplineExtrusion&		normals() { mHasNormals = true; return *this; }

	virtual size_t		getNumVertices() const override;
	virtual size_t		getNumIndices() const override;
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }

	virtual bool		hasAttrib( Attrib attr ) const override;
	virtual bool		canProvideAttrib( Attrib attr ) const override;
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		copyAttrib( Attrib attr, uint8_t dims, size_t stride, float *dest ) const override;

	virtual void		copyIndices( uint16_t *dest ) const override;
	virtual void		copyIndices( uint32_t *dest ) const override;	

protected:
	Vec2f		mPos, mScale;
	bool		mHasTexCoord0;
	bool		mHasNormals;

	void		calculateCurve( const std::function<Vec3f(float)> &pathCurve, int pathSegments, float radius, int radiusSegments );
	void		calculate();

	mutable bool						mCalculationsCached;
	mutable	int32_t						mNumVertices;
	mutable int32_t						mNumIndices;
	mutable std::unique_ptr<float[]>	mPositions;
	mutable std::unique_ptr<float[]>	mTexCoords;
	mutable std::unique_ptr<float[]>	mNormals;	
	mutable std::unique_ptr<uint32_t[]>	mIndices;
};
#endif

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
