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

#include "cinder/gl/VboMesh.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/ConstantConversions.h"
#include "cinder/gl/Environment.h"
#include "cinder/Log.h"

using namespace std;

namespace cinder { namespace gl {

/////////////////////////////////////////////////////////////////////////////////////////////////
// VboMeshGeomSource
#if ! defined( CINDER_GL_ES )
class VboMeshSource : public geom::Source {
  public:
	static std::shared_ptr<VboMeshSource>	create( const gl::VboMesh *vboMesh );
	
	void			loadInto( geom::Target *target, const geom::AttribSet &requestedAttribs ) const override;
	VboMeshSource*	clone() const override { return new VboMeshSource( *this ); }
	
	size_t			getNumVertices() const override;
	size_t			getNumIndices() const override;
	geom::Primitive	getPrimitive() const override;
	
	uint8_t			getAttribDims( geom::Attrib attr ) const override;
	geom::AttribSet	getAvailableAttribs() const override;
	
  protected:
	VboMeshSource( const gl::VboMesh *vboMesh );
  
	const gl::VboMesh		*mVboMesh;
};
#endif // ! defined( CINDER_GL_ES )

/////////////////////////////////////////////////////////////////////////////////////////////////
// VboMeshGeomTarget
class VboMeshGeomTarget : public geom::Target {
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

