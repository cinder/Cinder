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
		if( source.getAttribDims( attrib.getSemantic() ) )
			attribs.insert( attrib.getSemantic() );
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

#if defined( CINDER_GL_HAS_DRAW_INSTANCED )

void Batch::drawInstanced( GLsizei instanceCount )
{
	auto ctx = gl::context();
	
	gl::ScopedGlslProg ScopedGlslProg( mGlsl );
	gl::ScopedVao ScopedVao( mVao );
	ctx->setDefaultShaderVars();
	mVboMesh->drawInstancedImpl( instanceCount );
}

#endif // defined( CINDER_GL_HAS_DRAW_INSTANCED )

#if defined( CINDER_GL_HAS_DRAW_INDIRECT )

void Batch::drawIndirect( const GLvoid *indirect )
{
	auto ctx = gl::context();

	gl::ScopedGlslProg ScopedGlslProg( mGlsl );
	gl::ScopedVao ScopedVao( mVao );
	ctx->setDefaultShaderVars();
	mVboMesh->drawIndirectImpl( indirect );
}

#endif // defined( CINDER_GL_HAS_DRAW_INDIRECT )

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
	: mPrimType( primType ), mForceUpdate( false )
{
	if( useContextDefaultBuffers ) {
		auto ctx = gl::context();
		mVao = ctx->getDefaultVao();
		mVbo = ctx->getDefaultArrayVbo();
		mOwnsBuffers = false;
	}
	else {
		mVaoStorage = gl::Vao::create();
		mVao = mVaoStorage.get();
		mVbo = gl::Vbo::create( GL_ARRAY_BUFFER, 0 );
		mOwnsBuffers = true;
	}
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

void VertBatch::texCoord0( const vec4 &t )
{
	mTexCoords0.push_back( t );
}

void VertBatch::texCoord1( const vec4 &t )
{
	mTexCoords1.push_back( t );
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
	mForceUpdate = true;
	mVertices.push_back( v );

	if( ! mNormals.empty() ) {
		while( mNormals.size() < mVertices.size() )
			mNormals.push_back( mNormals.back() );
	}
	
	if( ! mColors.empty() ) {
		while( mColors.size() < mVertices.size() )
			mColors.push_back( mColors.back() );	
	}

	if( ! mTexCoords0.empty() ) {
		while( mTexCoords0.size() < mVertices.size() )
			mTexCoords0.push_back( mTexCoords0.back() );
	}

	if( ! mTexCoords1.empty() ) {
		while( mTexCoords1.size() < mVertices.size() )
			mTexCoords1.push_back( mTexCoords1.back() );
	}
}

void VertBatch::begin( GLenum primType )
{
	clear();
	mPrimType = primType;
}

void VertBatch::clear()
{
	mVertices.clear();
	mNormals.clear();
	mColors.clear();
	mTexCoords0.clear();
	mTexCoords1.clear();
}

void VertBatch::draw()
{
	ScopedVao scopedVao( mVao );
	setupBuffers();
	
	auto ctx = context();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( mPrimType, 0, (GLsizei)mVertices.size() );
}

void VertBatch::setupBuffers()
{
	auto ctx = gl::context();
	
	auto glslProg = ctx->getGlslProg();
	if( ! glslProg )
		return;

	const size_t verticesSizeBytes = mVertices.size() * sizeof(vec4);
	const size_t normalsSizeBytes = mNormals.size() * sizeof(vec3);
	const size_t colorsSizeBytes = mColors.size() * sizeof(ColorAf);
	const size_t texCoords0SizeBytes = mTexCoords0.size() * sizeof(vec4);
	const size_t texCoords1SizeBytes = mTexCoords1.size() * sizeof(vec4);
	size_t totalSizeBytes = verticesSizeBytes + normalsSizeBytes + colorsSizeBytes + texCoords0SizeBytes + texCoords1SizeBytes;
	
	ScopedBuffer scopedVbo( mVbo );

	// if this VBO was freshly made, or we don't own the buffer because we use the context defaults
	if( ( ! mVertices.empty() ) && ( mForceUpdate || ( ! mOwnsBuffers ) ) ) {
		mForceUpdate = false;
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

		// upload texCoords0
		if( ! mTexCoords0.empty() ) {
			mVbo->bufferSubData( offset, texCoords0SizeBytes, &mTexCoords0[0] );
			offset += texCoords0SizeBytes;
		}

		// upload texCoords1
		if( ! mTexCoords1.empty() ) {
			mVbo->bufferSubData( offset, texCoords1SizeBytes, &mTexCoords1[0] );
			offset += texCoords1SizeBytes;
		}
	}

	mVao->replacementBindBegin();

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

	if( glslProg->hasAttribSemantic( geom::Attrib::TEX_COORD_0 ) && ( ! mTexCoords0.empty() ) ) {
		int loc = glslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
		ctx->enableVertexAttribArray( loc );
		ctx->vertexAttribPointer( loc, 4, GL_FLOAT, false, 0, (const GLvoid*)offset );
		offset += texCoords0SizeBytes;
	}

	if( glslProg->hasAttribSemantic( geom::Attrib::TEX_COORD_1 ) && ( ! mTexCoords1.empty() ) ) {
		int loc = glslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_1 );
		ctx->enableVertexAttribArray( loc );
		ctx->vertexAttribPointer( loc, 4, GL_FLOAT, false, 0, (const GLvoid*)offset );
	}
	
	mVao->replacementBindEnd();
}

