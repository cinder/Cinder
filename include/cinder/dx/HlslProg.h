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

#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <exception>
#include <map>

#include "cinder/dx/dx.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Matrix.h"
#include "cinder/DataSource.h"
#include <d3d11.h>

namespace cinder { namespace dx {

class HlslProg;
typedef std::shared_ptr<HlslProg> HlslProgRef;

//! Represents a DirectX HLSL shader. \ImplShared
class HlslProg {
public:
	
	HlslProg() {}
	//! Constructs a shader from compiled object bytecode
	HlslProg( 
		DataSourceRef vertexShader, 
		DataSourceRef fragmentShader, 
		DataSourceRef geometryShader = DataSourceRef() 
	);
	//! Constructs a shader from compiled object bytecode
	HlslProg( 
		const BYTE *vertexShader, UINT vertexShaderSize, 
		const BYTE *fragmentShader, UINT fragmentShaderSize, 
		const BYTE *geometryShader = nullptr, UINT geometryShaderSize = 0
	);
	//! Constructs a shader from source. Can only be during development for Windows Store Apps.
	HlslProg( 
		const std::string& vertexEntryPoint, DataSourceRef vertexShader, 
		const std::string& fragmentEntryPoint, DataSourceRef fragmentShader, 
		const std::string& geometryEntryPoint = "", DataSourceRef geometryShader = DataSourceRef() 
	);
	//! Constructs a shader from source. Can only be during development for Windows Store Apps.
	HlslProg( 
		const std::string& vertexEntryPoint, const char *vertexShader, 
		const std::string& fragmentEntryPoint, const char *fragmentShader,
		const std::string& geometryEntryPoint = "", const char *geometryShader = nullptr
	);

	//! Constructs a shader from compiled object bytecode
	static HlslProgRef create ( 
		DataSourceRef vertexShader, 
		DataSourceRef fragmentShader, 
		DataSourceRef geometryShader = DataSourceRef() 
	);
	//! Constructs a shader from compiled object bytecode
	static HlslProgRef create( 
		const BYTE *vertexShader, UINT vertexShaderSize, 
		const BYTE *fragmentShader, UINT fragmentShaderSize, 
		const BYTE *geometryShader = nullptr, UINT geometryShaderSize = 0
	);
	//! Constructs a shader from source. Can only be during development for Windows Store Apps.
	static HlslProgRef create( 
		const std::string& vertexEntryPoint, DataSourceRef vertexShaderSrc, 
		const std::string& fragmentEntryPoint, DataSourceRef fragmentShaderSrc, 
		const std::string& geometryEntryPoint = "", DataSourceRef geometryShaderSrc = DataSourceRef() 
	);
	//! Constructs a shader from source. Can only be during development for Windows Store Apps.
	static HlslProgRef create( 
		const std::string& vertexEntryPoint, const char *vertexShaderSrc, 
		const std::string& fragmentEntryPoint, const char *fragmentShaderSrc,
		const std::string& geometryEntryPoint = "", const char *geometryShaderSrc = nullptr
	);

	void					bind() const;
	static void				unbind();

	ID3D11VertexShader*		GetVertexShader() { return mObj->mVS; }
	ID3D11PixelShader*		GetPixelShader() { return mObj->mPS; }
	ID3D11GeometryShader*	GetGeometryShader() { return mObj->mGS; }
	ID3D11ComputeShader*	GetComputeShader() { return mObj->mCS; }

	void	CreateCBufferVertex(UINT slot, UINT size);
	void*	MapCBufferVertex(UINT slot);
	void	UnmapCBufferVertex(UINT slot);

	void	CreateCBufferFragment(UINT slot, UINT size);
	void*	MapCBufferFragment(UINT slot);
	void	UnmapCBufferFragment(UINT slot);

	void	CreateCBufferGeometry(UINT slot, UINT size);
	void*	MapCBufferGeometry(UINT slot);
	void	UnmapCBufferGeometry(UINT slot);

	void	CreateCBufferCompute(UINT slot, UINT size);
	void*	MapCBufferCompute(UINT slot);
	void	UnmapCBufferCompute(UINT slot);

protected:
	struct Cbo	//constant buffer object
	{
		Cbo(UINT slot, UINT size);
		~Cbo();

		UINT mSlot;
		ID3D11Buffer *mCBuffer;
	};

	struct Obj {
		Obj() : mVS( nullptr ), mPS( nullptr ), mGS( nullptr ), mCS( nullptr ) {}
		~Obj();
		
		//GLuint						mHandle;
		//std::map<std::string,int>	mUniformLocs;
		ID3D11VertexShader*		mVS;
		ID3D11PixelShader*		mPS;
		ID3D11GeometryShader*	mGS;
		ID3D11ComputeShader*	mCS;
		std::vector<std::shared_ptr<Cbo>> mCBuffersVertex;
		std::vector<std::shared_ptr<Cbo>> mCBuffersFragment;
		std::vector<std::shared_ptr<Cbo>> mCBuffersGeometry;
		std::vector<std::shared_ptr<Cbo>> mCBuffersCompute;
	};
 
	std::shared_ptr<Obj>	mObj;

private:
	void initFromSource( 
		const std::string& vertexShaderName, const char *vertexShaderSrc, 
		const std::string& fragmentShaderName, const char *fragmentShaderSrc,
		const std::string& geometryShaderName, const char *geometryShaderSrc
	);

public:
	//@{
	//! Emulates shared_ptr-like behavior
	typedef std::shared_ptr<Obj> HlslProg::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &HlslProg::mObj; }
	void reset() { mObj.reset(); }
	//@}  
};

class HlslProgCompileExc : public std::exception {
public:	
	HlslProgCompileExc( const std::string &log, GLint aShaderType ) throw();
	virtual const char* what() const throw()
	{
		return mMessage;
	}

private:
	char	mMessage[16001];
	GLint	mShaderType;
};

class HlslNullProgramExc : public std::exception {
public:	
	virtual const char* what() const throw()
	{
		return "Hlsl: Attempt to use null shader";
	}

};

class HlslDuplicateCBufferExc : public std::exception {
public:
	virtual const char* what() const throw()
	{
		return "Hlsl: Attempt to create a duplicate constant buffer";
	}
};

} } // namespace cinder::dx
