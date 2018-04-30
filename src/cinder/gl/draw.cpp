/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.

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

#include "cinder/gl/draw.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/scoped.h"
#include "cinder/gl/Environment.h"
#include "cinder/Log.h"
#include "cinder/Text.h"
#include "cinder/Triangulate.h"

using namespace std;

namespace cinder { namespace gl {

void draw( const VboMeshRef& mesh, GLint first, GLsizei count )
{
	auto ctx = gl::context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	mesh->buildVao( curGlslProg );
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	mesh->drawImpl( first, count );
	ctx->popVao();
}

void drawArrays( GLenum mode, GLint first, GLsizei count )
{
	context()->drawArrays( mode, first, count );
}

void drawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices )
{
	context()->drawElements( mode, count, type, indices );
}

#if defined( CINDER_GL_HAS_MULTI_DRAW )
void multiDrawArrays( GLenum mode, GLint *first, GLsizei *count, GLsizei primcount )
{
	context()->multiDrawArrays( mode, first, count, primcount );
}

void multiDrawElements( GLenum mode, GLsizei *count, GLenum type, const GLvoid * const *indices, GLsizei primcount )
{
	context()->multiDrawElements( mode, count, type, indices, primcount );
}
#endif // defined( CINDER_GL_HAS_MULTI_DRAW )

#if defined( CINDER_GL_HAS_DRAW_INSTANCED )
void drawArraysInstanced( GLenum mode, GLint first, GLsizei count, GLsizei instanceCount )
{
	context()->drawArraysInstanced( mode, first, count, instanceCount );
}
	
void drawElementsInstanced( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei instanceCount )
{
	context()->drawElementsInstanced( mode, count, type, indices, instanceCount );
}
#endif // defined( CINDER_GL_HAS_DRAW_INSTANCED )

#if defined( CINDER_GL_HAS_DRAW_INDIRECT )
void drawArraysIndirect( GLenum mode, const GLvoid *indirect )
{
	context()->drawArraysIndirect( mode, indirect );
}

void drawElementsIndirect( GLenum mode, GLenum type, const GLvoid *indirect )
{
	context()->drawElementsIndirect( mode, type, indirect );
}
#endif // defined( CINDER_GL_HAS_DRAW_INDIRECT )

#if defined( CINDER_GL_HAS_MULTI_DRAW_INDIRECT )
void multiDrawArraysIndirect( GLenum mode, const GLvoid *indirect, GLsizei drawcount, GLsizei stride )
{
	context()->multiDrawArraysIndirect( mode, indirect, drawcount, stride );
}

void multiDrawElementsIndirect( GLenum mode, GLenum type, const GLvoid *indirect, GLsizei drawcount, GLsizei stride )
{
	context()->multiDrawElementsIndirect( mode, type, indirect, drawcount, stride );
}
#endif // defined( CINDER_GL_HAS_MULTI_DRAW_INDIRECT )

namespace {

std::array<vec3, 8> getCubePoints( const vec3 &c, const vec3 &size )
{
	vec3 s = size * 0.5f;																			// origin points
	std::array<vec3, 8> points = { vec3(  c.x + 1.0f * s.x, c.y + 1.0f * s.y, c.z + 1.0f * s.z ),	// upper front right
								   vec3(  c.x + 1.0f * s.x, c.y - 1.0f * s.y, c.z + 1.0f * s.z ),	// lower front right
								   vec3(  c.x + 1.0f * s.x, c.y - 1.0f * s.y, c.z - 1.0f * s.z ),	// lower back right
								   vec3(  c.x + 1.0f * s.x, c.y + 1.0f * s.y, c.z - 1.0f * s.z ),	// upper back right
								   vec3(  c.x - 1.0f * s.x, c.y + 1.0f * s.y, c.z + 1.0f * s.z ),	// upper front left
								   vec3(  c.x - 1.0f * s.x, c.y - 1.0f * s.y, c.z + 1.0f * s.z ),	// lower front left
								   vec3(  c.x - 1.0f * s.x, c.y - 1.0f * s.y, c.z - 1.0f * s.z ),	// lower back left
								   vec3(  c.x - 1.0f * s.x, c.y + 1.0f * s.y, c.z - 1.0f * s.z ) };	// upper back left
	return points;
}
	
void drawCubeImpl( const vec3 &c, const vec3 &size, bool faceColors )
{
	GLfloat sx = size.x * 0.5f;
	GLfloat sy = size.y * 0.5f;
	GLfloat sz = size.z * 0.5f;
	GLfloat vertices[24*3]={c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,		// +X
							c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,		// +Y
							c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,		// +Z
							c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	// -X
							c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	// -Y
							c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz};	// -Z

	static GLfloat normals[24*3]={ 1,0,0,	1,0,0,	1,0,0,	1,0,0,
								  0,1,0,	0,1,0,	0,1,0,	0,1,0,
									0,0,1,	0,0,1,	0,0,1,	0,0,1,
								  -1,0,0,	-1,0,0,	-1,0,0,	-1,0,0,
								  0,-1,0,	0,-1,0,  0,-1,0,0,-1,0,
								  0,0,-1,	0,0,-1,	0,0,-1,	0,0,-1};

	static GLubyte colors[24*4]={	255,0,0,255,	255,0,0,255,	255,0,0,255,	255,0,0,255,	// +X = red
									0,255,0,255,	0,255,0,255,	0,255,0,255,	0,255,0,255,	// +Y = green
									0,0,255,255,	0,0,255,255,	0,0,255,255,	0,0,255,255,	// +Z = blue
									0,255,255,255,	0,255,255,255,	0,255,255,255,	0,255,255,255,	// -X = cyan
									255,0,255,255,	255,0,255,255,	255,0,255,255,	255,0,255,255,	// -Y = purple
									255,255,0,255,	255,255,0,255,	255,255,0,255,	255,255,0,255 };// -Z = yellow

	static GLfloat texs[24*2]={	0,1,	1,1,	1,0,	0,0,
								1,1,	1,0,	0,0,	0,1,
								0,1,	1,1,	1,0,	0,0,
								1,1,	1,0,	0,0,	0,1,
								1,0,	0,0,	0,1,	1,1,
								1,0,	0,0,	0,1,	1,1 };

	static GLubyte elements[6*6] ={	0, 1, 2, 0, 2, 3,
									4, 5, 6, 4, 6, 7,
									8, 9,10, 8, 10,11,
									12,13,14,12,14,15,
									16,17,18,16,18,19,
									20,21,22,20,22,23 };

	Context *ctx = gl::context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	bool hasPositions = curGlslProg->hasAttribSemantic( geom::Attrib::POSITION );
	bool hasNormals = curGlslProg->hasAttribSemantic( geom::Attrib::NORMAL );
	bool hasTextureCoords = curGlslProg->hasAttribSemantic( geom::Attrib::TEX_COORD_0 );
	bool hasColors = faceColors && curGlslProg->hasAttribSemantic( geom::Attrib::COLOR );

	size_t totalArrayBufferSize = 0;
	if( hasPositions )
		totalArrayBufferSize += sizeof(float)*24*3;
	if( hasNormals )
		totalArrayBufferSize += sizeof(float)*24*3;
	if( hasTextureCoords )
		totalArrayBufferSize += sizeof(float)*24*2;
	if( hasColors )
		totalArrayBufferSize += 24*4;

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();

	VboRef defaultArrayVbo = ctx->getDefaultArrayVbo( totalArrayBufferSize );
	ScopedBuffer vboScp( defaultArrayVbo );
	VboRef elementVbo = ctx->getDefaultElementVbo( 6*6 );
	// we seem to need to orphan the existing element vbo on AMD on OS X
	elementVbo->bufferData( 36, nullptr, GL_STREAM_DRAW );

	elementVbo->bind();
	size_t curBufferOffset = 0;
	if( hasPositions ) {
		int loc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
		enableVertexAttribArray( loc );
		vertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0, (void*)curBufferOffset );
		defaultArrayVbo->bufferSubData( curBufferOffset, sizeof(float)*24*3, vertices );
		curBufferOffset += sizeof(float)*24*3;
	}
	if( hasNormals ) {
		int loc = curGlslProg->getAttribSemanticLocation( geom::Attrib::NORMAL );
		enableVertexAttribArray( loc );
		vertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0, (void*)curBufferOffset );
		defaultArrayVbo->bufferSubData( curBufferOffset, sizeof(float)*24*3, normals );
		curBufferOffset += sizeof(float)*24*3;
	}
	if( hasTextureCoords ) {
		int loc = curGlslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
		enableVertexAttribArray( loc );
		vertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0, (void*)curBufferOffset );
		defaultArrayVbo->bufferSubData( curBufferOffset, sizeof(float)*24*2, texs );
		curBufferOffset += sizeof(float)*24*2;
	}
	if( hasColors ) {
		int loc = curGlslProg->getAttribSemanticLocation( geom::Attrib::COLOR );
		enableVertexAttribArray( loc );
		vertexAttribPointer( loc, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)curBufferOffset );
		defaultArrayVbo->bufferSubData( curBufferOffset, 24*4, colors );
		curBufferOffset += 24*4;
	}

	elementVbo->bufferSubData( 0, 36, elements );
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawElements( GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0 );
	ctx->popVao();
}

} // anonymous namespace

