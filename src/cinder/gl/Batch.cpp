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

#include "cinder/gl/Batch.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

namespace cinder { namespace gl {

/////////////////////////////////////////////////////////////////////////////////////////////////
// BatchGeomTarget
class BatchGeomTarget : public geom::Target {
  public:
	BatchGeomTarget( geom::Primitive prim, const geom::BufferLayout &bufferLayout, uint8_t *data, Batch *batch )
		: mPrimitive( prim ), mBufferLayout( bufferLayout ), mData( data ), mBatch( batch )
	{
		mBatch->mNumIndices = 0; // this may be replaced later with a copyIndices call
	}
	
	virtual geom::Primitive	getPrimitive() const override;
	virtual uint8_t	getAttribDims( geom::Attrib attr ) const override;
	virtual void copyAttrib( geom::Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count ) override;
	virtual void copyIndices( geom::Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex ) override;
	
  protected:
	geom::Primitive				mPrimitive;
	const geom::BufferLayout	&mBufferLayout;
	uint8_t						*mData;
	Batch						*mBatch;
};

geom::Primitive	BatchGeomTarget::getPrimitive() const
{
	return mPrimitive;
}

uint8_t	BatchGeomTarget::getAttribDims( geom::Attrib attr ) const
{
	return mBufferLayout.getAttribDims( attr );
}

void BatchGeomTarget::copyAttrib( geom::Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count )
{
//	mMesh->copyAttrib( attr, dims, strideBytes, srcData, count );
	if( mBufferLayout.hasAttrib( attr ) ) {
		geom::AttribInfo attrInfo = mBufferLayout.getAttribInfo( attr );
		geom::copyData( dims, srcData, count, attrInfo.getDims(), attrInfo.getStride(), reinterpret_cast<float*>( mData + attrInfo.getOffset() ) ); 
	}
}

void BatchGeomTarget::copyIndices( geom::Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex )
{
	mBatch->mNumIndices = numIndices;

	if( requiredBytesPerIndex <= 2 ) {
		mBatch->mIndexType = GL_UNSIGNED_SHORT;
		std::unique_ptr<uint16_t[]> indices( new uint16_t[numIndices] );
		copyIndexData( source, numIndices, indices.get() );
		mBatch->mIndices = Vbo::create( GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(uint16_t), indices.get() );
	}
	else {
		mBatch->mIndexType = GL_UNSIGNED_INT;
		std::unique_ptr<uint32_t[]> indices( new uint32_t[numIndices] );
		copyIndexData( source, numIndices, indices.get() );
		mBatch->mIndices = Vbo::create( GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(uint32_t), indices.get() );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Batch
BatchRef Batch::create( const VboMeshRef &vboMesh, const gl::GlslProgRef &glsl, const AttributeMapping &attributeMapping )
{
	return BatchRef( new Batch( vboMesh, glsl, attributeMapping ) );
}

BatchRef Batch::create( const geom::Source &source, const gl::GlslProgRef &glsl )
{
	return BatchRef( new Batch( source, glsl ) );
}

BatchRef Batch::create( const geom::SourceRef &sourceRef, const gl::GlslProgRef &glsl )
{
	return BatchRef( new Batch( *sourceRef, glsl ) );
}

Batch::Batch( const VboMeshRef &vboMesh, const gl::GlslProgRef &glsl, const AttributeMapping &attributeMapping )
	: mGlsl( glsl )
{
	mIndices = vboMesh->getIndexVbo();
	mPrimitive = vboMesh->getGlPrimitive();
	mNumVertices = vboMesh->getNumVertices();
	mNumIndices = vboMesh->getNumIndices();
	mIndexType = vboMesh->getIndexDataType();
	mVertexArrayVbos = vboMesh->getVertexArrayLayoutVbos();
	initVao( attributeMapping );
}

Batch::Batch( const geom::Source &source, const gl::GlslProgRef &glsl )
	: mGlsl( glsl )
{
	init( source, mGlsl );
}


void Batch::init( const geom::Source &source, const gl::GlslProgRef &glsl )
{
	mNumVertices = source.getNumVertices();
	mPrimitive = toGl( source.getPrimitive() );

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
	
	BatchGeomTarget target( source.getPrimitive(), bufferLayout, buffer.get(), this );
	source.loadInto( &target );

	auto vbo = Vbo::create( GL_ARRAY_BUFFER, vertexDataSizeBytes, buffer.get() );
	mVertexArrayVbos.push_back( make_pair( bufferLayout, vbo ) );
	
	initVao();
}

void Batch::initVao( const AttributeMapping &attributeMapping )
{
	auto ctx = gl::context();
	ctx->pushBufferBinding( GL_ARRAY_BUFFER );

	mVao = Vao::create();
	ScopedVao ScopedVao( mVao );
	
	std::set<geom::Attrib> enabledAttribs;
	// iterate all the vertex array VBOs
	for( const auto &vertArrayVbo : mVertexArrayVbos ) {
		// bind this VBO (to the current VAO)
		vertArrayVbo.second->bind();
		// now iterate the attributes associated with this VBO
		for( const auto &attribInfo : vertArrayVbo.first.getAttribs() ) {
			int loc = -1;
			// first see if we have a mapping in 'attributeMapping'
			auto attributeMappingIt = attributeMapping.find( attribInfo.getAttrib() );
			if( attributeMappingIt != attributeMapping.end() )
				loc = mGlsl->getAttribLocation( attributeMappingIt->second );
			// otherwise, try to get the location of the attrib semantic in the shader if it's present
			else if( mGlsl->hasAttribSemantic( attribInfo.getAttrib() ) )
				loc = mGlsl->getAttribSemanticLocation( attribInfo.getAttrib() );

			if( loc != -1 ) {
				ctx->enableVertexAttribArray( loc );
				ctx->vertexAttribPointer( loc, attribInfo.getDims(), GL_FLOAT, GL_FALSE, attribInfo.getStride(), (const void*)attribInfo.getOffset() );
				if( attribInfo.getInstanceDivisor() > 0 )
					ctx->vertexAttribDivisor( loc, attribInfo.getInstanceDivisor() );
				enabledAttribs.insert( attribInfo.getAttrib() );
			}
		}
	}
	
	// warn the user if the shader expects any attribs which we couldn't supply. We make an exception for ciColor since it often comes from the Context instead
	const auto &glslActiveAttribs = mGlsl->getAttribSemantics();
	for( auto &glslActiveAttrib : glslActiveAttribs ) {
		if( (glslActiveAttrib.second != geom::Attrib::COLOR) && (enabledAttribs.count( glslActiveAttrib.second ) == 0) )
			CI_LOG_W( "Batch GlslProg expected an Attrib of " << geom::attribToString( glslActiveAttrib.second ) << " but vertex data doesn't provide it." );			
	}
	
	if( mNumIndices > 0 )
		mIndices->bind();

	ctx->popBufferBinding( GL_ARRAY_BUFFER );

	mAttribMapping = attributeMapping;
}

void Batch::setGlslProg( const GlslProgRef& glsl )
{
	mGlsl = glsl;
	initVao( mAttribMapping );
}

void Batch::draw()
{
	auto ctx = gl::context();
	
	gl::ScopedGlslProg ScopedGlslProg( mGlsl );
	gl::ScopedVao ScopedVao( mVao );
	ctx->setDefaultShaderVars();
	if( mNumIndices )
		ctx->drawElements( mPrimitive, mNumIndices, mIndexType, 0 );
	else
		ctx->drawArrays( mPrimitive, 0, mNumVertices );
}

#if ! defined( CINDER_GL_ES )
void Batch::drawInstanced( GLsizei primcount )
{
	auto ctx = gl::context();
	
	gl::ScopedGlslProg ScopedGlslProg( mGlsl );
	gl::ScopedVao ScopedVao( mVao );
	ctx->setDefaultShaderVars();
	if( mNumIndices )
		ctx->drawElementsInstanced( mPrimitive, mNumIndices, mIndexType, 0, primcount );
	else
		ctx->drawArraysInstanced( mPrimitive, 0, mNumVertices, primcount );
}
#endif

void Batch::bind()
{
	mGlsl->bind();
	mVao->bind();
}

void Batch::reassignContext( Context *context )
{
	mVao->reassignContext( context );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VertBatch
VertBatch::VertBatch( GLenum primType, bool useContextDefaultBuffers )
	: mPrimType( primType )
{
	if( useContextDefaultBuffers ) {
		auto ctx = gl::context();
		mVao = ctx->getDefaultVao();
		mVbo = ctx->getDefaultArrayVbo();
		mOwnsBuffers = false;
	}
	else
		mOwnsBuffers = true;
}

VertBatchRef VertBatch::create( GLenum primType, bool useContextDefaultBuffers )
{
	return VertBatchRef( new VertBatch( primType, useContextDefaultBuffers ) ); 
}

void VertBatch::setType( GLenum primType )
{
	mPrimType = primType;
}

void VertBatch::normal( const Vec3f &n )
{
	mNormals.push_back( n );
}

void VertBatch::color( const Colorf &color )
{
	mColors.push_back( color );
}

void VertBatch::color( const ColorAf &color )
{
	mColors.push_back( color );
}

void VertBatch::texCoord( const Vec4f &t )
{
	mTexCoords.push_back( t );
}

void VertBatch::vertex( const Vec4f &v )
{
	addVertex( v );
}

void VertBatch::vertex( const Vec4f &v, const ColorAf &c )
{
	mColors.push_back( c );
	addVertex( v );
}

void VertBatch::addVertex( const Vec4f &v )
{
	mVertices.push_back( v );

	if( ! mNormals.empty() ) {
		while( mNormals.size() < mVertices.size() )
			mNormals.push_back( mNormals.back() );
	}
	
	if( ! mColors.empty() ) {
		while( mColors.size() < mVertices.size() )
			mColors.push_back( mColors.back() );	
	}

	if( ! mTexCoords.empty() ) {
		while( mTexCoords.size() < mVertices.size() )
			mTexCoords.push_back( mTexCoords.back() );	
	}
}

void VertBatch::begin( GLenum primType )
{
	clear();
	mPrimType = primType;
}

void VertBatch::end()
{
}

void VertBatch::clear()
{
	mVertices.clear();
	mNormals.clear();
	mColors.clear();
	mTexCoords.clear();
	mVbo.reset();
	mVao.reset();
}

void VertBatch::draw()
{
	// this pushes the VAO, which needs to be popped
	setupBuffers();
	ScopedVao vao( mVao );
	
	auto ctx = context();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( mPrimType, 0, mVertices.size() );
	
	// this was set by setupBuffers
	ctx->popVao();
}

// Leaves mVAO bound
void VertBatch::setupBuffers()
{
	auto ctx = gl::context();
	
	GlslProgRef glslProg = ctx->getGlslProg();
	if( ! glslProg )
		return;

	const size_t verticesSizeBytes = mVertices.size() * sizeof(Vec4f);
	const size_t normalsSizeBytes = mNormals.size() * sizeof(Vec3f);
	const size_t colorsSizeBytes = mColors.size() * sizeof(ColorAf);
	const size_t texCoordsSizeBytes = mTexCoords.size() * sizeof(Vec4f);
	size_t totalSizeBytes = verticesSizeBytes + normalsSizeBytes + colorsSizeBytes + texCoordsSizeBytes;

	// allocate the VBO if we don't have one yet (which implies we're not using the context defaults)
	bool forceUpload = false;
	if( ! mVbo ) {
		// allocate the VBO and upload the data
		mVbo = gl::Vbo::create( GL_ARRAY_BUFFER, totalSizeBytes );
		forceUpload = true;
	}
	
	ScopedBuffer ScopedBuffer( mVbo );
	// if this VBO was freshly made, or we don't own the buffer because we use the context defaults
	if( forceUpload || ( ! mOwnsBuffers ) ) {
		mVbo->ensureMinimumSize( totalSizeBytes );
		
		// upload positions
		GLintptr offset = 0;
		mVbo->bufferSubData( offset, verticesSizeBytes, &mVertices[0] );
		offset += verticesSizeBytes;
		
		// upload normals
		if( ! mNormals.empty() ) {
			mVbo->bufferSubData( offset, normalsSizeBytes, &mNormals[0] );
			offset += normalsSizeBytes;
		}

		// upload colors
		if( ! mColors.empty() ) {
			mVbo->bufferSubData( offset, colorsSizeBytes, &mColors[0] );
			offset += colorsSizeBytes;
		}

		// upload texCoords
		if( ! mTexCoords.empty() ) {
			mVbo->bufferSubData( offset, texCoordsSizeBytes, &mTexCoords[0] );
			offset += texCoordsSizeBytes;
		}
	}

	// Setup the VAO
	ctx->pushVao();
	if( ! mOwnsBuffers )
		mVao->replacementBindBegin();
	else {
		mVao = gl::Vao::create();
		mVao->bind();
	}

	gl::ScopedBuffer vboScope( mVbo );
	size_t offset = 0;
	if( glslProg->hasAttribSemantic( geom::Attrib::POSITION ) ) {
		int loc = glslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
		ctx->enableVertexAttribArray( loc );
		ctx->vertexAttribPointer( loc, 4, GL_FLOAT, false, 0, (const GLvoid*)offset );
		offset += verticesSizeBytes;
	}

	if( glslProg->hasAttribSemantic( geom::Attrib::NORMAL ) && ( ! mNormals.empty() ) ) {
		int loc = glslProg->getAttribSemanticLocation( geom::Attrib::NORMAL );
		ctx->enableVertexAttribArray( loc );
		ctx->vertexAttribPointer( loc, 3, GL_FLOAT, false, 0, (const GLvoid*)offset );
		offset += normalsSizeBytes;
	}

	if( glslProg->hasAttribSemantic( geom::Attrib::COLOR ) && ( ! mColors.empty() ) ) {
		int loc = glslProg->getAttribSemanticLocation( geom::Attrib::COLOR );
		ctx->enableVertexAttribArray( loc );
		ctx->vertexAttribPointer( loc, 4, GL_FLOAT, false, 0, (const GLvoid*)offset );
		offset += colorsSizeBytes;
	}

	if( glslProg->hasAttribSemantic( geom::Attrib::TEX_COORD_0 ) && ( ! mTexCoords.empty() ) ) {
		int loc = glslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
		ctx->enableVertexAttribArray( loc );
		ctx->vertexAttribPointer( loc, 4, GL_FLOAT, false, 0, (const GLvoid*)offset );
	}
	
	if( ! mOwnsBuffers )
		mVao->replacementBindEnd();
}

} } // namespace cinder::gl