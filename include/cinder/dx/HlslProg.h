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

//! Represents a DirectX HLSL shader. \ImplShared
class HlslProg {
  public:
	HlslProg() {}
	HlslProg( DataSourceRef vertexShader, DataSourceRef fragmentShader = DataSourceRef(), DataSourceRef geometryShader = DataSourceRef());
	HlslProg( const BYTE *vertexShader, UINT vertexShaderSize, const BYTE *fragmentShader, UINT fragmentShaderSize, const BYTE *geometryShader, UINT geometryShaderSize );

	void			bind() const;
	static void		unbind();

	ID3D11VertexShader *GetVertexShader() { return mObj->mVS; }
	ID3D11PixelShader *GetPixelShader() { return mObj->mPS; }
	ID3D11GeometryShader *GetGeometryShader() { return mObj->mGS; }

	void CreateCBufferVertex(UINT slot, UINT size);
	void *MapCBufferVertex(UINT slot);
	void UnmapCBufferVertex(UINT slot);

	void CreateCBufferFragment(UINT slot, UINT size);
	void *MapCBufferFragment(UINT slot);
	void UnmapCBufferFragment(UINT slot);

	void CreateCBufferGeometry(UINT slot, UINT size);
	void *MapCBufferGeometry(UINT slot);
	void UnmapCBufferGeometry(UINT slot);

  protected:
	struct Cbo	//constant buffer object
	{
		Cbo(UINT slot, UINT size);
		~Cbo();

		UINT mSlot;
		ID3D11Buffer *mCBuffer;
	};

	struct Obj {
		Obj() : mVS(NULL), mPS(NULL), mGS(NULL) {}
		~Obj();
		
		//GLuint						mHandle;
		//std::map<std::string,int>	mUniformLocs;
		ID3D11VertexShader *mVS;
		ID3D11PixelShader *mPS;
		ID3D11GeometryShader *mGS;
		std::vector<std::shared_ptr<Cbo>> mCBuffersVertex;
		std::vector<std::shared_ptr<Cbo>> mCBuffersFragment;
		std::vector<std::shared_ptr<Cbo>> mCBuffersGeometry;
	};
 
	std::shared_ptr<Obj>	mObj;

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
