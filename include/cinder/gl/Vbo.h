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

#pragma once

#include "cinder/gl/gl.h"
#include "cinder/TriMesh.h"

#include <vector>
#include <utility>

namespace cinder { namespace gl {

class Vbo {
 public:
	Vbo() {}
	Vbo( GLenum aTarget );
	
	void		bind();
	void		unbind();
	
	void		bufferData( size_t size, const void *data, GLenum usage );
	void		bufferSubData( ptrdiff_t offset, size_t size, const void *data );
	
	uint8_t*	map( GLenum access );
	void		unmap();

	GLenum		getTarget() const { return mObj->mTarget; }
	GLuint		getId() const { return mObj->mId; }
	
 protected:
	struct Obj {
		Obj( GLenum aTarget );
		~Obj();

		GLenum			mTarget;
		GLuint			mId;
	};
	
	shared_ptr<Obj>	mObj;

  public:
	//@{
	//! Emulates shared_ptr-like behavior
	typedef shared_ptr<Obj> Vbo::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &Vbo::mObj; }
	void reset() { mObj.reset(); }
	//@}  
};

class VboMesh {
 public:
	enum { NONE, STATIC, DYNAMIC };
	enum { ATTR_INDICES, ATTR_POSITIONS, ATTR_NORMALS, ATTR_COLORS_RGB, ATTR_COLORS_RGBA, ATTR_TEXCOORDS2D_0, ATTR_TEXCOORDS2D_1, ATTR_TEXCOORDS2D_2, ATTR_TEXCOORDS2D_3, ATTR_TEXCOORDS3D_0, ATTR_TEXCOORDS3D_1, ATTR_TEXCOORDS3D_2, ATTR_TEXCOORDS3D_3, ATTR_TOTAL };
	enum { ATTR_MAX_TEXTURE_UNIT = 3 };

	struct Layout {
		Layout() { initAttributes(); }

		//! \return is the Layout unspecified, presumably TBG by a constructor for VboMesh
		bool	isDefaults() const { for( int a = 0; a < ATTR_TOTAL; ++a ) if( mAttributes[a] != NONE ) return false; return true; }
	
		bool	hasNormals() const { return hasDynamicNormals() || hasStaticNormals(); }
		bool	hasStaticNormals() const { return mAttributes[ATTR_NORMALS] == STATIC; }
		bool	hasDynamicNormals() const { return mAttributes[ATTR_NORMALS] == DYNAMIC; }
		void	setStaticNormals() { mAttributes[ATTR_NORMALS] = STATIC; }
		void	setDynamicNormals() { mAttributes[ATTR_NORMALS] = DYNAMIC; }		

		bool	hasColorsRGB() const { return hasDynamicColorsRGB() || hasStaticColorsRGB(); }
		bool	hasStaticColorsRGB() const { return mAttributes[ATTR_COLORS_RGB] == STATIC; }
		bool	hasDynamicColorsRGB() const { return mAttributes[ATTR_COLORS_RGB] == DYNAMIC; }
		void	setStaticColorsRGB() { mAttributes[ATTR_COLORS_RGB] = STATIC; mAttributes[ATTR_COLORS_RGBA] = NONE; }
		void	setDynamicColorsRGB() { mAttributes[ATTR_COLORS_RGB] = DYNAMIC; mAttributes[ATTR_COLORS_RGBA] = NONE; }		

		bool	hasColorsRGBA() const { return hasDynamicColorsRGBA() || hasStaticColorsRGBA(); }
		bool	hasStaticColorsRGBA() const { return mAttributes[ATTR_COLORS_RGBA] == STATIC; }
		bool	hasDynamicColorsRGBA() const { return mAttributes[ATTR_COLORS_RGBA] == DYNAMIC; }
		void	setStaticColorsRGBA() { mAttributes[ATTR_COLORS_RGBA] = STATIC; mAttributes[ATTR_COLORS_RGB] = NONE; }
		void	setDynamicColorsRGBA() { mAttributes[ATTR_COLORS_RGBA] = DYNAMIC; mAttributes[ATTR_COLORS_RGB] = NONE; }		
		