	VboMeshGeomTarget( geom::Primitive prim, VboMesh *vboMesh )
		: mPrimitive( prim ), mVboMesh( vboMesh )
	{
		mVboMesh->mNumIndices = 0; // this may be replaced later with a copyIndices call
		// create a vector of temporary data that parallels the VboMesh's vertexData
		for( const auto &vertexArrayBuffer : mVboMesh->getVertexArrayLayoutVbos() ) {
			size_t requiredBytes = vertexArrayBuffer.first.calcRequiredStorage( mVboMesh->mNumVertices );
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
	VboMesh						*mVboMesh;
};

geom::Primitive	VboMeshGeomTarget::getPrimitive() const
{
	return mPrimitive;
}

uint8_t	VboMeshGeomTarget::getAttribDims( geom::Attrib attr ) const
{
	return mVboMesh->getAttribDims( attr );
}

void VboMeshGeomTarget::copyAttrib( geom::Attrib attr, uint8_t dims, size_t /*strideBytes*/, const float *srcData, size_t count )
{
	// if we don't have it we don't want it
	if( getAttribDims( attr ) == 0 )
		return;
	
	// we need to find which element of 'mBufferData' containts 'attr'
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
	if( count != mVboMesh->mNumVertices ) {
		CI_LOG_E( "copyAttrib() called with " << count << " elements. " << mVboMesh->mNumVertices << " expected." );
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

void VboMeshGeomTarget::copyIndices( geom::Primitive /*primitive*/, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex )
{
// @TODO: Find a better way to handle this
#if defined( CINDER_GL_ES_2 )
	if( GL_UNSIGNED_SHORT == gl::env()->getPreferredIndexType() ) {
		requiredBytesPerIndex = 2;
		numIndices = std::min<size_t>( numIndices, 65535 );
	}
#endif

	mVboMesh->mNumIndices = (uint32_t)numIndices;
	if( mVboMesh->mNumIndices == 0 ) {
		mVboMesh->mIndices.reset();
	}
	else if( requiredBytesPerIndex <= 2 ) {
		mVboMesh->mIndexType = GL_UNSIGNED_SHORT;
		std::unique_ptr<uint16_t[]> indices( new uint16_t[numIndices] );
		copyIndexData( source, numIndices, indices.get() );
		if( ! mVboMesh->mIndices )
			mVboMesh->mIndices = Vbo::create( GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(uint16_t), indices.get() );
		else
			mVboMesh->mIndices->copyData( numIndices * sizeof(uint16_t), indices.get() );
	}
	else {
		mVboMesh->mIndexType = GL_UNSIGNED_INT;
		std::unique_ptr<uint32_t[]> indices( new uint32_t[numIndices] );
		copyIndexData( source, numIndices, indices.get() );
		if( ! mVboMesh->mIndices )
			mVboMesh->mIndices = Vbo::create( GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(uint32_t), indices.get() );
		else
			mVboMesh->mIndices->copyData( numIndices * sizeof(uint32_t), indices.get() );
	}
}

void VboMeshGeomTarget::copyBuffers()
{
	// iterate all the buffers in mBufferData and upload them to the corresponding VBO in the VboMesh
	for( auto bufferDataIt = mBufferData.begin(); bufferDataIt != mBufferData.end(); ++bufferDataIt ) {
		auto vertexArrayIt = mVboMesh->mVertexArrayVbos.begin() + std::distance( mBufferData.begin(), bufferDataIt );
		vertexArrayIt->second->copyData( bufferDataIt->mDataSize, bufferDataIt->mData.get() );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VboMesh::Layout
VboMesh::Layout& VboMesh::Layout::attrib( geom::Attrib attrib, uint8_t dims )
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

VboMesh::Layout& VboMesh::Layout::attrib( const geom::AttribInfo &attribInfo )
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

void VboMesh::Layout::allocate( size_t numVertices, geom::BufferLayout *resultBufferLayout, gl::VboRef *resultVbo ) const
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
		if( *resultVbo ) // non-null shared_ptr means the VBO should be resized
			(*resultVbo)->ensureMinimumSize( totalDataBytes );
		else // else allocate
			*resultVbo = Vbo::create( GL_ARRAY_BUFFER, totalDataBytes, nullptr, mUsage );
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// VboMesh
VboMeshRef VboMesh::create( const geom::Source &source )
{
	// Pass an empty std::vector<pair<Layout,VboRef>> to imply we want to pull data from the Source
	return VboMeshRef( new VboMesh( source, std::vector<pair<Layout,VboRef>>(), VboRef() ) );
}

VboMeshRef VboMesh::create( const geom::Source &source, const geom::AttribSet &requestedAttribs )
{
	// make an interleaved VboMesh::Layout with 'requestedAttribs'
	Layout layout;
	for( const auto &attrib : requestedAttribs )
		layout.attrib( attrib, 0 ); // 0 dim implies querying the Source for its dimension
	
	return VboMeshRef( new VboMesh( source, { { layout, nullptr } }, nullptr ) );
}

VboMeshRef VboMesh::create( const geom::Source &source, const std::vector<VboMesh::Layout> &vertexArrayLayouts )
{
	std::vector<std::pair<VboMesh::Layout,VboRef>> layoutVbos;
	for( const auto &vertexArrayLayout : vertexArrayLayouts )
		layoutVbos.push_back( std::make_pair( vertexArrayLayout, (VboRef)nullptr ) );

	return VboMeshRef( new VboMesh( source, layoutVbos, nullptr ) );
}

VboMeshRef VboMesh::create( const geom::Source &source, const std::vector<std::pair<VboMesh::Layout,VboRef>> &vertexArrayLayouts, const VboRef &indexVbo )
{
	return VboMeshRef( new VboMesh( source, vertexArrayLayouts, indexVbo ) );
}

VboMeshRef VboMesh::create( uint32_t numVertices, GLenum glPrimitive, const std::vector<pair<geom::BufferLayout,VboRef>> &vertexArrayBuffers, uint32_t numIndices, GLenum indexType, const VboRef &indexVbo )
{
	return VboMeshRef( new VboMesh( numVertices, numIndices, glPrimitive, indexType, vertexArrayBuffers, indexVbo ) );
}

VboMeshRef VboMesh::create( uint32_t numVertices, GLenum glPrimitive, const std::vector<Layout> &vertexArrayLayouts, uint32_t numIndices, GLenum indexType, const VboRef &indexVbo )
{
	return VboMeshRef( new VboMesh( numVertices, numIndices, glPrimitive, indexType, vertexArrayLayouts, indexVbo ) );
}

VboMesh::VboMesh( const geom::Source &source, std::vector<pair<Layout,VboRef>> vertexArrayBuffers, const VboRef &indexArrayVbo )
{
	// An empty vertexArrayBuffers implies we should just pull whatever attribs the Source is pushing. We arrived here from VboMesh::create( Source& )
	if( vertexArrayBuffers.empty() ) {
		// Create an interleaved Layout based on what's in the Source
		vertexArrayBuffers.push_back( std::pair<Layout,VboRef>( VboMesh::Layout().usage( GL_STATIC_DRAW ).interleave(), nullptr ) );
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
	for( const auto &vertexArrayBuffer : vertexArrayBuffers )
		for( const auto &attribInfo : vertexArrayBuffer.first.getAttribs() )
			requestedAttribs.insert( attribInfo.getAttrib() );

	mNumVertices = (uint32_t)source.getNumVertices();
	mGlPrimitive = toGl( source.getPrimitive() );

	// iterate 'vertexArrayBuffers' and allocate mVertexArrayVbos, which is the parallel vector of <geom::BufferLayout,VboRef> pairs
	for( const auto &vertexArrayBuffer : vertexArrayBuffers ) {
		geom::BufferLayout bufferLayout;
		VboRef vbo = vertexArrayBuffer.second;
		// we pass nullptr for the VBO if we already have one, to prevent re-allocation by allocate()
		vertexArrayBuffer.first.allocate( mNumVertices, &bufferLayout, &vbo );
		mVertexArrayVbos.push_back( make_pair( bufferLayout, vbo ) );
	}
	
	// Set our indices VBO to indexArrayVBO, which may well be empty, so that the target doesn't blow it away. Must do this before we loadInto().
	mIndices = indexArrayVbo;
	
	VboMeshGeomTarget target( source.getPrimitive(), this );
	source.loadInto( &target, requestedAttribs );
	// we need to let the target know it can copy from its internal buffers to our vertexData VBOs
	target.copyBuffers();
}

VboMesh::VboMesh( uint32_t numVertices, uint32_t numIndices, GLenum glPrimitive, GLenum indexType, const std::vector<pair<geom::BufferLayout,VboRef>> &vertexArrayBuffers, const VboRef &indexVbo )
	: mNumVertices( numVertices ), mNumIndices( numIndices ), mGlPrimitive( glPrimitive ), mIndexType( indexType ), mVertexArrayVbos( vertexArrayBuffers ), mIndices( indexVbo )
{
// @TODO: Find a better way to handle this
#if defined( CINDER_GL_ES_2 )
	if( GL_UNSIGNED_SHORT == gl::env()->getPreferredIndexType() ) {
		mIndexType = GL_UNSIGNED_SHORT;
	}
#endif

	if( ! mIndices )
		allocateIndexVbo();
}

VboMesh::VboMesh( uint32_t numVertices, uint32_t numIndices, GLenum glPrimitive, GLenum indexType, const std::vector<Layout> &vertexArrayLayouts, const VboRef &indexVbo )
	: mNumVertices( numVertices ), mNumIndices( numIndices ), mGlPrimitive( glPrimitive ), mIndexType( indexType ), mIndices( indexVbo )
{
// @TODO: Find a better way to handle this
#if defined( CINDER_GL_ES_2 )
	if( GL_UNSIGNED_SHORT == gl::env()->getPreferredIndexType() ) {
		mIndexType = GL_UNSIGNED_SHORT;
	}
#endif

	geom::BufferLayout bufferLayout;
	for( const auto &layout : vertexArrayLayouts ) {
		VboRef vbo;
		layout.allocate( numVertices, &bufferLayout, &vbo );
		mVertexArrayVbos.push_back( make_pair( bufferLayout, vbo ) );
	}
	
	if( ! mIndices )
		allocateIndexVbo();
}

void VboMesh::allocateIndexVbo()
{
	if( mNumIndices == 0 )
		return;

	size_t bytesRequired = 1; // GL_UNSIGNED_BYTE
	if( mIndexType == GL_UNSIGNED_SHORT )
		bytesRequired = 2;
	else if( mIndexType == GL_UNSIGNED_INT )
		bytesRequired = 4;
	else
		throw geom::ExcIllegalIndexType();
		
	mIndices = gl::Vbo::create( GL_ELEMENT_ARRAY_BUFFER, mNumIndices * bytesRequired, nullptr, GL_STATIC_DRAW );
}

void VboMesh::buildVao( const GlslProgRef &shader, const AttribGlslMap &attributeMapping )
{
	buildVao( shader.get(), attributeMapping );
}

void VboMesh::buildVao( const GlslProg* shader, const AttribGlslMap &attributeMapping )
{
	auto ctx = gl::context();
	std::set<geom::Attrib> enabledAttribs;
	// iterate all the vertex array VBOs
	for( const auto &vertArrayVbo : mVertexArrayVbos ) {
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
				for( uint32_t i = 0; i < numTimes; i++ ) {
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
		   /* From GL 4.3 core spec, section 11.1.1 (Vertex Attributes):
			* "For GetActiveAttrib, all active vertex shader input variables
			* are enumerated, including the special built-in inputs gl_VertexID
			* and gl_InstanceID."
			*/
		   ( glslAttribName != "gl_InstanceID" && glslAttribName != "gl_VertexID") )
			CI_LOG_W( "Batch GlslProg expected an Attrib of " << geom::attribToString( attribSemantic ) << ", with name " << glslAttribName << " but vertex data doesn't provide it." );
	}
	
	if( mIndices )
		mIndices->bind();

}

void VboMesh::drawImpl( GLint first, GLsizei count )
{
	if( mIndices ) {
		size_t firstByteOffset = first;
		if( mIndexType == GL_UNSIGNED_INT ) firstByteOffset *= 4;
		else if( mIndexType == GL_UNSIGNED_SHORT ) firstByteOffset *= 2;
		glDrawElements( mGlPrimitive, ( count < 0 ) ? ( mNumIndices - first ) : count, mIndexType, (GLvoid*)( firstByteOffset ) );
	}
	else
		glDrawArrays( mGlPrimitive, first, ( count < 0 ) ? ( mNumVertices - first ) : count );
}

#if defined( CINDER_GL_HAS_DRAW_INSTANCED )
void VboMesh::drawInstancedImpl( GLsizei instanceCount )
{
	auto ctx = gl::context();
	if( mIndices )
		ctx->drawElementsInstanced( mGlPrimitive, mNumIndices, mIndexType, (GLvoid*)( 0 ), instanceCount );
	else
		ctx->drawArraysInstanced( mGlPrimitive, 0, mNumVertices, instanceCount );
}
#endif // defined( CINDER_GL_HAS_DRAW_INSTANCED )

#if defined( CINDER_GL_HAS_DRAW_INDIRECT )
void VboMesh::drawIndirectImpl( const GLvoid *indirect )
{
	auto ctx = gl::context();
	if( mNumIndices )
		ctx->drawElementsIndirect( mGlPrimitive, mIndexType, indirect );
	else
		ctx->drawArraysIndirect( mGlPrimitive, indirect );
}
#endif // defined( CINDER_GL_HAS_DRAW_INDIRECT )

std::pair<geom::BufferLayout,VboRef>* VboMesh::findAttrib( geom::Attrib attr )
{
	for( auto &vertexArrayVbo : mVertexArrayVbos ) {
		if( vertexArrayVbo.first.hasAttrib( attr ) )
			return &vertexArrayVbo;
	}
	
	return nullptr;
}

void VboMesh::bufferAttrib( geom::Attrib attrib, size_t dataSizeBytes, const void *data )
{
	auto layoutVbo = findAttrib( attrib );
	if( ! layoutVbo )
		throw geom::ExcMissingAttrib();
	auto attribInfo = layoutVbo->first.getAttribInfo( attrib );

	// if the attribute is stored planar, a call to glBufferSubData should suffice
	if( attribInfo.getStride() == 0 || attribInfo.getStride() == attribInfo.getByteSize() ) {
		layoutVbo->second->bufferSubData( attribInfo.getOffset(), dataSizeBytes, data );
	}
	else { // interleaved data
#if ! defined( CINDER_GL_ANGLE ) || defined( CINDER_GL_ES_3 )
		uint8_t *ptr = reinterpret_cast<uint8_t*>( layoutVbo->second->mapWriteOnly() );
		if( ! ptr ) {
			CI_LOG_E( "Failed to map VBO" );
			return;
		}
  
		ptr += attribInfo.getOffset();
		const uint8_t *srcData = reinterpret_cast<const uint8_t*>( data );
		const uint8_t *endSrcData = (uint8_t*)data + dataSizeBytes;
		while( srcData < endSrcData ) {
			memcpy( ptr, srcData, attribInfo.getByteSize() );
			srcData += attribInfo.getByteSize();
			ptr += attribInfo.getStride();
		}
		
		layoutVbo->second->unmap();
#else
		CI_LOG_E( "ANGLE does not support bufferAttrib() with interleaved data" );
#endif		
	}
}

void VboMesh::bufferIndices( size_t dataSizeBytes, const void *data )
{
	if( ! mIndices ) {
		CI_LOG_E( "VboMesh::bufferIndices() called on VboMesh with null index VBO" );
		return;
	}
	
	mIndices->bufferSubData( 0, dataSizeBytes, data );
}

#if defined( CINDER_GL_ES_3 ) || (! defined( CINDER_GL_ANGLE ))
template<typename T>
VboMesh::MappedAttrib<T> VboMesh::mapAttribImpl( geom::Attrib attr, int dims, bool orphanExisting )
{
	std::pair<geom::BufferLayout,VboRef>* layoutVbo = findAttrib( attr );
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
		if( orphanExisting )
			mappedVboInfo.mPtr = layoutVbo->second->mapReplace();
		else
			mappedVboInfo.mPtr = layoutVbo->second->mapWriteOnly();
		mMappedVbos[layoutVbo->second] = mappedVboInfo;
		dataPtr = mappedVboInfo.mPtr;
	}
	
	auto attribInfo = layoutVbo->first.getAttribInfo( attr );
	
	if( dims != attribInfo.getDims() ) {
		CI_LOG_W( "Mapping geom::Attrib of dims " << (int)attribInfo.getDims() << " to type of dims " << dims );	
	}

	auto stride = ( attribInfo.getStride() == 0 ) ? sizeof(T) : attribInfo.getStride();
	return VboMesh::MappedAttrib<T>( this, layoutVbo->second, ((uint8_t*)dataPtr) + attribInfo.getOffset(), stride );
}

VboMesh::MappedAttrib<float> VboMesh::mapAttrib1f( geom::Attrib attr, bool orphanExisting )
{
	return mapAttribImpl<float>( attr, 1, orphanExisting );
}

VboMesh::MappedAttrib<vec2> VboMesh::mapAttrib2f( geom::Attrib attr, bool orphanExisting )
{
	return mapAttribImpl<vec2>( attr, 2, orphanExisting );
}

VboMesh::MappedAttrib<vec3> VboMesh::mapAttrib3f( geom::Attrib attr, bool orphanExisting )
{
	return mapAttribImpl<vec3>( attr, 3, orphanExisting );
}

VboMesh::MappedAttrib<vec4> VboMesh::mapAttrib4f( geom::Attrib attr, bool orphanExisting )
{
	return mapAttribImpl<vec4>( attr, 4, orphanExisting );
}

void VboMesh::MappedAttribBase::unmap()
{
	if( mMapping )
		mMapping->unmap();
	mPtr = nullptr;
}

VboMesh::MappedAttribBase::~MappedAttribBase()
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

void VboMesh::MappedAttribBase::Mapping::unmap()
{
	if( ! mMapped ) {
		CI_LOG_E( "MappedAttrib unmapped more than once" );
	}
	else {
		mMesh->unmapVboImpl( mVbo );
		mMapped = false;
	}
}

void VboMesh::unmapVboImpl( const VboRef &vbo )
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
		CI_LOG_E( "Attempto unmap VboMesh::MappedAttrib that was never mapped." );
}

#endif // defined(CINDER_GL_ES_3) || (! defined( CINDER_GL_ANGLE ))

std::vector<VboRef>	VboMesh::getVertexArrayVbos()
{
	std::vector<VboRef> result;
	for( auto &it : mVertexArrayVbos ) {
		result.push_back( it.second );
	}
	
	return result;
}

void VboMesh::appendVbo( const geom::BufferLayout &layout, const VboRef &vbo )
{
	mVertexArrayVbos.push_back( make_pair( layout, vbo ) );
}

uint8_t	VboMesh::getAttribDims( geom::Attrib attr ) const
{
	for( const auto &vertArrayVbo : mVertexArrayVbos ) {
		// now iterate the attributes associated with this VBO
		for( const auto &attribInfo : vertArrayVbo.first.getAttribs() ) {
			if( attribInfo.getAttrib() == attr )
				return attribInfo.getDims();
		}
	}
	
	// not found
	return 0;
}

geom::AttribSet	VboMesh::getAvailableAttribs() const
{
	geom::AttribSet result;
	
	for( auto &vertArrayVbo : mVertexArrayVbos ) {
		for( auto &attrib : vertArrayVbo.first.getAttribs() )
			result.insert( attrib.getAttrib() );
	}
	
	return result;
}

#if ! defined( CINDER_GL_ES )
geom::SourceRef	VboMesh::createSource() const
{
	return VboMeshSource::create( this );
}

void VboMesh::downloadIndices( uint32_t *dest ) const
{
	if( (! mIndices) || (getNumIndices() == 0) )
		return;

#if defined( CINDER_GL_ES )
	const void *data = mIndices->map( GL_READ_ONLY_OES );
#else
	const void *data = mIndices->map( GL_READ_ONLY );
#endif
	if( mIndexType == GL_UNSIGNED_SHORT ) {
		const uint16_t *source = reinterpret_cast<const uint16_t*>( data );
		for( size_t e = 0; e < getNumIndices(); ++e )
			dest[e] = source[e];
	}
	else
		memcpy( dest, data, getNumIndices() * sizeof(uint32_t) );
		
	mIndices->unmap();
}

void VboMesh::echoIndexRange( std::ostream &os, size_t startIndex, size_t endIndex )
{
	if( ( mNumIndices == 0 ) || ( ! mIndices ) )
		return;

	vector<uint32_t> indices;
	startIndex = std::min<size_t>( startIndex, mNumIndices );
	endIndex = std::min<size_t>( endIndex, mNumIndices );

	const void *rawData = mIndices->map( GL_READ_ONLY );
	switch( mIndexType ) {
		case GL_UNSIGNED_BYTE:
			for( size_t v = startIndex; v < endIndex; ++v )
				indices.push_back( reinterpret_cast<const uint8_t*>( rawData )[v] );
		break;
		case GL_UNSIGNED_SHORT:
			for( size_t v = startIndex; v < endIndex; ++v )
				indices.push_back( reinterpret_cast<const uint16_t*>( rawData )[v] );
		break;
		case GL_UNSIGNED_INT:
			for( size_t v = startIndex; v < endIndex; ++v )
				indices.push_back( reinterpret_cast<const uint32_t*>( rawData )[v] );
		break;
		default:
			return;
	}

	echoVertices( os, indices, true );
}

void VboMesh::echoVertexRange( std::ostream &os, size_t startIndex, size_t endIndex )
{
	vector<uint32_t> indices;
	startIndex = std::min<size_t>( startIndex, mNumVertices );
	endIndex = std::min<size_t>( endIndex, mNumVertices );
	indices.resize( endIndex - startIndex );
	for( size_t s = 0; s < endIndex - startIndex; ++s )
		indices[s] = (uint32_t)(s + startIndex);
	echoVertices( os, indices, false );
}

void VboMesh::echoVertices( std::ostream &os, const vector<uint32_t> &indices, bool printIndices )
{
	vector<string> attribSemanticNames;
	vector<vector<string>> attribData;
	vector<size_t> attribColLengths;
	vector<string> attribColLeadingSpaceStrings;

	// save the GL_ARRAY_BUFFER binding
	auto prevBufferBinding = gl::context()->getBufferBinding( GL_ARRAY_BUFFER );

	// iterate all the vertex array VBOs; map<geom::BufferLayout,VboRef>
	for( const auto &vertArrayVbo : mVertexArrayVbos ) {
		// map this VBO
		const void *rawData = vertArrayVbo.second->map( GL_READ_ONLY );
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
		for( int s = 0; s < numSpaces; s++ )
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
			for( int s = 0; s < numSpaces; s++ )
				ss << " ";
			ss << attribData[a][v];
		}
		os << ss.str();
		os << std::endl;
	}
	os << std::endl;

	// restore the GL_ARRAY_BUFFER binding
	gl::context()->bindBuffer( GL_ARRAY_BUFFER, prevBufferBinding );
}

#endif // ! defined( CINDER_GL_ES )


#if ! defined( CINDER_GL_ES )

VboMeshSource::VboMeshSource( const gl::VboMesh *vboMesh )
	: mVboMesh( vboMesh )
{
}

std::shared_ptr<VboMeshSource> VboMeshSource::create( const gl::VboMesh *vboMesh )
{
	return std::shared_ptr<VboMeshSource>( new VboMeshSource( vboMesh ) );
}

void VboMeshSource::loadInto( geom::Target *target, const geom::AttribSet & /*requestedAttribs*/ ) const
{
	// iterate all the vertex array VBOs; map<geom::BufferLayout,VboRef>
	for( const auto &vertArrayVbo : mVboMesh->getVertexArrayLayoutVbos() ) {
		// map this VBO
		const void *rawData = vertArrayVbo.second->map( GL_READ_ONLY );
		// now iterate the attributes associated with this VBO
		for( const auto &attribInfo : vertArrayVbo.first.getAttribs() ) {
			target->copyAttrib( attribInfo.getAttrib(), attribInfo.getDims(), attribInfo.getStride(), (const float*)((const uint8_t*)rawData + attribInfo.getOffset()), getNumVertices() );
		}
		
		vertArrayVbo.second->unmap();
	}
	
	// copy index data if it's present
	if( mVboMesh->getNumIndices() ) {
		uint8_t bytesPerIndex;
		switch( mVboMesh->getIndexDataType() ) {
			case GL_UNSIGNED_SHORT:
				bytesPerIndex = 2;
			break;
			default:
				bytesPerIndex = 4;
			break;
		}
		
		std::unique_ptr<uint32_t[]> indices( new uint32_t[mVboMesh->getNumIndices()] );
		mVboMesh->downloadIndices( indices.get() );
		target->copyIndices( gl::toGeomPrimitive( mVboMesh->getGlPrimitive() ), indices.get(), mVboMesh->getNumIndices(), bytesPerIndex );
	}
}

size_t VboMeshSource::getNumVertices() const
{
	return mVboMesh->getNumVertices();
}

size_t VboMeshSource::getNumIndices() const
{
	return mVboMesh->getNumIndices();
}

geom::Primitive VboMeshSource::getPrimitive() const
{
	return gl::toGeomPrimitive( mVboMesh->getGlPrimitive() );
}

uint8_t VboMeshSource::getAttribDims( geom::Attrib attr ) const
{
	return mVboMesh->getAttribDims( attr );
}

geom::AttribSet	VboMeshSource::getAvailableAttribs() const
{
	return mVboMesh->getAvailableAttribs();
}

#endif // ! defined( CINDER_GL_ES )

} } // namespace cinder::gl
