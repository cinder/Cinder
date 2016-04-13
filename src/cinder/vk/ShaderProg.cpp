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

#include "cinder/vk/ShaderProg.h"
#include "cinder/vk/ConstantConversion.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Device.h"
#include "cinder/vk/gl_types.h"
#include "cinder/vk/glsl_types.h"
#include "cinder/Utilities.h"
#include "cinder/Log.h"

#include "SPIRV/GlslangToSpv.h"

#include "./spir2cross/spir2cpp.hpp"

#include <algorithm>

namespace cinder { namespace vk {

	// -------------------------------------------------------------------------------------------------
// ShaderProg::Format
// -------------------------------------------------------------------------------------------------
ShaderDef::ShaderDef()
	: mColor( false ), mTextureMapping( false ), mTextureUnormalizedCoordinates( false ), mLambert( false ), mUniformBasedPosAndTexCoord( false )
{
}

ShaderDef& ShaderDef::texture()
{
	mTextureMapping = true;
	return *this;
}

ShaderDef& ShaderDef::uniformBasedPosAndTexCoord()
{
	mUniformBasedPosAndTexCoord = true;
	return *this;
}

ShaderDef& ShaderDef::color()
{
	mColor = true;
	return *this;
}

ShaderDef& ShaderDef::lambert()
{
	mLambert = true;
	return *this;
}

bool ShaderDef::operator<( const ShaderDef &rhs ) const
{
	if( rhs.mColor != mColor )
		return rhs.mColor;
	if( rhs.mTextureMapping != mTextureMapping )
		return rhs.mTextureMapping;
	if( rhs.mLambert != mLambert )
		return rhs.mLambert;
	if( rhs.mUniformBasedPosAndTexCoord != mUniformBasedPosAndTexCoord )
		return rhs.mUniformBasedPosAndTexCoord;
	
	return false;
}


// -------------------------------------------------------------------------------------------------
// ShaderProg::Format::ShaderData
// -------------------------------------------------------------------------------------------------
void ShaderProg::Format::ShaderData::loadShader()
{
	if( ! mDataSource ) {
		return;
	}

	ci::Buffer buffer;
	if( mDataSource->isFilePath() ) {
		auto path = mDataSource->getFilePath();
		if( ! path.empty() ) {
			DataSourceRef dataSource = loadFile( path );
			if( dataSource ) {
				buffer = ci::Buffer( dataSource );				
			}
		}
	}
	else {
		buffer = ci::Buffer( mDataSource );
	}

	if( buffer.getAllocatedSize() > 0 ) {
		// SPIR-V binary
		if( ( buffer.getSize() > 4 ) && ( spv::MagicNumber == *(reinterpret_cast<unsigned int*>( buffer.getData() ) ) ) ) {
			// Copy
			size_t n = buffer.getSize() / sizeof(uint32_t);
			mSpirvBinary.resize( n );
			std::memcpy( static_cast<void*>( mSpirvBinary.data() ), buffer.getData(), n*sizeof(uint32_t) );
			// Set
			mSourceText.clear();
			mDataType = Format::ShaderData::Type::SPIRV;
		}
		// GLSL source
		else {
			// Copy
			mSourceText.resize( buffer.getSize() + 1 );
			std::memcpy( (void*)mSourceText.data(), buffer.getData(), buffer.getSize() );
			mSourceText[buffer.getSize()] = 0;
			// Set
			mSpirvBinary.clear();
			mDataType = Format::ShaderData::Type::GLSL;
		}
	}

	mDataSource.reset();
}

void ShaderProg::Format::ShaderData::setShaderData( const DataSourceRef &dataSource, const std::string &entryPoint )
{
	// Clear
	mSourceText.clear();
	mSpirvBinary.clear();
	// Set
	mInitialized = true;
	mDataSource = dataSource;
	setEntryPoint( entryPoint );
}

void ShaderProg::Format::ShaderData::setShaderData( const std::string &sourceText, const std::string &entryPoint )
{
	// Clear
	mDataSource.reset();
	mSpirvBinary.clear();
	// Set
	mInitialized = true;
	mSourceText = sourceText;
	mDataType = Format::ShaderData::Type::GLSL;
	setEntryPoint( entryPoint );
}

void ShaderProg::Format::ShaderData::setShaderData( const std::vector<uint32_t> &spirvBinary, const std::string &entryPoint )
{
	// Clear
	mDataSource.reset();
	mSourceText.clear();
	// Set
	mInitialized = true;
	mSpirvBinary = spirvBinary;
	mDataType = Format::ShaderData::Type::SPIRV;
	setEntryPoint( entryPoint );
}

void ShaderProg::Format::ShaderData::setEntryPoint( const std::string &entryPoint )
{
	if( ! entryPoint.empty() ) {
		mEntryPoint = entryPoint;
	}
}

// -------------------------------------------------------------------------------------------------
// ShaderProg::Format
// -------------------------------------------------------------------------------------------------
ShaderProg::Format::Format()
{
	mShaderData[VK_SHADER_STAGE_VERTEX_BIT] = Format::ShaderData();
	mShaderData[VK_SHADER_STAGE_FRAGMENT_BIT] = Format::ShaderData();
	mShaderData[VK_SHADER_STAGE_GEOMETRY_BIT] = Format::ShaderData();
	mShaderData[VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT] = Format::ShaderData();
	mShaderData[VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT] = Format::ShaderData();
	mShaderData[VK_SHADER_STAGE_COMPUTE_BIT] = Format::ShaderData();
}

ShaderProg::Format& ShaderProg::Format::vertex( const DataSourceRef &dataSource, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_VERTEX_BIT].setShaderData( dataSource, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::vertex( const std::string &souceText, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_VERTEX_BIT].setShaderData( souceText, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::vertex( const std::vector<uint32_t> &spirvBinary, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_VERTEX_BIT].setShaderData( spirvBinary, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::fragment( const DataSourceRef &dataSource, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_FRAGMENT_BIT].setShaderData( dataSource, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::fragment( const std::string &souceText, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_FRAGMENT_BIT].setShaderData( souceText, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::fragment( const std::vector<uint32_t> &spirvBinary, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_FRAGMENT_BIT].setShaderData( spirvBinary, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::geometry( const DataSourceRef &dataSource, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_GEOMETRY_BIT].setShaderData( dataSource, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::geometry( const std::string &sourceText, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_GEOMETRY_BIT].setShaderData( sourceText, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::geometry( const std::vector<uint32_t> &spirvBinary, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_GEOMETRY_BIT].setShaderData( spirvBinary, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::tessellationCtrl( const DataSourceRef &dataSource, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT].setShaderData( dataSource, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::tessellationCtrl( const std::string &souceText, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT].setShaderData( souceText, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::tessellationCtrl( const std::vector<uint32_t> &spirvBinary, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT].setShaderData( spirvBinary, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::tessellationEval( const DataSourceRef &dataSource, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT].setShaderData( dataSource, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::tessellationEval( const std::string &souceText, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT].setShaderData( souceText, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::tessellationEval( const std::vector<uint32_t> &spirvBinary, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT].setShaderData( spirvBinary, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::compute( const DataSourceRef &dataSource, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_COMPUTE_BIT].setShaderData( dataSource, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::compute( const std::string &souceText, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_COMPUTE_BIT].setShaderData( souceText, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::compute( const std::vector<uint32_t> &spirvBinary, const std::string &entryPoint )
{
	mShaderData[VK_SHADER_STAGE_COMPUTE_BIT].setShaderData( spirvBinary, entryPoint );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::binding( const std::string& bindingName, uint32_t bindingNumber, uint32_t setNumber  )
{
	ShaderProg::Format::Binding binding = ShaderProg::Format::Binding( bindingNumber, setNumber );
	mBindings[bindingName] = binding;
	return *this;
}

ShaderProg::Format& ShaderProg::Format::set( uint32_t setNumber, uint32_t changeFrequency )
{
	mSets[setNumber] = changeFrequency;
	return *this;
}

ShaderProg::Format& ShaderProg::Format::attribute(geom::Attrib semantic, int32_t location, int32_t binding, GlslAttributeDataType type)
{
	mAttributes.push_back( Attribute( semantic, location, binding, type ) );
	return *this;
}

ShaderProg::Format& ShaderProg::Format::uniformLayout( const UniformLayout& layout )
{
	mUniformLayout = layout;
	mUserDefinedUniformLayout = true;
	return *this;
}

ShaderProg::Format& ShaderProg::Format::blockNameTranslateFn( BlockNameTranslateFn fn )
{
	mBlockNameTranslateFn = fn;
	return *this;
}

// -------------------------------------------------------------------------------------------------
// ShaderProg
// -------------------------------------------------------------------------------------------------
ShaderProg::ShaderProg( const ShaderProg::Format &format, vk::Device *device )
	: BaseDeviceObject( device )
{
	initialize( format );
}

ShaderProg::~ShaderProg()
{
	destroy();
}

void initResources( TBuiltInResource &resources )
{
    resources.maxLights = 32;
    resources.maxClipPlanes = 6;
    resources.maxTextureUnits = 32;
    resources.maxTextureCoords = 32;
    resources.maxVertexAttribs = 64;
    resources.maxVertexUniformComponents = 4096;
    resources.maxVaryingFloats = 64;
    resources.maxVertexTextureImageUnits = 32;
    resources.maxCombinedTextureImageUnits = 80;
    resources.maxTextureImageUnits = 32;
    resources.maxFragmentUniformComponents = 4096;
    resources.maxDrawBuffers = 32;
    resources.maxVertexUniformVectors = 128;
    resources.maxVaryingVectors = 8;
    resources.maxFragmentUniformVectors = 16;
    resources.maxVertexOutputVectors = 16;
    resources.maxFragmentInputVectors = 15;
    resources.minProgramTexelOffset = -8;
    resources.maxProgramTexelOffset = 7;
    resources.maxClipDistances = 8;
    resources.maxComputeWorkGroupCountX = 65535;
    resources.maxComputeWorkGroupCountY = 65535;
    resources.maxComputeWorkGroupCountZ = 65535;
    resources.maxComputeWorkGroupSizeX = 1024;
    resources.maxComputeWorkGroupSizeY = 1024;
    resources.maxComputeWorkGroupSizeZ = 64;
    resources.maxComputeUniformComponents = 1024;
    resources.maxComputeTextureImageUnits = 16;
    resources.maxComputeImageUniforms = 8;
    resources.maxComputeAtomicCounters = 8;
    resources.maxComputeAtomicCounterBuffers = 1;
    resources.maxVaryingComponents = 60;
    resources.maxVertexOutputComponents = 64;
    resources.maxGeometryInputComponents = 64;
    resources.maxGeometryOutputComponents = 128;
    resources.maxFragmentInputComponents = 128;
    resources.maxImageUnits = 8;
    resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
    resources.maxCombinedShaderOutputResources = 8;
    resources.maxImageSamples = 0;
    resources.maxVertexImageUniforms = 0;
    resources.maxTessControlImageUniforms = 0;
    resources.maxTessEvaluationImageUniforms = 0;
    resources.maxGeometryImageUniforms = 0;
    resources.maxFragmentImageUniforms = 8;
    resources.maxCombinedImageUniforms = 8;
    resources.maxGeometryTextureImageUnits = 16;
    resources.maxGeometryOutputVertices = 256;
    resources.maxGeometryTotalOutputComponents = 1024;
    resources.maxGeometryUniformComponents = 1024;
    resources.maxGeometryVaryingComponents = 64;
    resources.maxTessControlInputComponents = 128;
    resources.maxTessControlOutputComponents = 128;
    resources.maxTessControlTextureImageUnits = 16;
    resources.maxTessControlUniformComponents = 1024;
    resources.maxTessControlTotalOutputComponents = 4096;
    resources.maxTessEvaluationInputComponents = 128;
    resources.maxTessEvaluationOutputComponents = 128;
    resources.maxTessEvaluationTextureImageUnits = 16;
    resources.maxTessEvaluationUniformComponents = 1024;
    resources.maxTessPatchComponents = 120;
    resources.maxPatchVertices = 32;
    resources.maxTessGenLevel = 64;
    resources.maxViewports = 16;
    resources.maxVertexAtomicCounters = 0;
    resources.maxTessControlAtomicCounters = 0;
    resources.maxTessEvaluationAtomicCounters = 0;
    resources.maxGeometryAtomicCounters = 0;
    resources.maxFragmentAtomicCounters = 8;
    resources.maxCombinedAtomicCounters = 8;
    resources.maxAtomicCounterBindings = 1;
    resources.maxVertexAtomicCounterBuffers = 0;
    resources.maxTessControlAtomicCounterBuffers = 0;
    resources.maxTessEvaluationAtomicCounterBuffers = 0;
    resources.maxGeometryAtomicCounterBuffers = 0;
    resources.maxFragmentAtomicCounterBuffers = 1;
    resources.maxCombinedAtomicCounterBuffers = 1;
    resources.maxAtomicCounterBufferSize = 16384;
    resources.maxTransformFeedbackBuffers = 4;
    resources.maxTransformFeedbackInterleavedComponents = 64;
    resources.maxCullDistances = 8;
    resources.maxCombinedClipAndCullDistances = 8;
    resources.maxSamples = 4;
    resources.limits.nonInductiveForLoops = 1;
    resources.limits.whileLoops = 1;
    resources.limits.doWhileLoops = 1;
    resources.limits.generalUniformIndexing = 1;
    resources.limits.generalAttributeMatrixVectorIndexing = 1;
    resources.limits.generalVaryingIndexing = 1;
    resources.limits.generalSamplerIndexing = 1;
    resources.limits.generalVariableIndexing = 1;
    resources.limits.generalConstantMatrixVectorIndexing = 1;
}

EShLanguage findLanguage(const VkShaderStageFlagBits shader_type)
{
    switch (shader_type) {
    case VK_SHADER_STAGE_VERTEX_BIT:
        return EShLangVertex;

    case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
        return EShLangTessControl;

    case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
        return EShLangTessEvaluation;

    case VK_SHADER_STAGE_GEOMETRY_BIT:
        return EShLangGeometry;

    case VK_SHADER_STAGE_FRAGMENT_BIT:
        return EShLangFragment;

    case VK_SHADER_STAGE_COMPUTE_BIT:
        return EShLangCompute;

    default:
        return EShLangVertex;
    }
}

//
// Compile a given string containing GLSL into SPV for use by VK
// Return value of false means an error was encountered.
//
static bool glslToSpv( const VkShaderStageFlagBits shader_type, const char *pshader, std::vector<uint32_t> &spirv )
{
    glslang::TProgram& program = *new glslang::TProgram;
    const char *shaderStrings[1];
    TBuiltInResource Resources;
    initResources(Resources);

    // Enable SPIR-V and Vulkan rules when parsing GLSL
    EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

    EShLanguage stage = findLanguage(shader_type);
    glslang::TShader* shader = new glslang::TShader(stage);

    shaderStrings[0] = pshader;
    shader->setStrings(shaderStrings, 1);

    if (! shader->parse(&Resources, 100, false, messages)) {
        puts(shader->getInfoLog());
        puts(shader->getInfoDebugLog());
		std::string msg = shader->getInfoLog();
		throw std::runtime_error( msg );
        //return false; // something didn't work
    }

    program.addShader(shader);

    //
    // Program-level processing...
    //

    if (! program.link(messages)) {
        puts(program.getInfoLog());
        puts(program.getInfoDebugLog());
		std::string msg = program.getInfoLog();
		throw std::runtime_error( msg );
        //return false;
    }

    glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);

    return true;
}

static void sInitializeGlSlang()
{
	static bool sGlSlangIntialized = false;
	if( ! sGlSlangIntialized ) {
		glslang::InitializeProcess();
		//sGlSlangIntialized = true;
	}
}

static void sFinializeGlSlang()
{
	static bool sGlSlangFinalized = false;
	if( ! sGlSlangFinalized ) {
		glslang::FinalizeProcess();
		//sGlSlangFinalized = true;
	}
}

void removeComments( std::string& glslText )
{
	std::vector<std::string> lines = ci::split( glslText, "\n\r" );
	for( auto& line : lines ) {
		size_t pos = line.find( "//" );
		line = line.substr( 0, pos );
	}

	if( ! lines.empty() ) {
		glslText = "";
		for( size_t i = 0; i < lines.size(); ++i ) {
			glslText += lines[i];
			if( i < ( lines.size() - 1 ) ) {
				glslText += "\n";
			}
		}
	}
}

void printResource( const std::string& prefix, const spir2cross::Compiler& compiler, const spir2cross::Resource& res )
{
	uint64_t mask = compiler.get_decoration_mask( res.id );
	auto& type = compiler.get_type( res.type_id );

	// Push constant
	bool isPushConstant = ( spv::StorageClassPushConstant == compiler.get_storage_class( res.id ) );
	// Block
	bool isBlock = ( 0 != ( compiler.get_decoration_mask( type.self ) & ( ( 1ULL << spv::DecorationBlock ) | ( 1ULL << spv::DecorationBufferBlock ) ) ) );
	// Fallback Id
	uint32_t fallbackId = ( ( ! isPushConstant ) && isBlock ) ? res.type_id : res.id;
	// Name
	std::string name = ( ! res.name.empty() ) ? res.name : compiler.get_fallback_name( fallbackId );

	CI_LOG_I( "(" << prefix << ") : " << name );
}

bool compileGlslToSpirv( VkDevice device, VkShaderStageFlagBits shaderStage, const std::string& glslText, std::vector<uint32_t>* outSpirv )
{
	std::vector<uint32_t> spirv;
	bool retVal = glslToSpv( shaderStage, glslText.c_str(), spirv );

	if( retVal ) {
		*outSpirv = spirv;
	}

	return retVal;
}

GlslAttributeDataType spirTypeToGlslAttributeDataType( const spir2cross::SPIRType& spirType )
{
	GlslAttributeDataType result = vk::glsl_attr_unknown;
	switch( spirType.basetype ) {
		case spir2cross::SPIRType::Bool: {
			switch( spirType.vecsize ) {
				case 1: result = glsl_attr_bool; break;
				case 2: result = glsl_attr_bvec2; break;
				case 3: result = glsl_attr_bvec3; break;
				case 4: result = glsl_attr_bvec4; break;
			}
		}
		break;

		case spir2cross::SPIRType::Int: {
			switch( spirType.vecsize ) {
				case 1: result = glsl_attr_int; break;
				case 2: result = glsl_attr_ivec2; break;
				case 3: result = glsl_attr_ivec3; break;
				case 4: result = glsl_attr_ivec4; break;
			}
		}
		break;

		case spir2cross::SPIRType::UInt: {
			switch( spirType.vecsize ) {
				case 1: result = glsl_attr_unsigned_int; break;
				case 2: result = glsl_attr_uvec2; break;
				case 3: result = glsl_attr_uvec3; break;
				case 4: result = glsl_attr_uvec4; break;
			}
		}
		break;

		case spir2cross::SPIRType::Float: {
			switch( spirType.vecsize ) {
				case 1: result = glsl_attr_float; break;
				case 2: result = glsl_attr_vec2; break;
				case 3: result = glsl_attr_vec3; break;
				case 4: result = glsl_attr_vec4; break;
			}
		}
		break;
	}
	return result;
}

GlslUniformDataType spirTypeToGlslUniformDataType( const spir2cross::SPIRType& spirType )
{
	GlslUniformDataType result = vk::glsl_unknown;
	switch( spirType.basetype ) {
		case spir2cross::SPIRType::Bool: {
			switch( spirType.vecsize ) {
				case 1: result = glsl_bool; break;
				case 2: result = glsl_bvec2; break;
				case 3: result = glsl_bvec3; break;
				case 4: result = glsl_bvec4; break;
			}
		}
		break;

		case spir2cross::SPIRType::Int: {
			switch( spirType.vecsize ) {
				case 1: result = glsl_int; break;
				case 2: result = glsl_ivec2; break;
				case 3: result = glsl_ivec3; break;
				case 4: result = glsl_ivec4; break;
			}
		}
		break;

		case spir2cross::SPIRType::UInt: {
			switch( spirType.vecsize ) {
				case 1: result = glsl_unsigned_int; break;
				case 2: result = glsl_uvec2; break;
				case 3: result = glsl_uvec3; break;
				case 4: result = glsl_uvec4; break;
			}
		}
		break;

		case spir2cross::SPIRType::Float: {
			switch( spirType.columns ) {
				case 1: {
					switch( spirType.vecsize ) {
						case 1: result = glsl_float; break;
						case 2: result = glsl_vec2; break;
						case 3: result = glsl_vec3; break;
						case 4: result = glsl_vec4; break;
					}
				}
				break;

				case 2: {
					switch( spirType.vecsize ) {
						case 2: result = glsl_mat2; break;
						case 3: result = glsl_mat2x3; break;
						case 4: result = glsl_mat2x4; break;
					}
				}
				break;

				case 3: {
					switch( spirType.vecsize ) {
						case 2: result = glsl_mat3x2; break;
						case 3: result = glsl_mat3; break;
						case 4: result = glsl_mat3x4; break;
					}
				}
				break;

				case 4: {
					switch( spirType.vecsize ) {
						case 2: result = glsl_mat4x2; break;
						case 3: result = glsl_mat4x3; break;
						case 4: result = glsl_mat4; break;
					}
				}
				break;
			}
		}
		break;
	}
	return result;
}

static std::mutex sGlSlangMutex;

struct ShaderBuildData {
	VkShaderStageFlagBits			stage;
	uint32_t						stageIndex = UINT32_MAX;
	std::string						sourceText;
	std::vector<uint32_t>			spirvBinary;
	std::string						entryPoint;
	bool							needsCompile;
};

bool buildShaderModule( VkDevice device, const std::vector<uint32_t> &spirvBinary, VkShaderModule *shaderModule )
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType	= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext	= nullptr;
	createInfo.flags	= 0;
	createInfo.codeSize	= spirvBinary.size() * sizeof(uint32_t);
	createInfo.pCode	= spirvBinary.data();
	VkResult res = vkCreateShaderModule( device, &createInfo, nullptr, shaderModule );
	return ( VK_SUCCESS == res );
}

void extractAttributeData( const std::unique_ptr<spir2cross::CompilerGLSL>& backCompiler, std::vector<ShaderProg::Attribute> *outShaderAttributes )
{
	if( ! backCompiler ) {
		return;
	}

	for( auto& res : backCompiler->get_shader_resources().stage_inputs ) {
		spir2cross::SPIRType  spirType     = backCompiler->get_type( res.type_id );
		std::string           attrName     = res.name;
		uint32_t              attrLocation = backCompiler->get_decoration( res.id, spv::DecorationLocation );
		uint32_t              attrBinding  = 0;
		GlslAttributeDataType attrDataType = spirTypeToGlslAttributeDataType( spirType );
		geom::Attrib          attrSemantic = attributeNameToSemantic( res.name );

		if( glsl_attr_unknown == attrDataType ) {
			CI_LOG_W( "Unable to determine data type for vertex shader attr " << attrName );
			continue;
		}

		auto it = std::find_if(
			std::begin( *outShaderAttributes ), std::end( *outShaderAttributes ),
			[attrName]( const ShaderProg::Attribute& elem ) -> bool {
				return elem.getName() == attrName;
			}
		);

		// Update
		if( std::end( *outShaderAttributes ) != it ) {
			ShaderProg::Attribute& attr = *it;
			attr.setLocation( attrLocation );
			attr.setBinding( attrBinding );
			attr.setType( attrDataType );
			attr.setSemantic( attrSemantic );
		}
		// Add
		else {
			ShaderProg::Attribute attr = ShaderProg::Attribute( attrName, attrSemantic, static_cast<int32_t>( attrLocation ), static_cast<int32_t>( attrBinding ), attrDataType );
			outShaderAttributes->push_back( attr );
		}
	}

	std::sort( 
		std::begin( *outShaderAttributes ), std::end( *outShaderAttributes ),
		[]( const ShaderProg::Attribute& a, const ShaderProg::Attribute& b ) -> bool {
			return a.getLocation() < b.getLocation();
		}
	);
}

void extractBlock( VkShaderStageFlagBits shaderStage, const spir2cross::Resource& res, const std::unique_ptr<spir2cross::CompilerGLSL>& backCompiler, vk::UniformLayout *outUniformLayout )
{
	const uint64_t kBlockMask = ( 1ULL << spv::DecorationBlock ) | ( 1ULL << spv::DecorationBufferBlock );

	auto&    typeInfo       = backCompiler->get_type( res.type_id );
	bool     isPushConstant = ( spv::StorageClassPushConstant == backCompiler->get_storage_class( res.id ) );
	bool     isBlock        = ( 0 != ( backCompiler->get_decoration_mask( typeInfo.self ) & kBlockMask  ) );
	uint32_t typeId         = ( ( ! isPushConstant && isBlock ) ? res.type_id : res.id );

	auto                  bindingType   = isPushConstant ? vk::UniformLayout::Binding::Type::PUSH_CONSTANTS : vk::UniformLayout::Binding::Type::BLOCK;
	std::string           bindingName   = backCompiler->get_name( res.id );
	uint32_t              bindingNumber = backCompiler->get_decoration( res.id, spv::DecorationBinding );
	uint32_t              bindingSet    = backCompiler->get_decoration( res.id, spv::DecorationDescriptorSet );
	VkShaderStageFlagBits bindingStage  = shaderStage;

	outUniformLayout->addBinding( bindingType, bindingName, bindingNumber, bindingStage, bindingSet );
	outUniformLayout->addSet( bindingSet, CHANGES_DONTCARE );

	for( uint32_t index = 0; index < typeInfo.member_types.size(); ++index ) {
		std::string         memberName     = backCompiler->get_member_name( res.type_id, index );								
		uint32_t            memberId       = typeInfo.member_types[index];
		auto&               memberTypeInfo = backCompiler->get_type( memberId );
		uint32_t            memberOffset   = backCompiler->get_member_decoration( res.type_id, index, spv::DecorationOffset );
		GlslUniformDataType memberDataType = spirTypeToGlslUniformDataType( memberTypeInfo );

		std::string uniformName = bindingName + "." + memberName;
		outUniformLayout->addUniform( uniformName, memberDataType, memberOffset );
	}

	// Block size
	size_t blockSize = backCompiler->get_declared_struct_size( typeInfo );

	// Round up to next multiple of 16			
	size_t paddedSize = ( blockSize + 15 ) & ( ~15 );
	if( ( blockSize != paddedSize ) && ( ! isPushConstant ) ) {
		CI_LOG_I( "Padded uniform block " << bindingName << " from " << blockSize << " bytes to " << paddedSize << " bytes" );
	}

	// Use block size for push constants and padded size for UBOs
	outUniformLayout->setBlockSizeBytes( bindingName, isPushConstant ? blockSize : paddedSize );
	outUniformLayout->sortByOffset();
}

void extractUniformData( VkShaderStageFlagBits shaderStage, const std::unique_ptr<spir2cross::CompilerGLSL>& backCompiler, vk::UniformLayout *outUniformLayout )
{
	if( ! backCompiler ) {
		return;
	}

	// Extract uniform blocks from all shader stages
	for( auto& res : backCompiler->get_shader_resources().uniform_buffers ) {
		extractBlock( shaderStage, res, backCompiler, outUniformLayout );
	}

	// Extract samplers from all shader stages
	for( auto& res : backCompiler->get_shader_resources().sampled_images ) {
		std::string           bindingName   = backCompiler->get_name( res.id );
		uint32_t              bindingNumber = backCompiler->get_decoration( res.id, spv::DecorationBinding );
		uint32_t              bindingSet    = backCompiler->get_decoration( res.id, spv::DecorationDescriptorSet );
		VkShaderStageFlagBits bindingStage  = shaderStage;
							
		outUniformLayout->addBinding( vk::UniformLayout::Binding::Type::SAMPLER, bindingName, bindingNumber, bindingStage, bindingSet );
		outUniformLayout->addSet( bindingSet, CHANGES_DONTCARE );
	}

	// Extract storage images from all shader stages - but probably only just compute
	for( auto& res : backCompiler->get_shader_resources().storage_images ) {
		std::string           bindingName   = backCompiler->get_name( res.id );
		uint32_t              bindingNumber = backCompiler->get_decoration( res.id, spv::DecorationBinding );
		uint32_t              bindingSet    = backCompiler->get_decoration( res.id, spv::DecorationDescriptorSet );
		VkShaderStageFlagBits bindingStage  = shaderStage;
							
		outUniformLayout->addBinding( vk::UniformLayout::Binding::Type::STORAGE_IMAGE, bindingName, bindingNumber, bindingStage, bindingSet );
		outUniformLayout->addSet( bindingSet, CHANGES_DONTCARE );
	}

	// Extract storage buffers from all shader stages - but probably only just compute
	for( auto& res : backCompiler->get_shader_resources().storage_buffers) {
		std::string           bindingName   = backCompiler->get_name( res.id );
		uint32_t              bindingNumber = backCompiler->get_decoration( res.id, spv::DecorationBinding );
		uint32_t              bindingSet    = backCompiler->get_decoration( res.id, spv::DecorationDescriptorSet );
		VkShaderStageFlagBits bindingStage  = shaderStage;

		outUniformLayout->addBinding( vk::UniformLayout::Binding::Type::STORAGE_BUFFER, bindingName, bindingNumber, bindingStage, bindingSet );
		outUniformLayout->addSet( bindingSet, CHANGES_DONTCARE );
	}

	// Extract push constants from all shader stages
	for( auto& res : backCompiler->get_shader_resources().push_constant_buffers ) {
		extractBlock( shaderStage, res, backCompiler, outUniformLayout );
	}
}

void ShaderProg::initialize( const ShaderProg::Format &format )
{
	// Copy attributes
	mAttributes = format.mAttributes;	

	// Copy shader data
	std::map<VkShaderStageFlagBits, Format::ShaderData>	shaderData;
	for( const auto& elem : format.mShaderData ) {
		if( ! elem.second.isInitialized() ) {
			continue;
		}

		shaderData[elem.first] = elem.second;
	}
	// Throw if there aren't any shader stages present
	if( shaderData.empty() ) {
		throw std::runtime_error( "Cannot build shader without any stages defined" );
	}

	// --------------------------------- //
	// @ TODO: Add preprocessor for GLSL //
	// --------------------------------- //

	// Load any pending data
	for( auto& elem : shaderData ) {
		elem.second.loadShader();
	}

	// Allocate shader build data
	std::vector<ShaderBuildData> shaderBuildData = std::vector<ShaderBuildData>( shaderData.size() );

	// Set shader build data
	{
		const std::vector<VkShaderStageFlagBits> kOrderedShaderStageBits = { 
			VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT, VK_SHADER_STAGE_GEOMETRY_BIT, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, VK_SHADER_STAGE_COMPUTE_BIT 
		};

		uint32_t shaderStageIndex = 0;
		for( const auto& shaderStageBit : kOrderedShaderStageBits ) {
			auto it = shaderData.find( shaderStageBit );
			if( shaderData.end() == it ) {
				continue;
			}

			const auto& shaderData = it->second;
			shaderBuildData[shaderStageIndex].stage			= shaderStageBit;
			shaderBuildData[shaderStageIndex].stageIndex	= shaderStageIndex;
			shaderBuildData[shaderStageIndex].sourceText	= shaderData.getSourceText();
			shaderBuildData[shaderStageIndex].spirvBinary	= shaderData.getSpirvBinary();
			shaderBuildData[shaderStageIndex].entryPoint	= shaderData.getEntryPoint();
			shaderBuildData[shaderStageIndex].needsCompile	= ( ShaderProg::Format::ShaderData::Type::GLSL == shaderData.getDataType() ) ? true : false;

			++shaderStageIndex;
		}
	}

	// Initialize entry points and pipeline shader stages
	{
		mEntryPoints.resize( shaderBuildData.size() );
		mPipelineShaderStages.resize( shaderBuildData.size() );

		// Set member initial values
		for( size_t i = 0; i < mPipelineShaderStages.size(); ++i ) {
			mEntryPoints[i] = shaderBuildData[i].entryPoint;
			// 
			mPipelineShaderStages[i].sType					= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			mPipelineShaderStages[i].pNext					= nullptr;
			mPipelineShaderStages[i].flags					= 0;
			mPipelineShaderStages[i].stage					= shaderBuildData[i].stage;
			mPipelineShaderStages[i].module					= VK_NULL_HANDLE;
			mPipelineShaderStages[i].pName					= mEntryPoints[i].c_str();
			mPipelineShaderStages[i].pSpecializationInfo	= nullptr;
		}
	}
	
	// Compile and build shader
	{
		std::lock_guard<std::mutex> glslangLock( sGlSlangMutex );

		sInitializeGlSlang();

		// Build each stage separate as required by Vulkan
		for( auto& shader : shaderBuildData ) {
			// Compile shader if needed
			if( shader.needsCompile ) {
				try {
					if( ! compileGlslToSpirv( mDevice->getDevice(), shader.stage, shader.sourceText, &(shader.spirvBinary) ) ) {
						throw std::runtime_error( "Compiling GLSL to SPIR-V failed" );
					}
				}
				catch( const std::exception& e ) {
					std::stringstream ss;
					ss << "("<< toStringVkShaderStageFlagBits( shader.stage ) << ") " << e.what();
					throw std::runtime_error( ss.str() );
				}
			}

			// Throw if SPIR-V isn't available
			if( shader.spirvBinary.empty() ) {
				std::string msg = "No SPIR-V data found for shader stage " + toStringVkShaderStageFlagBits( shader.stage );
				throw std::runtime_error(  msg );
			}

			// Build shader module for this stage
			if( ! buildShaderModule( mDevice->getDevice(), shader.spirvBinary, &(mPipelineShaderStages[shader.stageIndex].module) ) ) {
				std::string msg = "Couldn't create module for shader stage " + toStringVkShaderStageFlagBits( shader.stage );
				throw std::runtime_error(  msg );
			}

			// Extract attributes and uniforms
			{
				auto backCompiler = std::unique_ptr<spir2cross::CompilerGLSL>( new spir2cross::CompilerGLSL( shader.spirvBinary ) );
				if( ! backCompiler ) {
					std::string msg = "Couldn't back compile SPIR-V for shader stage " + toStringVkShaderStageFlagBits( shader.stage );
					throw std::runtime_error(  msg );
				}

				// Extract shader attributes
				if( VK_SHADER_STAGE_VERTEX_BIT == shader.stage ) {
					extractAttributeData( backCompiler, &mAttributes );
				}

				// Extract uniform data
				if( ! format.userDefinedUniformLayout() ) {
					extractUniformData( shader.stage, backCompiler, &mUniformLayout );
				}
			}
		}

		// Process sets
		{	
			// Add sets from bindings - in case they're sparsely populated
			for( auto& binding : format.mBindings ) {
				mUniformLayout.setSet( binding.second.setNumber, CHANGES_DONTCARE );
			}

			// Update sets from ... sets
			for( const auto& it : format.mSets ) {
				mUniformLayout.setSet( it.first, it.second );
			}

			// If no sets by now, add a default set with set number 0.
			if( mUniformLayout.getSets().empty() ) {
				mUniformLayout.setSet( 0, CHANGES_DONTCARE );
			}
		}

		sFinializeGlSlang();
	}

	// Build a preferred vertex layout
	for( const auto &attrib : mAttributes ) {
		geom::Attrib semantic = attrib.getSemantic();
		uint8_t dim = glslAttributeTypeDim( attrib.getType() );
		if( dim ) {
			mVertexLayout.attrib( semantic, dim );
		}
	}

	mDevice->trackedObjectCreated( this );
}

void ShaderProg::destroy( bool removeFromTracking )
{
	if( mPipelineShaderStages.empty() ) {
		return;
	}

	for( auto& shaderStage : mPipelineShaderStages ) {
		vkDestroyShaderModule( mDevice->getDevice(), shaderStage.module, nullptr );
		shaderStage.module = VK_NULL_HANDLE;
	}
	mPipelineShaderStages.clear();

	if( removeFromTracking ) {
		mDevice->trackedObjectDestroyed( this );
	}
}

ShaderProgRef ShaderProg::create( const ShaderProg::Format &format, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	ShaderProgRef result = ShaderProgRef( new ShaderProg( format, device ) );
	return result;
}

ShaderProgRef ShaderProg::create( DataSourceRef vertexShader, DataSourceRef fragmentShader, vk::Device *device )
{
	ShaderProg::Format format = ShaderProg::Format()
		.vertex( vertexShader )
		.fragment( fragmentShader );
	return ShaderProg::create( format, device );
}

ShaderProgRef ShaderProg::create( const std::string &vertexShader, const std::string &fragmentShader, vk::Device *device )
{
	ShaderProg::Format format = ShaderProg::Format()
		.vertex( vertexShader )
		.fragment( fragmentShader );
	return ShaderProg::create( format, device );
}

bool ShaderProg::isCompute() const
{
	bool result = ( 1 == mPipelineShaderStages.size() ) && ( VK_SHADER_STAGE_COMPUTE_BIT == mPipelineShaderStages[0].stage );
	return result;
}

void ShaderProg::uniform( const std::string& name, const float value )
{
	mUniformLayout.uniform( name, value );
}

void ShaderProg::uniform( const std::string& name, const int32_t value )
{
	mUniformLayout.uniform( name, value );
}

void ShaderProg::uniform( const std::string& name, const uint32_t value )
{
	mUniformLayout.uniform( name, value );
}

void ShaderProg::uniform( const std::string& name, const bool value )
{
	mUniformLayout.uniform( name, value );
}

void ShaderProg::uniform( const std::string& name, const vec2& value )
{
	mUniformLayout.uniform( name, value );
}

void ShaderProg::uniform( const std::string& name, const vec3& value )
{
	mUniformLayout.uniform( name, value );
}

void ShaderProg::uniform( const std::string& name, const vec4& value )
{
	mUniformLayout.uniform( name, value );
}

void ShaderProg::uniform( const std::string& name, const ivec2& value )
{
	mUniformLayout.uniform( name, value );
}

void ShaderProg::uniform( const std::string& name, const ivec3& value )
{
	mUniformLayout.uniform( name, value );
}

void ShaderProg::uniform( const std::string& name, const ivec4& value )
{
	mUniformLayout.uniform( name, value );
}

void ShaderProg::uniform( const std::string& name, const uvec2& value )
{
	mUniformLayout.uniform( name, value );
}

void ShaderProg::uniform( const std::string& name, const uvec3& value )
{
	mUniformLayout.uniform( name, value );
}

void ShaderProg::uniform( const std::string& name, const uvec4& value )
{
	mUniformLayout.uniform( name, value );
}

void ShaderProg::uniform( const std::string& name, const mat2& value )
{
	mUniformLayout.uniform( name, value );
}

void ShaderProg::uniform( const std::string& name, const mat3& value )
{
	mUniformLayout.uniform( name, value );
}

void ShaderProg::uniform( const std::string& name, const mat4& value )
{
	mUniformLayout.uniform( name, value );
}

void ShaderProg::uniform( const std::string& name, const TextureBaseRef& texture )
{
	mUniformLayout.uniform( name, texture );
}

std::string ShaderProg::defaultBlockNameTranslate( const std::string& name )
{
	std::string result = name;
	std::vector<std::string> tokens = ci::split( name, "." );
	if( tokens[0].length() > 1 ) {
		std::string renamed = tokens[0];
		if( 'T' == renamed.back() ) {
			size_t len = renamed.length() - 1;
			renamed = renamed.substr( 0, len );
		}
		result = renamed;
		for( size_t i = 1; i < tokens.size(); ++i ) {
			result += ( "." + tokens[i] );
		}
	}
	return result;
}

}} // namespace cinder::vk