		bool	hasTexCoords2d( size_t unit = 0 ) const { return hasDynamicTexCoords2d( unit ) || hasStaticTexCoords2d( unit ); }
		bool	hasStaticTexCoords2d( size_t unit = 0 ) const { return mAttributes[ATTR_TEXCOORDS2D_0 + unit] == STATIC; }		
		bool	hasDynamicTexCoords2d( size_t unit = 0 ) const { return mAttributes[ATTR_TEXCOORDS2D_0 + unit] == DYNAMIC; }
		void	setStaticTexCoords2d( size_t unit = 0 ) { mAttributes[ATTR_TEXCOORDS2D_0 + unit] = STATIC; mAttributes[ATTR_TEXCOORDS3D_0 + unit] = NONE; }
		void	setDynamicTexCoords2d( size_t unit = 0 ) { mAttributes[ATTR_TEXCOORDS2D_0 + unit] = DYNAMIC; mAttributes[ATTR_TEXCOORDS3D_0 + unit] = NONE; }
		//! \return are there any texture units with static texCoords
		bool	hasStaticTexCoords() const;
		//! \return are there any texture units with dynamic texCoords
		bool	hasDynamicTexCoords() const;
		//! \return if texture unit \arg unit is enabled
		bool	hasTexCoords( size_t unit ) const { return ( mAttributes[ATTR_TEXCOORDS2D_0 + unit] != NONE ) || ( mAttributes[ATTR_TEXCOORDS3D_0 + unit] != NONE ); }

		bool	hasTexCoords3d( size_t unit = 0 ) const { return hasDynamicTexCoords3d( unit ) || hasStaticTexCoords3d( unit ); }
		bool	hasStaticTexCoords3d( size_t unit = 0 ) const { return mAttributes[ATTR_TEXCOORDS3D_0 + unit] == STATIC; }		
		bool	hasDynamicTexCoords3d( size_t unit = 0 ) const { return mAttributes[ATTR_TEXCOORDS3D_0 + unit] == DYNAMIC; }
		void	setStaticTexCoords3d( size_t unit = 0 ) { mAttributes[ATTR_TEXCOORDS3D_0 + unit] = STATIC; mAttributes[ATTR_TEXCOORDS2D_0 + unit] = NONE; }
		void	setDynamicTexCoords3d( size_t unit = 0 ) { mAttributes[ATTR_TEXCOORDS3D_0 + unit] = DYNAMIC; mAttributes[ATTR_TEXCOORDS2D_0 + unit] = NONE; }

		bool	hasIndices() const { return hasStaticIndices() || hasDynamicIndices(); }
		bool	hasStaticIndices() const { return mAttributes[ATTR_INDICES] == STATIC; }
		bool	hasDynamicIndices() const { return mAttributes[ATTR_INDICES] == DYNAMIC; }
		void	setStaticIndices() { mAttributes[ATTR_INDICES] = STATIC; }
		void	setDynamicIndices() { mAttributes[ATTR_INDICES] = DYNAMIC; }

		bool	hasPositions() const { return hasStaticPositions() || hasDynamicPositions(); }
		bool	hasStaticPositions() const { return mAttributes[ATTR_POSITIONS] == STATIC; }
		bool	hasDynamicPositions() const { return mAttributes[ATTR_POSITIONS] == DYNAMIC; }
		void	setStaticPositions() { mAttributes[ATTR_POSITIONS] = STATIC; }
		void	setDynamicPositions() { mAttributes[ATTR_POSITIONS] = DYNAMIC; }
		
		enum CustomAttr { CUSTOM_ATTR_FLOAT, CUSTOM_ATTR_FLOAT2, CUSTOM_ATTR_FLOAT3, CUSTOM_ATTR_FLOAT4, TOTAL_CUSTOM_ATTR_TYPES };
		static int sCustomAttrSizes[TOTAL_CUSTOM_ATTR_TYPES];
		static GLint sCustomAttrNumComponents[TOTAL_CUSTOM_ATTR_TYPES];
		static GLenum sCustomAttrTypes[TOTAL_CUSTOM_ATTR_TYPES];
		void	addDynamicCustomFloat() { mCustomDynamic.push_back( std::make_pair( CUSTOM_ATTR_FLOAT, 0 ) ); }
		void	addDynamicCustomVec2f() { mCustomDynamic.push_back( std::make_pair( CUSTOM_ATTR_FLOAT2, 0 ) ); }
		void	addDynamicCustomVec3f() { mCustomDynamic.push_back( std::make_pair( CUSTOM_ATTR_FLOAT3, 0 ) ); }
		void	addDynamicCustomVec4f() { mCustomDynamic.push_back( std::make_pair( CUSTOM_ATTR_FLOAT4, 0 ) ); }