void drawCube( const vec3 &center, const vec3 &size )
{
	drawCubeImpl( center, size, false );
}

void drawColorCube( const vec3 &center, const vec3 &size )
{
	drawCubeImpl( center, size, true );
}
	
void drawStrokedCube( const vec3 &center, const vec3 &size )
{
	auto vertices = getCubePoints( center, size );
	
	static std::array<GLubyte, 24> indices = { 0, 1, 1, 2, 2, 3, 3, 0,		// right side connection
											   4, 5, 5, 6, 6, 7, 7, 4,		// left side connection
											   0, 4, 1, 5, 2, 6, 3, 7 };	// right to left connections
	
	auto ctx = ci::gl::context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}
	
	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	gl::VboRef defaultVbo = ctx->getDefaultArrayVbo( sizeof(vec3) * 8 );
	gl::VboRef elementVbo = ctx->getDefaultElementVbo( 24 );
	gl::ScopedBuffer bufferBindScp( defaultVbo );
	defaultVbo->bufferSubData( 0, sizeof(vec3) * 8, vertices.data() );
	
	elementVbo->bind();
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		gl::enableVertexAttribArray( posLoc );
		gl::vertexAttribPointer( posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}
	
	elementVbo->bufferSubData( 0, 24, indices.data() );
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawElements( GL_LINES, 24, GL_UNSIGNED_BYTE, 0 );
	ctx->popVao();
}

void draw( const TextureRef &texture, const Area &srcArea, const Rectf &dstRect )
{
	if( ! texture )
		return;

	auto ctx = context();

	Rectf texRect = texture->getAreaTexCoords( srcArea );

	ScopedVao vaoScp( ctx->getDrawTextureVao() );
	ScopedBuffer vboScp( ctx->getDrawTextureVbo() );
	ScopedTextureBind texBindScope( texture );

	auto glsl = getStockShader( ShaderDef().uniformBasedPosAndTexCoord().color().texture( texture ) );
	ScopedGlslProg glslScp( glsl );
	glsl->uniform( "uTex0", 0 );
	glsl->uniform( "uPositionOffset", dstRect.getUpperLeft() );
	glsl->uniform( "uPositionScale", dstRect.getSize() );
	glsl->uniform( "uTexCoordOffset", texRect.getUpperLeft() );
	glsl->uniform( "uTexCoordScale", texRect.getSize() );

	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_TRIANGLE_STRIP, 0, 4 );
}

void draw( const TextureRef &texture, const Rectf &dstRect )
{
	if( ! texture )
		return;

	draw( texture, texture->getBounds(), dstRect );
}


void draw( const TextureRef &texture, const vec2 &dstOffset )
{
	if( ! texture )
		return;
	
	draw( texture, texture->getBounds(), Rectf( texture->getBounds() ) + dstOffset );
}

void draw( const Path2d &path, float approximationScale )
{
	if( path.getNumSegments() == 0 || path.getNumPoints() == 0 )
		return;

	auto ctx = context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	vector<vec2> points = path.subdivide( approximationScale );
	VboRef arrayVbo = ctx->getDefaultArrayVbo( sizeof(vec2) * points.size() );
	arrayVbo->bufferSubData( 0, sizeof(vec2) * points.size(), points.data() );

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	ScopedBuffer bufferBindScp( arrayVbo );
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)nullptr );
	}

	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_LINE_STRIP, 0, (GLsizei)points.size() );
	ctx->popVao();
}

void draw( const Shape2d &shape, float approximationScale )
{
	for( const auto &path : shape.getContours() )
		gl::draw( path, approximationScale );
}

void draw( const PolyLine2 &polyLine )
{
	auto ctx = context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	const vector<vec2> &points = polyLine.getPoints();
	VboRef arrayVbo = ctx->getDefaultArrayVbo( sizeof(vec2) * points.size() );
	arrayVbo->bufferSubData( 0, sizeof(vec2) * points.size(), points.data() );

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	ScopedBuffer bufferBindScp( arrayVbo );
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)nullptr );
	}

	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( ( polyLine.isClosed() ) ? GL_LINE_LOOP : GL_LINE_STRIP, 0, (GLsizei)points.size() );
	ctx->popVao();
}

void draw( const std::vector<vec3> &points, bool isClosed )
{
	auto ctx = context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}
	
	VboRef arrayVbo = ctx->getDefaultArrayVbo( sizeof(vec3) * points.size() );
	arrayVbo->bufferSubData( 0, sizeof(vec3) * points.size(), points.data() );

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	ScopedBuffer bufferBindScp( arrayVbo );
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)nullptr );
	}

	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( ( isClosed ) ? GL_LINE_LOOP : GL_LINE_STRIP, 0, (GLsizei)points.size() );
	ctx->popVao();
}

void drawLine( const vec3 &a, const vec3 &b )
{
	const int dims = 3;
	const int size = sizeof( vec3 ) * 2;
	array<vec3, 2> points = { a, b };

	auto ctx = context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();

	VboRef arrayVbo = ctx->getDefaultArrayVbo( size );
	ScopedBuffer bufferBindScp( arrayVbo );

	arrayVbo->bufferSubData( 0, size, points.data() );
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, dims, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)nullptr );
	}
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_LINES, 0, static_cast<int>( points.size() ) );
	ctx->popVao();
}

