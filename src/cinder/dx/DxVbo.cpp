// The copyright in this software is being made available under the BSD License, included below. 
// This software may be subject to other third party and contributor rights, including patent rights, 
// and no such rights are granted under this license.
//
// Copyright (c) 2013, Microsoft Open Technologies, Inc. 
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// - Neither the name of Microsoft Open Technologies, Inc. nor the names of its contributors 
//   may be used to endorse or promote products derived from this software 
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include "cinder/dx/DxVbo.h"
#include "cinder/app/AppImplMswRendererDx.h"
#include <sstream>

namespace Shaders
{
	//#include "CompiledStandardVboLayoutVS.inc"

	// DirectX 9.1
	namespace Dx9_1 {
		#include "compiled_4_0_level_9_1/CompiledStandardVboLayoutVS.inc"
	}

	// DirectX 9.3
	namespace Dx9_3 {
		#include "compiled_4_0_level_9_3/CompiledStandardVboLayoutVS.inc"
	}

	// DirectX 10.0 and 10.1
	namespace Dx10 {
		#include "compiled_4_1/CompiledStandardVboLayoutVS.inc"
	}

	// DirectX 11.0 and 11.1
	namespace Dx11 {
		#include "compiled_5_0/CompiledStandardVboLayoutVS.inc"
	}
}

using namespace std;

