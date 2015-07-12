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

#include "cinder/gl/platform.h"
#include "cinder/gl/Environment.h"
#include "cinder/Log.h"

#if defined( CINDER_GL_ES )

#include "cinder/gl/Shader.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/Texture.h"

#if defined( CINDER_COCOA_TOUCH )
	#include <OpenGLES/ES2/glext.h>
#endif

namespace cinder { namespace gl {

class EnvironmentEs : public Environment {
  public:
	void	initializeFunctionPointers() override;

	bool	isExtensionAvailable( const std::string &extName ) override;
	bool	supportsHardwareVao() override;
	bool	supportsTextureLod() const override;
	void	objectLabel( GLenum identifier, GLuint name, GLsizei length, const char *label ) override;
	
	void	allocateTexStorage1d( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, bool immutable, GLint texImageDataType ) override;
	void	allocateTexStorage2d( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, bool immutable, GLint texImageDataType ) override;
	void	allocateTexStorage3d( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, bool immutable ) override;
	void	allocateTexStorageCubeMap( GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, bool immutable ) override;

	std::string		generateVertexShader( const ShaderDef &shader ) override;
	std::string		generateFragmentShader( const ShaderDef &shader ) override;
	GlslProgRef		buildShader( const ShaderDef &shader ) override;
};

Environment* allocateEnvironmentEs()
{
	return new EnvironmentEs;
}

void EnvironmentEs::initializeFunctionPointers()
{
}

bool EnvironmentEs::isExtensionAvailable( const std::string &extName )
{
	static const char *sExtStr = reinterpret_cast<const char*>( glGetString( GL_EXTENSIONS ) );
	static std::map<std::string, bool> sExtMap;
	
	std::map<std::string,bool>::const_iterator extIt = sExtMap.find( extName );
	if ( extIt == sExtMap.end() ) {
		bool found		= false;
		size_t extNameLen	= extName.size();
		const char *p	= sExtStr;
		const char *end = sExtStr + strlen( sExtStr );
		while( p < end ) {
			size_t n = strcspn( p, " " );
			if ( (extNameLen == n) && ( strncmp( extName.c_str(), p, n) == 0 ) ) {
				found = true;
				break;
			}
			p += (n + 1);
		}
		sExtMap[extName] = found;
		return found;
	}
	else {
		return extIt->second;
	}
}

bool EnvironmentEs::supportsHardwareVao()
{
#if defined( CINDER_COCOA_TOUCH ) || defined( CINDER_GL_ES_3 )
	return true;
#else
	return false;
#endif
}

bool EnvironmentEs::supportsTextureLod() const
{
#if defined( CINDER_COCOA_TOUCH ) || defined( CINDER_GL_ES_3 )
	return true;
#else
	static bool result = isExtensionAvailable( "GL_EXT_shader_texture_lod" );
	return result;
#endif
}

void EnvironmentEs::objectLabel( GLenum identifier, GLuint name, GLsizei length, const char *label )
{
#if defined( CINDER_COCOA_TOUCH )
	glLabelObjectEXT( identifier, name, length, label );
#endif
}

void EnvironmentEs::allocateTexStorage1d( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, bool immutable, GLint texImageDataType )
{
	throw gl::Exception( "allocateTexStorage1d unimplemented on OpenGL ES" );
}

void EnvironmentEs::allocateTexStorage2d( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, bool immutable, GLint texImageDataType )
{
#if defined( CINDER_GL_ES_2 )
	// test at runtime for presence of 'glTexStorage2D' and just force mutable storage if it's not available
	// both ANGLE and iOS support EXT_texture_storage
	static auto texStorage2DFn = glTexStorage2DEXT;
#else
	static auto texStorage2DFn = glTexStorage2D;
#endif
	if( immutable && texStorage2DFn ) {
		texStorage2DFn( target, levels, internalFormat, width, height );
	}
	else {
		GLenum dataFormat, dataType;
		TextureBase::getInternalFormatInfo( internalFormat, &dataFormat, &dataType );
		if( texImageDataType != -1 )
			dataType = texImageDataType;
// on ES 2 non-sized formats are required for internalFormat
#if defined( CINDER_GL_ES_2 )
		internalFormat = dataFormat;
#endif
		glTexImage2D( target, 0, internalFormat, width, height, 0, dataFormat, dataType, nullptr );
	}
}

void EnvironmentEs::allocateTexStorage3d( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, bool immutable )
{
#if defined( CINDER_GL_ES_2 )
	CI_LOG_E( "allocateTexStorage3d called on unsupported platform" );
#else
  #if ! defined( CINDER_GL_ES_3 )
	// test at runtime for presence of 'glTexStorage2D' and just force mutable storage if it's not available
	// both ANGLE and iOS support EXT_texture_storage
	static auto texStorage3DFn = glTexStorage3DEXT;
  #else
	static auto texStorage3DFn = glTexStorage3D;
  #endif
	if( immutable && texStorage3DFn ) {
		texStorage3DFn( target, levels, internalFormat, width, height, depth );
	}
	else {
		GLenum dataFormat, dataType;
		TextureBase::getInternalFormatInfo( internalFormat, &dataFormat, &dataType );
		glTexImage3D( target, 0, internalFormat, width, height, depth, 0, dataFormat, dataType, nullptr );
	}
#endif
}

void EnvironmentEs::allocateTexStorageCubeMap( GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, bool immutable )
{
#if defined( CINDER_GL_ES_2 )
	// test at runtime for presence of 'glTexStorage2D' and just force mutable storage if it's not available
	// both ANGLE and iOS support EXT_texture_storage
	static auto texStorage2DFn = glTexStorage2DEXT;
#else
	static auto texStorage2DFn = glTexStorage2D;
#endif
	if( immutable && texStorage2DFn )
		texStorage2DFn( GL_TEXTURE_CUBE_MAP, levels, internalFormat, width, height );
	else {
		GLenum dataFormat, dataType;
		TextureBase::getInternalFormatInfo( internalFormat, &dataFormat, &dataType );
		for( int face = 0; face < 6; ++face )
			glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, internalFormat, width, height, 0, dataFormat, dataType, nullptr );
	}
}

std::string	EnvironmentEs::generateVertexShader( const ShaderDef &shader )
{
	std::string s;
	
	s +=		"uniform mat4	ciModelViewProjection;\n";

	if( shader.mLambert )
		s +=	"uniform mat3	ciNormalMatrix;\n";

	s +=		"\n"
				"attribute vec4		ciPosition;\n"
				;

	if( shader.mUniformBasedPosAndTexCoord ) {
		s +=	"uniform vec2 uPositionOffset, uPositionScale;\n";
		if( shader.mTextureMapping ) {
			s+= "uniform vec2 uTexCoordOffset, uTexCoordScale;\n";
		}
	}

	if( shader.mTextureMapping ) {
		s +=	"attribute vec2		ciTexCoord0;\n"
				"varying highp vec2	TexCoord;\n"
				;
	}
	if( shader.mColor ) {
		s +=	"attribute vec4		ciColor;\n"
				"varying vec4		Color;\n"
				;
	}
	if( shader.mLambert ) {
		s += "attribute vec3		ciNormal;\n"
			"varying highp vec3		Normal;\n"
			;
	}

	s +=		"void main( void )\n"
				"{\n"
				;
	if( shader.mUniformBasedPosAndTexCoord )
		s +=	"	gl_Position = ciModelViewProjection * ( vec4( uPositionOffset, 0, 0 ) + vec4( uPositionScale, 1, 1 ) * ciPosition );\n";
	else
		s +=	"	gl_Position	= ciModelViewProjection * ciPosition;\n"
				;
				
	if( shader.mTextureMapping ) {	
		if( shader.mUniformBasedPosAndTexCoord )
			s+= "	TexCoord	= uTexCoordOffset + uTexCoordScale * ciTexCoord0;\n";
		else
			s+=	"	TexCoord	= ciTexCoord0;\n";
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
	
	s +=		"}\n";
	
	return s;
}

std::string	EnvironmentEs::generateFragmentShader( const ShaderDef &shader )
{
	std::string s;

	s +=		"precision highp float;\n";

	if( shader.mTextureMapping ) {
		s +=	"uniform sampler2D	uTex0;\n"
				"varying highp vec2	TexCoord;\n"
				;
	}

	if( shader.mColor ) 
		s +=	"varying lowp vec4	Color;\n";

	if( shader.mLambert ) 
		s +=	"varying highp vec3	Normal;\n";

	s +=		"void main( void )\n"
				"{\n"
				;

	if( shader.mLambert ) {
		s +=	"	const vec3 L = vec3( 0, 0, 1 );\n"
				"	vec3 N = normalize( Normal );\n"
				"	float lambert = max( 0.0, dot( N, L ) );\n"
				;
	}
	
	s +=		"	gl_FragColor = vec4( 1 )";
	
	if( shader.mTextureMapping ) 
		s +=	" * texture2D( uTex0, TexCoord.st )";
	
	if( shader.mColor ) 
		s +=	" * Color";

	if( shader.mLambert )
		s +=	" * vec4( vec3( lambert ), 1.0 )";

	s +=		";\n";
	
	s +=		"}\n";
	
	return s;
}


GlslProgRef	EnvironmentEs::buildShader( const ShaderDef &shader )
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

#endif // defined( CINDER_GL_ES )