void drawLine( const vec2 &a, const vec2 &b )
{
	const int dims = 2;
	const int size = sizeof( vec2 ) * 2;
	array<vec2, 2> points = { a, b };
	auto ctx = context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();

	VboRef arrayVbo = ctx->getDefaultArrayVbo( size );
	ScopedBuffer bufferBindScp( arrayVbo );

	arrayVbo->bufferSubData( 0, size, points.data() );
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, dims, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)nullptr );
	}
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_LINES, 0, static_cast<int>( points.size() ) );
	ctx->popVao();
}

void draw( const TriMesh &mesh )
{
	if( mesh.getNumVertices() <= 0 )
		return;

	draw( (const geom::Source&)mesh );
}

namespace {

class DefaultVboTarget : public geom::Target {
  public:
	DefaultVboTarget( const geom::Source *source )
		: mSource( source ), mContext( context() ), mArrayVboOffset( 0 ), mTempStorageSizeBytes( 0 )
	{
		size_t requiredSize = 0;
		size_t numVertices = source->getNumVertices();

		for( const auto &attrib : source->getAvailableAttribs() ) {
			uint8_t attribDims = source->getAttribDims( attrib );
			if( attribDims > 0 ) {
				mRequestedAttribs.push_back( attrib );
				requiredSize += numVertices * attribDims * sizeof( float );
			}
		}

		mArrayVbo = mContext->getDefaultArrayVbo( requiredSize );
		mGlslProg = mContext->getGlslProg();

		CI_ASSERT_MSG( mGlslProg, "No GLSL program bound" );

		mContext->pushBufferBinding( mArrayVbo->getTarget(), mArrayVbo->getId() );
		if( source->getNumIndices() ) {
			mElementVbo = mContext->getDefaultElementVbo( source->getNumIndices() * sizeof( GLint ) );
			mContext->pushBufferBinding( mElementVbo->getTarget(), mElementVbo->getId() );
		}
	}

	~DefaultVboTarget()
	{
		mContext->popBufferBinding( mArrayVbo->getTarget() );
		if( mElementVbo )
			mContext->popBufferBinding( mElementVbo->getTarget() );
	}

	uint8_t	getAttribDims( geom::Attrib attr ) const override
	{
		return mSource->getAttribDims( attr );
	}

	GLenum	getIndexType() const
	{
		return mIndexType;
	}

	//! Returns whether \a attr has data
	bool attribHasData( geom::Attrib attr ) const
	{
		return find( mReceivedAttribs.begin(), mReceivedAttribs.end(), attr ) != mReceivedAttribs.end();
	}

	void copyAttrib( geom::Attrib attr, uint8_t dims, size_t strideBytes, const float *sourceData, size_t count ) override
	{
		// sometimes a geom::Source will give us an attribute we didn't actually want. In particular, COLOR
		if( find( mRequestedAttribs.begin(), mRequestedAttribs.end(), attr ) == mRequestedAttribs.end() )
			return;
	
		int loc = mGlslProg->getAttribSemanticLocation( attr );
		if( loc >= 0 ) {
			size_t totalBytes = count * dims * sizeof(float);

			// if this is not tightly packed, we're going to need a temporary
			if( ( strideBytes != 0 ) && ( strideBytes != dims * sizeof(float) ) ) {
				if( totalBytes > mTempStorageSizeBytes ) {
					// dim=4 should be the worst case; might as well allocate that much for potential next attrib
					mTempStorageSizeBytes = count * 4 /*dims*/ * sizeof(float);
					mTempStorage = unique_ptr<uint8_t[]>( new uint8_t[mTempStorageSizeBytes] );
				}
				geom::copyData( dims, strideBytes, sourceData, count, dims, 0, reinterpret_cast<float*>( mTempStorage.get() ) );
				
				mArrayVbo->bufferSubData( mArrayVboOffset, totalBytes, mTempStorage.get() );
			}
			else {
				mArrayVbo->bufferSubData( mArrayVboOffset, totalBytes, sourceData );
			}

			mContext->enableVertexAttribArray( loc );
			mContext->vertexAttribPointer( loc, dims, GL_FLOAT, GL_FALSE, 0, (void*)mArrayVboOffset );
			mArrayVboOffset += totalBytes;
		}
		
		mReceivedAttribs.push_back( attr );
	}

	void copyIndices( geom::Primitive /*primitive*/, const uint32_t *sourceData, size_t numIndices, uint8_t /*requiredBytesPerIndex*/ ) override
	{
		if( numIndices == 0 )
			return;

		mIndexType = GL_UNSIGNED_INT;
		mElementVbo->bufferSubData( 0, numIndices * 4, sourceData );
	}

	const geom::Source*		mSource;
	Context*				mContext;
	vector<geom::Attrib>	mRequestedAttribs, mReceivedAttribs;

	gl::VboRef			mArrayVbo, mElementVbo;
	const gl::GlslProg*	mGlslProg;
	size_t				mArrayVboOffset;
	
	
	GLenum				mIndexType;
	
	std::unique_ptr<uint8_t[]>	mTempStorage;
	size_t						mTempStorageSizeBytes;
};

} // anonymous namespace

void draw( const geom::Source &source )
{
	auto ctx = context();
	auto curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	// determine attribs requested by shader
	geom::AttribSet requestedAttribs;
	auto activeAttribs = curGlslProg->getActiveAttributes();
	for( auto &attrib : activeAttribs )
		if( attrib.getSemantic() != geom::Attrib::NUM_ATTRIBS )
			requestedAttribs.insert( attrib.getSemantic() );

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();

	DefaultVboTarget target( &source );
	source.loadInto( &target, requestedAttribs );

	ctx->getDefaultVao()->replacementBindEnd();

	// test for all expected attribs received
	for( auto &attrib : activeAttribs ) {
		geom::Attrib attribSemantic = attrib.getSemantic();
		if( ( attribSemantic != geom::Attrib::COLOR ) && ( ! target.attribHasData( attribSemantic ) ) )
			CI_LOG_W( "Batch GlslProg expected an Attrib of " << geom::attribToString( attribSemantic ) << ", with name "
				<< attrib.getName() << " but vertex data doesn't provide it." );
	}

	ctx->setDefaultShaderVars();

	GLenum primitive = toGl( source.getPrimitive() );
	const size_t numIndices = source.getNumIndices();
	if( numIndices )
		ctx->drawElements( primitive, (GLsizei)numIndices, target.getIndexType(), (GLvoid*)( 0 ) );
	else
		ctx->drawArrays( primitive, 0, (GLsizei)source.getNumVertices() );

	ctx->popVao();
}

