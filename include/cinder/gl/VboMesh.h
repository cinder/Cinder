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

#include "cinder/gl/Vao.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/GlslProg.h"

#include "cinder/Color.h"
#include "cinder/Vector.h"
#include "cinder/TriMesh.h"
#include "cinder/GeomIo.h"

#include <iosfwd>
#include <vector>

namespace cinder { namespace gl {

class VboMesh;
typedef std::shared_ptr<VboMesh> VboMeshRef;
	
class CI_API VboMesh {
  public:
	class CI_API Layout {
	  public:
		Layout() : mUsage( GL_STATIC_DRAW ), mInterleave( true ) {}

		//! Specifies whether the data is stored planar or interleaved. Deafult is interleaved.
		Layout&		interleave( bool interleave = true ) { mInterleave = interleave; return *this; }
		//! Returns whether the Layout stores data as interleaved (rather than planar)
		bool		getInterleave() const { return mInterleave; }
		/** For Desktop GL, \c GL_STREAM_DRAW, \c GL_STREAM_READ, \c GL_STREAM_COPY, \c GL_STATIC_DRAW, \c GL_STATIC_READ, \c GL_STATIC_COPY, \c GL_DYNAMIC_DRAW, \c GL_DYNAMIC_READ, or \c GL_DYNAMIC_COPY.
			For ES 2, \c GL_STREAM_DRAW, \c GL_STATIC_DRAW, or \c GL_DYNAMIC_DRAW **/
		Layout&		usage( GLenum usage ) { mUsage = usage; return *this; }
		//! Returns the usage for the Layout. Default is \c GL_STATIC_DRAW
		GLenum		getUsage() const { return mUsage; }
		//! Appends an attribute of semantic \a attrib which is \a dims-dimensional. Replaces AttribInfo if it exists for \a attrib
		Layout&		attrib( geom::Attrib attrib, uint8_t dims );
		//! Appends an attribute using a geom::AttribInfo. Replaces AttribInfo if it exists for \a attribInfo.getAttrib()
		Layout&		attrib( const geom::AttribInfo &attribInfo );

		std::vector<geom::AttribInfo>&			getAttribs() { return mAttribInfos; }
		const std::vector<geom::AttribInfo>&	getAttribs() const { return mAttribInfos; }
		//! Clears all attributes in the Layout
		void									clearAttribs() { mAttribInfos.clear(); }

	  protected:
		//! If \a resultVbo is null then no VBO is allocated
		void		allocate( size_t numVertices, geom::BufferLayout *resultBufferLayout, gl::VboRef *resultVbo ) const;

		GLenum							mUsage;
		bool							mInterleave;
		std::vector<geom::AttribInfo>	mAttribInfos;

		friend VboMesh;
	};
  
	//! Creates a VboMesh which represents the geom::Source \a source. Layout is derived from the contents of \a source.
	static VboMeshRef	create( const geom::Source &source );
	//! Creates a VboMesh which represents the geom::Source \a source using \a layout.
	static VboMeshRef	create( const geom::Source &source, const geom::AttribSet &requestedAttribs );
	//! Creates a VboMesh which represents the geom::Source \a source using 1 or more VboMesh::Layouts for vertex data.
	static VboMeshRef	create( const geom::Source &source, const std::vector<VboMesh::Layout> &vertexArrayLayouts );
	//! Creates a VboMesh which represents the geom::Source \a source using 1 or more Vbo/VboMesh::Layout pairs. A null VboRef requests allocation.
	static VboMeshRef	create( const geom::Source &source, const std::vector<std::pair<VboMesh::Layout,VboRef>> &vertexArrayLayouts, const VboRef &indexVbo = nullptr );
	//! Creates a VboMesh which represents the user's vertex buffer objects. Allows optional \a indexVbo to enable indexed vertices; creates a static index VBO if none provided.
	static VboMeshRef	create( uint32_t numVertices, GLenum glPrimitive, const std::vector<std::pair<geom::BufferLayout,VboRef>> &vertexArrayBuffers, uint32_t numIndices = 0, GLenum indexType = GL_UNSIGNED_SHORT, const VboRef &indexVbo = VboRef() );
	//! Creates a VboMesh which represents the user's vertex buffer objects. Allows optional \a indexVbo to enable indexed vertices; creates a static index VBO if none provided.
	static VboMeshRef	create( uint32_t numVertices, GLenum glPrimitive, const std::vector<Layout> &vertexArrayLayouts, uint32_t numIndices = 0, GLenum indexType = GL_UNSIGNED_SHORT, const VboRef &indexVbo = VboRef() );

