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
#include "cinder/Log.h"

using namespace std;

namespace cinder { namespace gl {

/////////////////////////////////////////////////////////////////////////////////////////////////
// VboMeshGeomSource
#if ! defined( CINDER_GL_ES )
class VboMeshSource : public geom::Source {
  public:
	static std::shared_ptr<VboMeshSource>	create( const gl::VboMesh *vboMesh );
	
	virtual void	loadInto( geom::Target *target ) const override;
	
	virtual size_t			getNumVertices() const override;
	virtual size_t			getNumIndices() const override;
	virtual geom::Primitive	getPrimitive() const override;
	
	virtual uint8_t			getAttribDims( geom::Attrib attr ) const override;
	
  protected:
	VboMeshSource( const gl::VboMesh *vboMesh );
  
	const gl::VboMesh		*mVboMesh;
};
#endif // ! defined( CINDER_GL_ES )

/////////////////////////////////////////////////////////////////////////////////////////////////
// VboMeshGeomTarget
class VboMeshGeomTarget : public geom::Target {
  public:
	VboMeshGeomTarget( geom::Primitive prim, const geom::BufferLayout &bufferLayout, uint8_t *data, VboMesh *vboMesh )
		: mPrimitive( prim ), mBufferLayout( bufferLayout ), mData( data ), mVboMesh( vboMesh )
	{
		mVboMesh->mNumIndices = 0; // this may be replaced later with a copyIndices call
	}
	
	virtual geom::Primitive	getPrimitive() const override;
	virtual uint8_t	getAttribDims( geom::Attrib attr ) const override;
	virtual void copyAttrib( geom::Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count ) override;
	virtual void copyIndices( geom::Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex ) override;
	
