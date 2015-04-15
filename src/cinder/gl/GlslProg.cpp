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
#include "cinder/gl/ConstantStrings.h"
#include "cinder/gl/Environment.h"
#include "cinder/gl/ShaderPreprocessor.h"
#include "cinder/gl/scoped.h"
#include "cinder/Log.h"

#include "glm/gtc/type_ptr.hpp"

using namespace std;

namespace cinder { namespace gl {

GlslProg::UniformSemanticMap	GlslProg::sDefaultUniformNameToSemanticMap;
GlslProg::AttribSemanticMap		GlslProg::sDefaultAttribNameToSemanticMap;

//////////////////////////////////////////////////////////////////////////
// GlslProg::Format
GlslProg::Format::Format()
	: mPreprocessingEnabled( true ), mVersion( 0 )
#if ! defined( CINDER_GL_ES_2 )
	, mTransformFormat( -1 )
#endif
{
	mAttribSemanticLocMap[geom::Attrib::POSITION] = 0;
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
	mAttribSemanticMap[attribName] = semantic;
	return *this;
}

GlslProg::Format& GlslProg::Format::uniform( UniformSemantic semantic, const std::string &attribName )
{
	mUniformSemanticMap[attribName] = semantic;
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
	mAttribNameLocMap[attribName] = location;
	return *this;
}

GlslProg::Format& GlslProg::Format::attribLocation( geom::Attrib attrib, GLint location )
{
	mAttribSemanticLocMap[attrib] = location;
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
	: mActiveUniformTypesCached( false ), mActiveAttribTypesCached( false ),
	mUniformSemanticsCached( false ), mUniformNameToSemanticMap( getDefaultUniformNameToSemanticMap() ),
	mAttribSemanticsCached( false ), mAttribNameToSemanticMap( getDefaultAttribNameToSemanticMap() )
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

	// copy the Format's attribute-semantic map
	for( auto &attribSemantic : format.getAttribSemantics() )
		mAttribNameToSemanticMap.insert( attribSemantic );

	// copy the Format's uniform-semantic map
	for( auto &uniformSemantic : format.getUniformSemantics() )
		mUniformNameToSemanticMap.insert( uniformSemantic );

	// THESE sections take all attribute locations which have been specified (either by their semantic or their names)
	// and ultimately maps them via glBindAttribLocation, which must be done ahead of linking
	auto attribLocations = format.getAttribNameLocations();
	
	// map the locations-specified semantics to their respective attribute name locations
	for( auto &semanticLoc : format.getAttribSemanticLocations() ) {
		string attribName;
		// first find if we have an attribute associated with a given semantic
		for( auto &attribSemantic : mAttribNameToSemanticMap ) {
			if( attribSemantic.second == semanticLoc.first ) {
				attribName = attribSemantic.first;
				break;
			}
		}
		
		// if we found an appropriate attribute-semantic pair, set attribLocations[attrib name] to be the semantic location
		if( ! attribName.empty() )
			attribLocations[attribName] = semanticLoc.second;
	}
	
	// finally, bind all location-specified attributes to their respective locations
	for( auto &attribLoc : attribLocations )
		glBindAttribLocation( mHandle, attribLoc.second, attribLoc.first.c_str() );
	
#if ! defined( CINDER_GL_ES_2 )
	if( ! format.getVaryings().empty() && format.getTransformFormat() > 0 ) {
		// This is a mess due to an NVidia driver bug on MSW which expects the memory passed to glTransformFeedbackVaryings
		// to still be around after the call. We allocate the storage and put it on the GlslProg itself to be freed at destruction
		size_t totalSizeBytes = 0;
		for( auto &v : format.getVaryings() )
			totalSizeBytes += v.length() + 1;

		mTransformFeedbackVaryingsChars = std::unique_ptr<std::vector<GLchar>>( new vector<GLchar>() );
		mTransformFeedbackVaryingsCharStarts = std::unique_ptr<std::vector<GLchar*>>( new vector<GLchar*>() );
		mTransformFeedbackVaryingsChars->resize( totalSizeBytes );
		size_t curOffset = 0;
		for( auto &v : format.getVaryings() ) {
			mTransformFeedbackVaryingsCharStarts->push_back( &(*mTransformFeedbackVaryingsChars)[curOffset] );
			memcpy( &(*mTransformFeedbackVaryingsChars)[curOffset], v.c_str(), v.length() + 1 );
			curOffset += v.length() + 1;
		}
		glTransformFeedbackVaryings( mHandle, (GLsizei)format.getVaryings().size(), mTransformFeedbackVaryingsCharStarts->data(), format.getTransformFormat() );
	}
#endif

#if ! defined( CINDER_GL_ES )
	// setup fragment data locations
	for( const auto &fragDataLocation : format.getFragDataLocations() )
		glBindFragDataLocation( mHandle, fragDataLocation.second, fragDataLocation.first.c_str() );
#endif

	link();
	
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

// int
void GlslProg::uniform( int location, int data ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform1i( location, data );
}

void GlslProg::uniform( const std::string &name, int data ) const
{
	ScopedGlslProg shaderBind( this );
	GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniform1i( loc, data );
}

// ivec2
void GlslProg::uniform( int location, const ivec2 &data ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform2i( location, data.x, data.y );
}

void GlslProg::uniform( const std::string &name, const ivec2 &data ) const
{
	ScopedGlslProg shaderBind( this );
	GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniform2i( loc, data.x, data.y );
}

// int *, count
void GlslProg::uniform( int location, const int *data, int count ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform1iv( location, count, data );
}

void GlslProg::uniform( const std::string &name, const int *data, int count ) const
{
	ScopedGlslProg shaderBind( this );
	GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniform1iv( loc, count, data );
}

// ivec2 *, count
void GlslProg::uniform( int location, const ivec2 *data, int count ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform2iv( location, count, &data[0].x );
}

void GlslProg::uniform( const std::string &name, const ivec2 *data, int count ) const
{
	ScopedGlslProg shaderBind( this );
	GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniform2iv( loc, count, &data[0].x );
}

// float
void GlslProg::uniform( int location, float data ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform1f( location, data );
}

void GlslProg::uniform( const std::string &name, float data ) const
{
	ScopedGlslProg shaderBind( this );
	GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniform1f( loc, data );
}

// vec2
void GlslProg::uniform( int location, const vec2 &data ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform2f( location, data.x, data.y );
}

void GlslProg::uniform( const std::string &name, const vec2 &data ) const
{
	ScopedGlslProg shaderBind( this );
	GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniform2f( loc, data.x, data.y );
}

// vec3
void GlslProg::uniform( int location, const vec3 &data ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform3f( location, data.x, data.y, data.z );
}

void GlslProg::uniform( const std::string &name, const vec3 &data ) const
{
	ScopedGlslProg shaderBind( this );
	GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniform3f( loc, data.x, data.y, data.z );
}

// vec4
void GlslProg::uniform( int location, const vec4 &data ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform4f( location, data.x, data.y, data.z, data.w );
}

void GlslProg::uniform( const std::string &name, const vec4 &data ) const
{
	ScopedGlslProg shaderBind( this );
	GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniform4f( loc, data.x, data.y, data.z, data.w );
}

// mat3
void GlslProg::uniform( int location, const mat3 &data, bool transpose ) const
{
    ScopedGlslProg shaderBind( this );
    glUniformMatrix3fv( location, 1, ( transpose ) ? GL_TRUE : GL_FALSE, glm::value_ptr( data ) );
}

void GlslProg::uniform( const std::string &name, const mat3 &data, bool transpose ) const
{
    ScopedGlslProg shaderBind( this );
    GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniformMatrix3fv( loc, 1, ( transpose ) ? GL_TRUE : GL_FALSE, glm::value_ptr( data ) );
}

// mat4
void GlslProg::uniform( int location, const mat4 &data, bool transpose ) const
{
    ScopedGlslProg shaderBind( this );
    glUniformMatrix4fv( location, 1, ( transpose ) ? GL_TRUE : GL_FALSE, glm::value_ptr( data ) );
}

void GlslProg::uniform( const std::string &name, const mat4 &data, bool transpose ) const
{
    ScopedGlslProg shaderBind( this );
    GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniformMatrix4fv( loc, 1, ( transpose ) ? GL_TRUE : GL_FALSE, glm::value_ptr( data ) );
}

// Color
void GlslProg::uniform( int location, const Color &data ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform3f( location, data.r, data.g, data.b );
}

void GlslProg::uniform( const std::string &name, const Color &data ) const
{
	ScopedGlslProg shaderBind( this );
	GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniform3f( loc, data.r, data.g, data.b );
}

// ColorA
void GlslProg::uniform( int location, const ColorA &data ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform4f( location, data.r, data.g, data.b, data.a );
}

void GlslProg::uniform( const std::string &name, const ColorA &data ) const
{
	ScopedGlslProg shaderBind( this );
	GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniform4f( loc, data.r, data.g, data.b, data.a );
}

// float*, count
void GlslProg::uniform( int location, const float *data, int count ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform1fv( location, count, data );
}

void GlslProg::uniform( const std::string &name, const float *data, int count ) const
{
	ScopedGlslProg shaderBind( this );
	GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniform1fv( loc, count, data );
}

// vec2*, count
void GlslProg::uniform( int location, const vec2 *data, int count ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform2fv( location, count, &data[0].x );
}

void GlslProg::uniform( const std::string &name, const vec2 *data, int count ) const
{
	ScopedGlslProg shaderBind( this );
	GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniform2fv( loc, count, &data[0].x );
}

// vec3*, count
void GlslProg::uniform( int location, const vec3 *data, int count ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform3fv( location, count, &data[0].x );
}

void GlslProg::uniform( const std::string &name, const vec3 *data, int count ) const
{
	ScopedGlslProg shaderBind( this );
	GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniform3fv( loc, count, &data[0].x );
}

// vec4*, count
void GlslProg::uniform( int location, const vec4 *data, int count ) const
{
	ScopedGlslProg shaderBind( this );
	glUniform4fv( location, count, &data[0].x );
}

void GlslProg::uniform( const std::string &name, const vec4 *data, int count ) const
{
	ScopedGlslProg shaderBind( this );
	GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniform4fv( loc, count, &data[0].x );
}

// mat3*, count
void GlslProg::uniform( int location, const mat3 *data, int count, bool transpose ) const
{
    ScopedGlslProg shaderBind( this );
    glUniformMatrix3fv( location, count, ( transpose ) ? GL_TRUE : GL_FALSE, glm::value_ptr( *data ) );
}

void GlslProg::uniform( const std::string &name, const mat3 *data, int count, bool transpose ) const
{
    ScopedGlslProg shaderBind( this );
    GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniformMatrix3fv( loc, count, ( transpose ) ? GL_TRUE : GL_FALSE, glm::value_ptr( *data ) );
}

// mat4*, count
void GlslProg::uniform( int location, const mat4 *data, int count, bool transpose ) const
{
    ScopedGlslProg shaderBind( this );
    glUniformMatrix4fv( location, count, ( transpose ) ? GL_TRUE : GL_FALSE, glm::value_ptr( *data ) );
}

void GlslProg::uniform( const std::string &name, const mat4 *data, int count, bool transpose ) const
{
    ScopedGlslProg shaderBind( this );
    GLint loc = getUniformLocation( name );
	if( loc == -1 ) {
		if( mLoggedMissingUniforms.count( name ) == 0 ) {
			CI_LOG_E( "Unknown uniform: \"" << name << "\"" );
			mLoggedMissingUniforms.insert( name );
		}
	}
	else
		glUniformMatrix4fv( loc, count, ( transpose ) ? GL_TRUE : GL_FALSE, glm::value_ptr( *data ) );
}

GLint GlslProg::getUniformLocation( const std::string &name ) const
{
	map<string,int>::const_iterator uniformIt = mUniformLocs.find( name );
	if( uniformIt == mUniformLocs.end() ) {
		GLint loc = glGetUniformLocation( mHandle, name.c_str() );
		mUniformLocs[name] = loc;
		return loc;
	}
	else {
		return uniformIt->second;
	}
}

#if ! defined( CINDER_GL_ES_2 )
void GlslProg::uniformBlock( int loc, int binding )
{
	glUniformBlockBinding( mHandle, loc, binding );
}

void GlslProg::uniformBlock( const std::string &name, GLint binding )
{
	GLint loc = getUniformBlockLocation( name );
	if( loc == -1 )
		CI_LOG_E( "Unknown uniform block: \"" << name << "\"" );
	else
		glUniformBlockBinding( mHandle, loc, binding );
}

GLint GlslProg::getUniformBlockLocation( const std::string &name ) const
{
	auto existing = mUniformBlockLocs.find( name );
	if( existing == mUniformBlockLocs.end() ) {
		const GLuint loc = glGetUniformBlockIndex( mHandle, name.c_str() );
		if( loc == GL_INVALID_INDEX )
			return -1;
		else
			mUniformBlockLocs[name] = loc;
		return loc;
	}
	else
		return existing->second;
}

GLint GlslProg::getUniformBlockSize( GLint blockIndex ) const
{
	auto existing = mUniformBlockSizes.find( blockIndex );
	if( existing == mUniformBlockSizes.end() ) {
		GLint blockSize = 0;
		glGetActiveUniformBlockiv( mHandle, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize );
		mUniformBlockSizes[blockIndex] = blockSize;
		return blockSize;
	}
	else
		return existing->second;
}
#endif // ! defined( CINDER_GL_ES_2 )

const std::map<std::string,GLenum>& GlslProg::getActiveUniformTypes() const
{
	if( ! mActiveUniformTypesCached ) {
		GLint numActiveUniforms = 0;
		glGetProgramiv( mHandle, GL_ACTIVE_UNIFORMS, &numActiveUniforms );
		for( GLint i = 0; i < numActiveUniforms; ++i ) {
			char name[512];
			GLsizei nameLength;
			GLint size;
			GLenum type;
			glGetActiveUniform( mHandle, (GLuint)i, 511, &nameLength, &size, &type, name );
			name[nameLength] = 0;
			mActiveUniformTypes[name] = type;
		}
		mActiveUniformTypesCached = true;
	}
	return mActiveUniformTypes;
}

const std::map<std::string,GLenum>& GlslProg::getActiveAttribTypes() const
{
	if( ! mActiveAttribTypesCached ) {
		GLint numActiveAttrs = 0;
		glGetProgramiv( mHandle, GL_ACTIVE_ATTRIBUTES, &numActiveAttrs );
		for( GLint i = 0; i < numActiveAttrs; ++i ) {
			char name[512];
			GLsizei nameLength;
			GLint size;
			GLenum type;
			glGetActiveAttrib( mHandle, (GLuint)i, 511, &nameLength, &size, &type, name );
			name[nameLength] = 0;
			mActiveAttribTypes[name] = type;
		}
		mActiveAttribTypesCached = true;
	}
	return mActiveAttribTypes;
}

const GlslProg::UniformSemanticMap& GlslProg::getUniformSemantics() const
{
	if( ! mUniformSemanticsCached ) {
		auto activeUniformTypes = getActiveUniformTypes();
	
		for( auto activeUnifIt = activeUniformTypes.begin(); activeUnifIt != activeUniformTypes.end(); ++activeUnifIt ) {
			// first find this active uniform by name in the mUniformNameToSemanticMap
			auto semantic = mUniformNameToSemanticMap.find( activeUnifIt->first );
			if( semantic != mUniformNameToSemanticMap.end() ) {
				// found this semantic, add it mUniformSemantics
				mUniformSemantics[semantic->first] = semantic->second;
			}
			// if this uniform starts with "ci[A-Z]" it is likely a typo
			else if( (activeUnifIt->first.length() > 4) && (activeUnifIt->first.substr( 0, 2 ) == "ci") && (isupper(activeUnifIt->first[2]) ) ) {
				CI_LOG_W( "\"" << activeUnifIt->first << "\" may reference a Cinder uniform but is not a known semantic" );
			}
		}
	
		mUniformSemanticsCached = true;
	}
	
	return mUniformSemantics;
}

const GlslProg::AttribSemanticMap& GlslProg::getAttribSemantics() const
{
	if( ! mAttribSemanticsCached ) {
		auto activeAttrTypes = getActiveAttribTypes();
	
		for( auto activeAttrIt = activeAttrTypes.begin(); activeAttrIt != activeAttrTypes.end(); ++activeAttrIt ) {
			// first find this active attribute by name in the mAttrNameToSemanticMap
			auto semantic = mAttribNameToSemanticMap.find( activeAttrIt->first );
			if( semantic != mAttribNameToSemanticMap.end() ) {
				// found this semantic, add it mAttrSemantics
				mAttribSemantics[semantic->first] = semantic->second;
			}
			// if this attribute starts with "ci[A-Z]" it is likely a typo
			else if( (activeAttrIt->first.length() > 4) && (activeAttrIt->first.substr( 0, 2 ) == "ci") && (isupper(activeAttrIt->first[2]) ) ) {
				CI_LOG_W( "\"" << activeAttrIt->first << "\" may reference a Cinder attribute but is not a known semantic" );
			}
		}
	
		mAttribSemanticsCached = true;
	}
	
	return mAttribSemantics;
}

bool GlslProg::hasAttribSemantic( geom::Attrib semantic ) const
{
	auto &semantics = getAttribSemantics();
	for( auto semIt = semantics.begin(); semIt != semantics.end(); ++semIt ) {
		if( semIt->second == semantic )
			return true;
	}
	
	return false;
}

GLint GlslProg::getAttribSemanticLocation( geom::Attrib semantic ) const
{
	auto &semantics = getAttribSemantics();
	for( auto semIt = semantics.begin(); semIt != semantics.end(); ++semIt ) {
		if( semIt->second == semantic )
			return getAttribLocation( semIt->first );
	}
	
	return -1;
}

GLint GlslProg::getAttribLocation( const std::string &name ) const
{
	auto existing = mAttribLocs.find( name );
	if( existing == mAttribLocs.end() ) {
		const GLint loc = glGetAttribLocation( mHandle, name.c_str() );
		if( loc != -1 )
			mAttribLocs[name] = loc;
		return loc;
	}
	else
		return existing->second;
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

std::ostream& operator<<( std::ostream &os, const GlslProg &rhs )
{
	os << "ID: " << rhs.mHandle << std::endl;
	if( ! rhs.mLabel.empty() )
		os << "    Label: " << rhs.mLabel << std::endl;
	os << " Uniforms: " << std::endl;
	auto uniformTypes = rhs.getActiveUniformTypes();
	for( auto &uni : uniformTypes ) {
		os << "  \"" << uni.first << "\":" << std::endl;
		os << "    Loc: " << rhs.getUniformLocation( uni.first ) << std::endl;
		os << "    Type: " << gl::constantToString( uni.second ) << std::endl;
		auto semIt = rhs.getUniformSemantics().find( uni.first );
		if( semIt != rhs.getUniformSemantics().end() ) {
			os << "    Semantic: <" << gl::uniformSemanticToString( semIt->second ) << ">" << std::endl;
		}
	}

	auto attribTypes = rhs.getActiveAttribTypes();
	os << " Attributes: " << std::endl;
	for( auto &attrib : attribTypes ) {
		os << "  \"" << attrib.first << "\":" << std::endl;
		os << "    Loc: " << rhs.getAttribLocation( attrib.first ) << std::endl;
		os << "    Type: " << gl::constantToString( attrib.second ) << std::endl;
		auto semIt = rhs.getAttribSemantics().find( attrib.first );
		if( semIt != rhs.getAttribSemantics().end() ) {
			os << "    Semantic: <" << geom::attribToString( semIt->second ) << ">" << std::endl;
		}
	}

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