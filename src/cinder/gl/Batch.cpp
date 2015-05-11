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
#include "cinder/gl/scoped.h"

#include "cinder/Log.h"

namespace cinder { namespace gl {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Batch
BatchRef Batch::create( const VboMeshRef &vboMesh, const gl::GlslProgRef &glsl, const AttributeMapping &attributeMapping )
{
	return BatchRef( new Batch( vboMesh, glsl, attributeMapping ) );
}

BatchRef Batch::create( const geom::Source &source, const gl::GlslProgRef &glsl, const AttributeMapping &attributeMapping )
{
	return BatchRef( new Batch( source, glsl, attributeMapping ) );
}

Batch::Batch( const VboMeshRef &vboMesh, const gl::GlslProgRef &glsl, const AttributeMapping &attributeMapping )
	: mGlsl( glsl ), mVboMesh( vboMesh )
{
	initVao( attributeMapping );
}

Batch::Batch( const geom::Source &source, const gl::GlslProgRef &glsl, const AttributeMapping &attributeMapping )
	: mGlsl( glsl )
{
	geom::AttribSet attribs;
	// include all the attributes in the custom attributeMapping
	for( const auto &attrib : attributeMapping ) {
		if( source.getAttribDims( attrib.first ) )
			attribs.insert( attrib.first );
	}
	// and then the attributes references by the GLSL
	for( const auto &attrib : glsl->getActiveAttributes() ) {
		if( source.getAttribDims( attrib.getAttributeSemantic() ) )
			attribs.insert( attrib.getAttributeSemantic() );
	}
	mVboMesh = gl::VboMesh::create( source, attribs );
	initVao( attributeMapping );
}

void Batch::initVao( const AttributeMapping &attributeMapping )
{
	auto ctx = gl::context();
	ctx->pushBufferBinding( GL_ARRAY_BUFFER );

	mVao = Vao::create();
	ctx->pushVao( mVao );
	
	mVboMesh->buildVao( mGlsl, attributeMapping );
	
	ctx->popVao();
	ctx->popBufferBinding( GL_ARRAY_BUFFER );
	
	if( ! mVao->getLayout().isVertexAttribArrayEnabled( 0 ) )
		CI_LOG_W("VertexAttribArray at location 0 not enabled, this has performance implications.");
	
	mAttribMapping = attributeMapping;
}

void Batch::replaceGlslProg( const GlslProgRef& glsl )
{
	mGlsl = glsl;
	initVao( mAttribMapping );
}

void Batch::replaceVboMesh( const VboMeshRef &vboMesh )
{
	mVboMesh = vboMesh;
	initVao( mAttribMapping );
}

void Batch::draw( GLint first, GLsizei count )
{
	auto ctx = gl::context();
	
	gl::ScopedGlslProg ScopedGlslProg( mGlsl );
	gl::ScopedVao ScopedVao( mVao );
	ctx->setDefaultShaderVars();
	mVboMesh->drawImpl( first, count );
}

#if (! defined( CINDER_GL_ES_2 )) || defined( CINDER_COCOA_TOUCH )
void Batch::drawInstanced( GLsizei instanceCount )
{
	auto ctx = gl::context();
	
	gl::ScopedGlslProg ScopedGlslProg( mGlsl );
	gl::ScopedVao ScopedVao( mVao );
	ctx->setDefaultShaderVars();
	mVboMesh->drawInstancedImpl( instanceCount );
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

void VertBatch::normal( const vec3 &n )
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

void VertBatch::texCoord( const vec4 &t )
{
	mTexCoords.push_back( t );
}

void VertBatch::vertex( const vec4 &v )
{
	addVertex( v );
}

void VertBatch::vertex( const vec4 &v, const ColorAf &c )
{
	mColors.push_back( c );
	addVertex( v );
}

void VertBatch::addVertex( const vec4 &v )
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
	mVao = nullptr;
}

void VertBatch::draw()
{
	// this pushes the VAO, which needs to be popped
	setupBuffers();
	ScopedVao vao( mVao );
	
	auto ctx = context();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( mPrimType, 0, (GLsizei)mVertices.size() );
}

// Leaves mVAO bound
void VertBatch::setupBuffers()
{
	auto ctx = gl::context();
	
	auto glslProg = ctx->getGlslProg();
	if( ! glslProg )
		return;

	const size_t verticesSizeBytes = mVertices.size() * sizeof(vec4);
	const size_t normalsSizeBytes = mNormals.size() * sizeof(vec3);
	const size_t colorsSizeBytes = mColors.size() * sizeof(ColorAf);
	const size_t texCoordsSizeBytes = mTexCoords.size() * sizeof(vec4);
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
	if( ( forceUpload || ( ! mOwnsBuffers ) ) && ( ! mVertices.empty() ) ) {
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
	ctx->pushVao(); // this will be popped by draw()
	if( ! mOwnsBuffers )
		mVao->replacementBindBegin();
	else {
		mVaoStorage = gl::Vao::create();
		mVao = mVaoStorage.get();
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
	ctx->popVao();
}

} } // namespace cinder::gl