namespace cinder { namespace dx {

enum { CUSTOM_ATTR_FLOAT, CUSTOM_ATTR_FLOAT2, CUSTOM_ATTR_FLOAT3, CUSTOM_ATTR_FLOAT4, TOTAL_CUSTOM_ATTR_TYPES };
int		VboMesh::Layout::sCustomAttrSizes[TOTAL_CUSTOM_ATTR_TYPES] = { 4, 8, 12, 16 };
GLint	VboMesh::Layout::sCustomAttrNumComponents[TOTAL_CUSTOM_ATTR_TYPES] = { 1, 2, 3, 4 };
GLenum	VboMesh::Layout::sCustomAttrTypes[TOTAL_CUSTOM_ATTR_TYPES] = { GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT };

Vbo::Obj::Obj() : mTarget((D3D11_BIND_FLAG)0), mId(NULL)
{
}

Vbo::Obj::~Obj()
{
	if(mId) mId->Release();
}

Vbo::Vbo(bool createBuffer)
{
	if(createBuffer)
		mObj = shared_ptr<Vbo::Obj>( new Obj() );
}

//void Vbo::bind()
//{
//}

//void Vbo::unbind()
//{
//}

void Vbo::bufferData( size_t size, const void *data, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlags, D3D11_CPU_ACCESS_FLAG cpuAccess )
{
	if(mObj->mId) mObj->mId->Release();
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = size;
	bufferDesc.Usage = usage;
	bufferDesc.BindFlags = bindFlags;
	bufferDesc.CPUAccessFlags = cpuAccess;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	if(data)
	{
		D3D11_SUBRESOURCE_DATA initialData;
		initialData.pSysMem = data;
		initialData.SysMemPitch = 0;
		initialData.SysMemSlicePitch = 0;
		getDxRenderer()->md3dDevice->CreateBuffer(&bufferDesc, &initialData, &mObj->mId);
	}
	else
		getDxRenderer()->md3dDevice->CreateBuffer(&bufferDesc, NULL, &mObj->mId);
}

void Vbo::bufferSubData( ptrdiff_t offset, size_t size, const void *data )
{
	//D3D11_BOX box;
	//box.left = offset;
	//box.right = offset + size;
	//box.bottom = 1;
	//box.top = 0;
	//box.front = 0;
	//box.back = 1;
	//getDxRenderer()->mDeviceContext->UpdateSubresource(mObj->mId, 0, &box, data, 0, 0);
	auto dx = getDxRenderer();
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dx->mDeviceContext->Map(mObj->mId, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy((char*)mappedResource.pData + offset, data, size);
	dx->mDeviceContext->Unmap(mObj->mId, 0);
}

uint8_t* Vbo::map( D3D11_MAP access )
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	getDxRenderer()->mDeviceContext->Map(mObj->mId, 0, access, 0, &subResource);
	return reinterpret_cast<uint8_t*>(subResource.pData);
}

void Vbo::unmap()
{
	getDxRenderer()->mDeviceContext->Unmap(mObj->mId, 0);
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

VboMesh::Obj::Obj() : mInputLayout(NULL), mUseQuads(false)
{
}

VboMesh::Obj::~Obj()
{
	if(mInputLayout) mInputLayout->Release();
}

VboMesh::VboMesh( const TriMesh &triMesh, Layout layout )
	: mObj( shared_ptr<Obj>( new Obj ) )
{
	if( layout.isDefaults() ) { // we need to start by preparing our layout
		if( triMesh.hasNormals() )
			mObj->mLayout.setStaticNormals();
		if( triMesh.hasColorsRGB() )
			mObj->mLayout.setStaticColorsRGB();
		if( triMesh.hasColorsRGBA() )
			mObj->mLayout.setStaticColorsRGBA();
		if( triMesh.hasTexCoords() )
			mObj->mLayout.setStaticTexCoords2d();
		mObj->mLayout.setStaticIndices();
		mObj->mLayout.setStaticPositions();
	}
	else
		mObj->mLayout = layout;

	mObj->mPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	mObj->mNumIndices = triMesh.getNumIndices();
	mObj->mNumVertices = triMesh.getNumVertices();

	initializeBuffers( false );
			
	// upload the indices
	if(!triMesh.getIndices().empty())
		getIndexVbo().bufferData( sizeof(uint32_t) * triMesh.getNumIndices(), &(triMesh.getIndices()[0]), /*(mObj->mLayout.hasStaticIndices()) ? D3D11_USAGE_DEFAULT : */D3D11_USAGE_DYNAMIC, D3D11_BIND_INDEX_BUFFER, /*(mObj->mLayout.hasStaticIndices()) ? (D3D11_CPU_ACCESS_FLAG)0 : */D3D11_CPU_ACCESS_WRITE );
	
	// upload the verts
	for( int buffer = STATIC_BUFFER; buffer <= DYNAMIC_BUFFER; ++buffer ) {
		if( ! mObj->mBuffers[buffer] || !mObj->mBuffers[buffer].getId())
			continue;
		
		uint8_t *ptr = mObj->mBuffers[buffer].map( D3D11_MAP_WRITE_DISCARD );
		
		bool copyPosition = ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticPositions() : mObj->mLayout.hasDynamicPositions();
		bool copyNormal = ( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticNormals() : mObj->mLayout.hasDynamicNormals() ) && triMesh.hasNormals();
		bool copyColorRGB = ( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticColorsRGB() : mObj->mLayout.hasDynamicColorsRGB() ) && triMesh.hasColorsRGB();
		bool copyColorRGBA = ( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticColorsRGBA() : mObj->mLayout.hasDynamicColorsRGBA() ) && triMesh.hasColorsRGBA();
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
			if( copyColorRGB ) {
				*(reinterpret_cast<Color*>(ptr)) = triMesh.getColorsRGB()[v];
				ptr += sizeof( Color );
			}
			if( copyColorRGBA ) {
				*(reinterpret_cast<ColorA*>(ptr)) = triMesh.getColorsRGBA()[v];
				ptr += sizeof( ColorA );
			}
			if( copyTexCoord2D ) {
				*(reinterpret_cast<Vec2f*>(ptr)) = triMesh.getTexCoords()[v];
				ptr += sizeof( Vec2f );
			}
		}
		
		mObj->mBuffers[buffer].unmap();
	}

	//unbindBuffers();	
}

VboMesh::VboMesh( const TriMesh2d &triMesh, Layout layout )
	: mObj( shared_ptr<Obj>( new Obj ) )
{
	if( layout.isDefaults() ) { // we need to start by preparing our layout
		if( triMesh.hasColorsRgb() )
			mObj->mLayout.setStaticColorsRGB();
		if( triMesh.hasColorsRgba() )
			mObj->mLayout.setStaticColorsRGBA();
		if( triMesh.hasTexCoords() )
			mObj->mLayout.setStaticTexCoords2d();
		mObj->mLayout.setStaticIndices();
		mObj->mLayout.setStaticPositions();
	}
	else
		mObj->mLayout = layout;

	mObj->mPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	mObj->mNumIndices = triMesh.getNumIndices();
	mObj->mNumVertices = triMesh.getNumVertices();

	initializeBuffers( false );
			
	// upload the indices
	if(!triMesh.getIndices().empty())
		getIndexVbo().bufferData( sizeof(uint32_t) * triMesh.getNumIndices(), &(triMesh.getIndices()[0]), /*(mObj->mLayout.hasStaticIndices()) ? D3D11_USAGE_DEFAULT : */D3D11_USAGE_DYNAMIC, D3D11_BIND_INDEX_BUFFER, /*(mObj->mLayout.hasStaticIndices()) ? (D3D11_CPU_ACCESS_FLAG)0 : */D3D11_CPU_ACCESS_WRITE );
	
	// upload the verts
	for( int buffer = STATIC_BUFFER; buffer <= DYNAMIC_BUFFER; ++buffer ) {
		if( ! mObj->mBuffers[buffer] || !mObj->mBuffers[buffer].getId())
			continue;
		
		uint8_t *ptr = mObj->mBuffers[buffer].map( D3D11_MAP_WRITE_DISCARD );
		
		bool copyPosition = ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticPositions() : mObj->mLayout.hasDynamicPositions();
		bool copyColorRGB = ( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticColorsRGB() : mObj->mLayout.hasDynamicColorsRGB() ) && triMesh.hasColorsRgb();
		bool copyColorRGBA = ( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticColorsRGBA() : mObj->mLayout.hasDynamicColorsRGBA() ) && triMesh.hasColorsRgba();
		bool copyTexCoord2D = ( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticTexCoords2d() : mObj->mLayout.hasDynamicTexCoords2d() ) && triMesh.hasTexCoords();
		
		for( size_t v = 0; v < mObj->mNumVertices; ++v ) {
			if( copyPosition ) {
				const Vec2f &p = triMesh.getVertices()[v];
				*(reinterpret_cast<Vec3f*>(ptr)) = Vec3f( p.x, p.y, 0 );
				ptr += sizeof( Vec3f );
			}
			if( copyColorRGB ) {
				*(reinterpret_cast<Color*>(ptr)) = triMesh.getColorsRGB()[v];
				ptr += sizeof( Color );
			}
			if( copyColorRGBA ) {
				*(reinterpret_cast<ColorA*>(ptr)) = triMesh.getColorsRGBA()[v];
				ptr += sizeof( ColorA );
			}
			if( copyTexCoord2D ) {
				*(reinterpret_cast<Vec2f*>(ptr)) = triMesh.getTexCoords()[v];
				ptr += sizeof( Vec2f );
			}
		}
		
		mObj->mBuffers[buffer].unmap();
	}

	//unbindBuffers();	
}

VboMesh::VboMesh( size_t numVertices, size_t numIndices, Layout layout, D3D11_PRIMITIVE_TOPOLOGY primitiveType )
	: mObj( shared_ptr<Obj>( new Obj ) )
{
	mObj->mLayout = layout;
	mObj->mPrimitiveType = primitiveType;
	mObj->mNumIndices = numIndices;
	mObj->mNumVertices = numVertices;

	initializeBuffers( true );
	
	// allocate buffer for indices
	if( mObj->mLayout.hasIndices() )
		mObj->mBuffers[INDEX_BUFFER].bufferData( sizeof(uint32_t) * mObj->mNumIndices, NULL, /*(mObj->mLayout.hasStaticIndices()) ? D3D11_USAGE_DEFAULT : */D3D11_USAGE_DYNAMIC, D3D11_BIND_INDEX_BUFFER, /*(mObj->mLayout.hasStaticIndices()) ? (D3D11_CPU_ACCESS_FLAG)0 : */D3D11_CPU_ACCESS_WRITE );
	
	//unbindBuffers();	
}

VboMesh::VboMesh( size_t numVertices, size_t numIndices, Layout layout, bool useQuads )
	: mObj( shared_ptr<Obj>( new Obj ) )
{
	mObj->mLayout = layout;
	mObj->mPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	mObj->mNumIndices = numIndices;
	mObj->mNumVertices = numVertices;
	if(useQuads && !layout.hasIndices())
		mObj->mNumVertices = numVertices / 4 * 6;
	mObj->mUseQuads = true;

	initializeBuffers( true );
	
	// allocate buffer for indices
	if( mObj->mLayout.hasIndices() )
	{
		int indexBufferSize;
		if(useQuads)
			indexBufferSize = sizeof(uint32_t) * mObj->mNumIndices / 4 * 6;
		else
			indexBufferSize = sizeof(uint32_t) * mObj->mNumIndices;
		mObj->mBuffers[INDEX_BUFFER].bufferData( indexBufferSize, NULL, /*(mObj->mLayout.hasStaticIndices()) ? D3D11_USAGE_DEFAULT : */D3D11_USAGE_DYNAMIC, D3D11_BIND_INDEX_BUFFER, /*(mObj->mLayout.hasStaticIndices()) ? (D3D11_CPU_ACCESS_FLAG)0 : */D3D11_CPU_ACCESS_WRITE );
	}
	
	//unbindBuffers();	
}

VboMesh::VboMesh( size_t numVertices, size_t numIndices, Layout layout, D3D11_PRIMITIVE_TOPOLOGY primitiveType, Vbo *indexBuffer, Vbo *staticBuffer, Vbo *dynamicBuffer )
	: mObj( shared_ptr<Obj>( new Obj ) )
{
	mObj->mLayout = layout;
	mObj->mPrimitiveType = primitiveType;
	mObj->mNumIndices = numIndices;
	mObj->mNumVertices = numVertices;

	if( indexBuffer ) {
		mObj->mBuffers[INDEX_BUFFER] = *indexBuffer;
		if( (indexBuffer->getTarget() & D3D11_BIND_INDEX_BUFFER) != D3D11_BIND_INDEX_BUFFER )
			throw VboInvalidTargetExc();
	}
	if( staticBuffer ) {
		mObj->mBuffers[STATIC_BUFFER] = *staticBuffer;
		if( (staticBuffer->getTarget() & D3D11_BIND_VERTEX_BUFFER) != D3D11_BIND_VERTEX_BUFFER )
			throw VboInvalidTargetExc();
	}
	if( dynamicBuffer ) {
		mObj->mBuffers[DYNAMIC_BUFFER] = *dynamicBuffer;
		if( (dynamicBuffer->getTarget() & D3D11_BIND_VERTEX_BUFFER) != D3D11_BIND_VERTEX_BUFFER )
			throw VboInvalidTargetExc();		
	}
	
	initializeBuffers( true );
	//unbindBuffers();
}

// If any buffers are not NULL they will be ignored
void VboMesh::initializeBuffers( bool staticDataPlanar )
{
	bool hasStaticBuffer = mObj->mLayout.hasStaticPositions() || mObj->mLayout.hasStaticNormals() || mObj->mLayout.hasStaticColorsRGB() || mObj->mLayout.hasStaticColorsRGBA() || mObj->mLayout.hasStaticTexCoords() || ( ! mObj->mLayout.mCustomStatic.empty() );
	bool hasDynamicBuffer = mObj->mLayout.hasDynamicPositions() || mObj->mLayout.hasDynamicNormals() || mObj->mLayout.hasDynamicColorsRGB() || mObj->mLayout.hasDynamicColorsRGBA() || mObj->mLayout.hasDynamicTexCoords() || ( ! mObj->mLayout.mCustomDynamic.empty() );

	if( ( mObj->mLayout.hasStaticIndices() || mObj->mLayout.hasDynamicIndices() ) && ( ! mObj->mBuffers[INDEX_BUFFER] ) )
		mObj->mBuffers[INDEX_BUFFER] = Vbo(true);
		
	D3D11_INPUT_ELEMENT_DESC ieDesc[ATTR_TOTAL];

	ieDesc[0].SemanticName = "POSITION";
	ieDesc[0].SemanticIndex = 0;
	ieDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	ieDesc[0].InputSlot = 0;
	ieDesc[0].AlignedByteOffset = 0;
	ieDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	ieDesc[0].InstanceDataStepRate = 0;

	ieDesc[1].SemanticName = "NORMAL";
	ieDesc[1].SemanticIndex = 0;
	ieDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	ieDesc[1].InputSlot = 1;
	ieDesc[1].AlignedByteOffset = 0;
	ieDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	ieDesc[1].InstanceDataStepRate = 0;

	ieDesc[2].SemanticName = "COLOR";
	ieDesc[2].SemanticIndex = 0;
	ieDesc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	ieDesc[2].InputSlot = 2;
	ieDesc[2].AlignedByteOffset = 0;
	ieDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	ieDesc[2].InstanceDataStepRate = 0;

	ieDesc[3].SemanticName = "TEXCOORD";
	ieDesc[3].SemanticIndex = 0;
	ieDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	ieDesc[3].InputSlot = 3;
	ieDesc[3].AlignedByteOffset = 0;
	ieDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	ieDesc[3].InstanceDataStepRate = 0;

	int elementCount = 0;
	if( hasStaticBuffer && staticDataPlanar ) { // Planar static buffer
		size_t offset = 0;
		bool doSetup = false;
	
		if( ! mObj->mBuffers[STATIC_BUFFER] ) {
			mObj->mBuffers[STATIC_BUFFER] = Vbo(true);
			doSetup = true;
		}


		if( mObj->mLayout.hasStaticPositions() ) {
			++elementCount;
			mObj->mPositionOffset = offset;
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
				offset += sizeof(GLfloat) * 3 * mObj->mNumVertices / 4 * 6;
			else
				offset += sizeof(GLfloat) * 3 * mObj->mNumVertices;
		}
		
		if( mObj->mLayout.hasStaticNormals() ) {
			++elementCount;
			mObj->mNormalOffset = offset;
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
				offset += sizeof(GLfloat) * 3 * mObj->mNumVertices / 4 * 6;
			else
				offset += sizeof(GLfloat) * 3 * mObj->mNumVertices;
		}

		if( mObj->mLayout.hasStaticColorsRGB() ) {
			if(!mObj->mLayout.hasStaticColorsRGBA()) {
				ieDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				++elementCount;
			}
			
			mObj->mColorRGBOffset = offset;
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
				offset += sizeof(GLfloat) * 3 * mObj->mNumVertices / 4 * 6;
			else
				offset += sizeof(GLfloat) * 3 * mObj->mNumVertices;
		}

		if( mObj->mLayout.hasStaticColorsRGBA() ) {
			ieDesc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			++elementCount;
			mObj->mColorRGBAOffset = offset;
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
				offset += sizeof(GLfloat) * 4 * mObj->mNumVertices / 4 * 6;
			else
				offset += sizeof(GLfloat) * 4 * mObj->mNumVertices;
		}
		
		for( size_t t = 0; t <= ATTR_MAX_TEXTURE_UNIT; ++t ) {
			if( mObj->mLayout.hasStaticTexCoords2d( t ) ) {
				ieDesc[3].SemanticIndex = t;
				ieDesc[3].InputSlot = 3 + t;
				ieDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
				++elementCount;

				mObj->mTexCoordOffset[t] = offset;
				if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
					offset += sizeof(GLfloat) * 2 * mObj->mNumVertices / 4 * 6;
				else
					offset += sizeof(GLfloat) * 2 * mObj->mNumVertices;
			}
			else if( mObj->mLayout.hasStaticTexCoords3d( t ) ) {
				ieDesc[3].SemanticIndex = t;
				ieDesc[3].InputSlot = 3 + t;
				ieDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				++elementCount;
				mObj->mTexCoordOffset[t] = offset;
				if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
					offset += sizeof(GLfloat) * 3 * mObj->mNumVertices / 4 * 6;
				else
					offset += sizeof(GLfloat) * 3 * mObj->mNumVertices;
			}
		}

		for( size_t c = 0; c < mObj->mLayout.mCustomStatic.size(); ++c ) {
			mObj->mLayout.mCustomStatic[c].second = offset;
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
				offset += VboMesh::Layout::sCustomAttrSizes[mObj->mLayout.mCustomStatic[c].first] * mObj->mNumVertices / 4 * 6;
			else
				offset += VboMesh::Layout::sCustomAttrSizes[mObj->mLayout.mCustomStatic[c].first] * mObj->mNumVertices;
		}
		
		mObj->mStaticStride = 0;
		
		// setup the buffer to be the summed size
		if( doSetup )
			mObj->mBuffers[STATIC_BUFFER].bufferData( offset, NULL, /*D3D11_USAGE_DEFAULT*/D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, /*(D3D11_CPU_ACCESS_FLAG)0*/D3D11_CPU_ACCESS_WRITE );
	}
	else if( hasStaticBuffer && ( ! staticDataPlanar ) ) { // Interleaved static buffer
		size_t offset = 0;

		if( ! mObj->mBuffers[STATIC_BUFFER] )
			mObj->mBuffers[STATIC_BUFFER] = Vbo(true);

		if( mObj->mLayout.hasStaticPositions() ) {
			ieDesc[0].AlignedByteOffset = offset;
			++elementCount;

			mObj->mPositionOffset = offset;
			offset += sizeof(GLfloat) * 3;
		}
		
		if( mObj->mLayout.hasStaticNormals() ) {
			ieDesc[1].AlignedByteOffset = offset;
			++elementCount;

			mObj->mNormalOffset = offset;
			offset += sizeof(GLfloat) * 3;
		}

		if( mObj->mLayout.hasStaticColorsRGB() ) {
			if(!mObj->mLayout.hasStaticColorsRGBA())
			{
				ieDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				ieDesc[2].AlignedByteOffset = offset;
				++elementCount;
			}

			mObj->mColorRGBOffset = offset;
			offset += sizeof(GLfloat) * 3;
		}
		else if( mObj->mLayout.hasStaticColorsRGBA() ) {
			ieDesc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			ieDesc[2].AlignedByteOffset = offset;
			++elementCount;

			mObj->mColorRGBAOffset = offset;
			offset += sizeof(GLfloat) * 4;
		}
		
		for( size_t t = 0; t <= ATTR_MAX_TEXTURE_UNIT; ++t ) {
			if( mObj->mLayout.hasStaticTexCoords2d( t ) ) {
				ieDesc[3].SemanticIndex = t;
				ieDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
				ieDesc[3].InputSlot = 3 + t;
				ieDesc[3].AlignedByteOffset = offset;
				++elementCount;

				mObj->mTexCoordOffset[t] = offset;
				offset += sizeof(GLfloat) * 2;
			}
			else if( mObj->mLayout.hasStaticTexCoords3d( t ) ) {
				ieDesc[elementCount].SemanticIndex = t;
				ieDesc[elementCount].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				ieDesc[elementCount].InputSlot = 3 + t;
				ieDesc[elementCount].AlignedByteOffset = offset;
				++elementCount;

				offset += sizeof(GLfloat) * 3;
			}
		}

		for( size_t c = 0; c < mObj->mLayout.mCustomStatic.size(); ++c ) {
			mObj->mLayout.mCustomStatic[c].second = offset;
			offset += VboMesh::Layout::sCustomAttrSizes[mObj->mLayout.mCustomStatic[c].first];
		}
		
		mObj->mStaticStride = offset;
		
		// setup the buffer to be the summed size
		int bufferSize;
		if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
			bufferSize = mObj->mStaticStride * mObj->mNumVertices / 4 * 6;
		else
			bufferSize = mObj->mStaticStride * mObj->mNumVertices;
		mObj->mBuffers[STATIC_BUFFER].bufferData( bufferSize, NULL, /*D3D11_USAGE_DEFAULT*/D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, /*(D3D11_CPU_ACCESS_FLAG)0*/D3D11_CPU_ACCESS_WRITE );
	}
	else {
		mObj->mStaticStride = 0;
	}
	
	if( hasDynamicBuffer ) {
		size_t offset = 0;

		if( ! mObj->mBuffers[DYNAMIC_BUFFER] )
			mObj->mBuffers[DYNAMIC_BUFFER] = Vbo(true);
		
		if( mObj->mLayout.hasDynamicPositions() ) {
			ieDesc[0].AlignedByteOffset = offset;
			++elementCount;

			mObj->mPositionOffset = offset;
			offset += sizeof(GLfloat) * 3;
		}
		
		if( mObj->mLayout.hasDynamicNormals() ) {
			ieDesc[1].AlignedByteOffset = offset;
			++elementCount;

			mObj->mNormalOffset = offset;
			offset += sizeof(GLfloat) * 3;
		}

		if( mObj->mLayout.hasDynamicColorsRGB() ) {
			if(!mObj->mLayout.hasDynamicColorsRGBA())
			{
				ieDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				ieDesc[2].AlignedByteOffset = offset;
				++elementCount;
			}

			mObj->mColorRGBOffset = offset;
			offset += sizeof(GLfloat) * 3;
		}
		else if( mObj->mLayout.hasDynamicColorsRGBA() ) {
			ieDesc[2].AlignedByteOffset = offset;
			++elementCount;

			mObj->mColorRGBAOffset = offset;
			offset += sizeof(GLfloat) * 4;
		}
		
		for( size_t t = 0; t <= ATTR_MAX_TEXTURE_UNIT; ++t ) {
			if( mObj->mLayout.hasDynamicTexCoords2d( t ) ) {
				ieDesc[3].SemanticIndex = t;
				ieDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
				ieDesc[3].InputSlot = 3 + t;
				ieDesc[3].AlignedByteOffset = offset;
				++elementCount;

				mObj->mTexCoordOffset[t] = offset;
				offset += sizeof(GLfloat) * 2;
			}
			else if( mObj->mLayout.hasDynamicTexCoords3d( t ) ) {
				ieDesc[3].SemanticIndex = t;
				ieDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				ieDesc[3].InputSlot = 3 + t;
				ieDesc[3].AlignedByteOffset = offset;
				++elementCount;

				mObj->mTexCoordOffset[t] = offset;
				offset += sizeof(GLfloat) * 3;
			}
		}

		for( size_t c = 0; c < mObj->mLayout.mCustomDynamic.size(); ++c ) {
			mObj->mLayout.mCustomDynamic[c].second = offset;
			offset += VboMesh::Layout::sCustomAttrSizes[mObj->mLayout.mCustomDynamic[c].first];
		}

		mObj->mDynamicStride = offset;
		
		// setup the buffer to be the summed sizeint bufferSize;
		int bufferSize;
		if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
			bufferSize = mObj->mDynamicStride * mObj->mNumVertices / 4 * 6;
		else
			bufferSize = mObj->mDynamicStride * mObj->mNumVertices;
		mObj->mBuffers[DYNAMIC_BUFFER].bufferData( bufferSize, NULL, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE );
	}
	else {
		mObj->mDynamicStride = 0;
	}

	//HRESULT hr = getDxRenderer()->md3dDevice->CreateInputLayout(ieDesc, std::max(elementCount, 4), Shaders::StandardVboLayoutVS, sizeof(Shaders::StandardVboLayoutVS), &mObj->mInputLayout);
	HRESULT hr = E_FAIL;
	if( D3D_FEATURE_LEVEL_9_1 == getDxRenderer()->mFeatureLevel ) {
		hr = getDxRenderer()->md3dDevice->CreateInputLayout(ieDesc, std::max(elementCount, 4), Shaders::Dx9_1::StandardVboLayoutVS, sizeof(Shaders::Dx9_1::StandardVboLayoutVS), &mObj->mInputLayout);
	}
	else if( D3D_FEATURE_LEVEL_9_3 == getDxRenderer()->mFeatureLevel ) {
		hr = getDxRenderer()->md3dDevice->CreateInputLayout(ieDesc, std::max(elementCount, 4), Shaders::Dx9_3::StandardVboLayoutVS, sizeof(Shaders::Dx9_3::StandardVboLayoutVS), &mObj->mInputLayout);
	}
	else if( D3D_FEATURE_LEVEL_10_1 == getDxRenderer()->mFeatureLevel ) {
		hr = getDxRenderer()->md3dDevice->CreateInputLayout(ieDesc, std::max(elementCount, 4), Shaders::Dx10::StandardVboLayoutVS, sizeof(Shaders::Dx10::StandardVboLayoutVS), &mObj->mInputLayout);
	}
	else if( D3D_FEATURE_LEVEL_11_0 == getDxRenderer()->mFeatureLevel || D3D_FEATURE_LEVEL_11_1 == getDxRenderer()->mFeatureLevel ) {
		hr = getDxRenderer()->md3dDevice->CreateInputLayout(ieDesc, std::max(elementCount, 4), Shaders::Dx11::StandardVboLayoutVS, sizeof(Shaders::Dx11::StandardVboLayoutVS), &mObj->mInputLayout);
	}		
		
	if(hr != S_OK) {
		__debugbreak();
	}

	// initialize all the custom attribute locations
	//if( ! mObj->mLayout.mCustomStatic.empty() )
	//	mObj->mCustomStaticLocations = vector<GLint>( mObj->mLayout.mCustomStatic.size(), -1 );
	//if( ! mObj->mLayout.mCustomDynamic.empty() )
	//	mObj->mCustomDynamicLocations = vector<GLint>( mObj->mLayout.mCustomDynamic.size(), -1 );
}

//void VboMesh::enableClientStates() const
//{
	//if( mObj->mLayout.hasPositions() )
	//	glEnableClientState( GL_VERTEX_ARRAY );
	//else
	//	glDisableClientState( GL_VERTEX_ARRAY );
	//if( mObj->mLayout.hasNormals() )
	//	glEnableClientState( GL_NORMAL_ARRAY );
	//else
	//	glDisableClientState( GL_NORMAL_ARRAY );
	//if( mObj->mLayout.hasColorsRGB() || mObj->mLayout.hasColorsRGBA() )
	//	glEnableClientState( GL_COLOR_ARRAY );
	//else
	//	glDisableClientState( GL_COLOR_ARRAY );
	//	
	//for( size_t t = 0; t <= ATTR_MAX_TEXTURE_UNIT; ++t ) {
	//	if( mObj->mLayout.hasTexCoords( t ) ) {
	//		glClientActiveTexture( GL_TEXTURE0 + t );
	//		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	//	}
	//}	
	//
	//for( size_t a = 0; a < mObj->mCustomStaticLocations.size(); ++a ) {
	//	if( mObj->mCustomStaticLocations[a] < 0 )
	//		throw;
	//	glEnableVertexAttribArray( mObj->mCustomStaticLocations[a] );
	//}

