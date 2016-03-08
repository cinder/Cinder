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

#include "cinder/vk/platform.h"
#include "cinder/vk/glsl_types.h"

#include "cinder/GeomIo.h"
#include "cinder/Matrix.h"
#include "cinder/Vector.h"

#include <map>

namespace cinder { namespace vk {

class Device;
class TextureBase;
class UniformBuffer;
class UniformSet;
using TextureBaseRef = std::shared_ptr<TextureBase>;
using UniformBufferRef = std::shared_ptr<UniformBuffer>;
using UniformSetRef = std::unique_ptr<UniformSet>;

// Uniform semantics
enum UniformSemantic {
	UNIFORM_MODEL_MATRIX,
	UNIFORM_MODEL_MATRIX_INVERSE,
	UNIFORM_MODEL_MATRIX_INVERSE_TRANSPOSE,
	UNIFORM_VIEW_MATRIX,
	UNIFORM_VIEW_MATRIX_INVERSE,
	UNIFORM_MODEL_VIEW,
	UNIFORM_MODEL_VIEW_INVERSE,
	UNIFORM_MODEL_VIEW_INVERSE_TRANSPOSE,
	UNIFORM_MODEL_VIEW_PROJECTION,
	UNIFORM_MODEL_VIEW_PROJECTION_INVERSE,
	UNIFORM_PROJECTION_MATRIX,
	UNIFORM_PROJECTION_MATRIX_INVERSE,
	UNIFORM_VIEW_PROJECTION,
	UNIFORM_NORMAL_MATRIX,
	UNIFORM_VIEWPORT_MATRIX,
	UNIFORM_WINDOW_SIZE,
	UNIFORM_ELAPSED_SECONDS,
	UNIFORM_USER_DEFINED
};

enum ChangeFrequency {
	SELDOM		= 0x00000001, // Once or twice after initial write
	REGULAR		= 0x00000002, // On state changes
	FREQUENT	= 0x00000004, // Every frame or every few frames
	ALL			= 0x0000000F
};

//! \class UniformLayout
//!
//!
class UniformLayout {
public:

	union Value {
		float		f;
		int32_t		i;
		uint32_t	u = 0;
	};

	using ValueStore = std::vector<std::vector<Value>>;

	//! \class Uniform
	//
	//
	class Uniform {
	public:

		Uniform() {}
		Uniform( const std::string& name, GlslUniformDataType dataType );
		virtual ~Uniform() {}

		const std::string&					getName() const { return mName; }
		GlslUniformDataType					getDataType() const { return mDataType; }
		size_t								getOffset() const { return mOffset; }
		uint32_t							getArraySize() const  { return mArraySize; }
		UniformSemantic						getUniformSemantic() const { return mSemantic; }
	private:
		std::string							mName;
		GlslUniformDataType					mDataType	= glsl_unknown;
		uint32_t							mOffset		= 0;
		uint32_t							mArraySize	= 1;
		UniformSemantic						mSemantic	= UniformSemantic::UNIFORM_USER_DEFINED;

		void setOffset( uint32_t offset ) { mOffset = offset; }
		void setArraySize( uint32_t arraySize ) { mArraySize = std::max<uint32_t>( 1, arraySize ); }
		friend class UniformLayout;
	};

	// ---------------------------------------------------------------------------------------------
	
	//! \class Block
	//
	//
	class Block {
	public:

		using UniformStore = std::pair<Uniform, ValueStore>;

		Block() {}
		virtual ~Block() {}

		const std::vector<UniformStore>&	getUniforms() const { return mUniforms; }

		// NOTE: The std140 layout rules are a bit complex and will take sometime to implement.
		//       For now, the size will just be from the glslang reflection data.
		//size_t							getSizeBytes() const;
		//size_t							getSizeBytesStd140() const;
		size_t								getSizeBytes() const { return mSizeBytes; }

		void								sortUniformsByOffset();

	private:
		std::vector<UniformStore>		mUniforms;
		size_t							mSizeBytes = 0;

		UniformStore* findUniformObject( const std::string& name, GlslUniformDataType dataType, bool addIfNotExits = false );
		void setSizeBytes( size_t size ) { mSizeBytes = size; }
		friend class UniformLayout;
	};

	// ---------------------------------------------------------------------------------------------

	//! \class Binding
	//
	//
	class Binding {
	public:

		enum class Type { 
			UNDEFINED	= 0, 
			BLOCK		= 1, 
			SAMPLER		= 2,
			ANY			= SAMPLER | BLOCK
		};

		Binding() {}
		Binding( const std::string& name, Binding::Type type );
		virtual ~Binding() {}

		int32_t								getBinding() const { return mBinding; }
		Binding::Type						getType() const { return mType; }
		const std::string&					getName() const { return mName; } 
		vk::ChangeFrequency					getChangeFrequency() const { return mChangeFrequency; }
		bool								isBlock() const { return Binding::Type::BLOCK == mType; }
		bool								isSampler() const { return Binding::Type::SAMPLER == mType; }

		const Block&						getBlock() const { return mBlock; }
		const vk::TextureBaseRef&			getTexture() const { return mTexture; }

		void								sortUniformsByOffset();

	private:
		int32_t								mBinding = -1;
		Binding::Type						mType = Binding::Type::UNDEFINED;
		std::string							mName;
		vk::ChangeFrequency					mChangeFrequency = ChangeFrequency::FREQUENT;

		// Uniform data
		Block								mBlock;
		// Sampler data
		vk::TextureBaseRef					mTexture;

