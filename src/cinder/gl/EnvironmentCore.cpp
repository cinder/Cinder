/*
 Copyright (c) 2013, The Cinder Project
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

#include "cinder/gl/Environment.h"
#include "cinder/gl/platform.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/Texture.h"

#if ! defined( CINDER_GL_ES )

namespace cinder { namespace gl {

class EnvironmentCore : public Environment {
  public:
	void	initializeFunctionPointers() override;

	bool	isExtensionAvailable( const std::string &extName ) const override;

	bool 	supportsFboMultiSample() const override;
	bool 	supportsCoverageSample() const override;
	bool	supportsHardwareVao() const override;
	bool 	supportsInstancedArrays() const override;
	bool	supportsTextureLod() const override;
	bool	supportsGeometryShader() const override;
	bool	supportsTessellationShader() const override;

	GLenum	getPreferredIndexType() const override;

	void	objectLabel( GLenum identifier, GLuint name, GLsizei length, const char *label ) override;

	void	allocateTexStorage1d( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, bool immutable, GLint texImageDataType ) override;
	void	allocateTexStorage2d( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, bool immutable, GLint texImageDataType ) override;
	void	allocateTexStorage3d( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, bool immutable ) override;
	void	allocateTexStorageCubeMap( GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, bool immutable ) override;
	
	std::string		generateVertexShader( const ShaderDef &shader ) override;
	std::string		generateFragmentShader( const ShaderDef &shader ) override;
	GlslProgRef		buildShader( const ShaderDef &shader ) override;
};

Environment* allocateEnvironmentCore()
{
	return new EnvironmentCore;
}

void EnvironmentCore::initializeFunctionPointers()
{
	static bool sInitialized = false;
	if( ! sInitialized ) {
		sInitialized = ( gladLoadGL() != 0 );
	}
}

bool EnvironmentCore::isExtensionAvailable( const std::string &extName ) const
{	
	static bool sInitialized = false;
	static std::set<std::string> sExtensions;
	if( ! sInitialized ) {
		GLint loop;
		GLint numExtensions;
		glGetIntegerv( GL_NUM_EXTENSIONS, &numExtensions );

		for( loop = 0 ;loop < numExtensions; loop++ ) {
			std::string s = (const char *)glGetStringi( GL_EXTENSIONS, loop );
			std::transform( s.begin(), s.end(), s.begin(), static_cast<int(*)(int)>( tolower ) );
			sExtensions.insert( s );
		}
		
		sInitialized = true;
	}

	// convert to lower case
	std::string extension = extName;
	std::transform( extension.begin(), extension.end(), extension.begin(), static_cast<int(*)(int)>( tolower ) );	
	return sExtensions.count( extension ) > 0;
}

bool EnvironmentCore::supportsFboMultiSample() const
{
	return true;
}

bool EnvironmentCore::supportsCoverageSample() const
{
	return true;
}

bool EnvironmentCore::supportsHardwareVao() const
{
	return true;
}

bool EnvironmentCore::supportsInstancedArrays() const
{
	return true;
}

bool EnvironmentCore::supportsTextureLod() const
{
	return true;
}

bool EnvironmentCore::supportsGeometryShader() const
{
	return isExtensionAvailable( "GL_EXT_geometry_shader" );
}

bool EnvironmentCore::supportsTessellationShader() const
{
	return isExtensionAvailable( "GL_EXT_tessellation_shader" );
}

GLenum EnvironmentCore::getPreferredIndexType() const
{
	return GL_UNSIGNED_INT;
}

void EnvironmentCore::objectLabel( GLenum identifier, GLuint name, GLsizei length, const char *label )
{
	static auto objectLabelFn = glObjectLabel;
	if( objectLabelFn )
		(*objectLabelFn)( identifier, name, length, label );
}

void EnvironmentCore::allocateTexStorage1d( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, bool immutable, GLint texImageDataType )
{
	static auto texStorage1DFn = glTexStorage1D;
	if( texStorage1DFn && immutable )
		texStorage1DFn( target, levels, internalFormat, width );
	else {
		GLenum dataFormat, dataType;
		TextureBase::getInternalFormatInfo( internalFormat, &dataFormat, &dataType );
		if( texImageDataType != -1 )
			dataType = texImageDataType;
		glTexImage1D( target, 0, internalFormat, width, 0, dataFormat, dataType, nullptr );
	}
}

void EnvironmentCore::allocateTexStorage2d( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, bool immutable, GLint texImageDataType )
{
	static auto texStorage2DFn = glTexStorage2D;
	if( texStorage2DFn && immutable )
		texStorage2DFn( target, levels, internalFormat, width, height );
	else {
		GLenum dataFormat, dataType;
		TextureBase::getInternalFormatInfo( internalFormat, &dataFormat, &dataType );
		if( texImageDataType != -1 )
			dataType = texImageDataType;
		glTexImage2D( target, 0, internalFormat, width, height, 0, dataFormat, dataType, nullptr );
	}
}

void EnvironmentCore::allocateTexStorage3d( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, bool immutable )
{
	static auto texStorage3DFn = glTexStorage3D;
	if( texStorage3DFn && immutable )
		texStorage3DFn( target, levels, internalFormat, width, height, depth );
	else {
		GLenum dataFormat, dataType;
		TextureBase::getInternalFormatInfo( internalFormat, &dataFormat, &dataType );
		glTexImage3D( target, 0, internalFormat, width, height, depth, 0, dataFormat, dataType, nullptr );
	}	
}

void EnvironmentCore::allocateTexStorageCubeMap( GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, bool immutable )
{
	static auto texStorage2DFn = glTexStorage2D;
	if( texStorage2DFn && immutable )
		texStorage2DFn( GL_TEXTURE_CUBE_MAP, levels, internalFormat, width, height );
	else {
		GLenum dataFormat, dataType;
		TextureBase::getInternalFormatInfo( internalFormat, &dataFormat, &dataType );
		for( int face = 0; face < 6; ++face )
			glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, internalFormat, width, height, 0, dataFormat, dataType, nullptr );
	}
}

std::string	EnvironmentCore::generateVertexShader( const ShaderDef &shader )
{	
	std::string s;
	
	s +=		"#version 150\n"
				"\n"
				"uniform mat4 ciModelViewProjection;\n"
				;

	if( shader.mLambert )
		s +=	"uniform mat3 ciNormalMatrix;\n";

	s +=		"\n"
				"in vec4 ciPosition;\n"
				;

	if( shader.mUniformBasedPosAndTexCoord ) {
		s +=	"uniform vec2 uPositionOffset, uPositionScale;\n";
		if( shader.mTextureMapping ) {
			s+=	"uniform vec2 uTexCoordOffset, uTexCoordScale;\n";
		}
	}
	
	if( shader.mTextureMapping ) {
		s +=	"in vec2 ciTexCoord0;\n"
				"out highp vec2 TexCoord;\n"
				;
	}
	if( shader.mColor ) {
		s +=	"in vec4 ciColor;\n"
				"out lowp vec4 Color;\n"
				;
	}
	if( shader.mLambert ) {
		s +=	"in vec3 ciNormal;\n"
				"out highp vec3 Normal;\n"
				;
	}

	s +=		"void main( void )\n"
				"{\n";
	if( shader.mUniformBasedPosAndTexCoord )
		s +=	"	gl_Position = ciModelViewProjection * ( vec4( uPositionOffset, 0, 0 ) + vec4( uPositionScale, 1, 1 ) * ciPosition );\n";
	else
		s +=	"	gl_Position = ciModelViewProjection * ciPosition;\n"
				;
	if( shader.mTextureMapping ) {
		if( shader.mUniformBasedPosAndTexCoord )
			s+=	"	TexCoord = uTexCoordOffset + uTexCoordScale * ciTexCoord0;\n";
		else
			s+=	"	TexCoord = ciTexCoord0;\n";
				;
	}
	if( shader.mColor ) {
		s +=	"	Color = ciColor;\n"
				;
	}
	if( shader.mLambert ) {
		s +=	"	Normal = ciNormalMatrix * ciNormal;\n"
				;
	}
	
	s +=		"}";
	
	return s;
}

std::string	EnvironmentCore::generateFragmentShader( const ShaderDef &shader )
{
	std::string s;
	
	s+=			"#version 150\n"
				"\n"
				"out vec4 oColor;\n"
				;

	if( shader.mTextureMapping ) {
		if( shader.mTextureMappingRectangleArb )
			s +="uniform sampler2DRect uTex0;\n";
		else
			s +="uniform sampler2D uTex0;\n";
		s	+=	"in vec2	TexCoord;\n";
				;
	}
	if( shader.mColor ) {
		s +=	"in vec4 Color;\n";
	}

	if( shader.mLambert ) {
		s +=	"in vec3 Normal;\n";
	}

	s +=		"void main( void )\n"
				"{\n"
				;

	if( shader.mLambert ) {
		s +=	"	const vec3 L = vec3( 0, 0, 1 );\n"
				"	vec3 N = normalize( Normal );\n"
				"	float lambert = max( 0.0, dot( N, L ) );\n"
				;
	}
	
	s += "	oColor = vec4( 1 )";

	if( shader.mTextureMapping ) {
		s +=	" * texture( uTex0, TexCoord.st )";
		if( ! Texture::supportsHardwareSwizzle() && ! shader.isTextureSwizzleDefault() )
			s += "." + shader.getTextureSwizzleString();
	}

	if( shader.mColor )
		s +=	" * Color";

	if( shader.mLambert )
		s +=	" * vec4( vec3( lambert ), 1.0 )";

	s +=	";\n";
	
	s +=	"}";
	
	return s;
}


GlslProgRef	EnvironmentCore::buildShader( const ShaderDef &shader )
{
	GlslProg::Format fmt = GlslProg::Format().vertex( generateVertexShader( shader ) )
												.fragment( generateFragmentShader( shader ) )
												.attribLocation( "ciPosition", 0 )
												.preprocess( false );
	if( shader.mTextureMapping )
		fmt.attribLocation( "ciTexCoord0", 1 );
	return GlslProg::create( fmt );		
}

} } // namespace cinder::gl

#endif // ! defined( CINDER_GL_ES )