namespace {
string equirectangularVertexShader()
{
	string s =
		"	uniform mat4 ciModelViewProjection;\n"
#if defined( CINDER_GL_ES_2 )
		"	attribute vec4 ciPosition; attribute vec2 ciTexCoord0;\n"
		"	varying highp vec2 TexCoord0;\n"
#else
		"	in vec4 ciPosition; in vec2 ciTexCoord0;\n"
		"	out vec2 TexCoord0;\n"
#endif
		"	void main() {\n"
		"		gl_Position = ciModelViewProjection * ciPosition;\n"
		"		TexCoord0 = ( ciTexCoord0 - vec2( 0.5 ) ) * vec2( 6.2831853, 3.1415926 );\n"
		"	}\n";
	return s;
}

string equirectangularFragmentShader( bool withLod )
{
	string s;
#if defined( CINDER_GL_ES_2 )
	if( withLod )
		s+="#extension GL_EXT_shader_texture_lod : require\n";
#endif
	if( withLod )
		s+="uniform highp float uLod;\n";
	s +="	uniform samplerCube uCubeMapTex;\n"
#if defined( CINDER_GL_ES_2 )
		"	varying highp vec2 TexCoord0;\n"
#else
		"	in highp vec2 TexCoord0;\n"
		"	out highp vec4 oColor;\n"
#endif
		"	void main() {\n"
		"		highp vec3 coords = vec3( cos( TexCoord0.x ) * cos( TexCoord0.y ), sin( TexCoord0.y ), sin( TexCoord0.x ) * cos( TexCoord0.y ) );\n";
#if defined( CINDER_GL_ES_2 )
		if( withLod )
			s+="gl_FragColor = textureCubeLodEXT( uCubeMapTex, coords, uLod );\n";
		else
			s+="gl_FragColor = textureCube( uCubeMapTex, coords );\n";
#else
		if( withLod )
			s+="oColor = textureLod( uCubeMapTex, coords, uLod );\n";
		else
			s+="oColor = texture( uCubeMapTex, coords );\n";
#endif
		s+="}\n";
	return s;
}

} // anonymous namespace

void drawEquirectangular( const gl::TextureCubeMapRef &texture, const Rectf &rect, float lod )
{
	static GlslProgRef glslNoLod = gl::GlslProg::create(
		GlslProg::Format().vertex( equirectangularVertexShader() )
			.fragment( equirectangularFragmentShader( false ) )
#if defined( CINDER_GL_ES_3 )
			.version( 300 )
#elif ! defined( CINDER_GL_ES )
			.version( 150 )
#endif
	);
	static GlslProgRef glslWithLod = env()->supportsTextureLod() ? gl::GlslProg::create(
		GlslProg::Format().vertex( equirectangularVertexShader() )
			.fragment( equirectangularFragmentShader( true ) )
#if defined( CINDER_GL_ES_3 )
			.version( 300 )
#elif ! defined( CINDER_GL_ES )
			.version( 150 )
#endif
	) : glslNoLod;

	bool useLod = (lod >= 0) && env()->supportsTextureLod();

	const auto& glsl = ( useLod ) ? glslWithLod : glslNoLod;
	gl::ScopedGlslProg scGlsl( glsl );
	glsl->uniform( "uCubeMapTex", 0 );
	if( useLod )
		glsl->uniform( "uLod", lod );

	 gl::ScopedTextureBind scTex( texture );

	drawSolidRect( rect, vec2( 0, 1 ), vec2( 1, 0 ) );
}

namespace { // drawCross helpers
string crossVertexShader()
{
	string s =
		"	uniform mat4 ciModelViewProjection;\n"
#if defined( CINDER_GL_ES_2 )
		"	attribute vec4 ciPosition; attribute vec3 ciTexCoord0;\n"
		"	varying highp vec3 TexCoord0;\n"
#else
		"	in vec4 ciPosition; in vec3 ciTexCoord0;\n"
		"	out vec3 TexCoord0;\n"
#endif
		"	void main() {\n"
		"		gl_Position = ciModelViewProjection * ciPosition;\n"
		"		TexCoord0 = ciTexCoord0;\n"
		"	}\n";
	return s;
}

string crossFragmentShader( bool withLod )
{
	string s;
#if defined( CINDER_GL_ES_2 )
	if( withLod )
		s+="#extension GL_EXT_shader_texture_lod : require\n";
#endif
	if( withLod )
		s+="uniform highp float uLod;\n";
	s +="	uniform samplerCube uCubeMapTex;\n"
#if defined( CINDER_GL_ES_2 )
		"	varying highp vec3 TexCoord0;\n"
#else
		"	in highp vec3 TexCoord0;\n"
		"	out highp vec4 oColor;\n"
#endif
		"	void main() {\n";
#if defined( CINDER_GL_ES_2 )
		if( withLod )
			s+="gl_FragColor = textureCubeLodEXT( uCubeMapTex, TexCoord0, uLod );\n";
		else
			s+="gl_FragColor = textureCube( uCubeMapTex, TexCoord0 );\n";
#else
		if( withLod )
			s+="oColor = textureLod( uCubeMapTex, TexCoord0, uLod );\n";
		else
			s+="oColor = texture( uCubeMapTex, TexCoord0 );\n";
#endif
		s+="}\n";
	return s;
}


// creates 6 vertices, representing a single face of the flattened cube
void genCrossFace( vec2 ul, vec2 lr, vec3 u, vec3 v, vec3 w, vector<vec2> *pos, vector<vec3> *texCoords )
{
	pos->emplace_back( lr.x, ul.y ); // upper-right
	texCoords->emplace_back( normalize( w + u + v ) );
	pos->emplace_back( ul.x, ul.y ); // upper-left
	texCoords->emplace_back( normalize( w + v ) );
	pos->emplace_back( lr.x, lr.y ); // lower-right
	texCoords->emplace_back( normalize( w + u ) );

	pos->emplace_back( ul.x, lr.y ); // lower-left
	texCoords->emplace_back( normalize(w) );
	pos->emplace_back( lr.x, lr.y ); // lower-right
	texCoords->emplace_back( normalize( w + u ) );
	pos->emplace_back( ul.x, ul.y ); // upper-left
	texCoords->emplace_back( normalize( w + v ) );
}

void drawCrossImpl( const gl::TextureCubeMapRef &texture, const vector<vec2> &positions, const vector<vec3> &texCoords, float lod )
{
	static GlslProgRef glslNoLod = gl::GlslProg::create(
		GlslProg::Format().vertex( crossVertexShader() )
			.fragment( crossFragmentShader( false ) )
#if defined( CINDER_GL_ES_3 )
			.version( 300 )
#elif ! defined( CINDER_GL_ES )
			.version( 150 )
#endif
	);
	static GlslProgRef glslWithLod = env()->supportsTextureLod() ? gl::GlslProg::create(
		GlslProg::Format().vertex( crossVertexShader() )
			.fragment( crossFragmentShader( true ) )
#if defined( CINDER_GL_ES_3 )
			.version( 300 )
#elif ! defined( CINDER_GL_ES )
			.version( 150 )
#endif
	) : glslNoLod;

	bool useLod = (lod >= 0) && env()->supportsTextureLod();

	const auto& glsl = ( useLod ) ? glslWithLod : glslNoLod;
	gl::ScopedGlslProg scGlsl( glsl );
	glsl->uniform( "uCubeMapTex", 0 );
	if( useLod )
		glsl->uniform( "uLod", lod );
	
	auto ctx = gl::context();
	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	gl::VboRef defaultVbo = ctx->getDefaultArrayVbo( sizeof(float)*(positions.size()*2+texCoords.size()*3) );
	gl::ScopedBuffer bufferBindScp( defaultVbo );
	gl::ScopedTextureBind texScp( texture );
	defaultVbo->bufferSubData( 0, sizeof(float)*positions.size()*2, positions.data() );
	defaultVbo->bufferSubData( sizeof(float)*positions.size()*2, sizeof(float)*texCoords.size()*3, texCoords.data() );

	int posLoc = glsl->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		gl::enableVertexAttribArray( posLoc );
		gl::vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}
	int texLoc = glsl->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
	if( texLoc >= 0 ) {
		gl::enableVertexAttribArray( texLoc );
		gl::vertexAttribPointer( texLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*positions.size()*2) );
	}
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_TRIANGLES, 0, (GLsizei)positions.size() );
	ctx->popVao();
}
} // anonymous namespace

