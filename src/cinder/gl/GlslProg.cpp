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

#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/ConstantConversions.h"
#include "cinder/gl/Environment.h"
#include "cinder/gl/ShaderPreprocessor.h"
#include "cinder/gl/scoped.h"
#include "cinder/Log.h"

#include "glm/gtc/type_ptr.hpp"

#include <type_traits>

using namespace std;

namespace cinder { namespace gl {
    
static GlslProg::UniformSemanticMap	sDefaultUniformNameToSemanticMap;
static GlslProg::AttribSemanticMap	sDefaultAttribNameToSemanticMap;
	
class UniformBuffer {
public:
	UniformBuffer( uint32_t bufferSize )
	: mBuffer( new uint8_t[bufferSize] ),
	mBufferSize( bufferSize )
	{
		memset( mBuffer, std::numeric_limits<int>::max(), mBufferSize );
	}
	
	UniformBuffer( const UniformBuffer &other ) = delete;
	UniformBuffer& operator=( const UniformBuffer &rhs ) = delete;
	UniformBuffer( UniformBuffer &&other ) = delete;
	UniformBuffer& operator=( UniformBuffer &&rhs ) = delete;
	
	~UniformBuffer()
	{
		auto ptr = (uint8_t*)mBuffer;
		delete [] ptr;
	}
	