  protected:
	geom::Primitive				mPrimitive;
	const geom::BufferLayout	&mBufferLayout;
	uint8_t						*mData;
	VboMesh						*mVboMesh;
};

geom::Primitive	VboMeshGeomTarget::getPrimitive() const
{
	return mPrimitive;
}

uint8_t	VboMeshGeomTarget::getAttribDims( geom::Attrib attr ) const
{
	return mBufferLayout.getAttribDims( attr );
}

void VboMeshGeomTarget::copyAttrib( geom::Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count )
{
//	mMesh->copyAttrib( attr, dims, strideBytes, srcData, count );
	if( mBufferLayout.hasAttrib( attr ) ) {
		geom::AttribInfo attrInfo = mBufferLayout.getAttribInfo( attr );
		geom::copyData( dims, srcData, count, attrInfo.getDims(), attrInfo.getStride(), reinterpret_cast<float*>( mData + attrInfo.getOffset() ) ); 
	}
}

void VboMeshGeomTarget::copyIndices( geom::Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex )
{
	mVboMesh->mNumIndices = numIndices;

	if( requiredBytesPerIndex <= 2 ) {
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
	*resultVbo = Vbo::create( GL_ARRAY_BUFFER, totalDataBytes, nullptr, mUsage );
}

///////////////////////////////////////////////////////////////////////////////////////
// VboMesh
VboMeshRef VboMesh::create( const geom::Source &source )
{
	return VboMeshRef( new VboMesh( source, VboRef(), VboRef() ) );
}

VboMeshRef VboMesh::create( uint32_t numVertices, GLenum glPrimitive, const std::vector<pair<geom::BufferLayout,VboRef>> &vertexArrayBuffers, uint32_t numIndices, GLenum indexType, const VboRef &indexVbo )
{
	return VboMeshRef( new VboMesh( numVertices, numIndices, glPrimitive, indexType, vertexArrayBuffers, indexVbo ) );
}

VboMeshRef VboMesh::create( const geom::Source &source, const VboRef &arrayVbo, const VboRef &indexArrayVbo )
{
	return VboMeshRef( new VboMesh( source, arrayVbo, indexArrayVbo ) );
}

VboMeshRef VboMesh::create( uint32_t numVertices, GLenum glPrimitive, const std::vector<Layout> &vertexArrayLayouts, uint32_t numIndices, GLenum indexType, const VboRef &indexVbo )
{
	return VboMeshRef( new VboMesh( numVertices, numIndices, glPrimitive, indexType, vertexArrayLayouts, indexVbo ) );
}

VboMesh::VboMesh( const geom::Source &source, const VboRef &arrayVbo, const VboRef &indexArrayVbo )
{
	mNumVertices = source.getNumVertices();

	mGlPrimitive = toGl( source.getPrimitive() );

	size_t vertexDataSizeBytes = 0;
	geom::BufferLayout bufferLayout;
	for( int attribIt = 0; attribIt < (int)geom::Attrib::NUM_ATTRIBS; ++attribIt ) {
		auto attribDims = source.getAttribDims( (geom::Attrib)attribIt );
		if( attribDims > 0 ) {
			bufferLayout.append( (geom::Attrib)attribIt, geom::DataType::FLOAT, attribDims, 0, vertexDataSizeBytes );
			vertexDataSizeBytes += attribDims * sizeof(float) * mNumVertices;
		}
	}

	// TODO: this should use mapBuffer when available
	std::unique_ptr<uint8_t[]> buffer( new uint8_t[vertexDataSizeBytes] );
	
	// Set our indices VBO to indexArrayVBO, which may well be empty, so that the target doesn't blow it away. Must do this before we loadInto().
	mIndices = indexArrayVbo;
	
	VboMeshGeomTarget target( source.getPrimitive(), bufferLayout, buffer.get(), this );
	source.loadInto( &target );

	VboRef vertexDataVbo = arrayVbo;
	if( ! vertexDataVbo )
		vertexDataVbo = gl::Vbo::create( GL_ARRAY_BUFFER, vertexDataSizeBytes, buffer.get() );
	else
		vertexDataVbo->copyData( vertexDataSizeBytes, buffer.get() );

	mVertexArrayVbos.push_back( make_pair( bufferLayout, vertexDataVbo ) );
}

VboMesh::VboMesh( uint32_t numVertices, uint32_t numIndices, GLenum glPrimitive, GLenum indexType, const std::vector<pair<geom::BufferLayout,VboRef>> &vertexArrayBuffers, const VboRef &indexVbo )
	: mNumVertices( numVertices ), mNumIndices( numIndices ), mGlPrimitive( glPrimitive ), mIndexType( indexType ), mVertexArrayVbos( vertexArrayBuffers ), mIndices( indexVbo )
{
	if( ! mIndices )
		allocateIndexVbo();
}

VboMesh::VboMesh( uint32_t numVertices, uint32_t numIndices, GLenum glPrimitive, GLenum indexType, const std::vector<Layout> &vertexArrayLayouts, const VboRef &indexVbo )
	: mNumVertices( numVertices ), mNumIndices( numIndices ), mGlPrimitive( glPrimitive ), mIndexType( indexType ), mIndices( indexVbo )
{
	geom::BufferLayout bufferLayout;
	VboRef vbo;
	for( const auto &layout : vertexArrayLayouts ) {
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
	auto ctx = gl::context();
	
	// iterate all the vertex array VBOs; map<geom::BufferLayout,VboRef>
	for( const auto &vertArrayVbo : mVertexArrayVbos ) {
		// bind this VBO (to the current VAO)
		vertArrayVbo.second->bind();
		// now iterate the attributes associated with this VBO
		for( const auto &attribInfo : vertArrayVbo.first.getAttribs() ) {
			int loc = -1;
			// first see if we have a mapping in 'attributeMapping'
			auto attributeMappingIt = attributeMapping.find( attribInfo.getAttrib() );
			if( attributeMappingIt != attributeMapping.end() ) {
				loc = shader->getAttribLocation( attributeMappingIt->second );
			}
			// otherwise, try to get the location of the attrib semantic in the shader if it's present
			else if( shader->hasAttribSemantic( attribInfo.getAttrib() ) ) {
				loc = shader->getAttribSemanticLocation( attribInfo.getAttrib() );			
			}
			
			// if either the shader's mapping or 'attributeMapping' has this semantic, add it to the VAO
			if( loc != -1 ) {
				ctx->enableVertexAttribArray( loc );
				ctx->vertexAttribPointer( loc, attribInfo.getDims(), GL_FLOAT, GL_FALSE, attribInfo.getStride(), (const void*)attribInfo.getOffset() );
				if( attribInfo.getInstanceDivisor() > 0 )
					ctx->vertexAttribDivisor( loc, attribInfo.getInstanceDivisor() );
			}
		}
	}
	
	if( mNumIndices > 0 )
		mIndices->bind();
}

void VboMesh::drawImpl()
{
	if( mNumIndices )
		glDrawElements( mGlPrimitive, mNumIndices, mIndexType, (GLvoid*)( 0 ) );
	else
		glDrawArrays( mGlPrimitive, 0, mNumVertices );
}

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
#if ! defined( CINDER_GL_ANGLE )
  #if defined( CINDER_GL_ES_2 )
		uint8_t *ptr = reinterpret_cast<uint8_t*>( layoutVbo->second->map( GL_WRITE_ONLY_OES ) );
  #else
		uint8_t *ptr = reinterpret_cast<uint8_t*>( layoutVbo->second->map( GL_WRITE_ONLY ) );
  #endif
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
		if( orphanExisting ) {
			layoutVbo->second->bufferData( layoutVbo->second->getSize(), nullptr, layoutVbo->second->getUsage() );
		}
#if defined( CINDER_GL_ES )
		mappedVboInfo.mPtr = layoutVbo->second->map( GL_WRITE_ONLY_OES );
#else
		mappedVboInfo.mPtr = layoutVbo->second->map( GL_WRITE_ONLY );
#endif
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

VboMesh::MappedAttrib<Vec2f> VboMesh::mapAttrib2f( geom::Attrib attr, bool orphanExisting )
{
	return mapAttribImpl<Vec2f>( attr, 2, orphanExisting );
}

VboMesh::MappedAttrib<Vec3f> VboMesh::mapAttrib3f( geom::Attrib attr, bool orphanExisting )
{
	return mapAttribImpl<Vec3f>( attr, 3, orphanExisting );
}

VboMesh::MappedAttrib<Vec4f> VboMesh::mapAttrib4f( geom::Attrib attr, bool orphanExisting )
{
	return mapAttribImpl<Vec4f>( attr, 4, orphanExisting );
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
		indices[s] = s + startIndex;
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
		int numSpaces = std::max<int>( colStartCharIndex - ss.str().length(), 0 );
		// center string
		numSpaces += std::max<int>( (attribColLengths[a] - (attribSemanticNames[a].length()+2)) / 2, 0 );
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
			int numSpaces = std::max<int>( colStartCharIndex - ss.str().length(), 0 );
			// center string
			numSpaces += std::max<int>( (attribColLengths[a] - attribData[a][v].length()) / 2, 0 );
			for( size_t s = 0; s < numSpaces; s++ )
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

void VboMeshSource::loadInto( geom::Target *target ) const
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
			case GL_UNSIGNED_INT:
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

#endif // ! defined( CINDER_GL_ES )

} } // namespace cinder::gl