void drawHorizontalCross( const gl::TextureCubeMapRef &texture, const Rectf &rect, float lod )
{
	Rectf fullRect( 0, 0, texture->getWidth() * 4.0f, texture->getHeight() * 3.0f );
	Rectf framedRect = fullRect.getCenteredFit( rect, true );
	vec2 faceSize( framedRect.getWidth() / 4, framedRect.getHeight() / 3 );

	vector<vec2> positions;
	vector<vec3> texCoords;
	const vec2 ul = rect.getUpperLeft();
	// -X
	genCrossFace( ul + vec2( 0, faceSize.y ), ul + vec2( faceSize.x, faceSize.y * 2 ), vec3( 0, 0, 2 ), vec3( 0, 2, 0 ), vec3( -1, -1, -1 ), &positions, &texCoords );
	// +Z
	genCrossFace( ul + faceSize, ul + faceSize * 2.0f, vec3( 2, 0, 0 ), vec3( 0, 2, 0 ), vec3( -1, -1, 1 ), &positions, &texCoords );
	// +X
	genCrossFace( ul + vec2( faceSize.x * 2, faceSize.y ), ul + vec2( faceSize.x * 3, faceSize.y * 2 ), vec3( 0, 0, -2 ), vec3( 0, 2, 0 ), vec3( 1, -1, 1 ), &positions, &texCoords );
	// -Z
	genCrossFace( ul + vec2( faceSize.x * 3, faceSize.y ), ul + vec2( faceSize.x * 4, faceSize.y * 2 ), vec3( -2, 0, 0 ), vec3( 0, 2, 0 ), vec3( 1, -1, -1 ), &positions, &texCoords );
	// +Y
	genCrossFace( ul + vec2( faceSize.x, 0 ), ul + vec2( faceSize.x * 2, faceSize.y ), vec3( 2, 0, 0 ), vec3( 0, 0, -2 ), vec3( -1, 1, 1 ), &positions, &texCoords );
	// -Y
	genCrossFace( ul + vec2( faceSize.x, faceSize.y * 2 ), ul + vec2( faceSize.x * 2, faceSize.y * 3 ), vec3( 2, 0, 0 ), vec3( 0, 0, 2 ), vec3( -1, -1, -1 ), &positions, &texCoords );

	drawCrossImpl( texture, positions, texCoords, lod );
}

void drawVerticalCross( const gl::TextureCubeMapRef &texture, const Rectf &rect, float lod )
{
	Rectf fullRect( 0, 0, texture->getWidth() * 3.0f, texture->getHeight() * 4.0f );
	Rectf framedRect = fullRect.getCenteredFit( rect, true );
	vec2 faceSize( framedRect.getWidth() / 3, framedRect.getHeight() / 4 );

	vector<vec2> positions;
	vector<vec3> texCoords;
	const vec2 ul = rect.getUpperLeft();

	// -X
	genCrossFace( ul + vec2( 0, faceSize.y ), ul + vec2( faceSize.x, faceSize.y * 2 ), vec3( 0, 0, 2 ), vec3( 0, 2, 0 ), vec3( -1, -1, -1 ), &positions, &texCoords );
	// +Z
	genCrossFace( ul + faceSize, ul + faceSize * 2.0f, vec3( 2, 0, 0 ), vec3( 0, 2, 0 ), vec3( -1, -1, 1 ), &positions, &texCoords );
	// +X
	genCrossFace( ul + vec2( faceSize.x * 2, faceSize.y ), ul + vec2( faceSize.x * 3, faceSize.y * 2 ), vec3( 0, 0, -2 ), vec3( 0, 2, 0 ), vec3( 1, -1, 1 ), &positions, &texCoords );
	// +Y
	genCrossFace( ul + vec2( faceSize.x, 0 ), ul + vec2( faceSize.x * 2, faceSize.y ), vec3( 2, 0, 0 ), vec3( 0, 0, -2 ), vec3( -1, 1, 1 ), &positions, &texCoords );
	// -Y
	genCrossFace( ul + vec2( faceSize.x, faceSize.y * 2 ), ul + vec2( faceSize.x * 2, faceSize.y * 3 ), vec3( 2, 0, 0 ), vec3( 0, 0, 2 ), vec3( -1, -1, -1 ), &positions, &texCoords );
	// -Z
	genCrossFace( ul + vec2( faceSize.x * 1, faceSize.y * 3 ), ul + vec2( faceSize.x * 2, faceSize.y * 4 ), vec3( 2, 0, 0 ), vec3( 0, -2, 0 ), vec3( -1, 1, -1 ), &positions, &texCoords );

	drawCrossImpl( texture, positions, texCoords, lod );
}

void drawSolid( const Path2d &path, float /*approximationScale*/ )
{
	draw( Triangulator( path ).calcMesh() );
}

void drawSolid( const Shape2d &shape, float /*approximationScale*/ )
{
	draw( Triangulator( shape ).calcMesh() );
}

void drawSolid( const PolyLine2 &polyLine )
{
	draw( Triangulator( polyLine ).calcMesh() );
}

void drawSolidRect( const Rectf &r, const vec2 &upperLeftTexCoord, const vec2 &lowerRightTexCoord )
{
	auto ctx = context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	GLfloat data[8+8]; // both verts and texCoords
	GLfloat *verts = data, *texs = data + 8;

	verts[0*2+0] = r.getX2(); texs[0*2+0] = lowerRightTexCoord.x;
	verts[0*2+1] = r.getY1(); texs[0*2+1] = upperLeftTexCoord.y;
	verts[1*2+0] = r.getX1(); texs[1*2+0] = upperLeftTexCoord.x;
	verts[1*2+1] = r.getY1(); texs[1*2+1] = upperLeftTexCoord.y;
	verts[2*2+0] = r.getX2(); texs[2*2+0] = lowerRightTexCoord.x;
	verts[2*2+1] = r.getY2(); texs[2*2+1] = lowerRightTexCoord.y;
	verts[3*2+0] = r.getX1(); texs[3*2+0] = upperLeftTexCoord.x;
	verts[3*2+1] = r.getY2(); texs[3*2+1] = lowerRightTexCoord.y;

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	VboRef defaultVbo = ctx->getDefaultArrayVbo( sizeof(float)*16 );
	ScopedBuffer bufferBindScp( defaultVbo );
	defaultVbo->bufferSubData( 0, sizeof(float)*16, data );

	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}
	int texLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
	if( texLoc >= 0 ) {
		enableVertexAttribArray( texLoc );
		vertexAttribPointer( texLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*8) );
	}
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	ctx->popVao();
}
	
void drawSolidRoundedRect( const Rectf &r, float cornerRadius, int numSegmentsPerCorner, const vec2 &upperLeftTexCoord, const vec2 &lowerRightTexCoord )
{
	draw( geom::RoundedRect( r ).cornerRadius( cornerRadius ).cornerSubdivisions( numSegmentsPerCorner ).texCoords( upperLeftTexCoord, lowerRightTexCoord ) );
}

