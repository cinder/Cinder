/*
 Copyright 2016 Google Inc.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.


 Copyright (c) 2016, The Cinder Project, All rights reserved.

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

#include "cinder/vk/IndexBuffer.h"
#include "cinder/vk/VertexBuffer.h"

#include "cinder/Color.h"
#include "cinder/Vector.h"
#include "cinder/TriMesh.h"
#include "cinder/GeomIo.h"

namespace cinder { namespace vk {

class ShaderProg;
using ShaderProgRef = std::shared_ptr<ShaderProg>;

class VertexBufferMesh;
using VertexBufferMeshRef = std::shared_ptr<VertexBufferMesh>;

class VertexInputDescription {
public:
	VertexInputDescription() {}
	virtual ~VertexInputDescription() {}

	bool										empty() const { return mSemanticAttributes.empty(); }

	void										addBinding( const VertexBufferRef& buffer, const VkVertexInputBindingDescription& binding );
	const std::vector<VkVertexInputBindingDescription>&		getBindings() const { return mBindings; }
	//std::vector<VkVertexInputBindingDescription>	getBindings() const;
	VkVertexInputBindingDescription*			getBinding( const VertexBufferRef& buffer );
	const VkVertexInputBindingDescription*		getBinding( const VertexBufferRef& buffer ) const;

	void										addAttribute( geom::Attrib semantic, const VkVertexInputAttributeDescription& attribute );
	//const std::vector<VkVertexInputAttributeDescription>&	getAttributes() const { return mAttributes; }
	std::vector<VkVertexInputAttributeDescription>	getAttributes() const;
	VkVertexInputAttributeDescription*			getAttribute( geom::Attrib semantic );
	const VkVertexInputAttributeDescription*	getAttribute( geom::Attrib semantic ) const;
	std::vector<geom::Attrib>					getSemanticAttributes() const;

	void										setAttributeLocationsAndBindings( const ShaderProgRef& shader );

private:
	using BufferToBindingMap = std::map<VertexBufferRef, VkVertexInputBindingDescription>;
	using SemanticAttribute = std::pair<geom::Attrib, VkVertexInputAttributeDescription>;

	BufferToBindingMap								mBufferToBindings;
	std::vector<SemanticAttribute>					mSemanticAttributes;
	std::vector<VkVertexInputBindingDescription>	mBindings;
	std::vector<VkVertexInputAttributeDescription>	mAttributes;
};

class VertexBufferMesh {
  public:
	class Layout {
	  public:
		Layout() : mInterleave( true ) {}

		//! Specifies whether the data is stored planar or interleaved. Default is interleaved.
		Layout&		interleave( bool interleave = true ) { mInterleave = interleave; return *this; }
		//! Returns whether the Layout stores data as interleaved (rather than planar)
		bool		getInterleave() const { return mInterleave; }
		//! Appends an attribute of semantic \a attrib which is \a dims-dimensional. Replaces AttribInfo if it exists for \a attrib
		Layout&		attrib( geom::Attrib attrib, uint8_t dims );
		//! Appends an attribute using a geom::AttribInfo. Replaces AttribInfo if it exists for \a attribInfo.getAttrib()
		Layout&		attrib( const geom::AttribInfo &attribInfo );

		std::vector<geom::AttribInfo>&			getAttribs() { return mAttribInfos; }
		const std::vector<geom::AttribInfo>&	getAttribs() const { return mAttribInfos; }
		//! Clears all attributes in the Layout
		void									clearAttribs() { mAttribInfos.clear(); }

		//std::pair<VkVertexInputBindingDescription, std::vector<VkVertexInputAttributeDescription>>	getVertexInputDescriptors() const;

	  protected:
		//! If \a resultVbo is null then no VBO is allocated
		void		allocate( size_t numVertices, geom::BufferLayout *resultBufferLayout, vk::VertexBufferRef *resultVbo ) const;

		bool							mInterleave;
		std::vector<geom::AttribInfo>	mAttribInfos;

		friend VertexBufferMesh;
	};
  
	//! Creates a VertexBufferMesh which represents the geom::Source \a source. Layout is derived from the contents of \a source.
	static VertexBufferMeshRef	create( const geom::Source &source );
	//! Creates a VertexBufferMesh which represents the geom::Source \a source using \a layout.
	static VertexBufferMeshRef	create( const geom::Source &source, const geom::AttribSet &requestedAttribs );
	//! Creates a VertexBufferMesh which represents the geom::Source \a source using 1 or more VertexBufferMesh::Layouts for vertex data.
	static VertexBufferMeshRef	create( const geom::Source &source, const std::vector<VertexBufferMesh::Layout> &vertexArrayLayouts );
	//! Creates a VertexBufferMesh which represents the geom::Source \a source using 1 or more Vbo/VertexBufferMesh::Layout pairs. A null VertexBufferRef requests allocation.
	static VertexBufferMeshRef	create( const geom::Source &source, const std::vector<std::pair<VertexBufferMesh::Layout,VertexBufferRef>> &vertexArrayLayouts, const IndexBufferRef &indexVbo = nullptr );
	//! Creates a VertexBufferMesh which represents the user's vertex buffer objects. Allows optional \a indexVbo to enable indexed vertices; creates a static VBO if none provided.
	static VertexBufferMeshRef	create( uint32_t numVertices, VkPrimitiveTopology glPrimitive, const std::vector<std::pair<geom::BufferLayout,VertexBufferRef>> &vertexArrayBuffers, uint32_t numIndices = 0, VkIndexType indexType = VK_INDEX_TYPE_UINT16, const IndexBufferRef &indexVbo = IndexBufferRef() );
	//! Creates a VertexBufferMesh which represents the user's vertex buffer objects. Allows optional \a indexVbo to enable indexed vertices; creates a static VBO if none provided.
	static VertexBufferMeshRef	create( uint32_t numVertices, VkPrimitiveTopology glPrimitive, const std::vector<Layout> &vertexArrayLayouts, uint32_t numIndices = 0, VkIndexType indexType = VK_INDEX_TYPE_UINT16, const IndexBufferRef &indexVbo = IndexBufferRef() );

	//! Maps a geom::Attrib to a named attribute in the GlslProg
	typedef std::map<geom::Attrib,std::string> AttribGlslMap;
	//! Constructs a VAO (in the currently bound VAO) that matches \a this to ShaderProg \a shader, overriding the mapping of a geom::Attrib to a named attribute via the 'a attributeMapping std::map
	//void		buildVao( const ShaderProgRef &shader, const AttribGlslMap &attributeMapping = AttribGlslMap() );
	//void		buildVao( const ShaderProg *shader, const AttribGlslMap &attributeMapping = AttribGlslMap() );

	//! Returns the number of vertices in the mesh
	uint32_t	getNumVertices() const { return mNumVertices; }
	//! Returns the number of indices for indexed geometry, otherwise 0
	uint32_t	getNumIndices() const { return mNumIndices; }
	//! Returns the primitive type, such as GL_TRIANGLES, GL_TRIANGLE_STRIP, etc
	VkPrimitiveTopology		getPrimitive() const { return mVkPrimitive; }
	//! Returns the data type of the indices contained in index vbo; either VK_INDEX_TYPE_UINT16 or VK_INDEX_TYPE_UINT32
	VkIndexType		getIndexDataType() const { return mIndexType; }

	//! Returns 0 if \a attr is not present
	uint8_t			getAttribDims( geom::Attrib attr ) const;
	//! Returns AttribSet of geom::Attribs present in the VertexBufferMesh
	geom::AttribSet	getAvailableAttribs() const;

	//! Returns the VBO containing the indices of the mesh, or a NULL for non-indexed geometry
	IndexBufferRef	getIndexVbo() { return mIndices; }

	//! Builds and returns a vector of VertexBufferRefs for the vertex data of the mesh
	std::vector<VertexBufferRef>									getVertexArrayVbos();
	//! Returns the vector of pairs of (BufferLayout,VertexBufferRef) for the vertex data of the mesh
	const std::vector<std::pair<geom::BufferLayout,VertexBufferRef>>&	getVertexArrayLayoutVbos() const { return mVertexArrayVertexBuffers; }
	//! Adds a new VBO (paired with its geom::BufferLayout) to the VertexBufferMesh
	void												appendVbo( const geom::BufferLayout &layout, const VertexBufferRef &vbo );	

	//! Returns a pair<geom::BufferLayout,VertexBufferRef>* that corresponds to \a attrib. Returns nullptr if not found
	std::pair<geom::BufferLayout,VertexBufferRef>*		findAttrib( geom::Attrib attr );

	const vk::VertexInputDescription&					getVertexInputDescription() const;

	//! Fails with an error on ES 2 platforms which don't support buffer mapping when used with interleaved buffers. When multiple attributes are stored interleaved in a single VBO, this can be less efficient than calling mapAttrib*() or manipulating the VBO directly.
	void		bufferAttrib( geom::Attrib attrib, size_t dataSizeBytes, const void *data );
	//! Fails with an error on ES 2 platforms which don't support buffer mapping when used with interleaved buffers. When multiple attributes are stored interleaved in a single VBO, this can be less efficient than calling mapAttrib*() or manipulating the VBO directly.
	template<typename T>
	void		bufferAttrib( geom::Attrib attrib, const std::vector<T> &data ) { bufferAttrib( attrib, sizeof(T) * data.size(), data.data() ); }

	void		bufferIndices( size_t dataSizeBytes, const void *data );

	class MappedAttribBase {
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
		MappedAttribBase( VertexBufferMesh *mesh, const VertexBufferRef &vbo, void *ptr, size_t stride )
			: mMapping( new Mapping( mesh, vbo ) ), mPtr( ptr ), mStride( stride )
		{}
		
		struct Mapping {
			Mapping( VertexBufferMesh *mesh, const VertexBufferRef &vbo )
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
			VertexBufferMesh	*mMesh;
			bool				mMapped;
			VertexBufferRef		mVbo;
			uint32_t			mRefCount;
		};
		
		friend class VertexBufferMesh;
		
		void		*mPtr;
		Mapping		*mMapping;
		size_t		mStride;	
	};

	template<typename T>
	class MappedAttrib : public MappedAttribBase {
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
		MappedAttrib( VertexBufferMesh *mesh, const VertexBufferRef &vbo, void *ptr, size_t stride )
			: MappedAttribBase( mesh, vbo, ptr, stride )
		{}
		
		friend class VertexBufferMesh;
	};

	//! Must call unmap() on the result. Throws ExcMissingAttrib if the VertexBufferMesh doesn't contain \a attr.
	MappedAttrib<float>		mapAttrib1f( geom::Attrib attr, bool orphanExisting = true );	
	//! Must call unmap() on the result. Throws ExcMissingAttrib if the VertexBufferMesh doesn't contain \a attr.
	MappedAttrib<vec2>		mapAttrib2f( geom::Attrib attr, bool orphanExisting = true );	
	//! Must call unmap() on the result. Throws ExcMissingAttrib if the VertexBufferMesh doesn't contain \a attr.
	MappedAttrib<vec3>		mapAttrib3f( geom::Attrib attr, bool orphanExisting = true );
	//! Must call unmap() on the result. Throws ExcMissingAttrib if the VertexBufferMesh doesn't contain \a attr.
	MappedAttrib<vec4>		mapAttrib4f( geom::Attrib attr, bool orphanExisting = true );

	//! Issues a glDraw* call, but without binding a VAO or sending shader vars. Consider gl::draw( VertexBufferMeshRef ) instead. Knows whether to call glDrawArrays or glDrawElements
	void		drawImpl( VkCommandBuffer cmdBuf, VkDeviceSize first = 0, VkDeviceSize count = -1 );

	/*
	//! Issues a glDraw*Instanced call, but without binding a VAO or sending shader vars. Consider gl::draw( VertexBufferMeshRef ) instead. Knows whether to call glDrawArrays or glDrawElements
	void		drawInstancedImpl( GLsizei instanceCount );
	*/

	//! Returns a geom::Source which references 'this'. Inefficient - primarily useful for debugging. The returned geom::SourceRef should not outlive 'this' (not a shared_ptr).
	geom::SourceRef	createSource() const;
	//! Copies indices to \a dest. Requires \a dest to provide storage for getNumIndices() indices. Inefficient - primarily useful for debugging.
	void		downloadIndices( uint32_t *dest ) const;	
	//! Echos all vertex data in range [\a startIndex, \a endIndex) to \a os. Inefficient - primarily useful for debugging.
	void		echoVertexRange( std::ostream &os, size_t startIndex, size_t endIndex );
	//! Echos all vertex data for the indices in range [\a startIndex, \a endIndex) to \a os. No-op for non-indexed geometry. Inefficient - primarily useful for debugging.
	void		echoIndexRange( std::ostream &os, size_t startIndex, size_t endIndex );

  protected:
	VertexBufferMesh( const geom::Source &source, std::vector<std::pair<Layout,VertexBufferRef>> vertexArrayBuffers, const IndexBufferRef &indexVbo );
	VertexBufferMesh( uint32_t numVertices, uint32_t numIndices, VkPrimitiveTopology glPrimitive, VkIndexType indexType, const std::vector<std::pair<geom::BufferLayout,VertexBufferRef>> &vertexArrayBuffers, const IndexBufferRef &indexVbo );
	VertexBufferMesh( uint32_t numVertices, uint32_t numIndices, VkPrimitiveTopology glPrimitive, VkIndexType indexType, const std::vector<Layout> &vertexArrayLayouts, const IndexBufferRef &indexVbo );

	void	allocateIndexVbo();

	void	echoVertices( std::ostream &os, const std::vector<uint32_t> &indices, bool printElements );

	template<typename T>
	MappedAttrib<T>		mapAttribImpl( geom::Attrib attr, int dims, bool orphanExisting );
	void				unmapVboImpl( const VertexBufferRef &vbo );

	struct MappedVboInfo {
		size_t		mRefCount;
		void		*mPtr;
	};

	std::map<VertexBufferRef,MappedVboInfo>		mMappedVbos;
	
	uint32_t			mNumVertices, mNumIndices;
	VkPrimitiveTopology	mVkPrimitive;
	VkIndexType			mIndexType;

	//struct VertexInputInfo {
	//	VkVertexInputBindingDescription					mBindingDescription;
	//	std::vector<VkVertexInputAttributeDescription>	mAttributeDescriptions;
	//};

	std::vector<std::pair<geom::BufferLayout, VertexBufferRef>>	mVertexArrayVertexBuffers;
	IndexBufferRef												mIndices;
	mutable vk::VertexInputDescription							mVertexInputDescription;
	
	friend class VertexBufferMeshGeomTarget;
	friend class Batch;
};

// -------------------------------------------------------------------------------------------------
// Mapped type names
// -------------------------------------------------------------------------------------------------
using VboMesh = VertexBufferMesh;
using VboMeshRef = VertexBufferMeshRef;

}} // namespace cinder::vk