	//for( size_t a = 0; a < mObj->mCustomDynamicLocations.size(); ++a ) {
	//	if( mObj->mCustomDynamicLocations[a] < 0 )
	//		throw;
	//	glEnableVertexAttribArray( mObj->mCustomDynamicLocations[a] );
	//}
//}

//void VboMesh::disableClientStates() const
//{
//	glDisableClientState( GL_VERTEX_ARRAY );
//	glDisableClientState( GL_NORMAL_ARRAY );
//	glDisableClientState( GL_COLOR_ARRAY );
//	for( size_t t = 0; t <= ATTR_MAX_TEXTURE_UNIT; ++t ) {
//		if( mObj->mLayout.hasTexCoords( t ) ) {
//			glClientActiveTexture( GL_TEXTURE0 + t );
//			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
//		}
//	}	
//	
//	for( size_t a = 0; a < mObj->mCustomStaticLocations.size(); ++a ) {
//		if( mObj->mCustomStaticLocations[a] < 0 )
//			throw;
//		glDisableVertexAttribArray( mObj->mCustomStaticLocations[a] );
//	}
//
//	for( size_t a = 0; a < mObj->mCustomDynamicLocations.size(); ++a ) {
//		if( mObj->mCustomDynamicLocations[a] < 0 )
//			throw;
//		glDisableVertexAttribArray( mObj->mCustomDynamicLocations[a] );
//	}
//}

void VboMesh::bindAllData() const
{
	if( mObj->mLayout.hasIndices() ) {
		//mObj->mBuffers[INDEX_BUFFER].bind();
		bindIndexBuffer();
	}
	auto dx = getDxRenderer();
	int elementCount = 0;
	for( int buffer = STATIC_BUFFER; buffer <= DYNAMIC_BUFFER; ++buffer ) {
		if( ! mObj->mBuffers[buffer] ) continue;
		
		//mObj->mBuffers[buffer].bind();
		UINT stride = ( buffer == STATIC_BUFFER ) ? mObj->mStaticStride : mObj->mDynamicStride;
		UINT offset = mObj->mPositionOffset;
		ID3D11Buffer *actualBuffer = mObj->mBuffers[buffer].getId();

		if( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticPositions() : mObj->mLayout.hasDynamicPositions() ) {
			UINT oldStride = stride;
			stride = std::max<UINT>(stride, sizeof(float) * 3);
			dx->mDeviceContext->IASetVertexBuffers(0, 1, &actualBuffer, &stride, &offset);
			stride = oldStride;
			//glVertexPointer( 3, GL_FLOAT, stride, (const GLvoid*)mObj->mPositionOffset );
		}
		
		offset = (buffer == STATIC_BUFFER) ? mObj->mNormalOffset : 0;
		if( ( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticNormals() : mObj->mLayout.hasDynamicNormals() ) ) {
			UINT oldStride = stride;
			stride = std::max<UINT>(stride, sizeof(float) * 3);
			dx->mDeviceContext->IASetVertexBuffers(1, 1, &actualBuffer, &stride, &offset);
			stride = oldStride;
			//glNormalPointer( GL_FLOAT, stride, ( const GLvoid *)mObj->mNormalOffset );
		}

		offset = (mObj->mLayout.hasStaticColorsRGB()) ? mObj->mColorRGBOffset : mObj->mColorRGBAOffset;
		offset = (buffer == STATIC_BUFFER) ? offset : 0;
		if( ( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticColorsRGB() : mObj->mLayout.hasDynamicColorsRGB() ) ) {
			UINT oldStride = stride;
			stride = std::max<UINT>(stride, sizeof(float) * 3);
			dx->mDeviceContext->IASetVertexBuffers(2, 1, &actualBuffer, &stride, &offset);
			stride = oldStride;
			//glColorPointer( 3, GL_FLOAT, stride, ( const GLvoid *)mObj->mColorRGBOffset );
		}
		else if( ( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticColorsRGBA() : mObj->mLayout.hasDynamicColorsRGBA() ) ) {
			UINT oldStride = stride;
			stride = std::max<UINT>(stride, sizeof(float) * 4);
			dx->mDeviceContext->IASetVertexBuffers(2, 1, &actualBuffer, &stride, &offset);
			stride = oldStride;
			//glColorPointer( 4, GL_FLOAT, stride, ( const GLvoid *)mObj->mColorRGBAOffset );
		}

		for( size_t t = 0; t <= ATTR_MAX_TEXTURE_UNIT; ++t ) {
			offset = (buffer == STATIC_BUFFER) ? mObj->mTexCoordOffset[t] : 0;
			
			if( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticTexCoords2d( t ) : mObj->mLayout.hasDynamicTexCoords2d( t ) ) {
				UINT oldStride = stride;
				stride = std::max<UINT>(stride, sizeof(float) * 2);
				dx->mDeviceContext->IASetVertexBuffers(3 + elementCount++, 1, &actualBuffer, &stride, &offset);
				stride = oldStride;
				//glClientActiveTexture( GL_TEXTURE0 + t );
				//glTexCoordPointer( 2, GL_FLOAT, stride, (const GLvoid *)mObj->mTexCoordOffset[t] );
			}
			else if( ( buffer == STATIC_BUFFER ) ? mObj->mLayout.hasStaticTexCoords3d( t ) : mObj->mLayout.hasDynamicTexCoords3d( t ) ) {
				UINT oldStride = stride;
				stride = std::max<UINT>(stride, sizeof(float) * 3);
				dx->mDeviceContext->IASetVertexBuffers(3 + elementCount++, 1, &actualBuffer, &stride, &offset);
				stride = oldStride;
				//glClientActiveTexture( GL_TEXTURE0 + t );
				//glTexCoordPointer( 3, GL_FLOAT, stride, (const GLvoid *)mObj->mTexCoordOffset[t] );
			}
		}	
	}
	dx->mDeviceContext->IASetInputLayout(mObj->mInputLayout);
//
//	for( int buffer = STATIC_BUFFER; buffer <= DYNAMIC_BUFFER; ++buffer ) {
//		if( ! mObj->mBuffers[buffer] ) continue;
//
//		const vector<pair<VboMesh::Layout::CustomAttr,size_t> > &attributes( ( buffer == DYNAMIC_BUFFER ) ? mObj->mLayout.mCustomDynamic : mObj->mLayout.mCustomStatic );
//		const vector<GLint> &locations( ( buffer == DYNAMIC_BUFFER ) ? mObj->mCustomDynamicLocations : mObj->mCustomStaticLocations );
//		GLsizei stride = ( ( buffer == DYNAMIC_BUFFER ) ? mObj->mDynamicStride : mObj->mStaticStride );
//	
//		if( attributes.empty() )
//			continue;
//		
//		mObj->mBuffers[buffer].bind();
//		
//		for( size_t a = 0; a < attributes.size(); ++a ) {
//			const GLvoid *offset = reinterpret_cast<const GLvoid*>( attributes[a].second );
//			glVertexAttribPointer( locations[a], Layout::sCustomAttrNumComponents[attributes[a].first], Layout::sCustomAttrTypes[attributes[a].first], GL_FALSE, stride, offset );
//		}	
//	}
}

void VboMesh::bindIndexBuffer() const
{
	getDxRenderer()->mDeviceContext->IASetIndexBuffer(mObj->mBuffers[INDEX_BUFFER].getId(), DXGI_FORMAT_R32_UINT, 0);
	//mObj->mBuffers[INDEX_BUFFER].bind();
}

//void VboMesh::unbindBuffers()
//{
//	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
//	glBindBuffer( GL_ARRAY_BUFFER, 0 );
//}

void VboMesh::bufferIndices( const std::vector<uint32_t> &indices )
{
	if(mObj->mUseQuads)
	{
		size_t quadIndexCount = indices.size() / 4 * 6;
		uint32_t *quadIndices = new uint32_t[quadIndexCount];
		for(unsigned i = 0, j = 0; i < indices.size(); i += 4, j += 6)
		{
			quadIndices[j+0] = indices[i+0];
			quadIndices[j+1] = indices[i+1];
			quadIndices[j+2] = indices[i+2];
			quadIndices[j+3] = indices[i+0];
			quadIndices[j+4] = indices[i+2];
			quadIndices[j+5] = indices[i+3];
		}
		mObj->mBuffers[INDEX_BUFFER].bufferData( sizeof(uint32_t) * quadIndexCount, quadIndices, /*(mObj->mLayout.hasStaticIndices()) ? D3D11_USAGE_DEFAULT : */D3D11_USAGE_DYNAMIC, D3D11_BIND_INDEX_BUFFER, /*(mObj->mLayout.hasStaticIndices()) ? (D3D11_CPU_ACCESS_FLAG)0 : */D3D11_CPU_ACCESS_WRITE );
		delete [] quadIndices;
	}
	else
		mObj->mBuffers[INDEX_BUFFER].bufferData( sizeof(uint32_t) * indices.size(), &(indices[0]), /*(mObj->mLayout.hasStaticIndices()) ? D3D11_USAGE_DEFAULT : */D3D11_USAGE_DYNAMIC, D3D11_BIND_INDEX_BUFFER, /*(mObj->mLayout.hasStaticIndices()) ? (D3D11_CPU_ACCESS_FLAG)0 : */D3D11_CPU_ACCESS_WRITE );
}

void VboMesh::bufferPositions( const std::vector<Vec3f> &positions )
{
	bufferPositions( &positions[0], positions.size() );
}

void VboMesh::bufferPositions( const Vec3f *positions, size_t count )
{
	if( mObj->mLayout.hasDynamicPositions() ) {
		if( mObj->mDynamicStride == 0 ) {
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
			{
				Vbo &vbo = getDynamicVbo();
				for(size_t i = 0, j = 0; i < count; i += 4, j += 6)
				{
					vbo.bufferSubData(mObj->mPositionOffset + (j+0) * sizeof(Vec3f), sizeof(Vec3f), positions+i+0);
					vbo.bufferSubData(mObj->mPositionOffset + (j+1) * sizeof(Vec3f), sizeof(Vec3f), positions+i+1);
					vbo.bufferSubData(mObj->mPositionOffset + (j+2) * sizeof(Vec3f), sizeof(Vec3f), positions+i+2);
					vbo.bufferSubData(mObj->mPositionOffset + (j+3) * sizeof(Vec3f), sizeof(Vec3f), positions+i+0);
					vbo.bufferSubData(mObj->mPositionOffset + (j+4) * sizeof(Vec3f), sizeof(Vec3f), positions+i+2);
					vbo.bufferSubData(mObj->mPositionOffset + (j+5) * sizeof(Vec3f), sizeof(Vec3f), positions+i+3);
				}
			}
			else
				getDynamicVbo().bufferSubData( mObj->mPositionOffset, sizeof(Vec3f) * count, positions );
		}
		else
			throw;
	}
	else if( mObj->mLayout.hasStaticPositions() ) {
		if( mObj->mStaticStride == 0 ) { // planar data
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
			{
				Vbo &vbo = getStaticVbo();
				for(size_t i = 0, j = 0; i < count; i += 4, j += 6)
				{
					vbo.bufferSubData(mObj->mPositionOffset + (j+0) * sizeof(Vec3f), sizeof(Vec3f), positions+i+0);
					vbo.bufferSubData(mObj->mPositionOffset + (j+1) * sizeof(Vec3f), sizeof(Vec3f), positions+i+1);
					vbo.bufferSubData(mObj->mPositionOffset + (j+2) * sizeof(Vec3f), sizeof(Vec3f), positions+i+2);
					vbo.bufferSubData(mObj->mPositionOffset + (j+3) * sizeof(Vec3f), sizeof(Vec3f), positions+i+0);
					vbo.bufferSubData(mObj->mPositionOffset + (j+4) * sizeof(Vec3f), sizeof(Vec3f), positions+i+2);
					vbo.bufferSubData(mObj->mPositionOffset + (j+5) * sizeof(Vec3f), sizeof(Vec3f), positions+i+3);
				}
			}
			else
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
		if( mObj->mDynamicStride == 0 ) {
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
			{
				Vbo &vbo = getDynamicVbo();
				for(size_t i = 0, j = 0; i < normals.size(); i += 4, j += 6)
				{
					vbo.bufferSubData(mObj->mNormalOffset + (j+0) * sizeof(Vec3f), sizeof(Vec3f), &normals[i+0]);
					vbo.bufferSubData(mObj->mNormalOffset + (j+1) * sizeof(Vec3f), sizeof(Vec3f), &normals[i+1]);
					vbo.bufferSubData(mObj->mNormalOffset + (j+2) * sizeof(Vec3f), sizeof(Vec3f), &normals[i+2]);
					vbo.bufferSubData(mObj->mNormalOffset + (j+3) * sizeof(Vec3f), sizeof(Vec3f), &normals[i+0]);
					vbo.bufferSubData(mObj->mNormalOffset + (j+4) * sizeof(Vec3f), sizeof(Vec3f), &normals[i+2]);
					vbo.bufferSubData(mObj->mNormalOffset + (j+5) * sizeof(Vec3f), sizeof(Vec3f), &normals[i+3]);
				}
			}
			else
				getDynamicVbo().bufferSubData( mObj->mNormalOffset, sizeof(Vec3f) * normals.size(), &(normals[0]) );
		}
		else
			throw;
	}
	else if( mObj->mLayout.hasStaticNormals() ) {
		if( mObj->mStaticStride == 0 ) { // planar data
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
			{
				Vbo &vbo = getStaticVbo();
				for(size_t i = 0, j = 0; i < normals.size(); i += 4, j += 6)
				{
					vbo.bufferSubData(mObj->mNormalOffset + (j+0) * sizeof(Vec3f), sizeof(Vec3f), &normals[i+0]);
					vbo.bufferSubData(mObj->mNormalOffset + (j+1) * sizeof(Vec3f), sizeof(Vec3f), &normals[i+1]);
					vbo.bufferSubData(mObj->mNormalOffset + (j+2) * sizeof(Vec3f), sizeof(Vec3f), &normals[i+2]);
					vbo.bufferSubData(mObj->mNormalOffset + (j+3) * sizeof(Vec3f), sizeof(Vec3f), &normals[i+0]);
					vbo.bufferSubData(mObj->mNormalOffset + (j+4) * sizeof(Vec3f), sizeof(Vec3f), &normals[i+2]);
					vbo.bufferSubData(mObj->mNormalOffset + (j+5) * sizeof(Vec3f), sizeof(Vec3f), &normals[i+3]);
				}
			}
			else
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
	if( mObj->mLayout.hasDynamicTexCoords2d(unit) ) {
		if( mObj->mDynamicStride == 0 ) {
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
			{
				Vbo &vbo = getDynamicVbo();
				for(size_t i = 0, j = 0; i < texCoords.size(); i += 4, j += 6)
				{
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+0) * sizeof(Vec2f), sizeof(Vec2f), &texCoords[i+0]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+1) * sizeof(Vec2f), sizeof(Vec2f), &texCoords[i+1]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+2) * sizeof(Vec2f), sizeof(Vec2f), &texCoords[i+2]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+3) * sizeof(Vec2f), sizeof(Vec2f), &texCoords[i+0]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+4) * sizeof(Vec2f), sizeof(Vec2f), &texCoords[i+2]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+5) * sizeof(Vec2f), sizeof(Vec2f), &texCoords[i+3]);
				}
			}
			else
				getDynamicVbo().bufferSubData( mObj->mTexCoordOffset[unit], sizeof(Vec2f) * texCoords.size(), &(texCoords[0]) );
		}
		else
			throw;
	}
	else if( mObj->mLayout.hasStaticTexCoords2d(unit) ) {
		if( mObj->mStaticStride == 0 ) { // planar data
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
			{
				Vbo &vbo = getStaticVbo();
				for(size_t i = 0, j = 0; i < texCoords.size(); i += 4, j += 6)
				{
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+0) * sizeof(Vec2f), sizeof(Vec2f), &texCoords[i+0]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+1) * sizeof(Vec2f), sizeof(Vec2f), &texCoords[i+1]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+2) * sizeof(Vec2f), sizeof(Vec2f), &texCoords[i+2]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+3) * sizeof(Vec2f), sizeof(Vec2f), &texCoords[i+0]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+4) * sizeof(Vec2f), sizeof(Vec2f), &texCoords[i+2]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+5) * sizeof(Vec2f), sizeof(Vec2f), &texCoords[i+3]);
				}
			}
			else
				getStaticVbo().bufferSubData( mObj->mTexCoordOffset[unit], sizeof(Vec2f) * texCoords.size(), &(texCoords[0]) );
		}
		else
			throw;
	}
	else
		throw;
}