void drawStrokedRect( const Rectf &rect )
{
	GLfloat verts[8];
	verts[0] = rect.x1;	verts[1] = rect.y1;
	verts[2] = rect.x2;	verts[3] = rect.y1;
	verts[4] = rect.x2;	verts[5] = rect.y2;
	verts[6] = rect.x1;	verts[7] = rect.y2;

	auto ctx = context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();

	VboRef defaultVbo = ctx->getDefaultArrayVbo( 8 * sizeof( float ) );
	ScopedBuffer bufferBindScp( defaultVbo );
	defaultVbo->bufferSubData( 0, 8 * sizeof( float ), verts );

	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}

	ctx->setDefaultShaderVars();
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->drawArrays( GL_LINE_LOOP, 0, 4 );
	ctx->popVao();
}

void drawStrokedRect( const Rectf &rect, float lineWidth )
{
	const float halfWidth = lineWidth / 2;
	GLfloat verts[32];
	verts[0] = rect.x1 + halfWidth;		verts[1] = rect.y2 - halfWidth;		// left bar
	verts[2] = rect.x1 + halfWidth;		verts[3] = rect.y1 + halfWidth;
	verts[4] = rect.x1 - halfWidth;		verts[5] = rect.y2 + halfWidth;
	verts[6] = rect.x1 - halfWidth;		verts[7] = rect.y1 - halfWidth;
	verts[8] = rect.x1 + halfWidth;		verts[9] = rect.y1 + halfWidth;
	verts[10] = rect.x2 - halfWidth;	verts[11] = rect.y1 + halfWidth;	// upper bar
	verts[12] = rect.x1 - halfWidth;	verts[13] = rect.y1 - halfWidth;
	verts[14] = rect.x2 + halfWidth;	verts[15] = rect.y1 - halfWidth;
	verts[16] = rect.x2 - halfWidth;	verts[17] = rect.y1 + halfWidth;	// right bar
	verts[18] = rect.x2 - halfWidth;	verts[19] = rect.y2 - halfWidth;
	verts[20] = rect.x2 + halfWidth;	verts[21] = rect.y1 - halfWidth;
	verts[22] = rect.x2 + halfWidth;	verts[23] = rect.y2 + halfWidth;
	verts[24] = rect.x2 - halfWidth;	verts[25] = rect.y2 - halfWidth;	// bottom bar
	verts[26] = rect.x1 + halfWidth;	verts[27] = rect.y2 - halfWidth;
	verts[28] = rect.x2 + halfWidth;	verts[29] = rect.y2 + halfWidth;
	verts[30] = rect.x1 - halfWidth;	verts[31] = rect.y2 + halfWidth;

	auto ctx = context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();

	VboRef defaultVbo = ctx->getDefaultArrayVbo( 32 * sizeof( float ) );
	ScopedBuffer bufferBindScp( defaultVbo );
	defaultVbo->bufferSubData( 0, 32 * sizeof( float ), verts );

	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}

	ctx->setDefaultShaderVars();
	ctx->getDefaultVao()->replacementBindEnd();	
	ctx->drawArrays( GL_TRIANGLE_STRIP, 0, 16 );
	ctx->popVao();
}
	
void drawStrokedRoundedRect( const Rectf &r, float cornerRadius, int numSegmentsPerCorner )
{
	draw( geom::WireRoundedRect( r, cornerRadius ).cornerSubdivisions( numSegmentsPerCorner ) );
}

void drawStrokedCircle( const vec2 &center, float radius, int numSegments )
{
	if( numSegments <= 0 )
		numSegments = (int)math<double>::floor( radius * M_PI * 2 );

	gl::draw( geom::WireCircle().center( center ).radius( radius ).subdivisions( numSegments ) );
}

void drawStrokedCircle( const vec2 &center, float radius, float lineWidth, int numSegments )
{
	if( numSegments <= 0 )
		numSegments = (int)math<double>::floor( radius * M_PI * 2 );

	gl::draw( geom::Ring().center( center ).radius( radius ).width( lineWidth ).subdivisions( numSegments ) );
}

void drawStrokedEllipse( const vec2 &center, float radiusX, float radiusY, int numSegments )
{
	auto ctx = context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}
	
	if( numSegments <= 0 )
		numSegments = static_cast<int>(math<double>::floor( std::max( radiusX, radiusY ) * M_PI * 2 ) );
	if( numSegments < 3 )
		numSegments = 3;
	// construct circle
	const size_t numVertices = numSegments;
	vector<vec2> positions;
	positions.assign( numVertices, center );	// all vertices start at center
	const float tDelta = 2.0f * static_cast<float>(M_PI) * (1.0f / numVertices);
	float t = 0;
	for( auto &pos : positions ) {
		const vec2 unit( math<float>::cos( t ), math<float>::sin( t ) );
		pos += unit * vec2( radiusX, radiusY );	// push out from center
		t += tDelta;
	}
	// copy data to GPU
	const size_t size = positions.size() * sizeof( vec2 );
	auto arrayVbo = ctx->getDefaultArrayVbo( size );
	arrayVbo->bufferSubData( 0, size, (GLvoid*)positions.data() );
	// set attributes
	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	ScopedBuffer bufferBindScp( arrayVbo );

	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)nullptr );
	}
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	// draw
	drawArrays( GL_LINE_LOOP, 0, (GLsizei)positions.size() );
	ctx->popVao();
}

