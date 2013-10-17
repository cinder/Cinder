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


#include "cinder/dx/dx.h"
#include "cinder/dx/HlslProg.h"
#include "cinder/app/AppImplMswRendererDx.h"

using namespace std;

namespace cinder { namespace dx {

HlslProg::Cbo::Cbo(UINT slot, UINT size) : mSlot(slot)
{
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = size;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	getDxRenderer()->md3dDevice->CreateBuffer(&bufferDesc, NULL, &mCBuffer);
}

HlslProg::Cbo::~Cbo()
{
	if(mCBuffer) mCBuffer->Release();
}

HlslProg::Obj::~Obj()
{
	if(mVS) mVS->Release();
	if(mPS) mPS->Release();
	if(mGS) mGS->Release();
}

//////////////////////////////////////////////////////////////////////////
// HlslProg
HlslProg::HlslProg( 
	DataSourceRef vertexShader, 
	DataSourceRef fragmentShader, 
	DataSourceRef geometryShader 
)
: mObj( shared_ptr<Obj>( new Obj ) )
{
	auto& d3ddevice = getDxRenderer()->md3dDevice;

	if( vertexShader ) {
		d3ddevice->CreateVertexShader(vertexShader->getBuffer().getData(), vertexShader->getBuffer().getDataSize(), NULL, &mObj->mVS);
	}
	if( fragmentShader ) {
		d3ddevice->CreatePixelShader(fragmentShader->getBuffer().getData(), fragmentShader->getBuffer().getDataSize(), NULL, &mObj->mPS);
	}
	if( geometryShader ) {
		d3ddevice->CreateGeometryShader(geometryShader->getBuffer().getData(), geometryShader->getBuffer().getDataSize(), NULL, &mObj->mGS);
	}
}

HlslProg::HlslProg( 
	const BYTE *vertexShader, UINT vertexShaderSize, 
	const BYTE *fragmentShader, UINT fragmentShaderSize, 
	const BYTE *geometryShader, UINT geometryShaderSize 
)
: mObj( shared_ptr<Obj>( new Obj ) )
{
	auto& d3ddevice = getDxRenderer()->md3dDevice;

	if( vertexShader ) {
		d3ddevice->CreateVertexShader( vertexShader, vertexShaderSize, NULL, &mObj->mVS );
	}
	if( fragmentShader ) {
		d3ddevice->CreatePixelShader( fragmentShader, fragmentShaderSize, NULL, &mObj->mPS );
	}
	if( geometryShader ) {
		d3ddevice->CreateGeometryShader( geometryShader, geometryShaderSize, NULL, &mObj->mGS );
	}
}

HlslProg::HlslProg( 
	const std::string& vertexShaderName, DataSourceRef vertexShaderSrc, 
	const std::string& fragmentShaderName, DataSourceRef fragmentShaderSrc, 
	const std::string& geometryShaderName, DataSourceRef geometryShaderSrc
)
: mObj( shared_ptr<Obj>( new Obj ) )
{
	initFromSource( 
		vertexShaderName, (const char*)(vertexShaderSrc->getBuffer().getData()),
		fragmentShaderName, (const char*)(fragmentShaderSrc->getBuffer().getData()),
		geometryShaderName, (const char*)(geometryShaderSrc->getBuffer().getData())
	);
}

HlslProg::HlslProg( 
	const std::string& vertexShaderName, const char *vertexShaderSrc, 
	const std::string& fragmentShaderName, const char *fragmentShaderSrc,
	const std::string& geometryShaderName, const char *geometryShaderSrc
)
: mObj( shared_ptr<Obj>( new Obj ) )
{
	initFromSource( 
		vertexShaderName, vertexShaderSrc,
		fragmentShaderName, fragmentShaderSrc,
		geometryShaderName, geometryShaderSrc
	);
}

void HlslProg::initFromSource( 
	const std::string& vertexShaderName, const char *vertexShaderSrc, 
	const std::string& fragmentShaderName, const char *fragmentShaderSrc, 
	const std::string& geometryShaderName, const char *geometryShaderSrc 
)
{

}

//void HlslProg::loadShader( Buffer shaderSourceBuffer, GLint shaderType )
//{
//	// we need to duplicate the contents of the buffer and append a null-terminator
//	shared_ptr<char> sourceBlock( new char[shaderSourceBuffer.getDataSize() + 1], checked_array_deleter<char>() );
//	memcpy( sourceBlock.get(), shaderSourceBuffer.getData(), shaderSourceBuffer.getDataSize() );
//	sourceBlock.get()[shaderSourceBuffer.getDataSize()] = 0; // null terminate
//	const char *sourceBlockPtr = sourceBlock.get();
//	loadShader( sourceBlockPtr, shaderType );
//}
//
//void HlslProg::loadShader( const char *shaderSource, GLint shaderType )
//{
//	GLuint handle = glCreateShader( shaderType );
//	glShaderSource( handle, 1, reinterpret_cast<const GLchar**>( &shaderSource ), NULL );
//	glCompileShader( handle );
//	
//	GLint status;
//	glGetShaderiv( (GLuint) handle, GL_COMPILE_STATUS, &status );
//	if( status != GL_TRUE ) {
//		std::string log = getShaderLog( (GLuint)handle );
//		throw HlslProgCompileExc( log, shaderType );
//	}
//	glAttachShader( mObj->mHandle, handle );
//}

void HlslProg::bind() const
{
	auto dx = getDxRenderer();
	//bind the shaders
	if(mObj->mVS)
		dx->mDeviceContext->VSSetShader(mObj->mVS, NULL, 0);
	if(mObj->mPS)
		dx->mDeviceContext->PSSetShader(mObj->mPS, NULL, 0);
	if(mObj->mGS)
		dx->mDeviceContext->GSSetShader(mObj->mGS, NULL, 0);

	//bind the buffers
	for(unsigned i = 0; i < mObj->mCBuffersVertex.size(); ++i)
		dx->mDeviceContext->VSSetConstantBuffers(mObj->mCBuffersVertex[i]->mSlot, 1, &mObj->mCBuffersVertex[i]->mCBuffer);
	
	for(unsigned i = 0; i < mObj->mCBuffersFragment.size(); ++i)
		dx->mDeviceContext->PSSetConstantBuffers(mObj->mCBuffersFragment[i]->mSlot, 1, &mObj->mCBuffersFragment[i]->mCBuffer);
		
	for(unsigned i = 0; i < mObj->mCBuffersGeometry.size(); ++i)
		dx->mDeviceContext->GSSetConstantBuffers(mObj->mCBuffersGeometry[i]->mSlot, 1, &mObj->mCBuffersGeometry[i]->mCBuffer);

	
	dx->setRenderFlag(app::AppImplMswRendererDx::CUSTOM_SHADER_ACTIVE);
}

void HlslProg::unbind()
{
	auto dx = getDxRenderer();
	dx->mDeviceContext->VSSetShader(NULL, NULL, 0);
	dx->mDeviceContext->PSSetShader(NULL, NULL, 0);
	dx->mDeviceContext->GSSetShader(NULL, NULL, 0);
	dx->clearRenderFlag(app::AppImplMswRendererDx::CUSTOM_SHADER_ACTIVE);
}

void HlslProg::CreateCBufferVertex(UINT slot, UINT size)
{
	for(unsigned i = 0; i < mObj->mCBuffersVertex.size(); ++i)
		if(mObj->mCBuffersVertex[i]->mSlot == slot)
			throw HlslDuplicateCBufferExc();
	mObj->mCBuffersVertex.push_back(std::shared_ptr<Cbo>(new Cbo(slot, size)));
}

void *HlslProg::MapCBufferVertex(UINT slot)
{
	for(unsigned i = 0; i < mObj->mCBuffersVertex.size(); ++i)
		if(mObj->mCBuffersVertex[i]->mSlot == slot)
		{
			D3D11_MAPPED_SUBRESOURCE resource;
			getDxRenderer()->mDeviceContext->Map(mObj->mCBuffersVertex[i]->mCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
			return resource.pData;
			break;
		}
	return NULL;
}

void HlslProg::UnmapCBufferVertex(UINT slot)
{
	for(unsigned i = 0; i < mObj->mCBuffersVertex.size(); ++i)
		if(mObj->mCBuffersVertex[i]->mSlot == slot)
		{
			getDxRenderer()->mDeviceContext->Unmap(mObj->mCBuffersVertex[i]->mCBuffer, 0);
			break;
		}
}

void HlslProg::CreateCBufferFragment(UINT slot, UINT size)
{
	for(unsigned i = 0; i < mObj->mCBuffersFragment.size(); ++i)
		if(mObj->mCBuffersFragment[i]->mSlot == slot)
			throw HlslDuplicateCBufferExc();
	mObj->mCBuffersFragment.push_back(std::shared_ptr<Cbo>(new Cbo(slot, size)));
}

void *HlslProg::MapCBufferFragment(UINT slot)
{
	for(unsigned i = 0; i < mObj->mCBuffersFragment.size(); ++i)
		if(mObj->mCBuffersFragment[i]->mSlot == slot)
		{
			D3D11_MAPPED_SUBRESOURCE resource;
			getDxRenderer()->mDeviceContext->Map(mObj->mCBuffersFragment[i]->mCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
			return resource.pData;
			break;
		}
	return NULL;
}

void HlslProg::UnmapCBufferFragment(UINT slot)
{
	for(unsigned i = 0; i < mObj->mCBuffersFragment.size(); ++i)
		if(mObj->mCBuffersFragment[i]->mSlot == slot)
		{
			getDxRenderer()->mDeviceContext->Unmap(mObj->mCBuffersFragment[i]->mCBuffer, 0);
			break;
		}
}

void HlslProg::CreateCBufferGeometry(UINT slot, UINT size)
{
	for(unsigned i = 0; i < mObj->mCBuffersGeometry.size(); ++i)
		if(mObj->mCBuffersGeometry[i]->mSlot == slot)
			throw HlslDuplicateCBufferExc();
	mObj->mCBuffersGeometry.push_back(std::shared_ptr<Cbo>(new Cbo(slot, size)));
}

void *HlslProg::MapCBufferGeometry(UINT slot)
{
	for(unsigned i = 0; i < mObj->mCBuffersGeometry.size(); ++i)
		if(mObj->mCBuffersGeometry[i]->mSlot == slot)
		{
			D3D11_MAPPED_SUBRESOURCE resource;
			getDxRenderer()->mDeviceContext->Map(mObj->mCBuffersGeometry[i]->mCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
			return resource.pData;
			break;
		}
	return NULL;
}

void HlslProg::UnmapCBufferGeometry(UINT slot)
{
	for(unsigned i = 0; i < mObj->mCBuffersGeometry.size(); ++i)
		if(mObj->mCBuffersGeometry[i]->mSlot == slot)
		{
			getDxRenderer()->mDeviceContext->Unmap(mObj->mCBuffersGeometry[i]->mCBuffer, 0);
			break;
		}
}

//////////////////////////////////////////////////////////////////////////
// HlslProgCompileExc
HlslProgCompileExc::HlslProgCompileExc( const std::string &log, GLint aShaderType ) throw()
	: mShaderType( aShaderType )
{
	if( mShaderType == GL_VERTEX_SHADER_ARB )
		strncpy_s( mMessage, "VERTEX: ", 1000 );
	else if( mShaderType == GL_FRAGMENT_SHADER_ARB )
		strncpy_s( mMessage, "FRAGMENT: ", 1000 );
	else if( mShaderType == GL_GEOMETRY_SHADER_EXT )
		strncpy_s( mMessage, "GEOMETRY: ", 1000 );
	else
		strncpy_s( mMessage, "UNKNOWN: ", 1000 );
	strncat_s( mMessage, log.c_str(), 15000 );
}
	
} } // namespace cinder::dx