	//! Maps a geom::Attrib to a named attribute in the GlslProg
	typedef std::map<geom::Attrib,std::string> AttribGlslMap;
	//! Constructs a VAO (in the currently bound VAO) that matches \a this to GlslProg \a shader, overriding the mapping of a geom::Attrib to a named attribute via the 'a attributeMapping std::map
	void		buildVao( const GlslProgRef &shader, const AttribGlslMap &attributeMapping = AttribGlslMap() );
	void		buildVao( const GlslProg* shader, const AttribGlslMap &attributeMapping = AttribGlslMap() );

	//! Returns the number of vertices in the mesh
	uint32_t	getNumVertices() const { return mNumVertices; }
	//! Returns the number of indices for indexed geometry, otherwise 0
	uint32_t	getNumIndices() const { return mNumIndices; }
	//! Returns the primitive type, such as GL_TRIANGLES, GL_TRIANGLE_STRIP, etc
	GLenum		getGlPrimitive() const { return mGlPrimitive; }
	//! Returns the data type of the indices contained in index vbo; either GL_UNSIGNED_SHORT or GL_UNSIGNED_INT
	GLenum		getIndexDataType() const { return mIndexType; }

	//! Returns 0 if \a attr is not present
	uint8_t			getAttribDims( geom::Attrib attr ) const;
	//! Returns AttribSet of geom::Attribs present in the VboMesh
	geom::AttribSet	getAvailableAttribs() const;

	//! Returns the VBO containing the indices of the mesh, or a NULL for non-indexed geometry
	VboRef		getIndexVbo() { return mIndices; }

	//! Updates internal vertex count to reflect an external change to underlying VBOs. Does NOT reallocate or modify storage.
	void		updateNumVertices( uint32_t numVertices ) { mNumVertices = numVertices; }
	//! Updates internal index count to reflect an external change to underlying VBOs. Does NOT reallocate or modify storage.
	void		updateNumIndices( uint32_t numIndices ) { mNumIndices = numIndices; }

	//! Builds and returns a vector of VboRefs for the vertex data of the mesh
	std::vector<VboRef>									getVertexArrayVbos();
	//! Returns the vector of pairs of (BufferLayout,VboRef) for the vertex data of the mesh
	const std::vector<std::pair<geom::BufferLayout,VboRef>>&	getVertexArrayLayoutVbos() const { return mVertexArrayVbos; }
	//! Adds a new VBO (paired with its geom::BufferLayout) to the VboMesh
	void												appendVbo( const geom::BufferLayout &layout, const VboRef &vbo );	

	//! Returns a pair<geom::BufferLayout,VboRef>* that corresponds to \a attrib. Returns nullptr if not found
	std::pair<geom::BufferLayout,VboRef>*		findAttrib( geom::Attrib attr );

	//! Fails with an error on ES 2 platforms which don't support buffer mapping when used with interleaved buffers. When multiple attributes are stored interleaved in a single VBO, this can be less efficient than calling mapAttrib*() or manipulating the VBO directly.
	void		bufferAttrib( geom::Attrib attrib, size_t dataSizeBytes, const void *data );
	//! Fails with an error on ES 2 platforms which don't support buffer mapping when used with interleaved buffers. When multiple attributes are stored interleaved in a single VBO, this can be less efficient than calling mapAttrib*() or manipulating the VBO directly.
	template<typename T>
	void		bufferAttrib( geom::Attrib attrib, const std::vector<T> &data ) { bufferAttrib( attrib, sizeof(T) * data.size(), data.data() ); }