		// These functions force their type since in most cases mBinding is set before it's know what the binding is needed.
		void setBinding( int32_t binding, ChangeFrequency changeFrequency ) { mBinding = binding; mChangeFrequency = changeFrequency; }
		void setBlockSizeBytes( size_t sizeBytes ) { mBlock.setSizeBytes( sizeBytes ); }
		void setTexture( const vk::TextureBaseRef& texture ) { mTexture = texture; mType = Binding::Type::SAMPLER; }
		friend class UniformLayout;
		friend class UniformSet;
	};

	// ---------------------------------------------------------------------------------------------

	UniformLayout();
	virtual ~UniformLayout();

	const std::vector<Binding>&			getBindings() const { return mBindings; }

	UniformLayout&						addUniform( const std::string& name, GlslUniformDataType dataType, uint32_t offset, uint32_t arraySize = 1 );
	UniformLayout&						addUniform( const std::string& name, const float& value, uint32_t offset, uint32_t arraySize = 1 );
	UniformLayout&						addUniform( const std::string& name, const vec2&  value, uint32_t offset, uint32_t arraySize = 1 );
	UniformLayout&						addUniform( const std::string& name, const vec3&  value, uint32_t offset, uint32_t arraySize = 1 );
	UniformLayout&						addUniform( const std::string& name, const vec4&  value, uint32_t offset, uint32_t arraySize = 1 );
	UniformLayout&						addUniform( const std::string& name, const mat2&  value, uint32_t offset, uint32_t arraySize = 1 );
	UniformLayout&						addUniform( const std::string& name, const mat3&  value, uint32_t offset, uint32_t arraySize = 1 );
	UniformLayout&						addUniform( const std::string& name, const mat4&  value, uint32_t offset, uint32_t arraySize = 1 );
	UniformLayout&						addUniform( const std::string& name, const vk::TextureBaseRef& texture );

	UniformLayout&						setBinding( const std::string& name, int32_t binding, ChangeFrequency changeFrequency );

	void								setBlockSizeBytes( const std::string& name, size_t sizeBytes );
	void								sortUniformsByOffset();

	// Update uniform variables
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
	void								uniform( const std::string& name, const vk::TextureBaseRef& texture );

private:
	std::vector<Binding>				mBindings;

	Binding*							findBindingObject( const std::string& name, Binding::Type bindingType, bool addIfNotExits = false );
	void								addUniformImpl( GlslUniformDataType dataType, const std::string& name, uint32_t offset, uint32_t arraySize );

	template <typename T>
	void setUniformValue( GlslUniformDataType dataType, const std::string& name, const std::vector<T>& values );
};

//! \class UniformSet
//!
//!
class UniformSet { 
public:

	//! \class Binding
	//!
	//!
	class Binding : public UniformLayout::Binding {
	public:
		Binding() {}
		Binding( const UniformLayout::Binding& binding )
			: UniformLayout::Binding( binding ) {}
		virtual ~Binding() {}

		const UniformBufferRef&		getUniformBuffer() const { return mUniformBuffer; }
	private:
		UniformBufferRef			mUniformBuffer;
		friend class UniformSet;
	};

	// ---------------------------------------------------------------------------------------------

	UniformSet( const UniformLayout& layout, Device *device );
	virtual ~UniformSet();

	static UniformSetRef			create( const UniformLayout& layout, Device *device = nullptr );

	const std::vector<Binding>&		getBindings() const { return mBindings; }
	const std::vector<VkDescriptorSetLayoutBinding>&	getDescriptorSetlayoutBindings() const { return mDescriptorSetLayoutBindings; } 

	void							uniform( const std::string& name, const float    value );
	void							uniform( const std::string& name, const int32_t  value );
	void							uniform( const std::string& name, const uint32_t value );
	void							uniform( const std::string& name, const bool     value );
	void							uniform( const std::string& name, const vec2&    value );
	void							uniform( const std::string& name, const vec3&    value );
	void							uniform( const std::string& name, const vec4&    value );
	void							uniform( const std::string& name, const ivec2&   value );
	void							uniform( const std::string& name, const ivec3&   value );
	void							uniform( const std::string& name, const ivec4&   value );
	void							uniform( const std::string& name, const uvec2&   value );
	void							uniform( const std::string& name, const uvec3&   value );
	void							uniform( const std::string& name, const uvec4&   value );
	void							uniform( const std::string& name, const mat2&    value );
	void							uniform( const std::string& name, const mat3&    value );
	void							uniform( const std::string& name, const mat4&    value );
	void							uniform( const std::string& name, const TextureBaseRef& texture );

	void							bufferPending();

	void							echoValues( std::ostream& os );

private:
	std::vector<Binding>						mBindings;
	// Since the bindings are immutable, create the VkDescriptorSetLayoutBinding to be used by caches.
	std::vector<VkDescriptorSetLayoutBinding>	mDescriptorSetLayoutBindings;

	Binding*						findBindingObject( const std::string& name, Binding::Type bindingType );

	template <typename T>
	void updateUniform( const std::string& name, const T& value );
};

// -------------------------------------------------------------------------------------------------
// Uniform semantics
// -------------------------------------------------------------------------------------------------
using UniformSemanticMap = std::map<std::string, UniformSemantic>;
using AttribSemanticMap = std::map<std::string, geom::Attrib>;

//! Converts a UniformSemantic to its name
std::string uniformSemanticToString( UniformSemantic uniformSemantic );
//! Default mapping from uniform name to semantic. Can be modified via the reference. Not thread-safe.
const vk::UniformSemanticMap& getDefaultUniformNameToSemanticMap();
//! Default mapping from attribute name to semantic. Can be modified via the reference. Not thread-safe.
const vk::AttribSemanticMap& getDefaultAttribNameToSemanticMap();
//! Returns semantic for default name
UniformSemantic uniformNameToSemantic( const std::string& name );

}} // namespace cinder::vk
