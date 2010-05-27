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

#include "cinder/gl/Vbo.h"
#include <sstream>

using std::vector;
using std::pair;

namespace cinder { namespace gl {

//enum { CUSTOM_ATTR_FLOAT, CUSTOM_ATTR_FLOAT2, CUSTOM_ATTR_FLOAT3, CUSTOM_ATTR_FLOAT4, TOTAL_CUSTOM_ATTR_TYPES };
int		VboMesh::Layout::sCustomAttrSizes[TOTAL_CUSTOM_ATTR_TYPES] = { 4, 8, 12, 16 };
GLint	VboMesh::Layout::sCustomAttrNumComponents[TOTAL_CUSTOM_ATTR_TYPES] = { 1, 2, 3, 4 };
GLenum	VboMesh::Layout::sCustomAttrTypes[TOTAL_CUSTOM_ATTR_TYPES] = { GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT };

Vbo::Obj::Obj( GLenum aTarget )
	: mTarget( aTarget )
{
	glGenBuffers( 1, &mId );
}

Vbo::Obj::~Obj()
{
	glDeleteBuffers( 1, &mId );
}

Vbo::Vbo( GLenum aTarget )
{
	mObj = shared_ptr<Vbo::Obj>( new Obj( aTarget ) );
}

void Vbo::bind()
{
	glBindBuffer( mObj->mTarget, mObj->mId );
}

void Vbo::unbind()
{
	glBindBuffer( mObj->mTarget, 0 );
}

void Vbo::bufferData( size_t size, const void *data, GLenum usage )
{
	bind();
	glBufferDataARB( mObj->mTarget, size, data, usage );
}

void Vbo::bufferSubData( ptrdiff_t offset, size_t size, const void *data )
{
	bind();
	glBufferSubDataARB( mObj->mTarget, offset, size, data );
}

uint8_t* Vbo::map( GLenum access )
{
	bind();
	return reinterpret_cast<uint8_t*>( glMapBuffer( mObj->mTarget, access ) );
}

void Vbo::unmap()
{
	bind();
	GLboolean result = glUnmapBuffer( mObj->mTarget );
	if( result != GL_TRUE )
		throw VboFailedUnmapExc();
}


bool VboMesh::Layout::hasStaticTexCoords() const
{
	for( size_t t = 0; t <= ATTR_MAX_TEXTURE_UNIT; ++t )
		if( ( mAttributes[ATTR_TEXCOORDS2D_0+t] == STATIC ) || ( mAttributes[ATTR_TEXCOORDS3D_0+t] == STATIC ) )
			return true;
		
	return false;
}

bool VboMesh::Layout::hasDynamicTexCoords() const
{
	for( size_t t = 0; t <= ATTR_MAX_TEXTURE_UNIT; ++t )
		if( ( mAttributes[ATTR_TEXCOORDS2D_0+t] == DYNAMIC ) || ( mAttributes[ATTR_TEXCOORDS3D_0+t] == DYNAMIC ) )
			return true;
		
	return false;
}


VboMesh::VboMesh( const TriMesh &triMesh, Layout layout )
	: mObj( shared_ptr<Obj>( new Obj ) )
{
	if( layout.isDefaults() ) { // we need to start by preparing our layout
		if( triMesh.hasNormals() )
			mObj->mLayout.setStaticNormals();
		if( triMesh.hasTexCoords() )
			mObj->mLayout.setStaticTexCoords2d();
		mObj->mLayout.setStaticIndices();
		mObj->mLayout.setStaticPositions();
	}

	mObj->mPrimitiveType = GL_TRIANGLES;
	mObj->mNumIndices = triMesh.getNumIndices();
	mObj->mNumVertices = triMesh.getNumVertices();

	initializeBuffers( false );
			
	// upload the indices
	getIndexVbo().bufferData( sizeof(uint32_t) * triMesh.getNumIndices(), &(triMesh.getIndices()[0]), (mObj->mLayout.hasStaticIndices()) ? GL_STATIC_DRAW : GL_STREAM_DRAW );
	
	// upload the verts
	for( int buffer = STATIC_BUFFER; buffer <= DYNAMIC_BUFFER; ++buffer ) {
		if( ! mObj->mBuffers[buffer] )
			continue;
		
		uint8_t *ptr = mObj->mBuffers[buffer].map( GL_WRITE_ONLY );
		
		bool copyPosition = ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticPositions() : mObj->mLayout.hasDynamicPositions();
		bool copyNormal = ( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticNormals() : mObj->mLayout.hasDynamicNormals() ) && triMesh.hasNormals();
		bool copyTexCoord2D = ( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticTexCoords2d() : mObj->mLayout.hasDynamicTexCoords2d() ) && triMesh.hasTexCoords();
		
		for( size_t v = 0; v < mObj->mNumVertices; ++v ) {
			if( copyPosition ) {
				*(reinterpret_cast<Vec3f*>(ptr)) = triMesh.getVertices()[v];
				ptr += sizeof( Vec3f );
			}
			if( copyNormal ) {
				*(reinterpret_cast<Vec3f*>(ptr)) = triMesh.getNormals()[v];
				ptr += sizeof( Vec3f );
			}
			if( copyTexCoord2D ) {
				*(reinterpret_cast<Vec2f*>(ptr)) = triMesh.getTexCoords()[v];
				ptr += sizeof( Vec2f );
			}
		}
		
		mObj->mBuffers[buffer].unmap();
	}

	unbindBuffers();	
}

VboMesh::VboMesh( size_t numVertices, size_t numIndices, Layout layout, GLenum primitiveType )
	: mObj( shared_ptr<Obj>( new Obj ) )
{
	mObj->mLayout = layout;
	mObj->mPrimitiveType = primitiveType;
	mObj->mNumIndices = numIndices;
	mObj->mNumVertices = numVertices;

	initializeBuffers( true );
	
	// allocate buffer for indices
	if( mObj->mLayout.hasIndices() )
		mObj->mBuffers[INDEX_BUFFER].bufferData( sizeof(uint32_t) * mObj->mNumIndices, NULL, (mObj->mLayout.hasStaticIndices()) ? GL_STATIC_DRAW : GL_STREAM_DRAW );
	
	unbindBuffers();	
}

VboMesh::VboMesh( size_t numVertices, size_t numIndices, Layout layout, GLenum primitiveType, Vbo *indexBuffer, Vbo *staticBuffer, Vbo *dynamicBuffer )
	: mObj( shared_ptr<Obj>( new Obj ) )
{
	mObj->mLayout = layout;
	mObj->mPrimitiveType = primitiveType;
	mObj->mNumIndices = numIndices;
	mObj->mNumVertices = numVertices;

	if( indexBuffer ) {
		mObj->mBuffers[INDEX_BUFFER] = *indexBuffer;
		if( indexBuffer->getTarget() != GL_ELEMENT_ARRAY_BUFFER )
			throw VboInvalidTargetExc();
	}
	if( staticBuffer ) {
		mObj->mBuffers[STATIC_BUFFER] = *staticBuffer;
		if( staticBuffer->getTarget() != GL_ARRAY_BUFFER )
			throw VboInvalidTargetExc();
	}
	if( dynamicBuffer ) {
		mObj->mBuffers[DYNAMIC_BUFFER] = *dynamicBuffer;
		if( dynamicBuffer->getTarget() != GL_ARRAY_BUFFER )
			throw VboInvalidTargetExc();		
	}
	
	initializeBuffers( true );
	unbindBuffers();
}

// If any buffers are not NULL they will be ignored
void VboMesh::initializeBuffers( bool staticDataPlanar )
{
	bool hasStaticBuffer = mObj->mLayout.hasStaticPositions() || mObj->mLayout.hasStaticNormals() || mObj->mLayout.hasStaticColorsRGB() || mObj->mLayout.hasStaticColorsRGBA() || mObj->mLayout.hasStaticTexCoords() || ( ! mObj->mLayout.mCustomStatic.empty() );
	bool hasDynamicBuffer = mObj->mLayout.hasDynamicPositions() || mObj->mLayout.hasDynamicNormals() || mObj->mLayout.hasDynamicColorsRGB() || mObj->mLayout.hasDynamicColorsRGBA() || mObj->mLayout.hasDynamicTexCoords() || ( ! mObj->mLayout.mCustomDynamic.empty() );

	if( ( mObj->mLayout.hasStaticIndices() || mObj->mLayout.hasDynamicIndices() ) && ( ! mObj->mBuffers[INDEX_BUFFER] ) )
		mObj->mBuffers[INDEX_BUFFER] = Vbo( GL_ELEMENT_ARRAY_BUFFER );

	if( hasStaticBuffer && staticDataPlanar ) { // Planar static buffer
		size_t offset = 0;
	
		if( ! mObj->mBuffers[STATIC_BUFFER] )
			mObj->mBuffers[STATIC_BUFFER] = Vbo( GL_ARRAY_BUFFER );

		if( mObj->mLayout.hasStaticPositions() ) {
			mObj->mPositionOffset = offset;
			offset += sizeof(GLfloat) * 3 * mObj->mNumVertices;
		}
		
		if( mObj->mLayout.hasStaticNormals() ) {
			mObj->mNormalOffset = offset;
			offset += sizeof(GLfloat) * 3 * mObj->mNumVertices;
		}

		if( mObj->mLayout.hasStaticColorsRGB() ) {
			mObj->mColorRGBOffset = offset;
			offset += sizeof(GLfloat) * 3 * mObj->mNumVertices;
		}

		if( mObj->mLayout.hasStaticColorsRGBA() ) {
			mObj->mColorRGBAOffset = offset;
			offset += sizeof(GLfloat) * 4 * mObj->mNumVertices;
		}
		
		for( size_t t = 0; t <= ATTR_MAX_TEXTURE_UNIT; ++t ) {
			if( mObj->mLayout.hasStaticTexCoords2d( t ) ) {
				mObj->mTexCoordOffset[t] = offset;
				offset += sizeof(GLfloat) * 2 * mObj->mNumVertices;
			}
			else if( mObj->mLayout.hasStaticTexCoords3d( t ) ) {
				mObj->mTexCoordOffset[t] = offset;
				offset += sizeof(GLfloat) * 3 * mObj->mNumVertices;
			}
		}

		for( size_t c = 0; c < mObj->mLayout.mCustomStatic.size(); ++c ) {
			mObj->mLayout.mCustomStatic[c].second = offset;
			offset += VboMesh::Layout::sCustomAttrSizes[mObj->mLayout.mCustomStatic[c].first] * mObj->mNumVertices;
		}
		
		mObj->mStaticStride = 0;
		
		// setup the buffer to be the summed size
		mObj->mBuffers[STATIC_BUFFER].bufferData( offset, NULL, GL_STATIC_DRAW );
	}
	else if( hasStaticBuffer && ( ! staticDataPlanar ) ) { // Interleaved static buffer
		size_t offset = 0;

		if( ! mObj->mBuffers[STATIC_BUFFER] )
			mObj->mBuffers[STATIC_BUFFER] = Vbo( GL_ARRAY_BUFFER );

		if( mObj->mLayout.hasStaticPositions() ) {
			mObj->mPositionOffset = offset;
			offset += sizeof(GLfloat) * 3;
		}
		
		if( mObj->mLayout.hasStaticNormals() ) {
			mObj->mNormalOffset = offset;
			offset += sizeof(GLfloat) * 3;
		}

		if( mObj->mLayout.hasStaticColorsRGB() ) {
			mObj->mColorRGBOffset = offset;
			offset += sizeof(GLfloat) * 3;
		}
		else if( mObj->mLayout.hasStaticColorsRGBA() ) {
			mObj->mColorRGBAOffset = offset;
			offset += sizeof(GLfloat) * 4;
		}
		
		for( size_t t = 0; t <= ATTR_MAX_TEXTURE_UNIT; ++t ) {
			if( mObj->mLayout.hasStaticTexCoords2d( t ) ) {
				mObj->mTexCoordOffset[t] = offset;
				offset += sizeof(GLfloat) * 2;
			}
			else if( mObj->mLayout.hasStaticTexCoords3d( t ) ) {
				mObj->mTexCoordOffset[t] = offset;
				offset += sizeof(GLfloat) * 3;
			}
		}

		for( size_t c = 0; c < mObj->mLayout.mCustomStatic.size(); ++c ) {
			mObj->mLayout.mCustomStatic[c].second = offset;
			offset += VboMesh::Layout::sCustomAttrSizes[mObj->mLayout.mCustomStatic[c].first];
		}
		
		mObj->mStaticStride = offset;
		
		// setup the buffer to be the summed size
		mObj->mBuffers[STATIC_BUFFER].bufferData( mObj->mStaticStride * mObj->mNumVertices, NULL, GL_STATIC_DRAW );
	}
	else {
		mObj->mStaticStride = 0;
	}
	
	if( hasDynamicBuffer ) {
		size_t offset = 0;

		if( ! mObj->mBuffers[DYNAMIC_BUFFER] )
			mObj->mBuffers[DYNAMIC_BUFFER] = Vbo( GL_ARRAY_BUFFER );
		
		if( mObj->mLayout.hasDynamicPositions() ) {
			mObj->mPositionOffset = offset;
			offset += sizeof(GLfloat) * 3;
		}
		
		if( mObj->mLayout.hasDynamicNormals() ) {
			mObj->mNormalOffset = offset;
			offset += sizeof(GLfloat) * 3;
		}

		if( mObj->mLayout.hasDynamicColorsRGB() ) {
			mObj->mColorRGBOffset = offset;
			offset += sizeof(GLfloat) * 3;
		}
		else if( mObj->mLayout.hasDynamicColorsRGBA() ) {
			mObj->mColorRGBAOffset = offset;
			offset += sizeof(GLfloat) * 4;
		}
		
		for( size_t t = 0; t <= ATTR_MAX_TEXTURE_UNIT; ++t ) {
			if( mObj->mLayout.hasDynamicTexCoords2d( t ) ) {
				mObj->mTexCoordOffset[t] = offset;
				offset += sizeof(GLfloat) * 2;
			}
			else if( mObj->mLayout.hasDynamicTexCoords3d( t ) ) {
				mObj->mTexCoordOffset[t] = offset;
				offset += sizeof(GLfloat) * 3;
			}
		}

		for( size_t c = 0; c < mObj->mLayout.mCustomDynamic.size(); ++c ) {
			mObj->mLayout.mCustomDynamic[c].second = offset;
			offset += VboMesh::Layout::sCustomAttrSizes[mObj->mLayout.mCustomDynamic[c].first];
		}

		mObj->mDynamicStride = offset;

		// setup the buffer to be the summed size
		mObj->mBuffers[DYNAMIC_BUFFER].bufferData( mObj->mDynamicStride * mObj->mNumVertices, NULL, GL_STREAM_DRAW );
	}
	else {
		mObj->mDynamicStride = 0;
	}

	// initialize all the custom attribute locations
	if( ! mObj->mLayout.mCustomStatic.empty() )
		mObj->mCustomStaticLocations = vector<GLint>( mObj->mLayout.mCustomStatic.size(), -1 );
	if( ! mObj->mLayout.mCustomDynamic.empty() )
		mObj->mCustomDynamicLocations = vector<GLint>( mObj->mLayout.mCustomDynamic.size(), -1 );
}

void VboMesh::enableClientStates() const
{
	if( mObj->mLayout.hasPositions() )
		glEnableClientState( GL_VERTEX_ARRAY );
	else
		glDisableClientState( GL_VERTEX_ARRAY );
	if( mObj->mLayout.hasNormals() )
		glEnableClientState( GL_NORMAL_ARRAY );
	else
		glDisableClientState( GL_NORMAL_ARRAY );
	if( mObj->mLayout.hasColorsRGB() || mObj->mLayout.hasColorsRGBA() )
		glEnableClientState( GL_COLOR_ARRAY );
	else
		glDisableClientState( GL_COLOR_ARRAY );
		
	for( size_t t = 0; t <= ATTR_MAX_TEXTURE_UNIT; ++t ) {
		if( mObj->mLayout.hasTexCoords( t ) ) {
			glClientActiveTexture( GL_TEXTURE0 + t );
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		}
	}	
	
	for( size_t a = 0; a < mObj->mCustomStaticLocations.size(); ++a ) {
		if( mObj->mCustomStaticLocations[a] < 0 )
			throw;
		glEnableVertexAttribArray( mObj->mCustomStaticLocations[a] );
	}

	for( size_t a = 0; a < mObj->mCustomDynamicLocations.size(); ++a ) {
		if( mObj->mCustomDynamicLocations[a] < 0 )
			throw;
		glEnableVertexAttribArray( mObj->mCustomDynamicLocations[a] );
	}
}

void VboMesh::disableClientStates() const
{
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	for( size_t t = 0; t <= ATTR_MAX_TEXTURE_UNIT; ++t ) {
		if( mObj->mLayout.hasTexCoords( t ) ) {
			glClientActiveTexture( GL_TEXTURE0 + t );
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		}
	}	
	
	for( size_t a = 0; a < mObj->mCustomStaticLocations.size(); ++a ) {
		if( mObj->mCustomStaticLocations[a] < 0 )
			throw;
		glDisableVertexAttribArray( mObj->mCustomStaticLocations[a] );
	}

	for( size_t a = 0; a < mObj->mCustomDynamicLocations.size(); ++a ) {
		if( mObj->mCustomDynamicLocations[a] < 0 )
			throw;
		glDisableVertexAttribArray( mObj->mCustomDynamicLocations[a] );
	}
}

void VboMesh::bindAllData() const
{
	if( mObj->mLayout.hasIndices() ) {
		mObj->mBuffers[INDEX_BUFFER].bind();
	}
	
	for( int buffer = STATIC_BUFFER; buffer <= DYNAMIC_BUFFER; ++buffer ) {
		if( ! mObj->mBuffers[buffer] ) continue;
		
		mObj->mBuffers[buffer].bind();
		uint8_t stride = ( buffer == STATIC_BUFFER ) ? mObj->mStaticStride : mObj->mDynamicStride;
		
		if( ( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticNormals() : mObj->mLayout.hasDynamicNormals() ) )
			glNormalPointer( GL_FLOAT, stride, ( const GLvoid *)mObj->mNormalOffset );

		if( ( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticColorsRGB() : mObj->mLayout.hasDynamicColorsRGB() ) )
			glColorPointer( 3, GL_FLOAT, stride, ( const GLvoid *)mObj->mColorRGBOffset );
		else if( ( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticColorsRGBA() : mObj->mLayout.hasDynamicColorsRGBA() ) )
			glColorPointer( 4, GL_FLOAT, stride, ( const GLvoid *)mObj->mColorRGBAOffset );


		for( size_t t = 0; t <= ATTR_MAX_TEXTURE_UNIT; ++t ) {
			if( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticTexCoords2d( t ) : mObj->mLayout.hasDynamicTexCoords2d( t ) ) {
				glClientActiveTexture( GL_TEXTURE0 + t );
				glTexCoordPointer( 2, GL_FLOAT, stride, (const GLvoid *)mObj->mTexCoordOffset[t] );
			}
			else if( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticTexCoords3d( t ) : mObj->mLayout.hasDynamicTexCoords3d( t ) ) {
				glClientActiveTexture( GL_TEXTURE0 + t );
				glTexCoordPointer( 3, GL_FLOAT, stride, (const GLvoid *)mObj->mTexCoordOffset[t] );
			}
		}	

		if( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticPositions() : mObj->mLayout.hasDynamicPositions() )
			glVertexPointer( 3, GL_FLOAT, stride, (const GLvoid*)mObj->mPositionOffset );
	}

	for( int buffer = STATIC_BUFFER; buffer <= DYNAMIC_BUFFER; ++buffer ) {
		if( ! mObj->mBuffers[buffer] ) continue;

		const vector<pair<VboMesh::Layout::CustomAttr,size_t> > &attributes( ( buffer == DYNAMIC_BUFFER ) ? mObj->mLayout.mCustomDynamic : mObj->mLayout.mCustomStatic );
		const vector<GLint> &locations( ( buffer == DYNAMIC_BUFFER ) ? mObj->mCustomDynamicLocations : mObj->mCustomStaticLocations );
		GLsizei stride = ( ( buffer == DYNAMIC_BUFFER ) ? mObj->mDynamicStride : mObj->mStaticStride );
	
		if( attributes.empty() )
			continue;
		
		mObj->mBuffers[buffer].bind();
		
		for( size_t a = 0; a < attributes.size(); ++a ) {
			const GLvoid *offset = reinterpret_cast<const GLvoid*>( attributes[a].second );
			glVertexAttribPointer( locations[a], Layout::sCustomAttrNumComponents[attributes[a].first], Layout::sCustomAttrTypes[attributes[a].first], GL_FALSE, stride, offset );
		}	
	}
}

void VboMesh::bindIndexBuffer() const
{
	mObj->mBuffers[INDEX_BUFFER].bind();
}

void VboMesh::unbindBuffers()
{
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void VboMesh::bufferIndices( const std::vector<uint32_t> &indices )
{
	mObj->mBuffers[INDEX_BUFFER].bufferData( sizeof(uint32_t) * indices.size(), &(indices[0]), (mObj->mLayout.hasStaticIndices()) ? GL_STATIC_DRAW : GL_STREAM_DRAW );
}

void VboMesh::bufferPositions( const std::vector<Vec3f> &positions )
{
	bufferPositions( &positions[0], positions.size() );
}

void VboMesh::bufferPositions( const Vec3f *positions, size_t count )
{
	if( mObj->mLayout.hasDynamicPositions() ) {
		if( mObj->mDynamicStride == 0 )
			getDynamicVbo().bufferSubData( mObj->mPositionOffset, sizeof(Vec3f) * count, positions );
		else
			throw;
	}
	else if( mObj->mLayout.hasStaticPositions() ) {
		if( mObj->mStaticStride == 0 ) { // planar data
			getStaticVbo().bufferSubData( mObj->mPositionOffset, sizeof(Vec3f) * count, positions );
		}
		else
			throw;
	}
	else
		throw;
}

void VboMesh::bufferNormals( const std::vector<Vec3f> &normals )
{
	if( mObj->mLayout.hasDynamicNormals() ) {
		if( mObj->mDynamicStride == 0 )
			getDynamicVbo().bufferSubData( mObj->mNormalOffset, sizeof(Vec3f) * normals.size(), &(normals[0]) );
		else
			throw;
	}
	else if( mObj->mLayout.hasStaticNormals() ) {
		if( mObj->mStaticStride == 0 ) { // planar data
			getStaticVbo().bufferSubData( mObj->mNormalOffset, sizeof(Vec3f) * normals.size(), &(normals[0]) );
		}
		else
			throw;
	}
	else
		throw;
}

void VboMesh::bufferTexCoords2d( size_t unit, const std::vector<Vec2f> &texCoords )
{
	if( mObj->mLayout.hasDynamicTexCoords2d() ) {
		if( mObj->mDynamicStride == 0 )
			getDynamicVbo().bufferSubData( mObj->mTexCoordOffset[unit], sizeof(Vec2f) * texCoords.size(), &(texCoords[0]) );
		else
			throw;
	}
	else if( mObj->mLayout.hasStaticTexCoords2d() ) {
		if( mObj->mStaticStride == 0 ) { // planar data
			getStaticVbo().bufferSubData( mObj->mTexCoordOffset[unit], sizeof(Vec2f) * texCoords.size(), &(texCoords[0]) );
		}
		else
			throw;
	}
	else
		throw;
}


VboMesh::VertexIter	VboMesh::mapVertexBuffer()
{
	return VertexIter( *this );
}

void VboMesh::VertexIter::set( const VertexIter &other )
{
	mObj = other.mObj;
	mPtr = other.mPtr;
	mData = other.mData;
	mDataEnd = other.mDataEnd;
	mPositionOffset = other.mPositionOffset;
	mNormalOffset = other.mNormalOffset;
	mColorRGBOffset = other.mColorRGBOffset;
	mColorRGBAOffset = other.mColorRGBAOffset;
	mStride = other.mStride;	
	for( size_t t = 0; t <= ATTR_MAX_TEXTURE_UNIT; ++t )
		mTexCoordOffset[t] = other.mTexCoordOffset[t];
}

VboMesh::VertexIter::VertexIter( const VboMesh &mesh )
	: mObj( shared_ptr<Obj>( new Obj( mesh ) ) )
{	
	mData = mObj->mData;
	mDataEnd = mObj->mDataEnd;
	mPtr = mData;
	mStride = mesh.mObj->mDynamicStride;
	
	mPositionOffset = mesh.mObj->mPositionOffset;
	mNormalOffset = mesh.mObj->mNormalOffset;
	mColorRGBOffset = mesh.mObj->mColorRGBOffset;
	mColorRGBAOffset = mesh.mObj->mColorRGBAOffset;	
	for( size_t t = 0; t <= ATTR_MAX_TEXTURE_UNIT; ++t )
		mTexCoordOffset[t] = mesh.mObj->mTexCoordOffset[t];
	for( size_t c = 0; c < mesh.mObj->mLayout.mCustomDynamic.size(); ++c )
		mObj->mCustomOffsets.push_back( mesh.mObj->mLayout.mCustomDynamic[c].second );
}

VboMesh::VertexIter::Obj::Obj( const VboMesh &mesh )
	: mVbo( mesh.getDynamicVbo() )
{ 	
	// Buffer NULL data to tell the driver we don't care about what's in there (See NVIDIA's "Using Vertex Buffer Objects" whitepaper)
	mVbo.bind();
	//mVbo.bufferData( mesh.mObj->mDynamicStride * mesh.mObj->mNumVertices, NULL, GL_STREAM_DRAW );
	glBufferDataARB( GL_ARRAY_BUFFER, mesh.mObj->mDynamicStride * mesh.mObj->mNumVertices, NULL, GL_STREAM_DRAW );
	//mData = mVbo.map( GL_WRITE_ONLY );
	mData = reinterpret_cast<uint8_t*>( glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY ) );
	mDataEnd = mData + mesh.mObj->mDynamicStride * mesh.getNumVertices();
}

VboMesh::VertexIter::Obj::~Obj()
{ 
	mVbo.unmap();
	mVbo.unbind();
}

} } // namespace cinder::gl