		int												mAttributes[ATTR_TOTAL];
		std::vector<std::pair<CustomAttr,size_t> >		mCustomDynamic, mCustomStatic; // pair of <types,offset>
		
	 private:
		void initAttributes() { for( int a = 0; a < ATTR_TOTAL; ++a ) mAttributes[a] = NONE; }
	};

	enum			{ INDEX_BUFFER = 0, STATIC_BUFFER, DYNAMIC_BUFFER, TOTAL_BUFFERS };
	
  protected:
	struct Obj {
		size_t			mNumIndices, mNumVertices;	

		Vbo				mBuffers[TOTAL_BUFFERS];
		size_t			mPositionOffset;
		size_t			mNormalOffset;
		size_t			mColorRGBOffset, mColorRGBAOffset;		
		size_t			mTexCoordOffset[ATTR_MAX_TEXTURE_UNIT+1];
		size_t			mStaticStride, mDynamicStride;	
		GLenum			mPrimitiveType;
		Layout			mLayout;
		std::vector<GLint>		mCustomStaticLocations;
		std::vector<GLint>		mCustomDynamicLocations;
	};

  public:
	class VertexIter;
 
	VboMesh() {}
	explicit VboMesh( const TriMesh &triMesh, Layout layout = Layout() );
	/*** Creates a VboMesh with \a numVertices vertices and \a numIndices indices. Dynamic data is stored interleaved and static data is planar. **/
	VboMesh( size_t numVertices, size_t numIndices, Layout layout, GLenum primitiveType );
	/*** Creates a VboMesh with \a numVertices vertices and \a numIndices indices. Accepts pointers to preexisting buffers, which may be NULL to request allocation **/
	VboMesh( size_t numVertices, size_t numIndices, Layout layout, GLenum primitiveType, Vbo *indexBuffer, Vbo *staticBuffer, Vbo *dynamicBuffer );

	size_t	getNumIndices() const { return mObj->mNumIndices; }
	size_t	getNumVertices() const { return mObj->mNumVertices; }
	GLenum	getPrimitiveType() const { return mObj->mPrimitiveType; }
	
	const Layout&	getLayout() const { return mObj->mLayout; }

	void			bindIndexBuffer() const;
	void			enableClientStates() const;
	void			disableClientStates() const;
	void			bindAllData() const;
	static void		unbindBuffers();

	void						bufferIndices( const std::vector<uint32_t> &indices );
	void						bufferPositions( const std::vector<Vec3f> &positions );
	void						bufferPositions( const Vec3f *positions, size_t count );
	void						bufferNormals( const std::vector<Vec3f> &normals );
	void						bufferTexCoords2d( size_t unit, const std::vector<Vec2f> &texCoords );
	class VertexIter			mapVertexBuffer();

	Vbo&				getIndexVbo() const { return mObj->mBuffers[INDEX_BUFFER]; }
	Vbo&				getStaticVbo() const { return mObj->mBuffers[STATIC_BUFFER]; }
	Vbo&				getDynamicVbo() const { return mObj->mBuffers[DYNAMIC_BUFFER]; }

	void				setCustomStaticLocation( size_t internalIndex, GLuint location ) { mObj->mCustomStaticLocations[internalIndex] = location; }
	void				setCustomDynamicLocation( size_t internalIndex, GLuint location ) { mObj->mCustomDynamicLocations[internalIndex] = location; }

	size_t						getTexCoordOffset( size_t unit ) const { return mObj->mTexCoordOffset[unit]; }
	void						setTexCoordOffset( size_t unit, size_t aTexCoordOffset ) { mObj->mTexCoordOffset[unit] = aTexCoordOffset; }	

	//@{
	//! Emulates shared_ptr-like behavior
	typedef shared_ptr<Obj> VboMesh::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &VboMesh::mObj; }
	void reset() { mObj.reset(); }
	//@}

	class VertexIter {
	 public:
		VertexIter( const VboMesh &mesh );