void VboMesh::bufferTexCoords3d( size_t unit, const std::vector<Vec3f> &texCoords )
{
	if( mObj->mLayout.hasDynamicTexCoords3d(unit) ) {
		if( mObj->mDynamicStride == 0 ) {
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
			{
				Vbo &vbo = getDynamicVbo();
				for(size_t i = 0, j = 0; i < texCoords.size(); i += 4, j += 6)
				{
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+0) * sizeof(Vec3f), sizeof(Vec3f), &texCoords[i+0]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+1) * sizeof(Vec3f), sizeof(Vec3f), &texCoords[i+1]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+2) * sizeof(Vec3f), sizeof(Vec3f), &texCoords[i+2]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+3) * sizeof(Vec3f), sizeof(Vec3f), &texCoords[i+0]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+4) * sizeof(Vec3f), sizeof(Vec3f), &texCoords[i+2]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+5) * sizeof(Vec3f), sizeof(Vec3f), &texCoords[i+3]);
				}
			}
			else
				getDynamicVbo().bufferSubData( mObj->mTexCoordOffset[unit], sizeof(Vec3f) * texCoords.size(), &(texCoords[0]) );
		}
		else
			throw;
	}
	else if( mObj->mLayout.hasStaticTexCoords3d(unit) ) {
		if( mObj->mStaticStride == 0 ) { // planar data
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
			{
				Vbo &vbo = getStaticVbo();
				for(size_t i = 0, j = 0; i < texCoords.size(); i += 4, j += 6)
				{
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+0) * sizeof(Vec3f), sizeof(Vec3f), &texCoords[i+0]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+1) * sizeof(Vec3f), sizeof(Vec3f), &texCoords[i+1]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+2) * sizeof(Vec3f), sizeof(Vec3f), &texCoords[i+2]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+3) * sizeof(Vec3f), sizeof(Vec3f), &texCoords[i+0]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+4) * sizeof(Vec3f), sizeof(Vec3f), &texCoords[i+2]);
					vbo.bufferSubData(mObj->mTexCoordOffset[unit] + (j+5) * sizeof(Vec3f), sizeof(Vec3f), &texCoords[i+3]);
				}
			}
			else
				getStaticVbo().bufferSubData( mObj->mTexCoordOffset[unit], sizeof(Vec3f) * texCoords.size(), &(texCoords[0]) );
		}
		else
			throw;
	}
	else
		throw;
}
	