	void		bufferIndices( size_t dataSizeBytes, const void *data );

#if defined( CINDER_GL_ES_3 ) || (! defined( CINDER_GL_ANGLE ))
	class CI_API MappedAttribBase {
	  public:
		//! Must be called before object is destructed.
		void		unmap();

		~MappedAttribBase();
		
		MappedAttribBase( const MappedAttribBase &rhs )
			: mMapping( rhs.mMapping ), mPtr( rhs.mPtr ), mStride( rhs.mStride )
		{
			mMapping->refCountInc();
		}

		MappedAttribBase& operator=( const MappedAttribBase &rhs )
		{
			mMapping = rhs.mMapping;
			mMapping->refCountInc();
			mPtr = rhs.mPtr;
			mStride = rhs.mStride;
			return *this;
		}
		
		MappedAttribBase( MappedAttribBase &&rhs )
		{
			mMapping = rhs.mMapping;
			rhs.mMapping = nullptr;
			mPtr = rhs.mPtr;
			mStride = rhs.mStride;
		}
		
		MappedAttribBase& operator=( MappedAttribBase &&rhs )
		{
			mMapping = rhs.mMapping;
			rhs.mMapping = nullptr;
			mPtr = rhs.mPtr;
			mStride = rhs.mStride;
			return *this;
		}
	
	  protected:
		MappedAttribBase( VboMesh *mesh, const VboRef &vbo, void *ptr, size_t stride )
			: mMapping( new Mapping( mesh, vbo ) ), mPtr( ptr ), mStride( stride )
		{}
		
		struct Mapping {
			Mapping( VboMesh *mesh, const VboRef &vbo )
				: mMesh( mesh ), mVbo( vbo ), mRefCount( 1 ), mMapped( true )
			{}
				
			void		refCountInc() { ++mRefCount; }
			//! Returns \c true if this was the last reference
			bool		refCountDec() { --mRefCount; return mRefCount == 0; }
			
			void		unmap();
			bool		isMapped() const { return mMapped; }
	
		  private:
  			Mapping( const Mapping &rhs );
			Mapping& operator=( const Mapping &rhs );

		  public:		
			VboMesh		*mMesh;
			bool		mMapped;
			VboRef		mVbo;
			uint32_t	mRefCount;
		};
		
		friend class VboMesh;
		
		void		*mPtr;
		Mapping		*mMapping;
		size_t		mStride;	
	};

	template<typename T>
	class CI_API MappedAttrib : public MappedAttribBase {
	  public:
		T&			operator*()			{ return *(reinterpret_cast<T*>( mPtr )); }
		const T&	operator*() const	{ return *(reinterpret_cast<const T*>( mPtr )); }

		T*			operator->()		{ return reinterpret_cast<T*>( mPtr ); }
		const T*	operator->() const	{ return reinterpret_cast<const T*>( mPtr ); }

		T&			operator[]( size_t i )			{ return *(reinterpret_cast<T*>( ((uint8_t*)mPtr) + mStride * i )); }
		const T&	operator[]( size_t i ) const	{ return *(reinterpret_cast<T*>( ((uint8_t*)mPtr) + mStride * i )); }
		
		// pre-increment
		MappedAttrib	operator++() { mPtr = ((uint8_t*)mPtr) + mStride; return *this; }
		// post-increment
		MappedAttrib	operator++(int) { auto result = *this; mPtr = ((uint8_t*)mPtr) + mStride; return result; }

	  protected:
		MappedAttrib( VboMesh *mesh, const VboRef &vbo, void *ptr, size_t stride )
			: MappedAttribBase( mesh, vbo, ptr, stride )
		{}
		
		friend class VboMesh;
	};