	bool shouldBuffer( uint32_t beginningByte, uint32_t size, const void * valuePointer )
	{
		auto ptr = ((uint8_t*)mBuffer + beginningByte);
		if( memcmp( ptr, valuePointer, size ) == 0 ) {
			return false;
		}
		else {
			memcpy( ptr, valuePointer, size );
			return true;
		}
	}
	
private:
	void*		mBuffer;
	uint32_t	mBufferSize;
};

//////////////////////////////////////////////////////////////////////////
// GlslProg::Format
GlslProg::Format::Format()
	: mPreprocessingEnabled( true ), mVersion( 0 )
#if ! defined( CINDER_GL_ES_2 )
	, mTransformFormat( -1 )
#endif
{
}

GlslProg::Format& GlslProg::Format::vertex( const DataSourceRef &dataSource )
{
	setShaderSource( dataSource, &mVertexShader, &mVertexShaderPath );
	return *this;
}

GlslProg::Format& GlslProg::Format::vertex( const string &vertexShader )
{
	setShaderSource( vertexShader, &mVertexShader, &mVertexShaderPath );
	return *this;
}

GlslProg::Format& GlslProg::Format::fragment( const DataSourceRef &dataSource )
{
	setShaderSource( dataSource, &mFragmentShader, &mFragmentShaderPath );
	return *this;
}

GlslProg::Format& GlslProg::Format::fragment( const string &fragmentShader )
{
	setShaderSource( fragmentShader, &mFragmentShader, &mFragmentShaderPath );
	return *this;
}

#if ! defined( CINDER_GL_ES )
GlslProg::Format& GlslProg::Format::geometry( const DataSourceRef &dataSource )
{
	setShaderSource( dataSource, &mGeometryShader, &mGeometryShaderPath );
	return *this;
}

GlslProg::Format& GlslProg::Format::geometry( const string &geometryShader )
{
	setShaderSource( geometryShader, &mGeometryShader, &mGeometryShaderPath );
	return *this;
}

GlslProg::Format& GlslProg::Format::tessellationCtrl( const DataSourceRef &dataSource )
{
	setShaderSource( dataSource, &mTessellationCtrlShader, &mTessellationCtrlShaderPath );
	return *this;
}

GlslProg::Format& GlslProg::Format::tessellationCtrl( const string &tessellationCtrlShader )
{
	setShaderSource( tessellationCtrlShader, &mTessellationCtrlShader, &mTessellationCtrlShaderPath );
	return *this;
}

GlslProg::Format& GlslProg::Format::tessellationEval( const DataSourceRef &dataSource )
{
	setShaderSource( dataSource, &mTessellationEvalShader, &mTessellationEvalShaderPath );
	return *this;
}

GlslProg::Format& GlslProg::Format::tessellationEval( const string &tessellationEvalShader )
{
	setShaderSource( tessellationEvalShader, &mTessellationEvalShader, &mTessellationEvalShaderPath );
	return *this;
}

#endif // ! defined( CINDER_GL_ES )

void GlslProg::Format::setShaderSource( const DataSourceRef &dataSource, string *shaderSourceDest, fs::path *shaderPathDest )
{
	if( dataSource ) {
		Buffer buffer( dataSource );
		shaderSourceDest->resize( buffer.getDataSize() + 1 );
		memcpy( (void *)shaderSourceDest->data(), buffer.getData(), buffer.getDataSize() );
		(*shaderSourceDest)[buffer.getDataSize()] = 0;
		if( dataSource->isFilePath() )
			*shaderPathDest = dataSource->getFilePath();
		else
			shaderPathDest->clear();
	}
	else {
		shaderSourceDest->clear();
		shaderPathDest->clear();
	}
}

void GlslProg::Format::setShaderSource( const std::string &source, std::string *shaderSourceDest, fs::path *shaderPathDest )
{
	*shaderSourceDest = source;
	shaderPathDest->clear();
}

GlslProg::Format& GlslProg::Format::attrib( geom::Attrib semantic, const std::string &attribName )
{
    bool exists = false;
    for( auto & attrib : mAttributes ) {
        if( attrib.mName == attribName ) {
            attrib.mSemantic = semantic;
            exists = true;
            break;
        }
        else if( attrib.mSemantic == semantic ) {
            attrib.mName = attribName;
            exists = true;
            break;
        }
    }
    if( ! exists ) {
        Attribute attrib;
        attrib.mName = attribName;
        attrib.mSemantic = semantic;
        mAttributes.push_back( attrib );
    }
	return *this;
}

GlslProg::Format& GlslProg::Format::uniform( UniformSemantic semantic, const std::string &uniformName )
{
    bool exists = false;
    for( auto & uniform : mUniforms ) {
        if( uniform.mName == uniformName ) {
            uniform.mSemantic = semantic;
            exists = true;
            break;
        }
        else if( uniform.mSemantic == semantic ) {
            uniform.mName = uniformName;
            exists = true;
            break;
        }
    }
    if( ! exists ) {
        Uniform uniform;
        uniform.mName = uniformName;
        uniform.mSemantic = semantic;
        mUniforms.push_back( uniform );
    }
	return *this;
}

GlslProg::Format& GlslProg::Format::define( const std::string &define )
{
	mDefineDirectives.push_back( define );
	return *this;
}

GlslProg::Format& GlslProg::Format::define( const std::string &define, const std::string &value )
{
	mDefineDirectives.push_back( define + " " + value );
	return *this;
}

GlslProg::Format& GlslProg::Format::defineDirectives( const std::vector<std::string> &defines )
{
	mDefineDirectives = defines;
	return *this;
}

GlslProg::Format& GlslProg::Format::version( int version )
{
	mVersion = version;
	return *this;
}

GlslProg::Format& GlslProg::Format::attribLocation( const std::string &attribName, GLint location )
{
    bool exists = false;
    for( auto & attrib : mAttributes ) {
        if( attrib.mName == attribName ) {
            attrib.mLoc = location;
            exists = true;
            break;
        }
        else if( attrib.mLoc == location ) {
            attrib.mName = attribName;
            exists = true;
            break;
        }
    }
    if( ! exists ) {
        Attribute attrib;
        attrib.mName = attribName;
        attrib.mLoc = location;
        mAttributes.push_back( attrib );
    }
	return *this;
}

GlslProg::Format& GlslProg::Format::attribLocation( geom::Attrib attribSemantic, GLint location )
{
	bool exists = false;
	for( auto & attrib : mAttributes ) {
		if( attrib.mSemantic == attribSemantic ) {
			attrib.mLoc = location;
			exists = true;
			break;
		}
		else if( attrib.mLoc == location ) {
			attrib.mSemantic = attribSemantic;
			exists = true;
			break;
		}
	}
	if( ! exists ) {
		Attribute attrib;
		attrib.mSemantic = attribSemantic;
		attrib.mLoc = location;
		mAttributes.push_back( attrib );
	}
	return *this;
}

#if ! defined( CINDER_GL_ES )
GlslProg::Format& GlslProg::Format::fragDataLocation( GLuint colorNumber, const std::string &name )
{
	mFragDataLocations[name] = colorNumber;
	return *this;
}
#endif

//////////////////////////////////////////////////////////////////////////
// GlslProg statics

GlslProgRef GlslProg::create( const Format &format )
{
	return GlslProgRef( new GlslProg( format ) );
}

#if ! defined( CINDER_GL_ES )
GlslProgRef GlslProg::create( DataSourceRef vertexShader, DataSourceRef fragmentShader, DataSourceRef geometryShader, DataSourceRef tessEvalShader, DataSourceRef tessCtrlShader )
{
	return GlslProgRef( new GlslProg( GlslProg::Format().vertex( vertexShader ).fragment( fragmentShader ).geometry( geometryShader ).tessellationEval( tessEvalShader ).tessellationCtrl( tessCtrlShader ) ) );
}

GlslProgRef GlslProg::create( const string &vertexShader, const string &fragmentShader, const string &geometryShader, const string &tessEvalShader, const string &tessCtrlShader )
{
	return GlslProgRef( new GlslProg( GlslProg::Format().vertex( vertexShader ).fragment( fragmentShader ).geometry( geometryShader ).tessellationEval( tessEvalShader ).tessellationCtrl( tessCtrlShader ) ) );
}
	
#else
GlslProgRef GlslProg::create( DataSourceRef vertexShader, DataSourceRef fragmentShader )
{
	return GlslProgRef( new GlslProg( GlslProg::Format().vertex( vertexShader ).fragment( fragmentShader ) ) );
}

GlslProgRef GlslProg::create( const string &vertexShader, const string &fragmentShader )
{
	return GlslProgRef( new GlslProg( GlslProg::Format().vertex( vertexShader ).fragment( fragmentShader ) ) );
}
	
#endif
	
GlslProg::~GlslProg()
{
	auto ctx = gl::context();
	if( ctx )
		ctx->glslProgDeleted( this );

	if( mHandle ) {
		glDeleteProgram( (GLuint)mHandle );
	}
}

//////////////////////////////////////////////////////////////////////////
// GlslProg

GlslProg::GlslProg( const Format &format )
: mPreprocessingEnabled( format.isPreprocessingEnabled() ), mUniformBuffer( nullptr )
#if ! defined( CINDER_GL_ES_2 )
 , mTransformFeedbackFormat( -1 )
#endif
{
	mHandle = glCreateProgram();

	if( format.isPreprocessingEnabled() ) {
		mShaderPreprocessor.reset( new ShaderPreprocessor );

		// copy the Format's define directives vector
		for( const auto &define : format.getDefineDirectives() )
			mShaderPreprocessor->addDefine( define );

		if( format.getVersion() )
			mShaderPreprocessor->setVersion( format.getVersion() );
	}

	if( ! format.getVertex().empty() )
		loadShader( format.getVertex(), format.mVertexShaderPath, GL_VERTEX_SHADER );
	if( ! format.getFragment().empty() )
		loadShader( format.getFragment(), format.mFragmentShaderPath, GL_FRAGMENT_SHADER );
#if ! defined( CINDER_GL_ES )
	if( ! format.getGeometry().empty() )
		loadShader( format.getGeometry(), format.mGeometryShaderPath, GL_GEOMETRY_SHADER );
	if( ! format.getTessellationCtrl().empty() )
		loadShader( format.getTessellationCtrl(), format.mTessellationCtrlShaderPath, GL_TESS_CONTROL_SHADER );
	if( ! format.getTessellationEval().empty() )
		loadShader( format.getTessellationEval(), format.mTessellationEvalShaderPath, GL_TESS_EVALUATION_SHADER );
#endif
    
    auto & userDefinedAttribs = format.getAttributes();
	
	// if the user has provided a location make sure to bind that location before
	// we go further, still don't know that this is good.
	for( auto &attrib : userDefinedAttribs )
		if( attrib.mLoc > -1 )
			glBindAttribLocation( mHandle, attrib.mLoc, attrib.mName.c_str() );
	
#if ! defined( CINDER_GL_ES_2 )
	if( ! format.getVaryings().empty() && format.getTransformFormat() > 0 ) {
		auto & userVaryings = format.getVaryings();
		mTransformFeedbackVaryings.resize( format.getVaryings().size() );
		std::vector<const GLchar*> transformFeedbackStarts;
		int i = 0;
		// This is a mess due to an NVidia driver bug on MSW which expects the memory passed to glTransformFeedbackVaryings
		// to still be around after the call. We allocate the storage and put it on the GlslProg itself to be freed at destruction
		for( auto & transformFeedbackVaryings : mTransformFeedbackVaryings ) {
			transformFeedbackVaryings.mName = userVaryings[i++];
			transformFeedbackStarts.push_back( transformFeedbackVaryings.mName.c_str() );
		}
		mTransformFeedbackFormat = format.getTransformFormat();
		glTransformFeedbackVaryings( mHandle, (GLsizei)mTransformFeedbackVaryings.size(), transformFeedbackStarts.data(), mTransformFeedbackFormat );
	}
#endif

#if ! defined( CINDER_GL_ES )
	// setup fragment data locations
	for( const auto &fragDataLocation : format.getFragDataLocations() )
		glBindFragDataLocation( mHandle, fragDataLocation.second, fragDataLocation.first.c_str() );
#endif

	link();
	
	cacheActiveAttribs();
#if ! defined( CINDER_GL_ES_2 )
	if( ! format.getVaryings().empty() ) {
		cacheActiveTransformFeedbackVaryings();
	}
	cacheActiveUniformBlocks();
#endif
	cacheActiveUniforms();
	
	auto & userDefinedUniforms = format.getUniforms();
	// check if the user thinks there's a uniform that isn't active
	for( auto &userUniform : userDefinedUniforms ) {
		bool foundUserDefined = false;
		for( auto & activeUniform : mUniforms ) {
			// if the user defined name and the active name are the same
			if( userUniform.mName == activeUniform.mName ) {
				foundUserDefined = true;
				// If we've found the uniform, change the semantic to the
				// user defined semantic.
				activeUniform.mSemantic = userUniform.mSemantic;
				break;
			}
		}
		if( ! foundUserDefined ) {
			CI_LOG_E( "Unknown uniform: \"" << userUniform.mName << "\"" );
			mLoggedUniformNames.insert( userUniform.mName );
		}
	}
	// make sure we get all of the semantic info correct from the user
	for( auto &userAttrib : userDefinedAttribs ) {
		bool active = false;
		for( auto &activeAttrib : mAttributes ) {
			// check if either the user defined name or user defined loc for
			// this attrib are the same as the active attribute
			if( userAttrib.mName == activeAttrib.mName ||
			    userAttrib.mLoc == activeAttrib.mLoc ) {
				// we've found the user defined attribute
				active = true;
				// check if the user defined attribute has cached what type
				// of semantic this is
				if( userAttrib.mSemantic != geom::Attrib::NUM_ATTRIBS ) {
					activeAttrib.mSemantic = userAttrib.mSemantic;
				}
				break;
			}
		}
		if( !active ) {
			CI_LOG_E( "Unknown attribute: \"" << userAttrib.mName << "\"" );
		}
	}
    
	setLabel( format.getLabel() );
	gl::context()->glslProgCreated( this );
}

GlslProg::UniformSemanticMap& GlslProg::getDefaultUniformNameToSemanticMap()
{
	static bool initialized = false;
	if( ! initialized ) {
		sDefaultUniformNameToSemanticMap["ciModelMatrix"] = UNIFORM_MODEL_MATRIX;
		sDefaultUniformNameToSemanticMap["ciModelMatrixInverse"] = UNIFORM_MODEL_MATRIX_INVERSE;
		sDefaultUniformNameToSemanticMap["ciModelMatrixInverseTranspose"] = UNIFORM_MODEL_MATRIX_INVERSE_TRANSPOSE;
		sDefaultUniformNameToSemanticMap["ciViewMatrix"] = UNIFORM_VIEW_MATRIX;
		sDefaultUniformNameToSemanticMap["ciViewMatrixInverse"] = UNIFORM_VIEW_MATRIX_INVERSE;
		sDefaultUniformNameToSemanticMap["ciModelView"] = UNIFORM_MODEL_VIEW;
		sDefaultUniformNameToSemanticMap["ciModelViewInverse"] = UNIFORM_MODEL_VIEW_INVERSE;
		sDefaultUniformNameToSemanticMap["ciModelViewInverseTranspose"] = UNIFORM_MODEL_VIEW_INVERSE_TRANSPOSE;
		sDefaultUniformNameToSemanticMap["ciModelViewProjection"] = UNIFORM_MODEL_VIEW_PROJECTION;
		sDefaultUniformNameToSemanticMap["ciModelViewProjectionInverse"] = UNIFORM_MODEL_VIEW_PROJECTION_INVERSE;
		sDefaultUniformNameToSemanticMap["ciProjectionMatrix"] = UNIFORM_PROJECTION_MATRIX;
		sDefaultUniformNameToSemanticMap["ciProjectionMatrixInverse"] = UNIFORM_PROJECTION_MATRIX_INVERSE;
		sDefaultUniformNameToSemanticMap["ciNormalMatrix"] = UNIFORM_NORMAL_MATRIX;
		sDefaultUniformNameToSemanticMap["ciViewportMatrix"] = UNIFORM_VIEWPORT_MATRIX;
		sDefaultUniformNameToSemanticMap["ciWindowSize"] = UNIFORM_WINDOW_SIZE;
		sDefaultUniformNameToSemanticMap["ciElapsedSeconds"] = UNIFORM_ELAPSED_SECONDS;
		initialized = true;
	}
	
	return sDefaultUniformNameToSemanticMap;
}

GlslProg::AttribSemanticMap& GlslProg::getDefaultAttribNameToSemanticMap()
{
	static bool initialized = false;
	if( ! initialized ) {
		sDefaultAttribNameToSemanticMap["ciPosition"] = geom::Attrib::POSITION;
		sDefaultAttribNameToSemanticMap["ciNormal"] = geom::Attrib::NORMAL;
		sDefaultAttribNameToSemanticMap["ciTangent"] = geom::Attrib::TANGENT;
		sDefaultAttribNameToSemanticMap["ciBitangent"] = geom::Attrib::BITANGENT;
		sDefaultAttribNameToSemanticMap["ciTexCoord0"] = geom::Attrib::TEX_COORD_0;
		sDefaultAttribNameToSemanticMap["ciTexCoord1"] = geom::Attrib::TEX_COORD_1;
		sDefaultAttribNameToSemanticMap["ciTexCoord2"] = geom::Attrib::TEX_COORD_2;
		sDefaultAttribNameToSemanticMap["ciTexCoord3"] = geom::Attrib::TEX_COORD_3;
		sDefaultAttribNameToSemanticMap["ciColor"] = geom::Attrib::COLOR;
		sDefaultAttribNameToSemanticMap["ciBoneIndex"] = geom::Attrib::BONE_INDEX;
		sDefaultAttribNameToSemanticMap["ciBoneWeight"] = geom::Attrib::BONE_WEIGHT;
		initialized = true;
	}
	
	return sDefaultAttribNameToSemanticMap;
}

void GlslProg::loadShader( const string &shaderSource, const fs::path &shaderPath, GLint shaderType )
{
	GLuint handle = glCreateShader( shaderType );
	if( mShaderPreprocessor ) {
		string preprocessedSource = mShaderPreprocessor->parse( shaderSource, shaderPath );
		const char *cStr = preprocessedSource.c_str();
		glShaderSource( handle, 1, reinterpret_cast<const GLchar**>( &cStr ), NULL );
	}
	else {
		const char *cStr = shaderSource.c_str();
		glShaderSource( handle, 1, reinterpret_cast<const GLchar**>( &cStr ), NULL );
	}

	glCompileShader( handle );
	
	GLint status;
	glGetShaderiv( (GLuint) handle, GL_COMPILE_STATUS, &status );
	if( status != GL_TRUE ) {
		std::string log = getShaderLog( (GLuint)handle );
		throw GlslProgCompileExc( log, shaderType );
	}
	glAttachShader( mHandle, handle );
}

void GlslProg::link()
{
	glLinkProgram( mHandle );
	
	// test for a GLSL link error and throw it if we have one
	GLint status;
	glGetProgramiv( mHandle, GL_LINK_STATUS, &status );
	if( status != GL_TRUE ) {
		string log;
		GLint logLength = 0;
		GLint charsWritten = 0;
		glGetProgramiv( mHandle, GL_INFO_LOG_LENGTH, &logLength );
		
		if( logLength > 0 ) {
			unique_ptr<GLchar[]> debugLog( new GLchar[logLength+1] );
			glGetProgramInfoLog( mHandle, logLength, &charsWritten, debugLog.get() );
			log.append( debugLog.get(), 0, logLength );
		}
		
		throw GlslProgLinkExc( log );
	}
}
	
void GlslProg::cacheActiveAttribs()
{
	GLint numActiveAttrs = 0;
	glGetProgramiv( mHandle, GL_ACTIVE_ATTRIBUTES, &numActiveAttrs );
	
	auto & semanticNameMap = getDefaultAttribNameToSemanticMap();
	for( GLint i = 0; i < numActiveAttrs; ++i ) {
		char name[512];
		GLsizei nameLength;
		GLint count;
		GLenum type;
		
		glGetActiveAttrib( mHandle, (GLuint)i, 511, &nameLength, &count, &type, name );
		auto loc = glGetAttribLocation( mHandle, name );
		
		geom::Attrib attributeSemantic = geom::Attrib::NUM_ATTRIBS;
		auto foundSemantic = semanticNameMap.find( name );
		if( foundSemantic != semanticNameMap.end() ) {
			attributeSemantic = foundSemantic->second;
		}
		
		Attribute attrib;
		attrib.mName		= name;
		attrib.mLoc			= loc;
		attrib.mCount		= count;
		attrib.mType		= type;
		attrib.mSemantic	= attributeSemantic;
		mAttributes.push_back( attrib );
	}
}
	
void GlslProg::cacheActiveUniforms()
{
	GLint numActiveUniforms = 0;
	glGetProgramiv( mHandle, GL_ACTIVE_UNIFORMS, &numActiveUniforms );
	
	auto & semanticNameMap = getDefaultUniformNameToSemanticMap();
	
	uint32_t uniformBufferSize = 0;
	
	for( GLint i = 0; i < numActiveUniforms; ++i ) {
		char name[512];
		GLsizei nameLength;
		GLint count;
		GLenum type;
		
		glGetActiveUniform( mHandle, (GLuint)i, 511, &nameLength, &count, &type, name );
		auto loc = glGetUniformLocation( mHandle, name );
		// This may be a part of a uniform block, in that case it will be counted as
		// an active uniform but have a location of -1. Disregard as we'll catch it
		// when we cache the uniform block.
		if( loc != -1 ) {
			UniformSemantic uniformSemantic = UniformSemantic::USER_DEFINED_UNIFORM;
			
			auto foundSemantic = semanticNameMap.find( name );
			if( foundSemantic != semanticNameMap.end() ) {
				uniformSemantic = foundSemantic->second;
			}
			
			Uniform uniform;
			uniform.mName		= name;
			uniform.mLoc		= loc;
			uniform.mIndex      = i;
			uniform.mCount		= count;
			uniform.mType		= type;
			uniform.mDataSize	= count * glTypeToBytes( type );
			uniform.mSemantic	= uniformSemantic;
			uniform.mBytePointer = uniformBufferSize;
			uniformBufferSize	+= uniform.mDataSize;
			mUniforms.push_back( uniform );
		}
	}
	
	if ( numActiveUniforms ) {
		mUniformBuffer = new UniformBuffer( uniformBufferSize );
	}
}

#if ! defined( CINDER_GL_ES_2 )
void GlslProg::cacheActiveUniformBlocks()
{
	GLint numActiveUniformBlocks = 0;
	glGetProgramiv( mHandle, GL_ACTIVE_UNIFORM_BLOCKS, &numActiveUniformBlocks );
	
	auto & semanticNameMap = getDefaultUniformNameToSemanticMap();
	for( GLint i = 0; i < numActiveUniformBlocks; i++ ) {
		GLchar name[500];
		GLsizei nameLength;
		GLint blockBinding;
		GLint dataSize;
		GLint numActiveUniforms;
		
		glGetActiveUniformBlockName( mHandle, i, 500, &nameLength, name );
		name[nameLength] = 0;
		
		glGetActiveUniformBlockiv( mHandle, i, GL_UNIFORM_BLOCK_BINDING, &blockBinding );
		glGetActiveUniformBlockiv( mHandle, i, GL_UNIFORM_BLOCK_DATA_SIZE, &dataSize );
		glGetActiveUniformBlockiv( mHandle, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numActiveUniforms );
		
		const GLuint loc = glGetUniformBlockIndex( mHandle, name );
		
		UniformBlock uniformBlock;
		uniformBlock.mName = name;
		uniformBlock.mLoc = loc;
		uniformBlock.mSize = dataSize;
		uniformBlock.mBlockBinding = blockBinding;
		
		std::vector<GLint> uniformIndices( numActiveUniforms );
		glGetActiveUniformBlockiv( mHandle, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniformIndices.data() );
		std::vector<GLuint> unSignedIndices( numActiveUniforms );
		int j = 0;
		for( auto & index : uniformIndices ) {
			char name[512];
			GLsizei nameLength;
			GLint count;
			GLenum type;
			
			glGetActiveUniform( mHandle, (GLuint)index, 511, &nameLength, &count, &type, name );
			
			UniformSemantic uniformSemantic = UniformSemantic::USER_DEFINED_UNIFORM;
			
			auto foundSemantic = semanticNameMap.find( name );
			if( foundSemantic != semanticNameMap.end() ) {
				uniformSemantic = foundSemantic->second;
			}
			
			Uniform uniform;
			uniform.mName		= name;
			uniform.mLoc		= -1;
			uniform.mIndex      = index;
			uniform.mCount		= count;
			uniform.mType		= type;
			uniform.mSemantic	= uniformSemantic;
			unSignedIndices[j++]= index;
			
			uniformBlock.mActiveUniforms.push_back( uniform );
		}

		std::vector<GLint> uniformOffset( numActiveUniforms );
		glGetActiveUniformsiv( mHandle,
							  numActiveUniforms,
							  unSignedIndices.data(),
							  GL_UNIFORM_OFFSET,
							  uniformOffset.data() );
		std::vector<GLint> uniformArrayStride( numActiveUniforms );
		glGetActiveUniformsiv( mHandle,
							  numActiveUniforms,
							  unSignedIndices.data(),
							  GL_UNIFORM_ARRAY_STRIDE,
							  uniformArrayStride.data() );
		std::vector<GLint> uniformMatrixStride( numActiveUniforms );
		glGetActiveUniformsiv( mHandle,
							  numActiveUniforms,
							  unSignedIndices.data(),
							  GL_UNIFORM_MATRIX_STRIDE,
							  uniformMatrixStride.data() );
		
		uniformBlock.mActiveUniformInfo.insert( make_pair( GL_UNIFORM_OFFSET, uniformOffset ) );
		uniformBlock.mActiveUniformInfo.insert( make_pair( GL_UNIFORM_ARRAY_STRIDE, uniformArrayStride ) );
		uniformBlock.mActiveUniformInfo.insert( make_pair( GL_UNIFORM_MATRIX_STRIDE, uniformMatrixStride ) );
		
		mUniformBlocks.push_back( uniformBlock );
	}
}

void GlslProg::cacheActiveTransformFeedbackVaryings()
{
	GLint numActiveTransformFeedbackVaryings;
	glGetProgramiv( mHandle, GL_TRANSFORM_FEEDBACK_VARYINGS, &numActiveTransformFeedbackVaryings );
	
	if( numActiveTransformFeedbackVaryings > 0 ) {
		for( int i = 0; i < numActiveTransformFeedbackVaryings; i++ ) {
			GLchar name[500];
			GLint length;
			GLsizei count;
			GLenum type;
			
			glGetTransformFeedbackVarying( mHandle, i, 500, &length, &count, &type, name );
			name[length] = 0;
			
			// So they've told us at setup what to look for. Find the varying, they told us
			// about so that we can finish the setup.
			auto varying = findTransformFeedbackVaryings( name );
			if( varying ) {
				varying->mCount = count;
				varying->mType = type;
			}
		}
		
		// TODO: This needs to be created correctly and checked, this is where the magic will happen.
//		static const std::string nextComponent = "glNextComponent";
//		static const std::string nextBuffer = "glNextBuffer";
//		
//		// This isn't pretty but it will work to tell us about what the attributes are
//		if( mTransformFeedbackFormat == GL_INTERLEAVED_ATTRIBS ) {
//			for( auto & varying : mTransformFeedbackVaryings ) {
//				// if the count is zero we must have reached a special variable, i.e. gl_Ne
//				if( varying.mCount == 0 ) {
//					if ( ! varying.mName.compare( 0, nextComponent.size(), nextComponent ) ) {
//						auto dimStr = varying.mName.substr( nextComponent.size(), 1 );
//						auto dimInt = std::stoi( dimStr );
//						
//					}
//					else if( varying.mName.compare( 0, nextBuffer.size(), nextBuffer ) ) {
//						
//					}
//				}
//			}
//		}
	}
}
#endif // ! defined( CINDER_GL_ES_2 )

void GlslProg::bind() const
{
	// this will in turn call bindImpl; this is so that the context can update its reference to the active shader
	gl::context()->bindGlslProg( const_cast<GlslProg*>( this ) );
}

// This is called by the Context whenever a GlslProg is bound. The indirection is so that the Context can update its reference to the active shader
void GlslProg::bindImpl() const
{
	glUseProgram( mHandle );
}

std::string GlslProg::getShaderLog( GLuint handle ) const
{
	string log;
	
	GLchar* debugLog;
	GLint charsWritten	= 0;
	GLint debugLength	= 0;
	glGetShaderiv( handle, GL_INFO_LOG_LENGTH, &debugLength );
	
	if ( debugLength > 0 ) {
		debugLog = new GLchar[ debugLength ];
		glGetShaderInfoLog( handle, debugLength, &charsWritten, debugLog );
		log.append( debugLog, 0, debugLength );
		delete [] debugLog;
	}
	
	return log;
}
	
void GlslProg::logMissingUniform( const std::string &name ) const
{
	if( mLoggedUniformNames.count( name ) == 0 ) {
		CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
		mLoggedUniformNames.insert( name );
	}
}
	
void GlslProg::logMissingUniform( int location ) const
{
	if( mLoggedUniformLocations.count( location ) == 0 ) {
		CI_LOG_E( "Unknown uniform location: \"" << location << "\"" );
		mLoggedUniformLocations.insert( location );
	}
}
	
void GlslProg::logUniformWrongType( const std::string &name, GLenum uniformType, const std::string &userType ) const
{
	if( mLoggedUniformNames.count( name ) == 0 ) {
		CI_LOG_W("Uniform type mismatch for \"" << name << "\", expected "
				 << gl::constantToString( uniformType ) << " and received a " << userType << ".");
		mLoggedUniformNames.insert( name );
	}
}
	
void GlslProg::setLabel( const std::string &label )
{
	mLabel = label;
#if defined( CINDER_GL_ES )
#if ! defined( CINDER_GL_ANGLE )
	env()->objectLabel( GL_PROGRAM_OBJECT_EXT, mHandle, (GLsizei)label.size(), label.c_str() );
#endif
#else
	env()->objectLabel( GL_PROGRAM, mHandle, (GLsizei)label.size(), label.c_str() );
#endif
}

GLint GlslProg::getUniformLocation( const std::string &name ) const
{
	auto found = findUniform( name );
	if( found )
		return found->mLoc;
	else
		return  -1;
}
	
GlslProg::Attribute* GlslProg::findAttrib( const std::string &name )
{
	Attribute* ret = nullptr;
	for( auto & attrib : mAttributes ) {
		if( attrib.mName == name ) {
			ret = &attrib;
			break;
		}
	}
	return ret;
}

const GlslProg::Attribute* GlslProg::findAttrib( const std::string &name ) const
{
	const Attribute* ret = nullptr;
	for( auto & attrib : mAttributes ) {
		if( attrib.mName == name ) {
			ret = &attrib;
			break;
		}
	}
	return ret;
}
	
GlslProg::Uniform* GlslProg::findUniform( const std::string &name )
{
	Uniform* ret = nullptr;
	for( auto & uniform : mUniforms ) {
		if( uniform.mName == name ) {
			ret = &uniform;
			break;
		}
	}
	return ret;
}
	
const GlslProg::Uniform* GlslProg::findUniform( const std::string &name ) const
{
	const Uniform* ret = nullptr;
	for( const auto & uniform : mUniforms ) {
		if( uniform.mName == name ) {
			ret = &uniform;
			break;
		}
	}
	return ret;
}
	
const GlslProg::Uniform* GlslProg::findUniform( int location ) const
{
	const Uniform* ret = nullptr;
	for( const auto & uniform : mUniforms ) {
		if( uniform.mLoc == location ) {
			ret = &uniform;
			break;
		}
	}
	return ret;
}
    
bool GlslProg::hasAttribSemantic( geom::Attrib semantic ) const
{
    return find_if( mAttributes.begin(),
                   mAttributes.end(),
                   [ semantic ]( const Attribute & attrib ){
                       return attrib.mSemantic == semantic;
                   }) != mAttributes.end();
}
    
GLint GlslProg::getAttribSemanticLocation( geom::Attrib semantic ) const
{
    auto found = find_if( mAttributes.begin(),
                         mAttributes.end(),
                         [ semantic ]( const Attribute &attrib ){
                             return attrib.mSemantic == semantic;
                         });
    if( found != mAttributes.end() )
        return found->mLoc;
    else
        return -1;
}
    
GLint GlslProg::getAttribLocation( const std::string &name ) const
{
	auto found = findAttrib( name );
    if( found )
        return found->mLoc;
    else
        return -1;
}
    
#if ! defined( CINDER_GL_ES_2 )

GlslProg::UniformBlock* GlslProg::findUniformBlock( const std::string &name )
{
	UniformBlock* ret = nullptr;
	for( auto & uniformBlock : mUniformBlocks ) {
		if( uniformBlock.mName == name ) {
			ret = &uniformBlock;
			break;
		}
	}
	return ret;
}
	
const GlslProg::UniformBlock* GlslProg::findUniformBlock( const std::string &name ) const
{
	const UniformBlock* ret = nullptr;
	for( auto & uniformBlock : mUniformBlocks ) {
		if( uniformBlock.mName == name ) {
			ret = &uniformBlock;
			break;
		}
	}
	return ret;
}

void GlslProg::uniformBlock( int loc, int binding )
{
	auto found = find_if( mUniformBlocks.begin(),
						 mUniformBlocks.end(),
						 [=]( const UniformBlock &block ){
							 return block.mLoc == loc;
						 });
	if( found != mUniformBlocks.end() ) {
		if( found->mBlockBinding != binding ) {
			found->mBlockBinding = binding;
			glUniformBlockBinding( mHandle, found->mLoc, binding );
		}
	}
	else {
		CI_LOG_E("Uniform block at " << loc << " location not found");
	}
}

void GlslProg::uniformBlock( const std::string &name, GLint binding )
{
	auto found = findUniformBlock( name );
	if( found ) {
		if( found->mBlockBinding != binding ) {
			found->mBlockBinding = binding;
			glUniformBlockBinding( mHandle, found->mLoc, binding );
		}
	}
	else {
		CI_LOG_E("Uniform block \"" << name << "\" not found");
	}
}

GLint GlslProg::getUniformBlockLocation( const std::string &name ) const
{
	auto found = findUniformBlock( name );
	if( found )
		return found->mLoc;
	else
		return -1;
}

GLint GlslProg::getUniformBlockSize( GLint blockBinding ) const
{
	auto found = find_if( mUniformBlocks.begin(),
						 mUniformBlocks.end(),
						 [=]( const UniformBlock &block ){
							 return block.mBlockBinding == blockBinding;
						 });
	if( found != mUniformBlocks.end() )
		return found->mSize;
	else
		return -1;
}
	
const GlslProg::TransformFeedbackVaryings* GlslProg::findTransformFeedbackVaryings( const std::string &name ) const
{
	const TransformFeedbackVaryings* ret = nullptr;
	for( const auto & varying : mTransformFeedbackVaryings ) {
		if( varying.mName == name ) {
			ret = &varying;
		}
	}
	return ret;
}
	
GlslProg::TransformFeedbackVaryings* GlslProg::findTransformFeedbackVaryings( const std::string &name )
{
	TransformFeedbackVaryings* ret = nullptr;
	for( auto & varying : mTransformFeedbackVaryings ) {
		if( varying.mName == name ) {
			ret = &varying;
		}
	}
	return ret;
}
#endif // ! defined( CINDER_GL_ES_2 )
	
bool GlslProg::checkUniformValue( const Uniform &uniform, const void *val, int count ) const
{
	if( mUniformBuffer ) {
		return mUniformBuffer->shouldBuffer( uniform.mBytePointer, glTypeToBytes( uniform.mType ) * count, val );
	}
	else {
		return false;
	}
}
	
template<typename LookUp, typename T>
inline void GlslProg::uniformImpl( const LookUp &lookUp, const T &data ) const
{
	auto found = findUniform( lookUp );
	if( ! found ) {
		logMissingUniform( lookUp );
		return;
	}
	if( validateUniform( *found, data ) )
		uniformFunc( found->mLoc, data );
}

template<typename LookUp, typename T>
inline void	GlslProg::uniformMatImpl( const LookUp &lookUp, const T &data, bool transpose ) const
{
	auto found = findUniform( lookUp );
	if( ! found ) {
		logMissingUniform( lookUp );
		return;
	}
	if( validateUniform( *found, data ) )
		uniformMatFunc( found->mLoc, data, transpose );
}

template<typename LookUp, typename T>
inline void	GlslProg::uniformImpl( const LookUp &lookUp, const T *data, int count ) const
{
	auto found = findUniform( lookUp );
	if( ! found ) {
		logMissingUniform( lookUp );
		return;
	}
	if( validateUniform( *found, data, count ) )
		uniformFunc( found->mLoc, data, count );
}

template<typename LookUp, typename T>
inline void	GlslProg::uniformMatImpl( const LookUp &lookUp, const T *data, int count, bool transpose ) const
{
	auto found = findUniform( lookUp );
	if( ! found ) {
		logMissingUniform( lookUp );
		return;
	}
	if( validateUniform( *found, data, count ) )
		uniformMatFunc( found->mLoc, data, count, transpose );
}
	
template<typename T>
inline bool GlslProg::validateUniform( const Uniform &uniform, const T &val ) const
{
	std::string type;
	if( ! checkUniformType<T>( uniform.mType, type ) ) {
		logUniformWrongType( uniform.mName, uniform.mType, type );
		return false;
	}
	else {
		return checkUniformValue( uniform, &val, 1 );
	}
}

template<typename T>
inline bool GlslProg::validateUniform( const Uniform &uniform, const T *val, int count ) const
{
	std::string type;
	if( ! checkUniformType<T>( uniform.mType, type ) ) {
		logUniformWrongType( uniform.mName, uniform.mType, type );
		return false;
	}
	else
		return checkUniformValue( uniform, val, count );
}

// bool
void GlslProg::uniform( const std::string &name, bool data ) const
{
	uniformImpl( name, data );
}
	
void GlslProg::uniform( int location, bool data ) const
{
	uniformImpl( location, data );
}
	
template<>
void GlslProg::uniformFunc<bool>( int location, const bool &data ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform1i( location, data );
}
	
template<>
inline bool GlslProg::checkUniformType<bool>( GLenum uniformType, std::string &type ) const
{
	type = "bool";
	return GL_BOOL == uniformType;
}
	
#if ! defined( CINDER_GL_ES_2 )
// bvec2
template<>
inline bool GlslProg::checkUniformType<glm::bvec2>( GLenum uniformType, std::string &type ) const
{
	type = "bvec2";
	return GL_BOOL_VEC2 == uniformType;
}

template<>
inline bool GlslProg::checkUniformType<glm::bvec3>( GLenum uniformType, std::string &type ) const
{
	type = "bvec3";
	return GL_BOOL_VEC3 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<glm::bvec4>( GLenum uniformType, std::string &type ) const
{
	type = "bvec4";
	return GL_BOOL_VEC4 == uniformType;
}
	
// uint32_t
void GlslProg::uniform( const std::string &name, uint32_t data ) const
{
	uniformImpl( name, data );
}
	
void GlslProg::uniform( int location, uint32_t data ) const
{
	uniformImpl( location, data );
}

template<>
void GlslProg::uniformFunc( int location, const uint32_t &data ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform1ui( location, data );
}
	
template<>
inline bool GlslProg::checkUniformType<uint32_t>( GLenum uniformType, std::string &type ) const
{
	type = "uint32_t";
	return GL_UNSIGNED_INT == uniformType;
}
	
// uvec2
void GlslProg::uniform( const std::string &name, const uvec2 &data ) const
{
	uniformImpl( name, data );
}
	
void GlslProg::uniform( int location, const uvec2 &data ) const
{
	uniformImpl( location, data );
}

template<>
void GlslProg::uniformFunc( int location, const uvec2 &data ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform2ui( location, data.x, data.y );
}
	
template<>
inline bool GlslProg::checkUniformType<uvec2>( GLenum uniformType, std::string &type ) const
{
	type = "uvec2";
	return GL_UNSIGNED_INT_VEC2 == uniformType;
}

// uvec3
void GlslProg::uniform( const std::string &name, const uvec3 &data ) const
{
	uniformImpl( name, data );
}
	
void GlslProg::uniform( int location, const uvec3 &data ) const
{
	uniformImpl( location, data );
}
	
template<>
void GlslProg::uniformFunc( int location, const uvec3 &data ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform3ui( location, data.x, data.y, data.z );
}
	
template<>
inline bool GlslProg::checkUniformType<uvec3>( GLenum uniformType, std::string &type ) const
{
	type = "uvec3";
	return GL_UNSIGNED_INT_VEC3 == uniformType;
}
	
// uvec4
void GlslProg::uniform( const std::string &name, const uvec4 &data ) const
{
	uniformImpl( name, data );
}

void GlslProg::uniform( int location, const uvec4 &data ) const
{
	uniformImpl( location, data );
}

template<>
void GlslProg::uniformFunc( int location, const uvec4 &data ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform4ui( location, data.x, data.y, data.z, data.w );
}
	
template<>
inline bool GlslProg::checkUniformType<uvec4>( GLenum uniformType, std::string &type ) const
{
	type = "uvec4";
	return GL_UNSIGNED_INT_VEC4 == uniformType;
}
#endif
	
// int
void GlslProg::uniform( const std::string &name, int data ) const
{
	uniformImpl( name, data );
}
	
void GlslProg::uniform( int location, int data ) const
{
	uniformImpl( location, data );
}

template<>
void GlslProg::uniformFunc( int location, const int &data ) const
{
	ScopedGlslProg shaderBind( this );
    glUniform1i( location, data );
}
	
template<>
inline bool GlslProg::checkUniformType<int>( GLenum uniformType, std::string &type ) const
{
	// This is somewhat unfortunate but needed because OpenGL made
	// all of these able to be buffered throught the integer path.
	// We could add more samplers here if needed.
	switch ( uniformType ) {
		case GL_BOOL: return true; break;
		case GL_INT: return true; break;
		case GL_SAMPLER_2D: return true; break;
#if ! defined( CINDER_GL_ES_2 )
		case GL_SAMPLER_2D_SHADOW: return true; break;
		case GL_SAMPLER_3D: return true; break;
#else
		case GL_SAMPLER_2D_SHADOW_EXT: return true; break;
#endif
		case GL_SAMPLER_CUBE: return true; break;
		default: type = "int"; return false; break;
	}
}

// ivec2
void GlslProg::uniform( const std::string &name, const ivec2 &data ) const
{
	uniformImpl( name, data );
}
	
void GlslProg::uniform( int location, const ivec2 &data ) const
{
	uniformImpl( location, data );
}

template<>
void GlslProg::uniformFunc( int location, const ivec2 &data ) const
{
    ScopedGlslProg shaderBind( this );
    glUniform2i( location, data.x, data.y );
}
	
template<>
inline bool GlslProg::checkUniformType<ivec2>( GLenum uniformType, std::string &type ) const
{
	type = "ivec2";
	return GL_INT_VEC2 == uniformType;
}

// ivec3
void GlslProg::uniform( const std::string &name, const ivec3 &data ) const
{
	uniformImpl( name, data );
}
	
void GlslProg::uniform( int location, const ivec3 &data ) const
{
	uniformImpl( location, data );
}
	
template<>
void GlslProg::uniformFunc( int location, const ivec3 &data ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform3i( location, data.x, data.y, data.z );
}
	
template<>
inline bool GlslProg::checkUniformType<ivec3>( GLenum uniformType, std::string &type ) const
{
	type = "ivec3";
	return GL_INT_VEC3 == uniformType;
}

// ivec4
void GlslProg::uniform( const std::string &name, const ivec4 &data ) const
{
	uniformImpl( name, data );
}

void GlslProg::uniform( int location, const ivec4 &data ) const
{
	uniformImpl( location, data );
}

template<>
void GlslProg::uniformFunc( int location, const ivec4 &data ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform4i( location, data.x, data.y, data.z, data.w );
}
	
template<>
inline bool GlslProg::checkUniformType<ivec4>( GLenum uniformType, std::string &type ) const
{
	type = "ivec4";
	return GL_INT_VEC4 == uniformType;
}

// int *, count
void GlslProg::uniform( const std::string &name, const int *data, int count ) const
{
	uniformImpl( name, data, count );
}
	
void GlslProg::uniform( int location, const int *data, int count ) const
{
	uniformImpl( location, data, count );
}
	
template<>
void GlslProg::uniformFunc( int location, const int *data, int count ) const
{
    ScopedGlslProg shaderBind( this );
    glUniform1iv( location, count, data );
}
	
void GlslProg::uniform( const std::string &name, const uint32_t *data, int count ) const
{
	uniformImpl( name, data, count );
}
	
void GlslProg::uniform( int location, const uint32_t *data, int count ) const
{
	uniformImpl( location, data, count );
}

template<>
void GlslProg::uniformFunc( int location, const uint32_t *data, int count ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform1uiv( location, count, data );
}

// ivec2 *, count
void GlslProg::uniform( const std::string &name, const ivec2 *data, int count ) const
{
	uniformImpl( name, data, count );
}
	
void GlslProg::uniform( int location, const ivec2 *data, int count ) const
{
	uniformImpl( location, data, count );
}
	
template<>
void GlslProg::uniformFunc( int location, const ivec2 *data, int count ) const
{
    ScopedGlslProg shaderBind( this );
    glUniform2iv( location, count, &data[0].x );
}

// float
void GlslProg::uniform( const std::string &name, float data ) const
{
	uniformImpl( name, data );
}
	
void GlslProg::uniform( int location, float data ) const
{
	uniformImpl( location, data );
}
	
template<>
void GlslProg::uniformFunc( int location, const float &data ) const
{
    ScopedGlslProg shaderBind( this );
    glUniform1f( location, data );
}
	
template<>
inline bool GlslProg::checkUniformType<float>( GLenum uniformType, std::string &type ) const
{
	type = "float";
	return GL_FLOAT == uniformType;
}

// vec2
void GlslProg::uniform( const std::string &name, const vec2 &data ) const
{
	uniformImpl( name, data );
}

void GlslProg::uniform( int location, const vec2 &data ) const
{
	uniformImpl( location, data );
}
	
template<>
void GlslProg::uniformFunc( int location, const vec2 &data ) const
{
    ScopedGlslProg shaderBind( this );
    glUniform2f( location, data.x, data.y );
}
	
template<>
inline bool GlslProg::checkUniformType<vec2>( GLenum uniformType, std::string &type ) const
{
	type = "vec2";
	return GL_FLOAT_VEC2 == uniformType;
}

// vec3
void GlslProg::uniform( const std::string &name, const vec3 &data ) const
{
	uniformImpl( name, data );
}

void GlslProg::uniform( int location, const vec3 &data ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform3f( location, data.x, data.y, data.z );
}
	
template<>
inline bool GlslProg::checkUniformType<vec3>( GLenum uniformType, std::string &type ) const
{
	type = "vec3";
	return GL_FLOAT_VEC3 == uniformType;
}

// vec4
void GlslProg::uniform( const std::string &name, const vec4 &data ) const
{
	uniformImpl( name, data );
}
	
void GlslProg::uniform( int location, const vec4 &data ) const
{
	uniformImpl( location, data );
}
	
template<>
void GlslProg::uniformFunc( int location, const vec4 &data ) const
{
    ScopedGlslProg shaderBind( this );
    glUniform4f( location, data.x, data.y, data.z, data.w );
}
	
template<>
inline bool GlslProg::checkUniformType<vec4>( GLenum uniformType, std::string &type ) const
{
	type = "vec4";
	return GL_FLOAT_VEC4 == uniformType;
}

// mat2
void GlslProg::uniform( const std::string &name, const mat2 &data, bool transpose ) const
{
	uniformMatImpl( name, data, transpose );
}
	
void GlslProg::uniform( int location, const mat2 &data, bool transpose ) const
{
	uniformMatImpl( location, data, transpose );
}
	
template<>
void GlslProg::uniformMatFunc( int location, const mat2 &data, bool transpose ) const
{
	ScopedGlslProg shaderBind( this );
	glUniformMatrix2fv( location, 1, ( transpose ) ? GL_TRUE : GL_FALSE, glm::value_ptr( data ) );
}
	
template<>
inline bool GlslProg::checkUniformType<mat2>( GLenum uniformType, std::string &type ) const
{
	type = "mat2";
	return GL_FLOAT_MAT2 == uniformType;
}
	
// mat3
void GlslProg::uniform( const std::string &name, const mat3 &data, bool transpose ) const
{
	uniformMatImpl( name, data, transpose );
}

void GlslProg::uniform( int location, const mat3 &data, bool transpose ) const
{
	uniformMatImpl( location, data, transpose );
}
	
template<>
void GlslProg::uniformMatFunc( int location, const mat3 &data, bool transpose ) const
{
    ScopedGlslProg shaderBind( this );
    glUniformMatrix3fv( location, 1, ( transpose ) ? GL_TRUE : GL_FALSE, glm::value_ptr( data ) );
}
	
template<>
inline bool GlslProg::checkUniformType<mat3>( GLenum uniformType, std::string &type ) const
{
	type = "mat3";
	return GL_FLOAT_MAT3 == uniformType;
}

// mat4
void GlslProg::uniform( const std::string &name, const mat4 &data, bool transpose ) const
{
	uniformMatImpl( name, data, transpose );
}
	
void GlslProg::uniform( int location, const mat4 &data, bool transpose ) const
{
	uniformMatImpl( location, data, transpose );
}

template<>
void GlslProg::uniformMatFunc( int location, const mat4 &data, bool transpose ) const
{
    ScopedGlslProg shaderBind( this );
    glUniformMatrix4fv( location, 1, ( transpose ) ? GL_TRUE : GL_FALSE, glm::value_ptr( data ) );
}
	
template<>
inline bool GlslProg::checkUniformType<mat4>( GLenum uniformType, std::string &type ) const
{
	type = "mat4";
	return GL_FLOAT_MAT4 == uniformType;
}

// Color
void GlslProg::uniform( const std::string &name, const Color &data ) const
{
	uniformImpl( name, data );
}
	
void GlslProg::uniform( int location, const Color &data ) const
{
	uniformImpl( location, data );
}

template<>
void GlslProg::uniformFunc( int location, const Color &data ) const
{
    ScopedGlslProg shaderBind( this );
    glUniform3f( location, data.r, data.g, data.b );
}
	
template<>
inline bool GlslProg::checkUniformType<Color>( GLenum uniformType, std::string &type ) const
{
	type = "vec3 as Color";
	return GL_FLOAT_VEC3 == uniformType;
}

// ColorA
void GlslProg::uniform( const std::string &name, const ColorA &data ) const
{
	uniformImpl( name, data );
}
	
void GlslProg::uniform( int location, const ColorA &data ) const
{
	uniformImpl( location, data );
}

template<>
void GlslProg::uniformFunc( int location, const ColorA &data ) const
{
    ScopedGlslProg shaderBind( this );
    glUniform4f( location, data.r, data.g, data.b, data.a );
}
	
template<>
inline bool GlslProg::checkUniformType<ColorA>( GLenum uniformType, std::string &type ) const
{
	type = "vec4 as ColorA";
	return GL_FLOAT_VEC4 == uniformType;
}

// float*, count
void GlslProg::uniform( const std::string &name, const float *data, int count ) const
{
	uniformImpl( name, data, count );
}
	
void GlslProg::uniform( int location, const float *data, int count ) const
{
	uniformImpl( location, data, count );
}
	
template<>
void GlslProg::uniformFunc( int location, const float *data, int count ) const
{
    ScopedGlslProg shaderBind( this );
    glUniform1fv( location, count, data );
}

// vec2*, count
void GlslProg::uniform( const std::string &name, const vec2 *data, int count ) const
{
	uniformImpl( name, data, count );
}
	
void GlslProg::uniform( int location, const vec2 *data, int count ) const
{
	uniformImpl( location, data, count );
}

template<>
void GlslProg::uniformFunc( int location, const vec2 *data, int count ) const
{
    ScopedGlslProg shaderBind( this );
    glUniform2fv( location, count, &data[0].x );
}

// vec3*, count
void GlslProg::uniform( const std::string &name, const vec3 *data, int count ) const
{
	uniformImpl( name, data, count );
}
	
void GlslProg::uniform( int location, const vec3 *data, int count ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform3fv( location, count, &data[0].x );
}

// vec4*, count
void GlslProg::uniform( const std::string &name, const vec4 *data, int count ) const
{
	uniformImpl( name, data, count );
}
	
void GlslProg::uniform( int location, const vec4 *data, int count ) const
{
	uniformImpl( location, data, count );
}

template<>
void GlslProg::uniformFunc( int location, const vec4 *data, int count ) const
{
    ScopedGlslProg shaderBind( this );
    glUniform4fv( location, count, &data[0].x );
}
	
// mat2*, count
void GlslProg::uniform( const std::string &name, const mat2 *data, int count, bool transpose ) const
{
	uniformMatImpl( name, data, count, transpose );
}

void GlslProg::uniform( int location, const mat2 *data, int count, bool transpose ) const
{
	uniformMatImpl( location, data, count, transpose );
}
	
template<>
void GlslProg::uniformMatFunc( int location, const mat2 *data, int count, bool transpose ) const
{
	ScopedGlslProg shaderBind( this );
	glUniformMatrix2fv( location, count, ( transpose ) ? GL_TRUE : GL_FALSE, glm::value_ptr( *data ) );
}

// mat3*, count
void GlslProg::uniform( const std::string &name, const mat3 *data, int count, bool transpose ) const
{
	uniformMatImpl( name, data, count, transpose );
}
	
void GlslProg::uniform( int location, const mat3 *data, int count, bool transpose ) const
{
	uniformMatImpl( location, data, count, transpose );
}
	
template<>
void GlslProg::uniformMatFunc( int location, const mat3 *data, int count, bool transpose ) const
{
    ScopedGlslProg shaderBind( this );
    glUniformMatrix3fv( location, count, ( transpose ) ? GL_TRUE : GL_FALSE, glm::value_ptr( *data ) );
}

// mat4*, count
void GlslProg::uniform( const std::string &name, const mat4 *data, int count, bool transpose ) const
{
	uniformMatImpl( name, data, count, transpose );
}
	
void GlslProg::uniform( int location, const mat4 *data, int count, bool transpose ) const
{
	uniformMatImpl( location, data, count, transpose );
}
	
template<>
void GlslProg::uniformMatFunc( int location, const mat4 *data, int count, bool transpose ) const
{
    ScopedGlslProg shaderBind( this );
    glUniformMatrix4fv( location, count, ( transpose ) ? GL_TRUE : GL_FALSE, glm::value_ptr( *data ) );
}

std::ostream& operator<<( std::ostream &os, const GlslProg &rhs )
{
	os << "ID: " << rhs.mHandle << std::endl;
	if( ! rhs.mLabel.empty() )
		os << "    Label: " << rhs.mLabel << std::endl;
	
	auto attribs = rhs.getActiveAttributes();
	os << "\tAttributes: " << std::endl;
	for( auto &attrib : attribs ) {
		os << "\t\t\"" << attrib.mName << "\":" << std::endl;
		os << "\t\t Loc: " << attrib.mLoc << std::endl;
		os << "\t\t Type: " << gl::constantToString( attrib.mType ) << std::endl;
		os << "\t\t Semantic: <" << geom::attribToString( attrib.mSemantic ) << ">" << std::endl;
	}
	
	os << "\tUniforms: " << std::endl;
	auto & uniforms = rhs.getActiveUniforms();
	for( auto &uniform : uniforms ) {
		os << "\t\t\"" << uniform.mName << "\":" << std::endl;
		os << "\t\t Loc: " << uniform.mLoc << std::endl;
		os << "\t\t Type: " << gl::constantToString( uniform.mType ) << std::endl;
		os << "\t\t Semantic: <" << gl::uniformSemanticToString( uniform.mSemantic ) << ">" << std::endl;
	}
	
#if ! defined( CINDER_GL_ES_2 )
	os << "\tUniform Blocks: " << std::endl;
	auto & uniformBlocks = rhs.getActiveUniformBlocks();
	for( auto & uniformBlock : uniformBlocks ) {
		os << "\t\t\"" << uniformBlock.mName << "\":" << std::endl;
		os << "\t\t Loc: " << uniformBlock.mLoc << std::endl;
		os << "\t\t Size: " << uniformBlock.mSize << std::endl;
		os << "\t\t BlockBinding: " << uniformBlock.mBlockBinding << std::endl;
		os << "\t\t Active Uniforms: " << endl;
		for( auto & uniform : uniformBlock.mActiveUniforms ) {
			os << "\t\t\t\"" << uniform.mName << "\":" << std::endl;
			os << "\t\t\t Loc: " << uniform.mLoc << std::endl;
			os << "\t\t\t Type: " << gl::constantToString( uniform.mType ) << std::endl;
			os << "\t\t\t Semantic: <" << gl::uniformSemanticToString( uniform.mSemantic ) << ">" << std::endl;
		}
	}
	
	os << "\tTransform Feedback Varyings: " << std::endl;
	auto & feedbackVaryings = rhs.getActiveTransformFeedbackVaryings();
	for( auto & varying : feedbackVaryings ) {
		os << "\t\t\"" << varying.mName << "\":" << std::endl;
		os << "\t\t Type: " << gl::constantToString( varying.mType ) << std::endl;
		os << "\t\t Count: " << varying.mCount << std::endl;
	}
#endif
	return os;
}

//////////////////////////////////////////////////////////////////////////
// GlslProgCompileExc
GlslProgCompileExc::GlslProgCompileExc( const std::string &log, GLint shaderType )
{
	string typeString;

	switch( shaderType ) {
		case GL_VERTEX_SHADER:			typeString = "VERTEX: "; break;
		case GL_FRAGMENT_SHADER:		typeString = "FRAGMENT: "; break;
#if ! defined( CINDER_GL_ES )
		case GL_GEOMETRY_SHADER:		typeString = "GEOMETRY: "; break;
		case GL_TESS_CONTROL_SHADER:	typeString = "TESSELLATION CONTROL: "; break;
		case GL_TESS_EVALUATION_SHADER:	typeString = "TESSELLATION EVALUATION: "; break;
#endif
		default:						typeString = "UNKNOWN: ";
	}

	setDescription( typeString + log );
}
	
} } // namespace cinder::gl