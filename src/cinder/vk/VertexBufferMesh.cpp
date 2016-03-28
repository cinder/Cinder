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

#include "cinder/vk/VertexBufferMesh.h"
#include "cinder/vk/ShaderProg.h"
#include "cinder/vk/wrapper.h"

#include "cinder/Log.h"

using namespace std;

namespace cinder { namespace vk {

/////////////////////////////////////////////////////////////////////////////////////////////////
// VertexInputDescription
void VertexInputDescription::addBinding( const VertexBufferRef& buffer, const VkVertexInputBindingDescription& binding )
{
	mBufferToBindings[buffer] = binding;
	mBindings.push_back( binding );
}

//std::vector<VkVertexInputBindingDescription> VertexInputDescription::getBindings() const
//{
//	std::vector<VkVertexInputBindingDescription> result;
//	for( const auto& it : mBufferToBindings ) {
//		result.push_back( it.second );
//	}
//	return result;
//}

VkVertexInputBindingDescription* VertexInputDescription::getBinding( const VertexBufferRef& buffer )
{
	auto it = mBufferToBindings.find( buffer );
	return ( mBufferToBindings.end() != it ) ? &(it->second) : nullptr;
}

const VkVertexInputBindingDescription* VertexInputDescription::getBinding( const VertexBufferRef& buffer ) const
{
	auto it = mBufferToBindings.find( buffer );
	return ( mBufferToBindings.end() != it ) ? &(it->second) : nullptr;
}

void VertexInputDescription::addAttribute( geom::Attrib semantic, const VkVertexInputAttributeDescription& attribute )
{
	auto it = std::find_if(
		std::begin( mSemanticAttributes ),
		std::end( mSemanticAttributes ),
		[&semantic]( const SemanticAttribute& elem ) -> bool {
			return elem.first == semantic;
		}
	);

	if( std::end( mSemanticAttributes ) == it ) {
		mSemanticAttributes.push_back( std::make_pair( semantic, attribute ) );
		mAttributes.push_back( attribute );
	}
	else {
		it->second = attribute;
		size_t index = std::distance( std::begin( mSemanticAttributes ), it );
		mAttributes[index] = attribute;
	}
}

std::vector<VkVertexInputAttributeDescription> VertexInputDescription::getAttributes() const
{
	std::vector<VkVertexInputAttributeDescription> result;
	for( const auto& attr : mSemanticAttributes ) {
		result.push_back( attr.second );
	}
	return result;
}

VkVertexInputAttributeDescription* VertexInputDescription::getAttribute( geom::Attrib semantic )
{
	auto it = std::find_if(
		std::begin( mSemanticAttributes ),
		std::end( mSemanticAttributes ),
		[&semantic]( const SemanticAttribute& elem ) -> bool {
			return elem.first == semantic;
		}
	);

	return ( std::end( mSemanticAttributes ) != it ) ? &(it->second) : nullptr;
}

const VkVertexInputAttributeDescription* VertexInputDescription::getAttribute( geom::Attrib semantic ) const
{
	auto it = std::find_if(
		std::begin( mSemanticAttributes ),
		std::end( mSemanticAttributes ),
		[&semantic]( const SemanticAttribute& elem ) -> bool {
			return elem.first == semantic;
		}
	);

	return ( std::end( mSemanticAttributes ) != it ) ? &(it->second) : nullptr;
}

std::vector<geom::Attrib> VertexInputDescription::getSemanticAttributes() const
{
	std::vector<geom::Attrib> result;
	for( const auto& it : mSemanticAttributes ) {
		result.push_back( it.first );
	}
	return result;
}

void VertexInputDescription::setAttributeLocationsAndBindings(const ShaderProgRef& shader)
{
	for( const auto& shaderAttr : shader->getActiveAttributes() ) {
		auto semantic = shaderAttr.getSemantic();

		auto it = std::find_if(
			std::begin( mSemanticAttributes ),
			std::end( mSemanticAttributes ),
			[&semantic]( const SemanticAttribute& elem ) -> bool {
				return elem.first == semantic;
			}
		);

		if( std::end( mSemanticAttributes ) != it ) {
			it->second.location = shaderAttr.getLocation();
			it->second.binding  = shaderAttr.getBinding();

			size_t index = std::distance( std::begin( mSemanticAttributes ), it );
			mAttributes[index].location = shaderAttr.getLocation();
			mAttributes[index].binding  = shaderAttr.getBinding();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// VertexBufferMeshGeomSource
class VertexBufferMeshSource : public geom::Source {
  public:
	static std::shared_ptr<VertexBufferMeshSource>	create( const vk::VertexBufferMesh *VertexBufferMesh );
	
	void			loadInto( geom::Target *target, const geom::AttribSet &requestedAttribs ) const override;
	VertexBufferMeshSource*	clone() const override { return new VertexBufferMeshSource( *this ); }
	
	size_t			getNumVertices() const override;
	size_t			getNumIndices() const override;
	geom::Primitive	getPrimitive() const override;
	
	uint8_t			getAttribDims( geom::Attrib attr ) const override;
	geom::AttribSet	getAvailableAttribs() const override;
	
  protected:
	VertexBufferMeshSource( const vk::VertexBufferMesh *VertexBufferMesh );
  
	const vk::VertexBufferMesh		*mVertexBufferMesh;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
// VertexBufferMeshGeomTarget
class VertexBufferMeshGeomTarget : public geom::Target {
  public:
	struct BufferData {
		BufferData( const geom::BufferLayout &layout, uint8_t *data, size_t dataSize )
			: mLayout( layout ), mData( data ), mDataSize( dataSize )
		{}
		BufferData( BufferData &&rhs )
			: mLayout( rhs.mLayout ), mData( std::move( rhs.mData ) ), mDataSize( rhs.mDataSize )
		{}
	
		geom::BufferLayout			mLayout;
		std::unique_ptr<uint8_t[]>	mData;
		size_t						mDataSize;
	};

	VertexBufferMeshGeomTarget( geom::Primitive prim, VertexBufferMesh *VertexBufferMesh )
		: mPrimitive( prim ), mVertexBufferMesh( VertexBufferMesh )
	{
		mVertexBufferMesh->mNumIndices = 0; // this may be replaced later with a copyIndices call
		// create a vector of temporary data that parallels the VertexBufferMesh's vertexData
		for( const auto &vertexArrayBuffer : mVertexBufferMesh->getVertexArrayLayoutVbos() ) {
			size_t requiredBytes = vertexArrayBuffer.first.calcRequiredStorage( mVertexBufferMesh->mNumVertices );
			mBufferData.push_back( BufferData( vertexArrayBuffer.first, new uint8_t[requiredBytes], requiredBytes ) );
		}
	}
	
	virtual geom::Primitive	getPrimitive() const;
	uint8_t	getAttribDims( geom::Attrib attr ) const override;
	void	copyAttrib( geom::Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count ) override;
	void	copyIndices( geom::Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex ) override;
	
	//! Must be called in order to upload temporary 'mBufferData' to VBOs
	void	copyBuffers();
	
  protected:
	geom::Primitive				mPrimitive;
	std::vector<BufferData>		mBufferData;
	VertexBufferMesh			*mVertexBufferMesh;
};

geom::Primitive	VertexBufferMeshGeomTarget::getPrimitive() const
{
	return mPrimitive;
}

uint8_t	VertexBufferMeshGeomTarget::getAttribDims( geom::Attrib attr ) const
{
	return mVertexBufferMesh->getAttribDims( attr );
}

void VertexBufferMeshGeomTarget::copyAttrib( geom::Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count )
{
	// if we don't have it we don't want it
	if( getAttribDims( attr ) == 0 )
		return;
	
	// we need to find which element of 'mBufferData' contains 'attr'
	uint8_t *dstData = nullptr;
	uint8_t dstDims;
	size_t dstStride, dstDataSize;
	for( const auto &bufferData : mBufferData ) {
		if( bufferData.mLayout.hasAttrib( attr ) ) {
			auto attrInfo = bufferData.mLayout.getAttribInfo( attr );
			dstDims = attrInfo.getDims();
			dstStride = attrInfo.getStride();
			dstData = bufferData.mData.get() + attrInfo.getOffset();
			dstDataSize = bufferData.mDataSize;
			break;
		}
	}
	CI_ASSERT( dstData );
	
	// verify we've been called with the number of vertices we were promised earlier
	if( count != mVertexBufferMesh->mNumVertices ) {
		CI_LOG_E( "copyAttrib() called with " << count << " elements. " << mVertexBufferMesh->mNumVertices << " expected." );
		return;
	}
	
	// verify we have room for this data
	auto testDstStride = dstStride ? dstStride : ( dstDims * sizeof(float) );
	if( dstDataSize < count * testDstStride ) {
		CI_LOG_E( "copyAttrib() called with inadequate attrib data storage allocated" );
		return;
	}
	
	if( dstData )
		geom::copyData( dims, srcData, count, dstDims, dstStride, reinterpret_cast<float*>( dstData ) );
}

void VertexBufferMeshGeomTarget::copyIndices( geom::Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex )
{
	mVertexBufferMesh->mNumIndices = (uint32_t)numIndices;

	if( requiredBytesPerIndex <= 2 ) {
		mVertexBufferMesh->mIndexType = VK_INDEX_TYPE_UINT16;
		std::unique_ptr<uint16_t[]> indices( new uint16_t[numIndices] );
		copyIndexData( source, numIndices, indices.get() );
		if( ! mVertexBufferMesh->mIndices ) {
			mVertexBufferMesh->mIndices = IndexBuffer::create( numIndices, mVertexBufferMesh->mIndexType, indices.get(), vk::IndexBuffer::Format( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ).setUsageTransferDestination() );
		}
		else {
			mVertexBufferMesh->mIndices->bufferData( numIndices * sizeof(uint16_t), indices.get() );
		}
	}
	else {
		mVertexBufferMesh->mIndexType = VK_INDEX_TYPE_UINT32;
		std::unique_ptr<uint32_t[]> indices( new uint32_t[numIndices] );
		copyIndexData( source, numIndices, indices.get() );
		if( ! mVertexBufferMesh->mIndices ) {			
			mVertexBufferMesh->mIndices = IndexBuffer::create( numIndices, mVertexBufferMesh->mIndexType, indices.get(), vk::IndexBuffer::Format( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ).setUsageTransferDestination() );
		}
		else {
			mVertexBufferMesh->mIndices->bufferData( numIndices * sizeof(uint32_t), indices.get() );
		}
	}
}

void VertexBufferMeshGeomTarget::copyBuffers()
{
	// iterate all the buffers in mBufferData and upload them to the corresponding VBO in the VertexBufferMesh
	for( auto bufferDataIt = mBufferData.begin(); bufferDataIt != mBufferData.end(); ++bufferDataIt ) {
		auto vertexArrayIt = mVertexBufferMesh->mVertexArrayVertexBuffers.begin() + std::distance( mBufferData.begin(), bufferDataIt );
		vertexArrayIt->second->bufferData( bufferDataIt->mDataSize, bufferDataIt->mData.get() );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VertexBufferMesh::Layout
VertexBufferMesh::Layout& VertexBufferMesh::Layout::attrib( geom::Attrib attrib, uint8_t dims )
{
	for( auto attribIt = mAttribInfos.begin(); attribIt != mAttribInfos.end(); ) {
		if( attribIt->getAttrib() == attrib )
			attribIt = mAttribInfos.erase( attribIt );
		else
			++attribIt;
	}
	mAttribInfos.push_back( geom::AttribInfo( attrib, geom::DataType::FLOAT, dims, 0, 0, 0 ) );
	return *this;
}

VertexBufferMesh::Layout& VertexBufferMesh::Layout::attrib( const geom::AttribInfo &attribInfo )
{
	for( auto attribIt = mAttribInfos.begin(); attribIt != mAttribInfos.end(); ) {
		if( attribIt->getAttrib() == attribInfo.getAttrib() )
			attribIt = mAttribInfos.erase( attribIt );
		else
			++attribIt;
	}
	mAttribInfos.push_back( attribInfo );
	return *this;
}

/*
std::pair<VkVertexInputBindingDescription,std::vector<VkVertexInputAttributeDescription>> VertexBufferMesh::Layout::getVertexInputDescriptors() const
{
	std::pair<VkVertexInputBindingDescription,std::vector<VkVertexInputAttributeDescription>> result;

	size_t totalStride = 0;
	for( const auto &attrib : mAttribInfos ) {
		totalStride += attrib.getByteSize();

		VkVertexInputAttributeDescription viad = {};
		viad.location = 0;
		viad.binding  = 0;
		viad.format   = toVkFormat( attrib );
		viad.offset   = totalStride;
		result.second.push_back( viad );
	}

	result.first.binding = 0;
	result.first.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	result.first.stride = totalStride;

	return result;
}
*/

void VertexBufferMesh::Layout::allocate( size_t numVertices,geom::BufferLayout *resultBufferLayout, vk::VertexBufferRef *resultVbo ) const
{
	auto attribInfos = mAttribInfos;

	// setup offsets and strides based on interleaved or planar
	size_t totalDataBytes;
	if( mInterleave ) {
		size_t totalStride = 0;
		for( const auto &attrib : attribInfos )
			totalStride += attrib.getByteSize();
		size_t currentOffset = 0;
		for( auto &attrib : attribInfos ) {
			attrib.setOffset( currentOffset );
			attrib.setStride( totalStride );
			currentOffset += attrib.getByteSize();
		}
		totalDataBytes = currentOffset * numVertices;
	}
	else { // planar data
		size_t currentOffset = 0;
		for( auto &attrib : attribInfos ) {
			attrib.setOffset( currentOffset );
			attrib.setStride( attrib.getByteSize() );
			currentOffset += attrib.getByteSize() * numVertices;
		}
		totalDataBytes = currentOffset;
	}

	*resultBufferLayout = geom::BufferLayout( attribInfos );

	if( resultVbo ) {
		if( *resultVbo ) { // non-null shared_ptr means the VBO should be resized
			//(*resultVbo)->ensureMinimumSize( totalDataBytes );
			(*resultVbo)->ensureMinimumSize( totalDataBytes );
		}
		else { // else allocate 
			//*resultVbo = Vbo::create( GL_ARRAY_BUFFER, totalDataBytes, nullptr, mUsage );
			*resultVbo = VertexBuffer::create( nullptr, totalDataBytes, vk::VertexBuffer::Format( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ).setUsageTransferDestination() );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// VertexBufferMesh
VertexBufferMeshRef VertexBufferMesh::create( const geom::Source &source )
{
	// Pass an empty std::vector<pair<Layout,VertexBufferRef>> to imply we want to pull data from the Source
	return VertexBufferMeshRef( new VertexBufferMesh( source, std::vector<pair<Layout,VertexBufferRef>>(), IndexBufferRef() ) );
}

VertexBufferMeshRef VertexBufferMesh::create( const geom::Source &source, const geom::AttribSet &requestedAttribs )
{
	// make an interleaved VertexBufferMesh::Layout with 'requestedAttribs'
	Layout layout;
	for( const auto &attrib : requestedAttribs )
		layout.attrib( attrib, 0 ); // 0 dim implies querying the Source for its dimension
	
	return VertexBufferMeshRef( new VertexBufferMesh( source, { { layout, nullptr } }, nullptr ) );
}

VertexBufferMeshRef VertexBufferMesh::create( const geom::Source &source, const std::vector<VertexBufferMesh::Layout> &vertexArrayLayouts )
{
	std::vector<std::pair<VertexBufferMesh::Layout,VertexBufferRef>> layoutVbos;
	for( const auto &vertexArrayLayout : vertexArrayLayouts )
		layoutVbos.push_back( std::make_pair( vertexArrayLayout, (VertexBufferRef)nullptr ) );

	return VertexBufferMeshRef( new VertexBufferMesh( source, layoutVbos, nullptr ) );
}

VertexBufferMeshRef VertexBufferMesh::create( const geom::Source &source, const std::vector<std::pair<VertexBufferMesh::Layout,VertexBufferRef>> &vertexArrayLayouts, const IndexBufferRef &indexVbo )
{
	return VertexBufferMeshRef( new VertexBufferMesh( source, vertexArrayLayouts, indexVbo ) );
}

VertexBufferMeshRef VertexBufferMesh::create( uint32_t numVertices, VkPrimitiveTopology glPrimitive, const std::vector<pair<geom::BufferLayout,VertexBufferRef>> &vertexArrayBuffers, uint32_t numIndices, VkIndexType indexType, const IndexBufferRef &indexVbo )
{
	return VertexBufferMeshRef( new VertexBufferMesh( numVertices, numIndices, glPrimitive, indexType, vertexArrayBuffers, indexVbo ) );
}

VertexBufferMeshRef VertexBufferMesh::create( uint32_t numVertices, VkPrimitiveTopology glPrimitive, const std::vector<Layout> &vertexArrayLayouts, uint32_t numIndices, VkIndexType indexType, const IndexBufferRef &indexVbo )
{
	return VertexBufferMeshRef( new VertexBufferMesh( numVertices, numIndices, glPrimitive, indexType, vertexArrayLayouts, indexVbo ) );
}

VertexBufferMesh::VertexBufferMesh( const geom::Source &source, std::vector<pair<Layout,VertexBufferRef>> vertexArrayBuffers, const IndexBufferRef &indexVbo )
{
	// An empty vertexArrayBuffers implies we should just pull whatever attribs the Source is pushing. We arrived here from VertexBufferMesh::create( Source& )
	if( vertexArrayBuffers.empty() ) {
		// Create an interleaved Layout based on what's in the Source
		vertexArrayBuffers.push_back( std::pair<Layout,VertexBufferRef>( VertexBufferMesh::Layout().interleave(), nullptr ) );
		for( auto &attrib : source.getAvailableAttribs() ) {
			auto dims = source.getAttribDims( attrib );
			if( dims > 0 )
				vertexArrayBuffers.back().first.attrib( attrib, dims );
		}
	}
	else {
		// For any attributes whose dims == 0, set the dims to be whatever dims the Source is pushing.
		for( auto &layoutVbo : vertexArrayBuffers ) {
			for( auto &attribInfo : layoutVbo.first.getAttribs() ) {
				if( attribInfo.getDims() == 0 )
					attribInfo.setDims( source.getAttribDims( attribInfo.getAttrib() ) );
			}
		}
	}

	// determine the requestedAttribs by iterating all the Layouts
	geom::AttribSet requestedAttribs;
	for( const auto &vertexArrayBuffer : vertexArrayBuffers ) {
		for( const auto &attribInfo : vertexArrayBuffer.first.getAttribs() ) {
			requestedAttribs.insert( attribInfo.getAttrib() );
		}
	}

	mNumVertices = (uint32_t)source.getNumVertices();
	mVkPrimitive = toVk( source.getPrimitive() );

	// iterate 'vertexArrayBuffers' and allocate mVertexArrayVbos, which is the parallel vector of <geom::BufferLayout,VertexBufferRef> pairs
	for( const auto &vertexArrayBuffer : vertexArrayBuffers ) {
		geom::BufferLayout bufferLayout;
		VertexBufferRef vbo = vertexArrayBuffer.second;
		// we pass nullptr for the VBO if we already have one, to prevent re-allocation by allocate()
		vertexArrayBuffer.first.allocate( mNumVertices, &bufferLayout, &vbo );
		mVertexArrayVertexBuffers.push_back( make_pair( bufferLayout, vbo ) );
	}
	
	// Set our indices VBO to indexArrayVBO, which may well be empty, so that the target doesn't blow it away. Must do this before we loadInto().
	mIndices = indexVbo;
	
	VertexBufferMeshGeomTarget target( source.getPrimitive(), this );
	source.loadInto( &target, requestedAttribs );
	// we need to let the target know it can copy from its internal buffers to our vertexData VBOs
	target.copyBuffers();
}

VertexBufferMesh::VertexBufferMesh( uint32_t numVertices, uint32_t numIndices, VkPrimitiveTopology glPrimitive, VkIndexType indexType, const std::vector<pair<geom::BufferLayout,VertexBufferRef>> &vertexArrayBuffers, const IndexBufferRef &indexVbo )
	: mNumVertices( numVertices ), mNumIndices( numIndices ), mVkPrimitive( glPrimitive ), mIndexType( indexType ), mVertexArrayVertexBuffers( vertexArrayBuffers ), mIndices( indexVbo )
{
	if( ! mIndices )
		allocateIndexVbo();
}

VertexBufferMesh::VertexBufferMesh( uint32_t numVertices, uint32_t numIndices, VkPrimitiveTopology glPrimitive, VkIndexType indexType, const std::vector<Layout> &vertexArrayLayouts, const IndexBufferRef &indexVbo )
	: mNumVertices( numVertices ), mNumIndices( numIndices ), mVkPrimitive( glPrimitive ), mIndexType( indexType ), mIndices( indexVbo )
{
	geom::BufferLayout bufferLayout;
	for( const auto &layout : vertexArrayLayouts ) {
		VertexBufferRef vbo;
		layout.allocate( numVertices, &bufferLayout, &vbo );
		mVertexArrayVertexBuffers.push_back( make_pair( bufferLayout, vbo ) );
	}
	
	if( ! mIndices )
		allocateIndexVbo();
}

void VertexBufferMesh::allocateIndexVbo()
{
	if( mNumIndices == 0 )
		return;

	size_t bytesRequired = 2; // VK_INDEX_TYPE_UINT16
	if( mIndexType == VK_INDEX_TYPE_UINT16 )
		bytesRequired = 2;
	else if( mIndexType == VK_INDEX_TYPE_UINT32 )
		bytesRequired = 4;
	else
		throw geom::ExcIllegalIndexType();
		
	mIndices = vk::IndexBuffer::create( mNumIndices, mIndexType, nullptr, vk::IndexBuffer::Format( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) );
}

/*
void VertexBufferMesh::buildVao( const vk::ShaderProgRef &shader, const AttribGlslMap &attributeMapping )
{
	buildVao( shader.get(), attributeMapping );
}

void VertexBufferMesh::buildVao( const vk::ShaderProg* shader, const AttribGlslMap &attributeMapping )
{
	auto ctx = gl::context();
	std::set<geom::Attrib> enabledAttribs;
	// iterate all the vertex array VBOs
	for( const auto &vertArrayVbo : mVertexArrayVertexBuffers ) {
		// bind this VBO (to the current VAO)
		vertArrayVbo.second->bind();
		// now iterate the attributes associated with this VBO
		for( const auto &vertAttribInfo : vertArrayVbo.first.getAttribs() ) {
			const GlslProg::Attribute *shaderAttribInfo = nullptr;
			// first see if we have a mapping in 'attributeMapping'
			auto attributeMappingIt = attributeMapping.find( vertAttribInfo.getAttrib() );
			
			if( attributeMappingIt != attributeMapping.end() )
				shaderAttribInfo = shader->findAttrib( attributeMappingIt->second );
			// otherwise, try to get the location of the attrib semantic in the shader if it's present
			else if( shader->hasAttribSemantic( vertAttribInfo.getAttrib() ) )
				shaderAttribInfo = shader->findAttrib( vertAttribInfo.getAttrib() );
			
			if( shaderAttribInfo ) {
				auto shaderLoc = shaderAttribInfo->getLocation();
				
				auto numDims = (int)vertAttribInfo.getDims();
				
				// examples of glsl types and the expected layout
				// in mat4 model	- 16 dims(vertArray), 16 dims(shader), count 1, 4 per, numTimes 4(split) * 1(count)
				// in vec3 stuff[3] -  9 dims(vertArray),  3 dims(shader), count 3, 3 per, numTimes 1(split) * 3(count)
				// in mat4 model[3] - 48 dims(vertArray), 16 dims(shader), count 4, 4 per, numTimes 4(split) * 3(count)
				// in vec4 stuff	-  4 dims(vertArray),  4 dims(shader), count 1, 4 per, numTimes 1(split) * 1(count)
				// in vec2 stuff[5] - 10 dims(vertArray),  2 dims(shader), count 5, 2 per, numTimes 1(split) * 5(count)
				// in mat2x3 stuff	-  6 dims(vertArray),  6 dims(shader), count 1, 2 per, numTimes 3(split) * 1(count)
				// in mat3x2 stuff	-  6 dims(vertArray),  6 dims(shader), count 1, 3 per, numTimes 2(split) * 1(count)
				// in mat2x2 stuff[2]- 8 dims(vertArray),  4 dims(shader), count 2, 2 per, numTimes 2(split) * 2(count)
				
				// num array elements or 1 if not an array
				uint32_t shaderAttribCount = shaderAttribInfo->getCount();
				uint32_t numDimsPerVertexPointer;
				uint32_t numLocationsExpected;
				GlslProg::Attribute::getShaderAttribLayout( shaderAttribInfo->getType(), &numDimsPerVertexPointer, &numLocationsExpected );
				uint32_t shaderAttribTotalDims = numDimsPerVertexPointer * numLocationsExpected * shaderAttribCount;
				if( numDims != shaderAttribTotalDims ) {
					if( numDims <= 4 )
						numDimsPerVertexPointer = numDims;
					else {
						CI_LOG_E( geom::attribToString( vertAttribInfo.getAttrib() ) + "'s BufferLayout defined dimensions(" << numDims << ") does not match shader expected dimensions(" << shaderAttribTotalDims << ") for " << shaderAttribInfo->getName() << ", skipping attribute");
						continue;
					}
				}
				
				uint32_t dataTypeBytes = 0;
				switch ( vertAttribInfo.getDataType() ) {
					case geom::DataType::FLOAT: dataTypeBytes = 4; break;
					case geom::DataType::INTEGER: dataTypeBytes = 4; break;
					case geom::DataType::DOUBLE: dataTypeBytes = 8; break;
				}
				
				uint32_t numTimes = numLocationsExpected * shaderAttribCount;
				size_t currentInnerOffset = 0;
				for( int i = 0; i < numTimes; i++ ) {
					ctx->enableVertexAttribArray( shaderLoc + i );
					if( vertAttribInfo.getDataType() != geom::DataType::INTEGER )
						ctx->vertexAttribPointer( shaderLoc + i, numDimsPerVertexPointer, GL_FLOAT, GL_FALSE, (GLsizei)vertAttribInfo.getStride(), (const void*)(vertAttribInfo.getOffset() + currentInnerOffset) );
#if ! defined( CINDER_GL_ES_2 )
					else
						ctx->vertexAttribIPointer( shaderLoc + i, numDimsPerVertexPointer, GL_INT, (GLsizei)vertAttribInfo.getStride(), (const void*)(vertAttribInfo.getOffset() + currentInnerOffset) );
#endif
					if( vertAttribInfo.getInstanceDivisor() > 0 )
						ctx->vertexAttribDivisor( shaderLoc + i, vertAttribInfo.getInstanceDivisor() );
					currentInnerOffset += (numDimsPerVertexPointer * dataTypeBytes);
				}
				
				enabledAttribs.insert( vertAttribInfo.getAttrib() );
			}
		}
	}
	
	// warn the user if the shader expects any attribs which we couldn't supply. We make an exception for ciColor since it often comes from the Context instead
	const auto &glslActiveAttribs = shader->getActiveAttributes();
	for( auto &glslActiveAttrib : glslActiveAttribs ) {
		bool attribMappingFound = false;
		auto glslAttribName = glslActiveAttrib.getName();
		for( auto & attrib : attributeMapping ) {
			if( glslAttribName == attrib.second ) {
				attribMappingFound = true;
			}
		}
		auto attribSemantic = glslActiveAttrib.getSemantic();
		if( (attribSemantic != geom::Attrib::COLOR) &&
		   (enabledAttribs.count( attribSemantic ) == 0) &&
		   ! attribMappingFound &&
		    // From GL 4.3 core spec, section 11.1.1 (Vertex Attributes):
		    // "For GetActiveAttrib, all active vertex shader input variables
			// are enumerated, including the special built-in inputs gl_VertexID
			// and gl_InstanceID."
			//
		   ( glslAttribName != "gl_InstanceID" && glslAttribName != "gl_VertexID") )
			CI_LOG_W( "Batch GlslProg expected an Attrib of " << geom::attribToString( attribSemantic ) << ", with name " << glslAttribName << " but vertex data doesn't provide it." );
	}
	
	if( mIndices )
		mIndices->bind();

}
*/

void VertexBufferMesh::drawImpl( VkCommandBuffer cmdBuf, VkDeviceSize first, VkDeviceSize count )
{

	//if( mNumIndices ) {
	//	size_t firstByteOffset = first;
	//	if( mIndexType == GL_UNSIGNED_INT ) firstByteOffset *= 4;
	//	else if( mIndexType == GL_UNSIGNED_SHORT ) firstByteOffset *= 2;
	//	glDrawElements( mVkPrimitive, ( count < 0 ) ? ( mNumIndices - first ) : count, mIndexType, (GLvoid*)( firstByteOffset ) );
	//}
	//else
	//	glDrawArrays( mVkPrimitive, first, ( count < 0 ) ? ( mNumVertices - first ) : count );
}

/*
void VertexBufferMesh::drawInstancedImpl( GLsizei instanceCount )
{
	auto ctx = gl::context();
	if( mNumIndices )
		ctx->drawElementsInstanced( mVkPrimitive, mNumIndices, mIndexType, (GLvoid*)( 0 ), instanceCount );
	else
		ctx->drawArraysInstanced( mVkPrimitive, 0, mNumVertices, instanceCount );
}
*/

std::pair<geom::BufferLayout,VertexBufferRef>* VertexBufferMesh::findAttrib( geom::Attrib attr )
{
	for( auto &vertexArrayVbo : mVertexArrayVertexBuffers ) {
		if( vertexArrayVbo.first.hasAttrib( attr ) )
			return &vertexArrayVbo;
	}
	
	return nullptr;
}

const VertexInputDescription& VertexBufferMesh::getVertexInputDescription() const
{
	if( mVertexInputDescription.empty() ) {
		uint32_t bindingId = 0;
		for( const auto& it : mVertexArrayVertexBuffers ) {
			const auto& layout = it.first;
			const auto& buffer = it.second;

			size_t totalStride = 0;
			for( const auto& attrib : layout.getAttribs() ) {
				VkVertexInputAttributeDescription viad = {};
				viad.location = 0;
				viad.binding  = 0;
				viad.format   = toVkFormat( attrib );
				viad.offset   = static_cast<uint32_t>( totalStride );
				mVertexInputDescription.addAttribute( attrib.getAttrib(), viad );

				size_t sizeBytes =  attrib.getByteSize();
				totalStride += sizeBytes;
			}

			VkVertexInputBindingDescription binding = {};
			binding.binding   = bindingId;
			binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			binding.stride    = static_cast<uint32_t>( totalStride );
			mVertexInputDescription.addBinding( buffer, binding );

			++bindingId;
		}
	}

	return mVertexInputDescription;
}

void VertexBufferMesh::bufferAttrib( geom::Attrib attrib, size_t dataSizeBytes, const void *data )
{
	auto layoutVbo = findAttrib( attrib );
	if( ! layoutVbo )
		throw geom::ExcMissingAttrib();
	auto attribInfo = layoutVbo->first.getAttribInfo( attrib );

	// if the attribute is stored planar, a call to VertexBuffer::bufferSubData should suffice
	if( attribInfo.getStride() == 0 || attribInfo.getStride() == attribInfo.getByteSize() ) {
		layoutVbo->second->bufferSubData( attribInfo.getOffset(), dataSizeBytes, data  );
	}
	else { // interleaved data
		uint8_t *ptr = reinterpret_cast<uint8_t*>( layoutVbo->second->map() );
		if( ! ptr ) {
			CI_LOG_E( "Failed to map VBO" );
			return;
		}
  
		ptr += attribInfo.getOffset();
		const uint8_t *srcData = reinterpret_cast<const uint8_t*>( data );
		const uint8_t *endSrcData = (uint8_t*)data + dataSizeBytes;
		while( srcData < endSrcData ) {
			std::memcpy( ptr, srcData, attribInfo.getByteSize() );
			srcData += attribInfo.getByteSize();
			ptr += attribInfo.getStride();
		}
		
		layoutVbo->second->unmap();
	}
}

void VertexBufferMesh::bufferIndices( size_t dataSizeBytes, const void *data )
{
	if( ! mIndices ) {
		CI_LOG_E( "VertexBufferMesh::bufferIndices() called on VertexBufferMesh with null index VBO" );
		return;
	}
	
	mIndices->bufferSubData( 0, dataSizeBytes, data );
}

template<typename T>
VertexBufferMesh::MappedAttrib<T> VertexBufferMesh::mapAttribImpl( geom::Attrib attr, int dims, bool orphanExisting )
{
	std::pair<geom::BufferLayout,VertexBufferRef>* layoutVbo = findAttrib( attr );
	if( ! layoutVbo )
		throw geom::ExcMissingAttrib();
	
	void *dataPtr;
	
	// see if this VBO has already been mapped
	auto existingIt = mMappedVbos.find( layoutVbo->second );
	if( existingIt != mMappedVbos.end() ) {
		existingIt->second.mRefCount++;
		dataPtr = existingIt->second.mPtr;
	}
	else {
		MappedVboInfo mappedVboInfo;
		mappedVboInfo.mRefCount = 1;
		if( orphanExisting ) {
			mappedVboInfo.mPtr = layoutVbo->second->map();
		}
		else {
			mappedVboInfo.mPtr = layoutVbo->second->map();
		}
		mMappedVbos[layoutVbo->second] = mappedVboInfo;
		dataPtr = mappedVboInfo.mPtr;
	}
	
	auto attribInfo = layoutVbo->first.getAttribInfo( attr );
	
	if( dims != attribInfo.getDims() ) {
		CI_LOG_W( "Mapping geom::Attrib of dims " << (int)attribInfo.getDims() << " to type of dims " << dims );	
	}

	auto stride = ( attribInfo.getStride() == 0 ) ? sizeof(T) : attribInfo.getStride();
	return VertexBufferMesh::MappedAttrib<T>( this, layoutVbo->second, ((uint8_t*)dataPtr) + attribInfo.getOffset(), stride );
}

VertexBufferMesh::MappedAttrib<float> VertexBufferMesh::mapAttrib1f( geom::Attrib attr, bool orphanExisting )
{
	return mapAttribImpl<float>( attr, 1, orphanExisting );
}

VertexBufferMesh::MappedAttrib<vec2> VertexBufferMesh::mapAttrib2f( geom::Attrib attr, bool orphanExisting )
{
	return mapAttribImpl<vec2>( attr, 2, orphanExisting );
}

VertexBufferMesh::MappedAttrib<vec3> VertexBufferMesh::mapAttrib3f( geom::Attrib attr, bool orphanExisting )
{
	return mapAttribImpl<vec3>( attr, 3, orphanExisting );
}

VertexBufferMesh::MappedAttrib<vec4> VertexBufferMesh::mapAttrib4f( geom::Attrib attr, bool orphanExisting )
{
	return mapAttribImpl<vec4>( attr, 4, orphanExisting );
}

void VertexBufferMesh::MappedAttribBase::unmap()
{
	if( mMapping )
		mMapping->unmap();
	mPtr = nullptr;
}

VertexBufferMesh::MappedAttribBase::~MappedAttribBase()
{
	if( mMapping ) {
		// if this was the last reference the user forgot to unmap
		if( mMapping->refCountDec() ) {
			// if this isn't null the user forgot to unmap
			if( mMapping->isMapped() )
				CI_LOG_E( "MappedAttrib was never unmapped" );
			delete mMapping;
		}
	}
}

void VertexBufferMesh::MappedAttribBase::Mapping::unmap()
{
	if( ! mMapped ) {
		CI_LOG_E( "MappedAttrib unmapped more than once" );
	}
	else {
		mMesh->unmapVboImpl( mVbo );
		mMapped = false;
	}
}

void VertexBufferMesh::unmapVboImpl( const VertexBufferRef &vbo )
{
	// find the corresponding entry for this VBO in mMappedVbos
	auto existingIt = mMappedVbos.find( vbo );
	if( existingIt != mMappedVbos.end() ) {
		CI_ASSERT( existingIt->second.mRefCount > 0 );
		
		existingIt->second.mRefCount--;
		// last one out turn off the lights; unmap the VBO and erase this entry from mMappedVbos
		if( existingIt->second.mRefCount == 0 ) {
			existingIt->first->unmap();
			mMappedVbos.erase( existingIt );
		}
	}
	else
		CI_LOG_E( "Attempto unmap VertexBufferMesh::MappedAttrib that was never mapped." );
}

std::vector<VertexBufferRef>	VertexBufferMesh::getVertexArrayVbos()
{
	std::vector<VertexBufferRef> result;
	for( auto &it : mVertexArrayVertexBuffers ) {
		result.push_back( it.second );
	}
	
	return result;
}

void VertexBufferMesh::appendVbo( const geom::BufferLayout &layout, const VertexBufferRef &vbo )
{
	mVertexArrayVertexBuffers.push_back( make_pair( layout, vbo ) );
}

uint8_t	VertexBufferMesh::getAttribDims( geom::Attrib attr ) const
{
	for( const auto &vertArrayVbo : mVertexArrayVertexBuffers ) {
		// now iterate the attributes associated with this VBO
		for( const auto &attribInfo : vertArrayVbo.first.getAttribs() ) {
			if( attribInfo.getAttrib() == attr )
				return attribInfo.getDims();
		}
	}
	
	// not found
	return 0;
}

geom::AttribSet	VertexBufferMesh::getAvailableAttribs() const
{
	geom::AttribSet result;
	
	for( auto &vertArrayVbo : mVertexArrayVertexBuffers ) {
		for( auto &attrib : vertArrayVbo.first.getAttribs() )
			result.insert( attrib.getAttrib() );
	}
	
	return result;
}

geom::SourceRef	VertexBufferMesh::createSource() const
{
	return VertexBufferMeshSource::create( this );
}

void VertexBufferMesh::downloadIndices( uint32_t *dest ) const
{
	if( (! mIndices) || (getNumIndices() == 0) )
		return;

	const void *data = mIndices->map();
	if( mIndexType == VK_INDEX_TYPE_UINT16 ) {
		const uint16_t *source = reinterpret_cast<const uint16_t*>( data );
		for( size_t e = 0; e < getNumIndices(); ++e )
			dest[e] = source[e];
	}
	else
		memcpy( dest, data, getNumIndices() * sizeof(uint32_t) );
		
	mIndices->unmap();
}

void VertexBufferMesh::echoIndexRange( std::ostream &os, size_t startIndex, size_t endIndex )
{
	if( ( mNumIndices == 0 ) || ( ! mIndices ) )
		return;

	vector<uint32_t> indices;
	startIndex = std::min<size_t>( startIndex, mNumIndices );
	endIndex = std::min<size_t>( endIndex, mNumIndices );

	const void *rawData = mIndices->map();
	switch( mIndexType ) {
		case VK_INDEX_TYPE_UINT16:
			for( size_t v = startIndex; v < endIndex; ++v )
				indices.push_back( reinterpret_cast<const uint16_t*>( rawData )[v] );
		break;
		case VK_INDEX_TYPE_UINT32:
			for( size_t v = startIndex; v < endIndex; ++v )
				indices.push_back( reinterpret_cast<const uint32_t*>( rawData )[v] );
		break;
		default:
			return;
	}

	echoVertices( os, indices, true );
}

void VertexBufferMesh::echoVertexRange( std::ostream &os, size_t startIndex, size_t endIndex )
{
	vector<uint32_t> indices;
	startIndex = std::min<size_t>( startIndex, mNumVertices );
	endIndex = std::min<size_t>( endIndex, mNumVertices );
	indices.resize( endIndex - startIndex );
	for( size_t s = 0; s < endIndex - startIndex; ++s )
		indices[s] = (uint32_t)(s + startIndex);
	echoVertices( os, indices, false );
}

void VertexBufferMesh::echoVertices( std::ostream &os, const vector<uint32_t> &indices, bool printIndices )
{
	vector<string> attribSemanticNames;
	vector<vector<string>> attribData;
	vector<size_t> attribColLengths;
	vector<string> attribColLeadingSpaceStrings;

	// iterate all the vertex array VBOs; map<geom::BufferLayout,VertexBufferRef>
	for( const auto &vertArrayVbo : mVertexArrayVertexBuffers ) {
		// map this VBO
		const void *rawData = vertArrayVbo.second->map();
		// now iterate the attributes associated with this VBO
		for( const auto &attribInfo : vertArrayVbo.first.getAttribs() ) {
			attribSemanticNames.push_back( geom::attribToString( attribInfo.getAttrib() ) );
			attribData.push_back( vector<string>() );
			size_t stride = ( attribInfo.getStride() == 0 ) ? attribInfo.getDims() * sizeof(float) : attribInfo.getStride();
			for( size_t vIt : indices ) {
				ostringstream ss;
				const float *dataFloat = reinterpret_cast<const float*>( (const uint8_t*)rawData + attribInfo.getOffset() + vIt * stride );
				for( uint8_t d = 0; d < attribInfo.getDims(); ++d ) {
					ss << dataFloat[d];
					if( d != attribInfo.getDims() - 1 )
						ss << ",";
				}
				attribData.back().push_back( ss.str() );
			}
			
			// now calculate the longest string in the column
			attribColLengths.push_back( attribSemanticNames.back().length() + 3 );
			for( auto &attribDataStr : attribData.back() ) {
				attribColLengths.back() = std::max<size_t>( attribColLengths.back(), attribDataStr.length() + 2 );
			}
		}
		
		vertArrayVbo.second->unmap();
	}

	// if we're printing indices then we need to determine the widest as a string
	size_t rowStart = 0;
	if( printIndices ) {
		for( uint32_t v : indices )
			rowStart = std::max( rowStart, to_string( v ).length() );
		rowStart += 2; // account for ": "
	}

	// print attrib semantic header
	ostringstream ss;
	for( size_t a = 0; a < attribSemanticNames.size(); ++a ) {
		// character offset where we should be for this column
		size_t colStartCharIndex = rowStart;
		for( size_t sumA = 0; sumA < a; ++sumA )
			colStartCharIndex += attribColLengths[sumA];
		// offset relative to the previous
		int numSpaces = std::max<int>( (int)(colStartCharIndex - ss.str().length()), 0 );
		// center string
		numSpaces += std::max<int>( (int)(attribColLengths[a] - (attribSemanticNames[a].length()+2)) / 2, 0 );
		for( size_t s = 0; s < numSpaces; s++ )
			ss << " ";
		ss << "<" << attribSemanticNames[a] << "> ";
	}
	os << ss.str();
	os << std::endl;

	// print data rows
	for( size_t v = 0; v < indices.size(); ++v ) {
		ostringstream ss;
		if( printIndices ) {
			string elementStr = to_string( indices[v] ) + ":";
			for( size_t space = 0; space < rowStart - elementStr.length(); ++space )
				ss << ' ';
			ss << elementStr;
			ss << ' ';
		}

		for( size_t a = 0; a < attribSemanticNames.size(); ++a ) {
			// character offset where we should be for this column
			size_t colStartCharIndex = rowStart;
			for( size_t sumA = 0; sumA < a; ++sumA )
				colStartCharIndex += attribColLengths[sumA];
			// offset relative to the previous
			int numSpaces = std::max<int>( (int)(colStartCharIndex - ss.str().length()), 0 );
			// center string
			numSpaces += std::max<int>( (int)(attribColLengths[a] - attribData[a][v].length()) / 2, 0 );
			for( size_t s = 0; s < numSpaces; s++ )
				ss << " ";
			ss << attribData[a][v];
		}
		os << ss.str();
		os << std::endl;
	}
	os << std::endl;
}

VertexBufferMeshSource::VertexBufferMeshSource( const vk::VertexBufferMesh *VertexBufferMesh )
	: mVertexBufferMesh( VertexBufferMesh )
{
}

std::shared_ptr<VertexBufferMeshSource> VertexBufferMeshSource::create( const vk::VertexBufferMesh *VertexBufferMesh )
{
	return std::shared_ptr<VertexBufferMeshSource>( new VertexBufferMeshSource( VertexBufferMesh ) );
}

void VertexBufferMeshSource::loadInto( geom::Target *target, const geom::AttribSet &requestedAttribs ) const
{
	// iterate all the vertex array VBOs; map<geom::BufferLayout,VertexBufferRef>
	for( const auto &vertArrayVbo : mVertexBufferMesh->getVertexArrayLayoutVbos() ) {
		// map this VBO
		const void *rawData = vertArrayVbo.second->map();
		// now iterate the attributes associated with this VBO
		for( const auto &attribInfo : vertArrayVbo.first.getAttribs() ) {
			target->copyAttrib( attribInfo.getAttrib(), attribInfo.getDims(), attribInfo.getStride(), (const float*)((const uint8_t*)rawData + attribInfo.getOffset()), getNumVertices() );
		}
		
		vertArrayVbo.second->unmap();
	}
	
	// copy index data if it's present
	if( mVertexBufferMesh->getNumIndices() ) {
		uint8_t bytesPerIndex;
		switch( mVertexBufferMesh->getIndexDataType() ) {
			case VK_INDEX_TYPE_UINT16:
				bytesPerIndex = 2;
			break;
			default:
				bytesPerIndex = 4;
			break;
		}
		
		std::unique_ptr<uint32_t[]> indices( new uint32_t[mVertexBufferMesh->getNumIndices()] );
		mVertexBufferMesh->downloadIndices( indices.get() );
		target->copyIndices( vk::toGeomPrimitive( mVertexBufferMesh->getPrimitive() ), indices.get(), mVertexBufferMesh->getNumIndices(), bytesPerIndex );
	}
}

size_t VertexBufferMeshSource::getNumVertices() const
{
	return mVertexBufferMesh->getNumVertices();
}

size_t VertexBufferMeshSource::getNumIndices() const
{
	return mVertexBufferMesh->getNumIndices();
}

geom::Primitive VertexBufferMeshSource::getPrimitive() const
{
	return vk::toGeomPrimitive( mVertexBufferMesh->getPrimitive() );
}

uint8_t VertexBufferMeshSource::getAttribDims( geom::Attrib attr ) const
{
	return mVertexBufferMesh->getAttribDims( attr );
}

geom::AttribSet	VertexBufferMeshSource::getAvailableAttribs() const
{
	return mVertexBufferMesh->getAvailableAttribs();
}

}} // namespace ciner::vk