void drawSolidCircle( const vec2 &center, float radius, int numSegments )
{
	auto ctx = context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();

	if( numSegments <= 0 )
		numSegments = (int)math<double>::floor( radius * M_PI * 2 );
	if( numSegments < 3 ) numSegments = 3;
	size_t numVertices = numSegments + 2;

	size_t worstCaseSize = numVertices * sizeof(float) * ( 2 + 2 + 3 );
	VboRef defaultVbo = ctx->getDefaultArrayVbo( worstCaseSize );
	ScopedBuffer vboScp( defaultVbo );

	size_t dataSizeBytes = 0;

	size_t vertsOffset{}, texCoordsOffset{}, normalsOffset{};
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)dataSizeBytes );
		vertsOffset = dataSizeBytes;
		dataSizeBytes += numVertices * 2 * sizeof(float);
	}
	int texLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
	if( texLoc >= 0 ) {
		enableVertexAttribArray( texLoc );
		vertexAttribPointer( texLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)dataSizeBytes );
		texCoordsOffset = dataSizeBytes;
		dataSizeBytes += numVertices * 2 * sizeof(float);
	}
	int normalLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::NORMAL );
	if( normalLoc >= 0 ) {
		enableVertexAttribArray( normalLoc );
		vertexAttribPointer( normalLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(dataSizeBytes) );
		normalsOffset = dataSizeBytes;
		dataSizeBytes += numVertices * 3 * sizeof(float);
	}

	unique_ptr<uint8_t[]> data( new uint8_t[dataSizeBytes] );
	vec2 *verts = ( posLoc >= 0 ) ? reinterpret_cast<vec2*>( data.get() + vertsOffset ) : nullptr;
	vec2 *texCoords = ( texLoc >= 0 ) ? reinterpret_cast<vec2*>( data.get() + texCoordsOffset ) : nullptr;
	vec3 *normals = ( normalLoc >= 0 ) ? reinterpret_cast<vec3*>( data.get() + normalsOffset ) : nullptr;

	if( verts )
		verts[0] = center;
	if( texCoords )
		texCoords[0] = vec2( 0.5f, 0.5f );
	if( normals )
		normals[0] = vec3( 0, 0, 1 );
	const float tDelta = 1.0f / numSegments * 2 * (float)M_PI;
	for( int s = 0; s <= numSegments; s++ ) {
		const vec2 unit( math<float>::cos( s * tDelta ), math<float>::sin( s * tDelta ) );
		if( verts )
			verts[s+1] = center + unit * radius;
		if( texCoords )
			texCoords[s+1] = unit * 0.5f + vec2( 0.5f, 0.5f );
		if( normals )
			normals[s+1] = vec3( 0, 0, 1 );
	}

	defaultVbo->bufferSubData( 0, dataSizeBytes, data.get() );
	ctx->getDefaultVao()->replacementBindEnd();

	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_TRIANGLE_FAN, 0, numSegments + 2 );
	ctx->popVao();
}
void drawSolidEllipse( const vec2 &center, float radiusX, float radiusY, int numSegments )
{
	auto ctx = context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();

	if( numSegments <= 0 ) {
		numSegments = (int)math<double>::floor( std::max(radiusX,radiusY) * M_PI * 2 );
	}
	if( numSegments < 2 ) numSegments = 2;
	size_t numVertices = (numSegments+2)*2;
	
	size_t worstCaseSize = numVertices * sizeof(float) * ( 2 + 2 + 3 );
	VboRef defaultVbo = ctx->getDefaultArrayVbo( worstCaseSize );
	ScopedBuffer vboScp( defaultVbo );

	size_t dataSizeBytes = 0;

	size_t vertsOffset, texCoordsOffset, normalsOffset;
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)dataSizeBytes );
		vertsOffset = dataSizeBytes;
		dataSizeBytes += numVertices * 2 * sizeof(float);
	}
	int texLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
	if( texLoc >= 0 ) {
		enableVertexAttribArray( texLoc );
		vertexAttribPointer( texLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)dataSizeBytes );
		texCoordsOffset = dataSizeBytes;
		dataSizeBytes += numVertices * 2 * sizeof(float);
	}
	int normalLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::NORMAL );
	if( normalLoc >= 0 ) {
		enableVertexAttribArray( normalLoc );
		vertexAttribPointer( normalLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(dataSizeBytes) );
		normalsOffset = dataSizeBytes;
		dataSizeBytes += numVertices * 3 * sizeof(float);
	}

	unique_ptr<uint8_t[]> data( new uint8_t[dataSizeBytes] );
	vec2 *verts = ( posLoc >= 0 ) ? reinterpret_cast<vec2*>( data.get() + vertsOffset ) : nullptr;
	vec2 *texCoords = ( texLoc >= 0 ) ? reinterpret_cast<vec2*>( data.get() + texCoordsOffset ) : nullptr;
	vec3 *normals = ( normalLoc >= 0 ) ? reinterpret_cast<vec3*>( data.get() + normalsOffset ) : nullptr;

	if( verts )
		verts[0] = center;
	if( texCoords )
		texCoords[0] = vec2( 0.5f, 0.5f );
	if( normals )
		normals[0] = vec3( 0, 0, 1 );
	const float tDelta = 1.0f / numSegments * 2 * (float)M_PI;
	float t = 0;
	for( int s = 0; s <= numSegments; s++ ) {
		const vec2 unit( math<float>::cos( t ), math<float>::sin( t ) );
		if( verts )
			verts[s+1] = center + unit * vec2( radiusX, radiusY );
		if( texCoords )
			texCoords[s+1] = unit * 0.5f + vec2( 0.5f, 0.5f );
		if( normals )
			normals[s+1] = vec3( 0, 0, 1 );
		t += tDelta;
	}

	defaultVbo->bufferSubData( 0, dataSizeBytes, data.get() );
	ctx->getDefaultVao()->replacementBindEnd();

	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_TRIANGLE_FAN, 0, numSegments + 2 );
	ctx->popVao();
}

void drawSolidTriangle( const vec2 pts[3] )
{
	drawSolidTriangle( pts, nullptr );
}

void drawSolidTriangle( const vec2 &pt0, const vec2 &pt1, const vec2 &pt2 )
{
	vec2 pts[3] = { pt0, pt1, pt2 };
	drawSolidTriangle( pts, nullptr );
}

//! Renders a textured triangle.
void drawSolidTriangle( const vec2 &pt0, const vec2 &pt1, const vec2 &pt2, const vec2 &texCoord0, const vec2 &texCoord1, const vec2 &texCoord2 )
{
	vec2 pts[3] = { pt0, pt1, pt2 };
	vec2 texs[3] = { texCoord0, texCoord1, texCoord2 };
	
	drawSolidTriangle( pts, texs );
}

void drawStrokedTriangle( const vec2 &pt0, const vec2 &pt1, const vec2 &pt2 )
{
	vec2 pts[3] = { pt0, pt1, pt2 };
	drawSolidTriangle( pts, nullptr );
}

void drawSolidTriangle( const vec2 pts[3], const vec2 texCoord[3] )
{
	auto ctx = context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	GLfloat data[3*2+3*2]; // both verts and texCoords
	memcpy( data, pts, sizeof(float) * 3 * 2 );
	if( texCoord )
		memcpy( data + 3 * 2, texCoord, sizeof(float) * 3 * 2 );

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	VboRef defaultVbo = ctx->getDefaultArrayVbo( sizeof(float)*12 );
	ScopedBuffer bufferBindScp( defaultVbo );
	defaultVbo->bufferSubData( 0, sizeof(float) * ( texCoord ? 12 : 6 ), data );

	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}
	if( texCoord ) {
		int texLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
		if( texLoc >= 0 ) {
			enableVertexAttribArray( texLoc );
			vertexAttribPointer( texLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*6) );
		}
	}
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_TRIANGLES, 0, 3 );
	ctx->popVao();
}

void drawSolidTriangle( const vec3 &pt0, const vec3 &pt1, const vec3 &pt2 )
{
	vec3 pts[3] = { pt0, pt1, pt2 };
	drawSolidTriangle( pts, nullptr );
}

//! Renders a textured triangle.
void drawSolidTriangle( const vec3 &pt0, const vec3 &pt1, const vec3 &pt2, const vec2 &texCoord0, const vec2 &texCoord1, const vec2 &texCoord2 )
{
	vec3 pts[3] = { pt0, pt1, pt2 };
	vec2 texs[3] = { texCoord0, texCoord1, texCoord2 };
	drawSolidTriangle( pts, texs );
}

void drawSolidTriangle( const vec3 pts[3], const vec2 texCoord[3] )
{
	auto ctx = context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	GLfloat data[3*3+3*2]; // both verts and texCoords
	memcpy( data, pts, sizeof(float) * 3 * 3 );
	if( texCoord )
		memcpy( data + 3 * 2, texCoord, sizeof(float) * 3 * 2 );

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	VboRef defaultVbo = ctx->getDefaultArrayVbo( sizeof(float)*15 );
	ScopedBuffer bufferBindScp( defaultVbo );
	defaultVbo->bufferSubData( 0, sizeof(float) * ( texCoord ? 15 : 9 ), data );

	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}
	if( texCoord ) {
		int texLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
		if( texLoc >= 0 ) {
			enableVertexAttribArray( texLoc );
			vertexAttribPointer( texLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*6) );
		}
	}
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_TRIANGLES, 0, 3 );
	ctx->popVao();
}

void drawSphere( const Sphere &sphere, int subdivisions )
{
	drawSphere( sphere.getCenter(), sphere.getRadius(), subdivisions );
}

void drawSphere( const vec3 &center, float radius, int subdivisions )
{
	draw( geom::Sphere().center( center ).radius( radius ).subdivisions( subdivisions ) );
}

