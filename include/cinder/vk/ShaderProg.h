/*
 Copyright 2016 Google Inc.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.


 Copyright (c) 2016, The Cinder Project, All rights reserved.

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

#pragma once

#include "cinder/vk/BaseVkObject.h"
#include "cinder/vk/UniformLayout.h"
#include "cinder/vk/VertexBufferMesh.h"
#include "cinder/DataSource.h"
#include "cinder/GeomIo.h"

namespace cinder { namespace vk {

class ShaderProg;
using ShaderProgRef = std::shared_ptr<ShaderProg>;


//! \class ShaderDef
//!
//!
class ShaderDef {
public:
	ShaderDef();

	ShaderDef&				color();	
	ShaderDef&				texture();
	ShaderDef&				lambert();
	// Used by draw(TextureRef&) stock shader; scales ciPosition and ciTexCoord according to
	// uniform "uPositionScale", "uPositionOffset", "uTexCoord0Scale", "uTexCoord0Offset"
	ShaderDef&				uniformBasedPosAndTexCoord();

	bool operator<( const ShaderDef &rhs ) const;
	
protected:
	bool					mColor;
	bool					mTextureMapping;
	bool					mTextureUnormalizedCoordinates;
	bool					mLambert;
	bool					mUniformBasedPosAndTexCoord;
	
	friend class Context;
};

//! \class ShaderProg
//!
//!
class ShaderProg : public BaseVkObject {
public:

	// Argument can be blockName or blockName.uniformName format
	using BlockNameTranslateFn = std::function<std::string(const std::string&)>;

	//! \class Attribute
	//!
	struct Attribute {

		Attribute( geom::Attrib semantic, int32_t location, int32_t binding, GlslAttributeDataType type ) 
			: mSemantic( semantic ), mLocation( location ), mBinding( binding ), mType( type ) {}
		virtual ~Attribute() {}

		//! Returns a const reference of the name as defined in the Vertex Shader.
		const std::string&	getName() const { return mName; }
		//! Returns the number of attributes expected by the Vertex Shader. mCount will be
		//! 1 unless this attribute is an array.
		int32_t				getCount() const { return mCount; }
		//! Returns the Vertex Shader generated or user defined binding of this attribute.
		int32_t				getLocation() const { return mLocation; }
		//! Returns the GLenum representation of the type of this attribute (for example, \c GL_FLOAT_VEC3)
		int32_t				getBinding() const { return mBinding; }
		//! Returns the Vertex Shader generated or user defined location of this attribute.
		GlslAttributeDataType	getType() const { return mType; }
		//! Returns the defined geom::Attrib semantic.
		geom::Attrib		getSemantic() const { return mSemantic; }

	  private:
		std::string			mName;
		int32_t				mCount = 0;
		int32_t				mLocation = -1;
		int32_t				mBinding = -1;
		GlslAttributeDataType	mType = glsl_attr_unknown;
		geom::Attrib		mSemantic = geom::Attrib::USER_DEFINED;

		friend class ShaderProg;
	};

	//! \class Format
	//!
	//!
	class Format {
	public:
		Format();

		Format&			vertex( const DataSourceRef &dataSource );
		Format&			vertex( const std::string &text );
		Format&			fragment( const DataSourceRef &dataSource );
		Format&			fragment( const std::string &text );
		Format&			geometry( const DataSourceRef &dataSource );
		Format&			geometry( const std::string &text );
		Format&			tessellationCtrl( const DataSourceRef &dataSource );
		Format&			tessellationCtrl( const std::string &text );
		Format&			tessellationEval( const DataSourceRef &dataSource );
		Format&			tessellationEval( const std::string &text );
		Format&			compute( const DataSourceRef &dataSource );
		Format&			compute( const std::string &text );

		Format&			binding( const std::string& bindingName, uint32_t bindingNumber );
		Format&			attribute( geom::Attrib semantic, int32_t location, int32_t binding, GlslAttributeDataType type );
		Format&			uniformLayout( const UniformLayout& layout );
		bool			userDefinedUniformLayout() const { return mUserDefinedUniformLayout; }

		Format&			blockNameTranslateFn( BlockNameTranslateFn fn );

	private:
		std::string					mVertexShader;
		std::string					mFragmentShader;
		std::string					mGeometryShader;
		std::string					mTessellationCtrlShader;
		std::string					mTessellationEvalShader;
		std::string					mComputeShader;

		fs::path					mVertexShaderPath;
		fs::path					mFragmentShaderPath;
		fs::path					mGeometryShaderPath;
		fs::path					mTessellationCtrlShaderPath;
		fs::path					mTessellationEvalShaderPath;
		fs::path					mComputeShaderPath;

		std::map<std::string, uint32_t>	mBindings;
		std::vector<Attribute>			mAttributes;
		UniformLayout					mUniformLayout;
		bool							mUserDefinedUniformLayout = false;

		BlockNameTranslateFn		mBlockNameTranslateFn;

		void			setShaderSource( const DataSourceRef &dataSource, std::string *shaderSourceDest, fs::path *shaderPathDest );

		friend class ShaderProg;
	};

	ShaderProg();
	ShaderProg( const ShaderProg::Format &format, Context* context );
	virtual ~ShaderProg();

	static ShaderProgRef	create( const ShaderProg::Format &format, Context *context = nullptr );
	static ShaderProgRef	create( DataSourceRef vertexShader, DataSourceRef fragmentShader , Context *context = nullptr );
	static ShaderProgRef	create( const std::string &vertexShader, const std::string &fragmentShader, Context *context = nullptr );

	const std::vector<VkPipelineShaderStageCreateInfo>&		getShaderStages() const { return mShaderStages; }

	const std::vector<Attribute>&		getActiveAttributes() const { return mAttributes; }
	const vk::VertexBufferMesh::Layout&	getVertexLayout() const { return mVertexLayout; }
	const UniformLayout&				getUniformLayout() const { return mUniformLayout; }

	void								uniform( const std::string& name, const float    value );
	void								uniform( const std::string& name, const int32_t  value );
	void								uniform( const std::string& name, const uint32_t value );
	void								uniform( const std::string& name, const bool     value );
	void								uniform( const std::string& name, const vec2&    value );
	void								uniform( const std::string& name, const vec3&    value );
	void								uniform( const std::string& name, const vec4&    value );
	void								uniform( const std::string& name, const ivec2&   value );
	void								uniform( const std::string& name, const ivec3&   value );
	void								uniform( const std::string& name, const ivec4&   value );
	void								uniform( const std::string& name, const uvec2&   value );
	void								uniform( const std::string& name, const uvec3&   value );
	void								uniform( const std::string& name, const uvec4&   value );
	void								uniform( const std::string& name, const mat2&    value );
	void								uniform( const std::string& name, const mat3&    value );
	void								uniform( const std::string& name, const mat4&    value );
	void								uniform( const std::string& name, const TextureBaseRef& texture );

/*
	void								sampler2D( const std::string& name, const TextureBaseRef& texture );
	void								sampler2DRect( const std::string& name, const TextureBaseRef& texture );
	void								sampler2DShadow( const std::string& name, const TextureBaseRef& texture );
	void								samplerCube( const std::string& name, const TextureBaseRef& texture );
*/
	static std::string					defaultBlockNameTranslate( const std::string& name );

private:
	std::vector<VkPipelineShaderStageCreateInfo>	mShaderStages;
	std::vector<Attribute>							mAttributes;
	vk::VertexBufferMesh::Layout					mVertexLayout;
	UniformLayout									mUniformLayout;

	void initialize( const ShaderProg::Format &format );
	void destroy( bool removeFromTracking = true );
	friend class Context;
};

// -------------------------------------------------------------------------------------------------
// Mapped type names
// -------------------------------------------------------------------------------------------------
using GlslProg = ShaderProg;
using GlslProgRef = ShaderProgRef;

}} // namespace cinder::vk