		void	setPosition( const Vec3f &v ) { *(reinterpret_cast<Vec3f*>( &mPtr[mPositionOffset] )) = v; }
		void	setPosition( float x, float y, float z ) { *(reinterpret_cast<Vec3f*>( &mPtr[mPositionOffset] )) = Vec3f( x, y, z ); }
		void	setNormal( const Vec3f &n ) { *(reinterpret_cast<Vec3f*>( &mPtr[mNormalOffset] )) = n; }
		void	setColorRGB( const Color &n ) { *(reinterpret_cast<Color*>( &mPtr[mColorRGBOffset] )) = n; }
		void	setColorRGBA( const ColorA &n ) { *(reinterpret_cast<ColorA*>( &mPtr[mColorRGBAOffset] )) = n; }
		void	setTexCoord2d0( const Vec2f &t ) { *(reinterpret_cast<Vec2f*>( &mPtr[mTexCoordOffset[0]] )) = t; }
		void	setTexCoord3d0( const Vec3f &t ) { *(reinterpret_cast<Vec3f*>( &mPtr[mTexCoordOffset[0]] )) = t; }
		void	setTexCoord2d1( const Vec2f &t ) { *(reinterpret_cast<Vec2f*>( &mPtr[mTexCoordOffset[1]] )) = t; }
		void	setTexCoord3d1( const Vec3f &t ) { *(reinterpret_cast<Vec3f*>( &mPtr[mTexCoordOffset[1]] )) = t; }
		void	setTexCoord2d2( const Vec2f &t ) { *(reinterpret_cast<Vec2f*>( &mPtr[mTexCoordOffset[2]] )) = t; }
		void	setTexCoord3d2( const Vec3f &t ) { *(reinterpret_cast<Vec3f*>( &mPtr[mTexCoordOffset[2]] )) = t; }
		void	setCustomFloat( size_t index, float v ) { *(reinterpret_cast<float*>( &mPtr[mObj->mCustomOffsets[index]] )) = v; }
		void	setCustomVec2f( size_t index, const Vec2f &v ) { *(reinterpret_cast<Vec2f*>( &mPtr[mObj->mCustomOffsets[index]] )) = v; }
		void	setCustomVec3f( size_t index, const Vec3f &v ) { *(reinterpret_cast<Vec3f*>( &mPtr[mObj->mCustomOffsets[index]] )) = v; }
		void	setCustomVec4f( size_t index, const Vec4f &v ) { *(reinterpret_cast<Vec4f*>( &mPtr[mObj->mCustomOffsets[index]] )) = v; }

		void operator++() { mPtr += mStride; }
		bool	isDone() const { return mPtr < mDataEnd; }
		
		//! \return Which vertex the iterator is pointing to
		size_t		getIndex() const { return ( mPtr - mData ) / mStride; }
		//! \return Which vertex the iterator is pointing to
		size_t		getStride() const { return mStride; }
		//! \return Raw pointer where the iterator is currently writing
		void*		getPointer() const { return reinterpret_cast<void*>( mPtr ); }
		//! \return pointer where the iterator is currently writing positions
		Vec3f*		getPositionPointer() const { return reinterpret_cast<Vec3f*>( &mPtr[mPositionOffset] ); }		

//		VertexIter( const VertexIter &other ) { set( other ); }	
//		VertexIter& operator=( const VertexIter &other ) { set( other ); return *this; }
		
	 protected:
		void set( const VertexIter &other );

		struct Obj {
		 public:
			Obj( const VboMesh &mesh );
			~Obj();
			
			uint8_t					*mData, *mDataEnd;
			std::vector<size_t>		mCustomOffsets;			
			Vbo						mVbo;
		};
	 	 
		shared_ptr<Obj>		mObj;
		uint8_t				*mPtr;
		uint8_t				*mData, *mDataEnd; // we cache these from the Obj to reduce dereferencing
		size_t				mPositionOffset, mNormalOffset;
		size_t				mColorRGBOffset, mColorRGBAOffset;
		size_t				mTexCoordOffset[ATTR_MAX_TEXTURE_UNIT+1];
		uint8_t				mStride;
	};

 protected:
	void	initializeBuffers( bool staticDataPlanar );

	shared_ptr<Obj>		mObj;
};

class VboExc : public std::exception {
 public:
	virtual const char* what() const throw() { return "OpenGL Vbo exception"; }
};

class VboInvalidTargetExc : public VboExc {
 public:
	virtual const char* what() const throw() { return "OpenGL Vbo exception: Invalid Target"; }
};

class VboFailedMapExc : public VboExc {
 public:
	virtual const char* what() const throw() { return "OpenGL Vbo exception: Map failure"; } 
};

class VboFailedUnmapExc : public VboExc {
 public:
	virtual const char* what() const throw() { return "OpenGL Vbo exception: Unmap failure"; } 
};

} } // namespace cinder::gl