void drawBillboard( const vec3 &pos, const vec2 &scale, float rotationRadians, const vec3 &bbRight, const vec3 &bbUp, const Rectf &texCoords )
{
	auto ctx = context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	GLfloat data[12+8]; // both verts and texCoords
	vec3 *verts = (vec3*)data;
	float *texCoordsOut = data + 12;

	float sinA = math<float>::sin( rotationRadians );
	float cosA = math<float>::cos( rotationRadians );

	verts[0] = pos + bbRight * ( -0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
	texCoordsOut[0*2+0] = texCoords.getX1(); texCoordsOut[0*2+1] = texCoords.getY1();
	verts[1] = pos + bbRight * ( -0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
	texCoordsOut[1*2+0] = texCoords.getX1(); texCoordsOut[1*2+1] = texCoords.getY2();
	verts[2] = pos + bbRight * ( 0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
	texCoordsOut[2*2+0] = texCoords.getX2(); texCoordsOut[2*2+1] = texCoords.getY1();
	verts[3] = pos + bbRight * ( 0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
	texCoordsOut[3*2+0] = texCoords.getX2(); texCoordsOut[3*2+1] = texCoords.getY2();

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	VboRef defaultVbo = ctx->getDefaultArrayVbo( sizeof(float)*20 );
	ScopedBuffer bufferBindScp( defaultVbo );
	defaultVbo->bufferSubData( 0, sizeof(float)*20, data );

	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}
	int texLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
	if( texLoc >= 0 ) {
		enableVertexAttribArray( texLoc );
		vertexAttribPointer( texLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*12) );
	}

	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	ctx->popVao();
}
	
void drawFrustum( const Camera &cam )
{
	vec3 nearTopLeft, nearTopRight, nearBottomLeft, nearBottomRight;
	cam.getNearClipCoordinates( &nearTopLeft, &nearTopRight, &nearBottomLeft, &nearBottomRight );
	
	vec3 farTopLeft, farTopRight, farBottomLeft, farBottomRight;
	cam.getFarClipCoordinates( &farTopLeft, &farTopRight, &farBottomLeft, &farBottomRight );

	// extract camera position from modelview matrix, so that it will work with any camera	
	//  see: http://www.gamedev.net/topic/397751-how-to-get-camera-position/page__p__3638207#entry3638207
	mat4 view = cam.getViewMatrix();
	vec3 eye;
	eye.x = -( view[0][0] * view[3][0] + view[0][1] * view[3][1] + view[0][2] * view[3][2] );
	eye.y = -( view[1][0] * view[3][0] + view[1][1] * view[3][1] + view[1][2] * view[3][2] );
	eye.z = -( view[2][0] * view[3][0] + view[2][1] * view[3][1] + view[2][2] * view[3][2] );

														// indices
	std::array<vec3, 9> vertices = { eye,				// 0
									nearTopLeft,		// 1
									nearTopRight,		// 2
									nearBottomLeft,		// 3
									nearBottomRight,	// 4
									farTopLeft,			// 5
									farTopRight,		// 6
									farBottomLeft,		// 7
									farBottomRight };	// 8		// indice usage
	std::array<GLubyte, 32> indices = { 0, 1, 0, 2, 0, 4, 0, 3,		// draws from eye to near plane
										5, 1, 6, 2, 7, 3, 8, 4,		// draws from far to near corners
										1, 2, 2, 4, 4, 3, 3, 1,		// draws near rect
										5, 6, 6, 8, 8, 7, 7, 5};	// draws far rect
	
	auto ctx = ci::gl::context();
	const GlslProg* curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}
	
	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	gl::VboRef defaultVbo = ctx->getDefaultArrayVbo( sizeof(vec3)*9 );
	gl::VboRef elementVbo = ctx->getDefaultElementVbo( 32 );
	gl::ScopedBuffer bufferBindScp( defaultVbo );
	defaultVbo->bufferSubData( 0, sizeof(vec3)*9, vertices.data() );
	
	elementVbo->bind();
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		gl::enableVertexAttribArray( posLoc );
		gl::vertexAttribPointer( posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}
	
	elementVbo->bufferSubData( 0, 32, indices.data() );
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawElements( GL_LINES, 32, GL_UNSIGNED_BYTE, 0 );
	ctx->popVao();
}
	
void drawCoordinateFrame( float axisLength, float headLength, float headRadius )
{
	gl::ScopedColor color( ColorA( 1.0f, 0.0f, 0.0f, 1.0f ) );
	drawVector( vec3( 0.0f ), vec3( 1.0f, 0.0f, 0.0f ) * axisLength, headLength, headRadius );
	gl::color( 0.0f, 1.0f, 0.0f, 1.0f );
	drawVector( vec3( 0.0f ), vec3( 0.0f, 1.0f, 0.0f ) * axisLength, headLength, headRadius );
	gl::color( 0.0f, 0.0f, 1.0f, 1.0f );
	drawVector( vec3( 0.0f ), vec3( 0.0f, 0.0f, 1.0f ) * axisLength, headLength, headRadius );
}
	
void drawVector( const vec3& start, const vec3& end, float headLength, float headRadius )
{
	vec3 dir = end - start;
	vec3 ori = end - normalize( dir ) * headLength;
	gl::draw( geom::Cone().base( headRadius ).height( headLength ).origin( ori ).direction( dir ) );

	// Since lines don't have normals, let's make sure we're only rendering colors.
	gl::ScopedGlslProg shader( gl::getStockShader( gl::ShaderDef().color() ) );
	gl::drawLine( start, end );
}

namespace {
void drawStringHelper( const std::string &str, const vec2 &pos, const ColorA &color, Font font, int justification )
{
#if ! defined( CINDER_ANDROID )
	
	if( str.empty() )
		return;

	// justification: { left = -1, center = 0, right = 1 }
	ScopedColor colorScp( Color::white() );

	static Font defaultFont = Font::getDefault();
	if( ! font )
		font = defaultFont;

	float baselineOffset;
#if defined( CINDER_COCOA_TOUCH )
	ivec2 actualSize;
	Surface8u pow2Surface( renderStringPow2( str, font, color, &actualSize, &baselineOffset ) );
	gl::TextureRef tex = gl::Texture::create( pow2Surface );
	tex->setCleanBounds( Area( 0, 0, actualSize.x, actualSize.y ) );
	baselineOffset += pow2Surface.getHeight();
#else
	gl::TextureRef tex = gl::Texture::create( renderString( str, font, color, &baselineOffset ) );
#endif

	if( justification == -1 ) // left
		draw( tex, pos - vec2( 0, baselineOffset ) );
	else if( justification == 0 ) // center
		draw( tex, pos - vec2( tex->getWidth() * 0.5f, baselineOffset ) );
	else // right
		draw( tex, pos - vec2( (float)tex->getWidth(), baselineOffset ) );

#endif // ! defined( CINDER_ANDROID )
}
} // anonymous namespace

void drawString( const std::string &str, const vec2 &pos, const ColorA &color, Font font )
{
	drawStringHelper( str, pos, color, font, -1 );
}

void drawStringCentered( const std::string &str, const vec2 &pos, const ColorA &color, Font font )
{
	drawStringHelper( str, pos, color, font, 0 );
}

void drawStringRight( const std::string &str, const vec2 &pos, const ColorA &color, Font font )
{
	drawStringHelper( str, pos, color, font, 1 );
}

} } // namespace cinder::gl
