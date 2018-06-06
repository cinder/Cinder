/*
 Copyright (c) 2010, The Cinder Project
 All rights reserved.
 
 This code is designed for use with the Cinder C++ library, http://libcinder.org

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
#include "cinder/gl/scoped.h"
#include "cinder/Log.h"
#include "cinder/Noncopyable.h"
#include "cinder/Utilities.h"

#include "glm/gtc/type_ptr.hpp"

#include <type_traits>

// For stoi and std:;to_string
#if defined( CINDER_ANDROID )
	#include "cinder/android/CinderAndroid.h"
#endif

// uncomment this in order to prevent uniform value caching
//#define DISABLE_UNIFORM_CACHING

using namespace std;

namespace cinder { namespace gl {
    
static GlslProg::UniformSemanticMap	sDefaultUniformNameToSemanticMap;
static GlslProg::AttribSemanticMap	sDefaultAttribNameToSemanticMap;
	
class UniformValueCache : cinder::Noncopyable {
  public:
	UniformValueCache( uint32_t bufferSize )
		: mBuffer( new uint8_t[bufferSize] ), mValidBytes( new bool[bufferSize] ), mBufferSize( bufferSize )
	{
		for( uint32_t b = 0; b < bufferSize; ++b )
			mValidBytes[b] = false;
	}
	
	// 'uniformByteOffset' expresses where the first element of an array is: example[0], although 'arrayIndex' may be >0
	bool shouldBuffer( uint32_t uniformByteOffset, uint32_t typeSize, int arrayIndex, int indexCount, const void* valuePointer )
	{
		// are all the bytes in this range valid?
		bool cacheValid = true;
		for( uint32_t b = uniformByteOffset + arrayIndex * typeSize; b < uniformByteOffset + ( arrayIndex + indexCount ) * typeSize; ++b ) {
			if( mValidBytes[b] == false ) {
				cacheValid = false;
				break;
			}
		}
	
		uint8_t* cachePtr = mBuffer.get() + uniformByteOffset + arrayIndex * typeSize;
		
		if( ! cacheValid ) { // if not, record it and cache it
			for( uint32_t b = uniformByteOffset + arrayIndex * typeSize; b < uniformByteOffset + ( arrayIndex + indexCount ) * typeSize; ++b )
				mValidBytes[b] = true;

			memcpy( cachePtr, valuePointer, typeSize * indexCount );
			return true;
		}
		else {
			// we've seen this byteOffset before; did its value change?
			if( memcmp( cachePtr, valuePointer, typeSize * indexCount ) == 0 ) {
				return false;
			}
			else { // yes? then cache the latest
				memcpy( cachePtr, valuePointer, typeSize * indexCount );
				return true;
			}
		}
	}
	
  private:
	std::vector<uint32_t>			mCachedByteOffsets;
	std::unique_ptr<uint8_t[]>		mBuffer;
	std::unique_ptr<bool[]>			mValidBytes;
	uint32_t						mBufferSize;
};
	
//////////////////////////////////////////////////////////////////////////
// GlslProg::Attribute
	
// This function and variable layout is based on the specification from
// https://www.opengl.org/registry/specs/ARB/vertex_attrib_64bit.txt
void GlslProg::Attribute::getShaderAttribLayout( GLenum type, uint32_t *numDimsPerVertexPointer, uint32_t *numLocationsExpected )
{
	switch ( type ) {
		case GL_UNSIGNED_INT:
		case GL_INT:
		case GL_FLOAT:
			*numDimsPerVertexPointer = 1; *numLocationsExpected = 1;
		break;
#if ! defined( CINDER_GL_ES_2 )
		case GL_UNSIGNED_INT_VEC2:
#endif
		case GL_INT_VEC2:
		case GL_FLOAT_VEC2:
			*numDimsPerVertexPointer = 2; *numLocationsExpected = 1;
		break;
#if ! defined( CINDER_GL_ES_2 )
		case GL_UNSIGNED_INT_VEC3:
#endif
		case GL_INT_VEC3:
		case GL_FLOAT_VEC3:
			*numDimsPerVertexPointer = 3; *numLocationsExpected = 1;
		break;
#if ! defined( CINDER_GL_ES_2 )
		case GL_UNSIGNED_INT_VEC4:
#endif
		case GL_INT_VEC4:
		case GL_FLOAT_VEC4:
			*numDimsPerVertexPointer = 4; *numLocationsExpected = 1;
		break;
		case GL_FLOAT_MAT2:
			*numDimsPerVertexPointer = 2; *numLocationsExpected = 2;
		break;
#if ! defined( CINDER_GL_ES_2 )
		case GL_FLOAT_MAT2x3:
			*numDimsPerVertexPointer = 3; *numLocationsExpected = 2;
		break;
		case GL_FLOAT_MAT3x2:
			*numDimsPerVertexPointer = 2; *numLocationsExpected = 3;
		break;
		case GL_FLOAT_MAT4x2:
			*numDimsPerVertexPointer = 2; *numLocationsExpected = 4;
		break;
		case GL_FLOAT_MAT2x4:
			*numDimsPerVertexPointer = 4; *numLocationsExpected = 2;
		break;
#endif
		case GL_FLOAT_MAT3:
			*numDimsPerVertexPointer = 3; *numLocationsExpected = 3;
		break;
#if ! defined( CINDER_GL_ES_2 )
		case GL_FLOAT_MAT3x4:
			*numDimsPerVertexPointer = 4; *numLocationsExpected = 3;
		break;
		case GL_FLOAT_MAT4x3:
			*numDimsPerVertexPointer = 3; *numLocationsExpected = 4;
		break;
#endif
		case GL_FLOAT_MAT4:
			*numDimsPerVertexPointer = 4; *numLocationsExpected = 4;
		break;
		default:
			CI_LOG_E( "Unknown type for attrib: " << constantToString( type ) );
			*numDimsPerVertexPointer = 0; *numLocationsExpected = 0;
			break;
	}
}

//////////////////////////////////////////////////////////////////////////
// GlslProg::Format
GlslProg::Format::Format()
#if defined( CINDER_GL_HAS_TRANSFORM_FEEDBACK )
	: mTransformFormat( -1 )
#endif
{
	mPreprocessor = make_shared<ShaderPreprocessor>();
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

#if defined( CINDER_GL_HAS_GEOM_SHADER )

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

#endif // defined( CINDER_GL_HAS_GEOM_SHADER )

#if defined( CINDER_GL_HAS_TESS_SHADER )
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

#endif // defined( CINDER_GL_HAS_TESS_SHADER )

#if defined( CINDER_GL_HAS_COMPUTE_SHADER )

GlslProg::Format& GlslProg::Format::compute( const DataSourceRef &dataSource )
{
	setShaderSource( dataSource, &mComputeShader, &mComputeShaderPath );
	return *this;
}

GlslProg::Format& GlslProg::Format::compute( const string &computeShader )
{
	setShaderSource( computeShader, &mComputeShader, &mComputeShaderPath );
	return *this;
}

#endif // defined( CINDER_GL_HAS_COMPUTE_SHADER )

void GlslProg::Format::setShaderSource( const DataSourceRef &dataSource, string *shaderSourceDest, fs::path *shaderPathDest )
{
	if( dataSource ) {
		auto buffer = dataSource->getBuffer();
		const char *data = static_cast<const char *>( buffer->getData() );
		*shaderSourceDest = string( data, data + buffer->getSize() );

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
	for( auto& attrib : mAttributes ) {
		if( attrib.mName == attribName ) {
			attrib.mSemantic = semantic;
			return *this;
		}
		else if( attrib.mSemantic == semantic ) {
			attrib.mName = attribName;
			return *this;
		}
	}

	// no existing definition; add it
	Attribute attrib;
	attrib.mName = attribName;
	attrib.mSemantic = semantic;
	mAttributes.push_back( attrib );

	return *this;
}

GlslProg::Format& GlslProg::Format::uniform( UniformSemantic semantic, const std::string &uniformName )
{
	for( auto& uniform : mUniforms ) {
		if( uniform.mName == uniformName ) {
			uniform.mSemantic = semantic;
			return *this;
		}
		else if( uniform.mSemantic == semantic ) {
			uniform.mName = uniformName;
			return *this;
		}
	}

	// no existing definition; add it
	Uniform uniform;
	uniform.mName = uniformName;
	uniform.mSemantic = semantic;
	mUniforms.push_back( uniform );

	return *this;
}

bool GlslProg::Format::isPreprocessingEnabled() const
{ 
	return (bool)mPreprocessor;
}

void GlslProg::Format::setPreprocessingEnabled( bool enable )
{ 
	if( enable ) {
		if( ! mPreprocessor )
			mPreprocessor = make_shared<ShaderPreprocessor>();
	}
	else {
		mPreprocessor = nullptr;
	}
}

GlslProg::Format& GlslProg::Format::version( int version )
{
	if( mPreprocessor )
		mPreprocessor->setVersion( version );

	return *this;
}

int	GlslProg::Format::getVersion() const
{ 
	return mPreprocessor ? mPreprocessor->getVersion() : 0;
}

GlslProg::Format& GlslProg::Format::define( const std::string &define )
{
	if( mPreprocessor )
		mPreprocessor->addDefine( define );

	return *this;
}

GlslProg::Format& GlslProg::Format::define( const std::string &define, const std::string &value )
{
	if( mPreprocessor )
		mPreprocessor->addDefine( define, value );

	return *this;
}

const std::vector<std::pair<std::string,std::string>>& GlslProg::Format::getDefines() const
{
	if( mPreprocessor ) {
		return mPreprocessor->getDefines();
	}

	static std::vector<std::pair<std::string,std::string>> sEmpty;
	return sEmpty;
}

GlslProg::Format& GlslProg::Format::attribLocation( const std::string &attribName, GLint location )
{
	for( auto& attrib : mAttributes ) {
		if( attrib.mName == attribName ) {
			attrib.mLoc = location;
			return *this;
		}
		else if( attrib.mLoc == location ) {
			attrib.mName = attribName;
			return *this;
		}
	}

	// no existing definition; add it
	Attribute attrib;
	attrib.mName = attribName;
	attrib.mLoc = location;
	mAttributes.push_back( attrib );

	return *this;
}

GlslProg::Format& GlslProg::Format::attribLocation( geom::Attrib attribSemantic, GLint location )
{
	for( auto& attrib : mAttributes ) {
		if( attrib.mSemantic == attribSemantic ) {
			attrib.mLoc = location;
			return *this;
		}
		else if( attrib.mLoc == location ) {
			attrib.mSemantic = attribSemantic;
			return *this;
		}
	}

	Attribute attrib;
	attrib.mSemantic = attribSemantic;
	attrib.mLoc = location;
	mAttributes.push_back( attrib );

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
	: mUniformValueCache( nullptr )
#if defined( CINDER_GL_HAS_TRANSFORM_FEEDBACK )
		, mTransformFeedbackFormat( -1 )
#endif
{
	mHandle = glCreateProgram();

	GLuint vertexHandle = loadShader( format.getVertex(), format.mVertexShaderPath, GL_VERTEX_SHADER, format.getPreprocessor() );
	GLuint fragmentHandle = loadShader( format.getFragment(), format.mFragmentShaderPath, GL_FRAGMENT_SHADER, format.getPreprocessor() );
#if defined( CINDER_GL_HAS_GEOM_SHADER )
	GLuint geometryHandle = loadShader( format.getGeometry(), format.mGeometryShaderPath, GL_GEOMETRY_SHADER, format.getPreprocessor() );
#endif
#if defined( CINDER_GL_HAS_TESS_SHADER )
	GLuint tessellationCtrlHandle = loadShader( format.getTessellationCtrl(), format.mTessellationCtrlShaderPath, GL_TESS_CONTROL_SHADER, format.getPreprocessor() );
	GLuint tessellationEvalHandle = loadShader( format.getTessellationEval(), format.mTessellationEvalShaderPath, GL_TESS_EVALUATION_SHADER, format.getPreprocessor() );
#endif
#if defined( CINDER_GL_HAS_COMPUTE_SHADER )
	GLuint computeHandle = loadShader( format.getCompute(), format.mComputeShaderPath, GL_COMPUTE_SHADER, format.getPreprocessor() );
#endif

	auto &userDefinedAttribs = format.getAttributes();
	
	bool foundPositionSemantic = false;
	// if the user has provided a location make sure to bind that location before we go further
	for( auto &attrib : userDefinedAttribs ) {
		if( attrib.mLoc > -1 ) {
			if( attrib.mName == "ciPosition" || attrib.mSemantic == geom::Attrib::POSITION )
				foundPositionSemantic = true;
			
			// The user may have defined a location for an attrib but is relying on the default
			// name used by Cinder. Therefore, we need to find the attrib in the map and affix
			// the default name.
			std::string attribName = attrib.mName;
			if( attribName.empty() ) {
				const auto &defaultAttribMap = getDefaultAttribNameToSemanticMap();
				auto attribSemantic = attrib.mSemantic;
				auto foundDefaultAttrib = find_if( defaultAttribMap.begin(), defaultAttribMap.end(),
					[attribSemantic]( const std::pair<const std::string, geom::Attrib> &defaultAttrib ){
						return defaultAttrib.second == attribSemantic;
				});
				if( foundDefaultAttrib != defaultAttribMap.end() )
					attribName = foundDefaultAttrib->first;
				else {
					CI_LOG_E( "Defined Location for unknown semantic and unknown name" );
					continue;
				}
			}
			glBindAttribLocation( mHandle, attrib.mLoc, attribName.c_str() );
		}
	}
	
	// we always want to force position to be at vertex attribute 0; an imperfect workaround for
	// http://stackoverflow.com/questions/13348885/why-does-opengl-drawing-fail-when-vertex-attrib-array-zero-is-disabled
	if( ! foundPositionSemantic )
		glBindAttribLocation( mHandle, 0, "ciPosition" );
	
#if defined( CINDER_GL_HAS_TRANSFORM_FEEDBACK )
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

	// Detach all shaders, allowing GL to free the associated memory
	if( vertexHandle )
		glDetachShader( mHandle, vertexHandle );
	if( fragmentHandle )
		glDetachShader( mHandle, fragmentHandle );
#if defined( CINDER_GL_HAS_GEOM_SHADER )
	if( geometryHandle )
		glDetachShader( mHandle, geometryHandle );
#endif
#if defined( CINDER_GL_HAS_TESS_SHADER )
	if( tessellationCtrlHandle )
		glDetachShader( mHandle, tessellationCtrlHandle );
	if( tessellationEvalHandle )
		glDetachShader( mHandle, tessellationEvalHandle );
#endif
#if defined( CINDER_GL_HAS_COMPUTE_SHADER )
	if( computeHandle )
		glDetachShader( mHandle, computeHandle );	
#endif
	
	cacheActiveAttribs();
	cacheActiveUniforms();
#if defined( CINDER_GL_HAS_TRANSFORM_FEEDBACK )
	if( ! format.getVaryings().empty() ) {
		cacheActiveTransformFeedbackVaryings();
	}
#endif
#if defined( CINDER_GL_HAS_UNIFORM_BLOCKS )
	cacheActiveUniformBlocks();
#endif

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
			CI_LOG_W( "Unknown uniform: \"" << userUniform.mName << "\"" );
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
				if( userAttrib.mSemantic != geom::Attrib::USER_DEFINED ) {
					activeAttrib.mSemantic = userAttrib.mSemantic;
				}
				break;
			}
		}
		if( ! active ) {
			CI_LOG_W( "Unknown attribute: \"" << userAttrib.mName << "\"" );
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
		sDefaultUniformNameToSemanticMap["ciViewProjection"] = UNIFORM_VIEW_PROJECTION;
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

GLuint GlslProg::loadShader( const string &shaderSource, const fs::path &shaderPath, GLint shaderType, const ShaderPreprocessorRef &preprocessor )
{
	GLuint handle = 0;

	if( ! shaderSource.empty() ) {
		handle = glCreateShader( shaderType );

		if( preprocessor ) {
			set<fs::path> includedFiles;
			string preprocessedSource = preprocessor->parse( shaderSource, shaderPath, &includedFiles );
			mShaderPreprocessorIncludedFiles.insert( mShaderPreprocessorIncludedFiles.end(), includedFiles.begin(), includedFiles.end() );

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
#if defined( CINDER_ANDROID ) | defined( CINDER_LINUX )
			std::vector<std::string> lines = ci::split( shaderSource, "\r\n" );
			if( ! lines.empty() ) {
				std::stringstream ss;
				for( size_t i = 0; i < lines.size(); ++i ) {
					ss << std::setw( (int)std::log( lines.size() ) ) << (i + 1) << ": " << lines[i] << "\n"; 
				}
				log += "\n" + ss.str();
			}
#endif
			// Since the GlslProg destructor will not be called after we throw, we must delete all
			// owned GL objects here to avoid leaking. Any other attached shaders will be cleaned up
			// when the program is deleted
			glDeleteShader( handle );
			glDeleteProgram( mHandle );

			throw GlslProgCompileExc( log, shaderType );
		}

		glAttachShader( mHandle, handle );

		// Scope the shader's lifetime to the program's. It will be cleaned up when later detached.
		glDeleteShader( handle );
	}

	return handle;
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

		// Delete owned GL objects before throwing to avoid a leak
		glDeleteProgram( mHandle );
		
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
		
		geom::Attrib attributeSemantic = geom::Attrib::USER_DEFINED;
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
	
	uint32_t uniformValueCacheSize = 0;
	
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
			UniformSemantic uniformSemantic = UniformSemantic::UNIFORM_USER_DEFINED;
			
			auto foundSemantic = semanticNameMap.find( name );
			if( foundSemantic != semanticNameMap.end() ) {
				uniformSemantic = foundSemantic->second;
			}
			
			Uniform uniform;
			uniform.mName			= name;
			uniform.mLoc			= loc;
			uniform.mIndex			= i;
			uniform.mCount			= count;
			uniform.mType			= type;
			uniform.mTypeSize		= gl::typeToBytes( type );
			uniform.mSemantic		= uniformSemantic;
			uniform.mBytePointer	= uniformValueCacheSize;
			uniformValueCacheSize  += uniform.mTypeSize * count;
			mUniforms.push_back( uniform );
		}
	}
	
#if ! defined( DISABLE_UNIFORM_CACHING )
	if( numActiveUniforms )
		mUniformValueCache = unique_ptr<UniformValueCache>( new UniformValueCache( uniformValueCacheSize ) );
#endif
}

#if defined( CINDER_GL_HAS_UNIFORM_BLOCKS )
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
		uniformBlock.mDataSize = dataSize;
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
			
			UniformSemantic uniformSemantic = UniformSemantic::UNIFORM_USER_DEFINED;
			
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

#endif // defined( CINDER_GL_HAS_UNIFORM_BLOCKS )
#if defined( CINDER_GL_HAS_TRANSFORM_FEEDBACK )

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
	}
}
#endif // defined( CINDER_GL_HAS_TRANSFORM_FEEDBACK )

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
	
	if( debugLength > 0 ) {
		debugLog = new GLchar[debugLength];
		glGetShaderInfoLog( handle, debugLength, &charsWritten, debugLog );
		log.append( debugLog, 0, debugLength );
		delete [] debugLog;
	}
	
	return log;
}
	
void GlslProg::logMissingUniform( const std::string &name ) const
{
	if( mLoggedUniformNames.count( name ) == 0 ) {
		CI_LOG_W( "Unknown uniform: \"" << name << "\"" );
		mLoggedUniformNames.insert( name );
	}
}
	
void GlslProg::logMissingUniform( int location ) const
{
	if( mLoggedUniformLocations.count( location ) == 0 ) {
		CI_LOG_W( "Unknown uniform location: \"" << location << "\"" );
		mLoggedUniformLocations.insert( location );
	}
}
	
void GlslProg::logUniformWrongType( const std::string &name, GLenum uniformType, const std::string &userType ) const
{
	if( mLoggedUniformNames.count( name ) == 0 ) {
		CI_LOG_W("Uniform type mismatch for \"" << name << "\", expected "
				 << gl::constantToString( uniformType ) << " and received " << userType );
		mLoggedUniformNames.insert( name );
	}
}
	
void GlslProg::setLabel( const std::string &label )
{
	mLabel = label;
#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
  #if defined( CINDER_GL_HAS_DEBUG_LABEL )
	env()->objectLabel( GL_PROGRAM_OBJECT_EXT, mHandle, (GLsizei)label.size(), label.c_str() );
  #endif
#else 
  #if defined( CINDER_GL_ES )
    #if ! defined( CINDER_GL_ANGLE )
	env()->objectLabel( GL_PROGRAM_OBJECT_EXT, mHandle, (GLsizei)label.size(), label.c_str() );
 	 #endif
  #else
	env()->objectLabel( GL_PROGRAM, mHandle, (GLsizei)label.size(), label.c_str() );
  #endif
#endif
}

GLint GlslProg::getUniformLocation( const std::string &name ) const
{
	int uniformLocation;
	const Uniform *uniform = findUniform( name, &uniformLocation );
	if( uniform )
		return uniformLocation;
	else
		return -1;
}
	
GlslProg::Attribute* GlslProg::findAttrib( const std::string &name )
{
	Attribute *ret = nullptr;
	for( auto &attrib : mAttributes ) {
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
	for( auto &attrib : mAttributes ) {
		if( attrib.mName == name ) {
			ret = &attrib;
			break;
		}
	}
	return ret;
}
	
const GlslProg::Attribute* GlslProg::findAttrib( geom::Attrib semantic ) const
{
	const Attribute *ret = nullptr;
	for( const auto &attrib : mAttributes ) {
		if( attrib.mSemantic == semantic ) {
			ret = &attrib;
			break;
		}
	}
	return ret;
}

const GlslProg::Uniform* GlslProg::findUniform( const std::string &name, int *resultLocation ) const
{
	// first check if there is an exact name match with mUniforms and simply return it if we find one
	for( const auto & uniform : mUniforms ) {
		if( uniform.mName == name ) {
			if( resultLocation ) {
				*resultLocation = uniform.mLoc;
			}
			return &uniform;
		}
	}

	// search for array brackets, they need to be handled as a special case
	size_t requestedNameLeftSquareBracket = name.find( '[' );
	size_t requestedNameRightSquareBracket = ( requestedNameLeftSquareBracket != string::npos ? name.find( ']' ) : string::npos );

	// if we didn't find an exact match, look for the array version in the active uniforms list
	bool needsLocationOffset = false;
	const Uniform* resultUniform = nullptr;	
	for( const auto &uniform : mUniforms ) {
		size_t activeUniformLeftSquareBracket = uniform.mName.find( '[' );
		// skip match detection if this active uniform isn't an array
		if( activeUniformLeftSquareBracket == string::npos )
			continue;

		string uniformBaseName = uniform.mName.substr( 0, activeUniformLeftSquareBracket );
		if( requestedNameLeftSquareBracket == string::npos ) {
			// name is non-indexed, try to match the uniform base name with the entire requested uniform name
			if( uniformBaseName == name ) {
				resultUniform = &uniform;
				break;
			}
		}
		else {
			// try to find a match between the active uniform base name and the requested uniform's base name
			if( uniformBaseName == name.substr( 0, requestedNameLeftSquareBracket ) ) {
				// If the requested name is an array of structs and a struct member is part of it, make sure it matches the active uniform too
				if( name.size() - 1 > requestedNameRightSquareBracket ) {
					// try to match the struct member portion of each name
					if( name.substr( requestedNameRightSquareBracket, name.size() ) == uniform.mName.substr( uniform.mName.find( ']' ), name.size() ) ) {
						resultUniform = &uniform;
						needsLocationOffset = true;
						break;
					}
				}
				else {
					// the bases of the requested and active uniform match
					resultUniform = &uniform;
					needsLocationOffset = true;
					break;
				}
			}
		}
	}

	if( resultLocation && resultUniform ) {
		if( needsLocationOffset ) {
			CI_ASSERT( requestedNameLeftSquareBracket != string::npos && requestedNameRightSquareBracket != string::npos );

			// pull out the index and use it as an offset location for the resulting uniform
			// the try / catch handles cases where the index is a non-number, in which case we don't have a match
			try {
				string indexStr = name.substr( requestedNameLeftSquareBracket + 1, requestedNameRightSquareBracket - requestedNameLeftSquareBracket - 1 );
				*resultLocation = resultUniform->mLoc + stoi( indexStr );
			}
			catch( std::logic_error & ) {
				return nullptr;
			}
		}
		else
			*resultLocation = resultUniform->mLoc;
	}
	
	return resultUniform;
}

const GlslProg::Uniform* GlslProg::findUniform( int location, int *resultLocation ) const
{
	const Uniform* ret = nullptr;
	for( const auto & uniform : mUniforms ) {
		if( ( location >= uniform.mLoc ) && ( location < uniform.mLoc + uniform.mCount ) ) {
			ret = &uniform;
			// '*resultLocation' becomes 'location' to match signature & behavior of findUniform( const string& )
			if( resultLocation )
				*resultLocation = location;
			break;
		}
	}
	
	return ret;
}
    
bool GlslProg::hasAttribSemantic( geom::Attrib semantic ) const
{
    auto found = find_if( mAttributes.begin(), mAttributes.end(),
                   [semantic]( const Attribute &attrib ) {
                       return attrib.mSemantic == semantic;
				   } );

	return found != mAttributes.end();
}
    
GLint GlslProg::getAttribSemanticLocation( geom::Attrib semantic ) const
{
    auto found = find_if( mAttributes.begin(), mAttributes.end(),
                         [semantic]( const Attribute &attrib ) {
                             return attrib.mSemantic == semantic;
                         } );

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
    
#if defined( CINDER_GL_HAS_UNIFORM_BLOCKS )

GlslProg::UniformBlock* GlslProg::findUniformBlock( const std::string &name )
{
	UniformBlock *ret = nullptr;
	for( auto &uniformBlock : mUniformBlocks ) {
		if( uniformBlock.mName == name ) {
			ret = &uniformBlock;
			break;
		}
	}
	return ret;
}
	
const GlslProg::UniformBlock* GlslProg::findUniformBlock( const std::string &name ) const
{
	const UniformBlock *ret = nullptr;
	for( auto &uniformBlock : mUniformBlocks ) {
		if( uniformBlock.mName == name ) {
			ret = &uniformBlock;
			break;
		}
	}
	return ret;
}

void GlslProg::uniformBlock( int loc, int binding ) const
{
	auto found = find_if( mUniformBlocks.begin(), mUniformBlocks.end(),
						 [loc]( const UniformBlock &block ) {
							 return block.mLoc == loc;
						 } );

	if( found != mUniformBlocks.end() ) {
		if( found->mBlockBinding != binding ) {
			found->mBlockBinding = binding;
			glUniformBlockBinding( mHandle, found->mLoc, binding );
		}
	}
	else {
		CI_LOG_W( "Uniform block at " << loc << " location not found" );
	}
}

void GlslProg::uniformBlock( const std::string &name, GLint binding ) const
{
	auto found = findUniformBlock( name );
	if( found ) {
		if( found->mBlockBinding != binding ) {
			found->mBlockBinding = binding;
			glUniformBlockBinding( mHandle, found->mLoc, binding );
		}
	}
	else {
		CI_LOG_W( "Uniform block \"" << name << "\" not found" );
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
	auto found = find_if( mUniformBlocks.begin(), mUniformBlocks.end(),
						 [=]( const UniformBlock &block ) {
							 return block.mBlockBinding == blockBinding;
						 } );

	if( found != mUniformBlocks.end() )
		return found->mDataSize;
	else
		return -1;
}

#endif // defined( CINDER_GL_HAS_UNIFORM_BLOCKS )
#if defined( CINDER_GL_HAS_TRANSFORM_FEEDBACK )

const GlslProg::TransformFeedbackVaryings* GlslProg::findTransformFeedbackVaryings( const std::string &name ) const
{
	const TransformFeedbackVaryings *ret = nullptr;
	for( const auto &varying : mTransformFeedbackVaryings ) {
		if( varying.mName == name ) {
			ret = &varying;
		}
	}
	return ret;
}
	
GlslProg::TransformFeedbackVaryings* GlslProg::findTransformFeedbackVaryings( const std::string &name )
{
	TransformFeedbackVaryings *ret = nullptr;
	for( auto &varying : mTransformFeedbackVaryings ) {
		if( varying.mName == name ) {
			ret = &varying;
		}
	}
	return ret;
}

#endif // defined( CINDER_GL_HAS_TRANSFORM_FEEDBACK )

bool GlslProg::checkUniformValueCache( const Uniform &uniform, int location, const void *val, int count ) const
{
	if( mUniformValueCache )
		return mUniformValueCache->shouldBuffer( uniform.mBytePointer, uniform.mTypeSize, location - uniform.mLoc, count, val );
	else // no uniform cache means we've disabled it
		return true;
}
	
template<typename LookUp, typename T>
inline void GlslProg::uniformImpl( const LookUp &lookUp, const T &data ) const
{
	int uniformLocation = -1;
	auto found = findUniform( lookUp, &uniformLocation );
	if( ! found ) {
		logMissingUniform( lookUp );
		return;
	}
	if( validateUniform( *found, uniformLocation, data ) )
		uniformFunc( uniformLocation, data );
}

template<typename LookUp, typename T>
inline void	GlslProg::uniformMatImpl( const LookUp &lookUp, const T &data, bool transpose ) const
{
	int uniformLocation = -1;
	auto found = findUniform( lookUp, &uniformLocation );
	if( ! found ) {
		logMissingUniform( lookUp );
		return;
	}
	if( validateUniform( *found, uniformLocation, data ) )
		uniformMatFunc( uniformLocation, data, transpose );
}

template<typename LookUp, typename T>
inline void	GlslProg::uniformImpl( const LookUp &lookUp, const T *data, int count ) const
{
	int uniformLocation = -1;
	auto found = findUniform( lookUp, &uniformLocation );
	if( ! found ) {
		logMissingUniform( lookUp );
		return;
	}
	if( validateUniform( *found, uniformLocation, data, count ) )
		uniformFunc( uniformLocation, data, count );
}

template<typename LookUp, typename T>
inline void	GlslProg::uniformMatImpl( const LookUp &lookUp, const T *data, int count, bool transpose ) const
{
	int uniformLocation = -1;
	auto found = findUniform( lookUp, &uniformLocation );
	if( ! found ) {
		logMissingUniform( lookUp );
		return;
	}
	if( validateUniform( *found, uniformLocation, data, count ) )
		uniformMatFunc( uniformLocation, data, count, transpose );
}
	
template<typename T>
inline bool GlslProg::validateUniform( const Uniform &uniform, int uniformLocation, const T &val ) const
{
	if( ! checkUniformType<T>( uniform.mType ) ) {
		logUniformWrongType( uniform.mName, uniform.mType, cppTypeToGlslTypeName<T>() );
		return false;
	}
	else {
		return checkUniformValueCache( uniform, uniformLocation, &val, 1 );
	}
}

template<typename T>
inline bool GlslProg::validateUniform( const Uniform &uniform, int uniformLocation, const T *val, int count ) const
{
	if( ! checkUniformType<T>( uniform.mType ) ) {
		logUniformWrongType( uniform.mName, uniform.mType, cppTypeToGlslTypeName<T>() + "[" + std::to_string( count ) + "]" );
		return false;
	}
	else {
		return checkUniformValueCache( uniform, uniformLocation, val, count );
	}
}

template<typename T>
string GlslProg::cppTypeToGlslTypeName()
{
		 if( std::is_same<T,bool>::value ) return "bool";
	else if( std::is_same<T,glm::bvec2>::value ) return "bvec2";
	else if( std::is_same<T,glm::bvec3>::value ) return "bvec3";
	else if( std::is_same<T,glm::bvec4>::value ) return "bvec4";
	// unsigned int
	else if( std::is_same<T,uint32_t>::value ) return "uint32_t";
	else if( std::is_same<T,glm::uvec2>::value ) return "uvec2";
	else if( std::is_same<T,glm::uvec3>::value ) return "uvec3";
	else if( std::is_same<T,glm::uvec4>::value ) return "uvec4";
	// signed int
	else if( std::is_same<T,int32_t>::value ) return "int32_t";
	else if( std::is_same<T,glm::ivec2>::value ) return "ivec2";
	else if( std::is_same<T,glm::ivec3>::value ) return "ivec3";
	else if( std::is_same<T,glm::ivec4>::value ) return "ivec4";
	// float
	else if( std::is_same<T,float>::value ) return "float";
	else if( std::is_same<T,glm::vec2>::value ) return "vec2";
	else if( std::is_same<T,glm::vec3>::value ) return "vec3";
	else if( std::is_same<T,glm::vec4>::value ) return "vec4";
	// matrix
	else if( std::is_same<T,glm::mat2>::value ) return "mat2";
	else if( std::is_same<T,glm::mat3>::value ) return "mat3";
	else if( std::is_same<T,glm::mat4>::value ) return "mat4";
	else
		return "unknown";
}

template<typename T>
bool GlslProg::checkUniformType( GLenum uniformType ) const
{
	switch( uniformType ) {
		// bool
		case GL_BOOL: return std::is_same<T,bool>::value || std::is_same<T,int32_t>::value;
		case GL_BOOL_VEC2: return std::is_same<T,glm::bvec2>::value;
		case GL_BOOL_VEC3: return std::is_same<T,glm::bvec3>::value;
		case GL_BOOL_VEC4: return std::is_same<T,glm::bvec4>::value;
		// signed int
		case GL_INT: return std::is_same<T,int32_t>::value;
		case GL_INT_VEC2: return std::is_same<T,glm::ivec2>::value;
		case GL_INT_VEC3: return std::is_same<T,glm::ivec3>::value;
		case GL_INT_VEC4: return std::is_same<T,glm::ivec4>::value;
		case GL_SAMPLER_2D: return std::is_same<T,int32_t>::value;
#if defined( CINDER_ANDROID )
		case GL_SAMPLER_EXTERNAL_OES: return std::is_same<T,int32_t>::value;		
#endif		
		// unigned int
		case GL_UNSIGNED_INT: return std::is_same<T,uint32_t>::value;
#if ! defined( CINDER_GL_ES )
		case GL_SAMPLER_1D:						return std::is_same<T,int32_t>::value;
		case GL_SAMPLER_BUFFER_EXT:				return std::is_same<T,int32_t>::value;
		case GL_UNSIGNED_INT_SAMPLER_BUFFER:	return std::is_same<T, int32_t>::value;
		case GL_SAMPLER_2D_RECT:				return std::is_same<T,int32_t>::value;
		case GL_INT_SAMPLER_2D_RECT:			return std::is_same<T,int32_t>::value;
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT:	return std::is_same<T,int32_t>::value;		
#endif
#if ! defined( CINDER_GL_ES_2 )
		case GL_SAMPLER_3D:						return std::is_same<T,int32_t>::value;
		case GL_UNSIGNED_INT_VEC2:				return std::is_same<T,glm::uvec2>::value;
		case GL_UNSIGNED_INT_VEC3:				return std::is_same<T,glm::uvec3>::value;
		case GL_UNSIGNED_INT_VEC4:				return std::is_same<T,glm::uvec4>::value;
		case GL_SAMPLER_2D_SHADOW:				return std::is_same<T,int32_t>::value;
		case GL_SAMPLER_2D_ARRAY:				return std::is_same<T,int32_t>::value;		
		case GL_SAMPLER_2D_ARRAY_SHADOW:		return std::is_same<T,int32_t>::value;
		case GL_SAMPLER_CUBE_SHADOW:			return std::is_same<T,int32_t>::value;
		case GL_INT_SAMPLER_2D:					return std::is_same<T,int32_t>::value;
		case GL_INT_SAMPLER_3D:					return std::is_same<T,int32_t>::value;
		case GL_INT_SAMPLER_CUBE:				return std::is_same<T,int32_t>::value;
		case GL_INT_SAMPLER_2D_ARRAY:			return std::is_same<T,int32_t>::value;		
		case GL_UNSIGNED_INT_SAMPLER_2D:		return std::is_same<T,int32_t>::value;
		case GL_UNSIGNED_INT_SAMPLER_3D:		return std::is_same<T,int32_t>::value;
		case GL_UNSIGNED_INT_SAMPLER_CUBE:		return std::is_same<T,int32_t>::value;		
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:	return std::is_same<T,int32_t>::value;
#else
	#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
	  #if defined( CINDER_GL_HAS_SHADOW_SAMPLERS )
		case GL_SAMPLER_2D_SHADOW_EXT: return std::is_same<T,int32_t>::value;
	  #endif
	#else
		case GL_SAMPLER_2D_SHADOW_EXT: return std::is_same<T,int32_t>::value;
	#endif
#endif
		case GL_SAMPLER_CUBE: return std::is_same<T,int32_t>::value;
		// float
		case GL_FLOAT: return std::is_same<T,float>::value;
		case GL_FLOAT_VEC2: return std::is_same<T,glm::vec2>::value;
		case GL_FLOAT_VEC3: return std::is_same<T,glm::vec3>::value;
		case GL_FLOAT_VEC4: return std::is_same<T,glm::vec4>::value;
		// matrix
		case GL_FLOAT_MAT2: return std::is_same<T,glm::mat2>::value;
		case GL_FLOAT_MAT3: return std::is_same<T,glm::mat3>::value;
		case GL_FLOAT_MAT4: return std::is_same<T,glm::mat4>::value;
		default:
			CI_LOG_E( "Unknown uniform type" ); return false;
	}
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
	
#if ! defined( CINDER_GL_ES_2 )
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

#endif // ! defined( CINDER_GL_ES_2 )
	
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

// vec3
void GlslProg::uniform( const std::string &name, const vec3 &data ) const
{
	uniformImpl( name, data );
}

void GlslProg::uniform( int location, const vec3 &data ) const
{
	uniformImpl( location, data );
}

template<>
void GlslProg::uniformFunc( int location, const vec3 &data ) const
{
	ScopedGlslProg shaderBind( this );
    glUniform3f( location, data.x, data.y, data.z );
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
	uniformImpl( location, data, count );
}

template<>
void GlslProg::uniformFunc( int location, const vec3 *data, int count ) const
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
		os << "\t\t\"" << attrib.getName() << "\":" << std::endl;
		os << "\t\t Loc: " << attrib.getLocation() << std::endl;
		os << "\t\t Count: " << attrib.getCount() << std::endl;
		os << "\t\t Type: " << gl::constantToString( attrib.getType() ) << std::endl;
		os << "\t\t Semantic: <" << geom::attribToString( attrib.getSemantic() ) << ">" << std::endl;
	}
	
	os << "\tUniforms: " << std::endl;
	auto & uniforms = rhs.getActiveUniforms();
	for( auto &uniform : uniforms ) {
		os << "\t\t\"" << uniform.getName() << "\":" << std::endl;
		os << "\t\t Loc: " << uniform.getLocation() << std::endl;
		os << "\t\t Count: " << uniform.getCount() << std::endl;
		os << "\t\t Type: " << gl::constantToString( uniform.getType() ) << std::endl;
		os << "\t\t Semantic: <" << gl::uniformSemanticToString( uniform.getUniformSemantic() ) << ">" << std::endl;
	}
	
#if defined( CINDER_GL_HAS_UNIFORM_BLOCKS )
	os << "\tUniform Blocks: " << std::endl;
	auto & uniformBlocks = rhs.getActiveUniformBlocks();
	for( auto & uniformBlock : uniformBlocks ) {
		os << "\t\t\"" << uniformBlock.getName() << "\":" << std::endl;
		os << "\t\t Loc: " << uniformBlock.getLocation() << std::endl;
		os << "\t\t Size: " << uniformBlock.getDataSize() << std::endl;
		os << "\t\t BlockBinding: " << uniformBlock.getBlockBinding() << std::endl;
		os << "\t\t Active Uniforms: " << endl;
		for( auto & uniform : uniformBlock.getActiveUniforms() ) {
			os << "\t\t\t\"" << uniform.getName() << "\":" << std::endl;
			os << "\t\t\t Loc: " << uniform.getLocation() << std::endl;
			os << "\t\t\t Type: " << gl::constantToString( uniform.getType() ) << std::endl;
			os << "\t\t\t Semantic: <" << gl::uniformSemanticToString( uniform.getUniformSemantic() ) << ">" << std::endl;
		}
	}
#endif // defined( CINDER_GL_HAS_UNIFORM_BLOCKS )
#if defined( CINDER_GL_HAS_TRANSFORM_FEEDBACK )
	os << "\tTransform Feedback Varyings: " << std::endl;
	auto & feedbackVaryings = rhs.getActiveTransformFeedbackVaryings();
	for( auto & varying : feedbackVaryings ) {
		os << "\t\t\"" << varying.getName() << "\":" << std::endl;
		os << "\t\t Type: " << gl::constantToString( varying.getType() ) << std::endl;
		os << "\t\t Count: " << varying.getCount() << std::endl;
	}
#endif // defined( CINDER_GL_HAS_TRANSFORM_FEEDBACK )
	return os;
}

//////////////////////////////////////////////////////////////////////////
// GlslProgCompileExc
GlslProgCompileExc::GlslProgCompileExc( const std::string &log, GLint shaderType ) : mShaderType( shaderType )
{
	string typeString;

	switch( shaderType ) {
		case GL_VERTEX_SHADER:			typeString = "VERTEX: "; break;
		case GL_FRAGMENT_SHADER:		typeString = "FRAGMENT: "; break;
#if defined( CINDER_GL_HAS_GEOM_SHADER )
		case GL_GEOMETRY_SHADER:		typeString = "GEOMETRY: "; break;
#endif
#if defined( CINDER_GL_HAS_TESS_SHADER )
		case GL_TESS_CONTROL_SHADER:	typeString = "TESSELLATION CONTROL: "; break;
		case GL_TESS_EVALUATION_SHADER:	typeString = "TESSELLATION EVALUATION: "; break;
#endif
#if defined( CINDER_GL_HAS_COMPUTE_SHADER )
		case GL_COMPUTE_SHADER:			typeString = "COMPUTE: "; break;
#endif
		default:						typeString = "UNKNOWN: ";
	}

	setDescription( typeString + log );
}
	
} } // namespace cinder::gl