size_t VertBatch::getNumVertices() const
{
	return mVertices.size();
}

size_t VertBatch::getNumIndices() const
{
	return 0;
}

geom::Primitive	VertBatch::getPrimitive() const
{
	return gl::toGeomPrimitive( mPrimType );
}

uint8_t	VertBatch::getAttribDims( geom::Attrib attr ) const
{
	switch( attr ) {
		case geom::Attrib::POSITION:
			return mVertices.empty() ? 0 :4;
		case geom::Attrib::NORMAL:
			return mNormals.empty() ? 0 : 3;
		case geom::Attrib::COLOR:
			return mColors.empty() ? 0 : 4;
		case geom::Attrib::TEX_COORD_0:
			return mTexCoords0.empty() ? 0 : 4;
		case geom::Attrib::TEX_COORD_1:
			return mTexCoords1.empty() ? 0 : 4;
		default:
			return 0;
	}
}

void VertBatch::loadInto( geom::Target *target, const geom::AttribSet & /*requestedAttribs*/ ) const
{
	if( ! mVertices.empty() )
		target->copyAttrib( geom::Attrib::POSITION, 4, 0, value_ptr( *mVertices.data() ), mVertices.size() );
	if( ! mNormals.empty() )
		target->copyAttrib( geom::Attrib::NORMAL, 3, 0, value_ptr( *mNormals.data() ), mNormals.size() );
	if( ! mColors.empty() )
		target->copyAttrib( geom::Attrib::COLOR, 4, 0, (const float*)mColors.data(), mColors.size() );
	if( ! mTexCoords0.empty() )
		target->copyAttrib( geom::Attrib::TEX_COORD_0, 4, 0, value_ptr( *mTexCoords0.data() ), mTexCoords0.size() );
	if( ! mTexCoords1.empty() )
		target->copyAttrib( geom::Attrib::TEX_COORD_1, 4, 0, value_ptr( *mTexCoords1.data() ), mTexCoords1.size() );
}

geom::AttribSet	VertBatch::getAvailableAttribs() const
{
	geom::AttribSet attribs;
	attribs.insert( geom::Attrib::POSITION );
	
	if( ! mNormals.empty() )
		attribs.insert( geom::Attrib::NORMAL );
	if( ! mColors.empty() )
		attribs.insert( geom::Attrib::COLOR );
	if( ! mTexCoords0.empty() )
		attribs.insert( geom::Attrib::TEX_COORD_0 );
	if( ! mTexCoords1.empty() )
		attribs.insert( geom::Attrib::TEX_COORD_1 );
	
	return attribs;
}


} } // namespace cinder::gl