void VboMesh::bufferColorsRGB( const std::vector<Color> &colors )
{
	if( mObj->mLayout.hasDynamicColorsRGB() ) {
		if( mObj->mDynamicStride == 0 ) {
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
			{
				Vbo &vbo = getDynamicVbo();
				for(size_t i = 0, j = 0; i < colors.size(); i += 4, j += 6)
				{
					vbo.bufferSubData(mObj->mColorRGBOffset + (j+0) * sizeof(Color), sizeof(Color), &colors[i+0]);
					vbo.bufferSubData(mObj->mColorRGBOffset + (j+1) * sizeof(Color), sizeof(Color), &colors[i+1]);
					vbo.bufferSubData(mObj->mColorRGBOffset + (j+2) * sizeof(Color), sizeof(Color), &colors[i+2]);
					vbo.bufferSubData(mObj->mColorRGBOffset + (j+3) * sizeof(Color), sizeof(Color), &colors[i+0]);
					vbo.bufferSubData(mObj->mColorRGBOffset + (j+4) * sizeof(Color), sizeof(Color), &colors[i+2]);
					vbo.bufferSubData(mObj->mColorRGBOffset + (j+5) * sizeof(Color), sizeof(Color), &colors[i+3]);
				}
			}
			else
				getDynamicVbo().bufferSubData( mObj->mColorRGBOffset, sizeof(Color) * colors.size(), &(colors[0]) );
		}
		else
			throw;
	}
	else if( mObj->mLayout.hasStaticColorsRGB() ) {
		if( mObj->mStaticStride == 0 ) { // planar data
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
			{
				Vbo &vbo = getStaticVbo();
				for(size_t i = 0, j = 0; i < colors.size(); i += 4, j += 6)
				{
					vbo.bufferSubData(mObj->mColorRGBOffset + (j+0) * sizeof(Color), sizeof(Color), &colors[i+0]);
					vbo.bufferSubData(mObj->mColorRGBOffset + (j+1) * sizeof(Color), sizeof(Color), &colors[i+1]);
					vbo.bufferSubData(mObj->mColorRGBOffset + (j+2) * sizeof(Color), sizeof(Color), &colors[i+2]);
					vbo.bufferSubData(mObj->mColorRGBOffset + (j+3) * sizeof(Color), sizeof(Color), &colors[i+0]);
					vbo.bufferSubData(mObj->mColorRGBOffset + (j+4) * sizeof(Color), sizeof(Color), &colors[i+2]);
					vbo.bufferSubData(mObj->mColorRGBOffset + (j+5) * sizeof(Color), sizeof(Color), &colors[i+3]);
				}
			}
			else
				getStaticVbo().bufferSubData( mObj->mColorRGBOffset, sizeof(Color) * colors.size(), &(colors[0]) );
		}
		else
			throw;
	}
	else
		throw;
}