	//! Must call unmap() on the result. Throws ExcMissingAttrib if the VboMesh doesn't contain \a attr.
	MappedAttrib<float>		mapAttrib1f( geom::Attrib attr, bool orphanExisting = true );	
	//! Must call unmap() on the result. Throws ExcMissingAttrib if the VboMesh doesn't contain \a attr.
	MappedAttrib<vec2>		mapAttrib2f( geom::Attrib attr, bool orphanExisting = true );	
	//! Must call unmap() on the result. Throws ExcMissingAttrib if the VboMesh doesn't contain \a attr.
	MappedAttrib<vec3>		mapAttrib3f( geom::Attrib attr, bool orphanExisting = true );
	//! Must call unmap() on the result. Throws ExcMissingAttrib if the VboMesh doesn't contain \a attr.
	MappedAttrib<vec4>		mapAttrib4f( geom::Attrib attr, bool orphanExisting = true );
#endif // ! defined(CINDER_GL_ES_3) || (! defined( CINDER_GL_ANGLE ))

	//! Issues a glDraw* call, but without binding a VAO or sending shader vars. Consider gl::draw( VboMeshRef ) instead. Knows whether to call glDrawArrays or glDrawElements
	void		drawImpl( GLint first = 0, GLsizei count = -1 );
#if defined( CINDER_GL_HAS_DRAW_INSTANCED )
	//! Issues a glDraw*Instanced call, but without binding a VAO or sending shader vars. Consider gl::draw( VboMeshRef ) instead. Knows whether to call glDrawArrays or glDrawElements
	void		drawInstancedImpl( GLsizei instanceCount );
#endif
#if defined( CINDER_GL_HAS_DRAW_INDIRECT )
	//! Issues a glDraw*Indirect call, but without binding a VAO or sending shader vars. Knows whether to call glDrawArrays of glDrawElements
	void		drawIndirectImpl( const GLvoid *indirect );
#endif

#if ! defined( CINDER_GL_ES )
	//! Returns a geom::Source which references 'this'. Inefficient - primarily useful for debugging. The returned geom::SourceRef should not outlive 'this' (not a shared_ptr).
	geom::SourceRef	createSource() const;
	
	//! Copies indices to \a dest. Requires \a dest to provide storage for getNumIndices() indices. Inefficient - primarily useful for debugging.
	void		downloadIndices( uint32_t *dest ) const;
	
	//! Echos all vertex data in range [\a startIndex, \a endIndex) to \a os. Inefficient - primarily useful for debugging.
	void		echoVertexRange( std::ostream &os, size_t startIndex, size_t endIndex );
	//! Echos all vertex data for the indices in range [\a startIndex, \a endIndex) to \a os. No-op for non-indexed geometry. Inefficient - primarily useful for debugging.
	void		echoIndexRange( std::ostream &os, size_t startIndex, size_t endIndex );
#endif

  protected:
	VboMesh( const geom::Source &source, std::vector<std::pair<Layout,VboRef>> vertexArrayBuffers, const VboRef &indexArrayVbo );
	VboMesh( uint32_t numVertices, uint32_t numIndices, GLenum glPrimitive, GLenum indexType, const std::vector<std::pair<geom::BufferLayout,VboRef>> &vertexArrayBuffers, const VboRef &indexVbo );
	VboMesh( uint32_t numVertices, uint32_t numIndices, GLenum glPrimitive, GLenum indexType, const std::vector<Layout> &vertexArrayLayouts, const VboRef &indexVbo );

	void	allocateIndexVbo();

	void	echoVertices( std::ostream &os, const std::vector<uint32_t> &indices, bool printElements );

#if defined( CINDER_GL_ES_3 ) || (! defined( CINDER_GL_ANGLE ))
	template<typename T>
	MappedAttrib<T>		mapAttribImpl( geom::Attrib attr, int dims, bool orphanExisting );
	void				unmapVboImpl( const VboRef &vbo );

	struct MappedVboInfo {
		size_t		mRefCount;
		void		*mPtr;
	};

	std::map<VboRef,MappedVboInfo>		mMappedVbos;
#endif
	
	uint32_t			mNumVertices, mNumIndices;
	GLenum				mGlPrimitive;
	GLenum				mIndexType;

	std::vector<std::pair<geom::BufferLayout,VboRef>>	mVertexArrayVbos;
	VboRef												mIndices;
	
	friend class VboMeshGeomTarget;
	friend class Batch;
};

} } // namespace cinder::gl