void VboMesh::bufferColorsRGBA( const std::vector<ColorA> &colors )
{
	if( mObj->mLayout.hasDynamicColorsRGBA() ) {
		if( mObj->mDynamicStride == 0 ) {
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
			{
				Vbo &vbo = getDynamicVbo();
				for(size_t i = 0, j = 0; i < colors.size(); i += 4, j += 6)
				{
					vbo.bufferSubData(mObj->mColorRGBAOffset + (j+0) * sizeof(ColorA), sizeof(ColorA), &colors[i+0]);
					vbo.bufferSubData(mObj->mColorRGBAOffset + (j+1) * sizeof(ColorA), sizeof(ColorA), &colors[i+1]);
					vbo.bufferSubData(mObj->mColorRGBAOffset + (j+2) * sizeof(ColorA), sizeof(ColorA), &colors[i+2]);
					vbo.bufferSubData(mObj->mColorRGBAOffset + (j+3) * sizeof(ColorA), sizeof(ColorA), &colors[i+0]);
					vbo.bufferSubData(mObj->mColorRGBAOffset + (j+4) * sizeof(ColorA), sizeof(ColorA), &colors[i+2]);
					vbo.bufferSubData(mObj->mColorRGBAOffset + (j+5) * sizeof(ColorA), sizeof(ColorA), &colors[i+3]);
				}
			}
			else
				getDynamicVbo().bufferSubData( mObj->mColorRGBAOffset, sizeof(ColorA) * colors.size(), &(colors[0]) );
		}
		else
			throw;
	}
	else if( mObj->mLayout.hasStaticColorsRGBA() ) {
		if( mObj->mStaticStride == 0 ) { // planar data
			if(mObj->mUseQuads && !mObj->mLayout.hasIndices())
			{
				Vbo &vbo = getStaticVbo();
				for(size_t i = 0, j = 0; i < colors.size(); i += 4, j += 6)
				{
					vbo.bufferSubData(mObj->mColorRGBAOffset + (j+0) * sizeof(ColorA), sizeof(ColorA), &colors[i+0]);
					vbo.bufferSubData(mObj->mColorRGBAOffset + (j+1) * sizeof(ColorA), sizeof(ColorA), &colors[i+1]);
					vbo.bufferSubData(mObj->mColorRGBAOffset + (j+2) * sizeof(ColorA), sizeof(ColorA), &colors[i+2]);
					vbo.bufferSubData(mObj->mColorRGBAOffset + (j+3) * sizeof(ColorA), sizeof(ColorA), &colors[i+0]);
					vbo.bufferSubData(mObj->mColorRGBAOffset + (j+4) * sizeof(ColorA), sizeof(ColorA), &colors[i+2]);
					vbo.bufferSubData(mObj->mColorRGBAOffset + (j+5) * sizeof(ColorA), sizeof(ColorA), &colors[i+3]);
				}
			}
			else
				getStaticVbo().bufferSubData( mObj->mColorRGBAOffset, sizeof(ColorA) * colors.size(), &(colors[0]) );
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
	//mVbo.bind();
	//mVbo.bufferData( mesh.mObj->mDynamicStride * mesh.mObj->mNumVertices, NULL, GL_STREAM_DRAW );
	//glBufferDataARB( GL_ARRAY_BUFFER, mesh.mObj->mDynamicStride * mesh.mObj->mNumVertices, NULL, GL_STREAM_DRAW );
	//mData = mVbo.map( GL_WRITE_ONLY );
	//mData = reinterpret_cast<uint8_t*>( glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY ) );
	mVbo.bufferData(mesh.mObj->mDynamicStride * mesh.mObj->mNumVertices, NULL, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE);
	mData = mVbo.map(D3D11_MAP_WRITE_DISCARD);
	mDataEnd = mData + mesh.mObj->mDynamicStride * mesh.getNumVertices();
}

VboMesh::VertexIter::Obj::~Obj()
{ 
	mVbo.unmap();
	//mVbo.unbind();
}

} } // namespace cinder::gl
