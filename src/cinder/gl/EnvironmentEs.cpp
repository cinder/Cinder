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

#include "cinder/app/App.h"

#if defined( CINDER_GL_ES )

#include "cinder/gl/Shader.h"
#include "cinder/gl/ConstantConversions.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/Texture.h"

#if defined( CINDER_COCOA_TOUCH )
	#include <OpenGLES/ES2/glext.h>
#endif

#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
 	#include "EGL/egl.h"
#endif

#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
/* 
  #if ! defined( CINDER_GL_HAS_INSTANCED_ARRAYS )
 	using PFNGLVERTEXATTRIBDIVISOREXTPROC = void*;
  #endif

  #if ! defined( CINDER_GL_HAS_MAP_BUFFER_RANGE )
 	using PFNGLMAPBUFFERRANGEEXTPROC = void*;
 	using PFNGLFLUSHMAPPEDBUFFERRANGEEXTPROC = void*;
  #endif
*/  

/*
  #if ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_2 )
	PFNGLBINDVERTEXARRAYOESPROC			fnptr_ci_glBindVertexArrayOES = nullptr;
	PFNGLDELETEVERTEXARRAYSOESPROC		fnptr_ci_glDeleteVertexArraysOES = nullptr;
	PFNGLGENVERTEXARRAYSOESPROC			fnptr_ci_glGenVertexArraysOES = nullptr;
	PFNGLISVERTEXARRAYOESPROC			fnptr_ci_glIsVertexArrayOES = nullptr;

	PFNGLVERTEXATTRIBDIVISOREXTPROC		fnptr_ci_glVertexAttribDivisorEXT= nullptr;

		PFNGLMAPBUFFERRANGEEXTPROC 			fnptr_ci_glMapBufferRangeEXT = nullptr;
	PFNGLFLUSHMAPPEDBUFFERRANGEEXTPROC	fnptr_ci_glFlushMappedBufferRangeEXT = nullptr;
  #endif	
 
	PFNGLMAPBUFFEROESPROC				fnptr_ci_glMapBufferOES = nullptr;
	PFNGLUNMAPBUFFEROESPROC				fnptr_ci_glUnmapBufferOES = nullptr;
	PFNGLGETBUFFERPOINTERVOESPROC		fnptr_ci_glGetBufferPointervOES = nullptr;
*/	
#endif

namespace cinder { namespace gl {

class EnvironmentEs : public Environment {
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

Environment* allocateEnvironmentEs()
{
	return new EnvironmentEs;
}

void EnvironmentEs::initializeFunctionPointers()
{
#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
	gladLoadGLES2Loader( (GLADloadproc)eglGetProcAddress );

/*
   #if ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_2 )
	if( supportsHardwareVao() ) {
		fnptr_ci_glBindVertexArrayOES = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress( "glBindVertexArrayOES" );
		fnptr_ci_glDeleteVertexArraysOES = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress( "glDeleteVertexArraysOES" );
		fnptr_ci_glGenVertexArraysOES = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress( "glGenVertexArraysOES" );
		fnptr_ci_glIsVertexArrayOES = (PFNGLISVERTEXARRAYOESPROC)eglGetProcAddress( "glIsVertexArrayOES" );		
	}

	if( supportsMapBufferRange() ) {
	 	fnptr_ci_glMapBufferRangeEXT = (PFNGLMAPBUFFERRANGEEXTPROC)eglGetProcAddress( "glMapBufferRangeEXT" );
		fnptr_ci_glFlushMappedBufferRangeEXT = (PFNGLFLUSHMAPPEDBUFFERRANGEEXTPROC)eglGetProcAddress( "glFlushMappedBufferRangeEXT" );
	} 
  #endif

 	if( isExtensionAvailable( "GL_OES_mapbuffer" ) ) {
		fnptr_ci_glMapBufferOES  = (PFNGLMAPBUFFEROESPROC)eglGetProcAddress( "glMapBufferOES" );
		fnptr_ci_glUnmapBufferOES  = (PFNGLUNMAPBUFFEROESPROC)eglGetProcAddress( "glUnmapBufferOES" );
		fnptr_ci_glGetBufferPointervOES	= (PFNGLGETBUFFERPOINTERVOESPROC)eglGetProcAddress( "glGetBufferPointervOES" );
	}
*/
#endif
}

bool EnvironmentEs::isExtensionAvailable( const std::string &extName ) const
{
	static bool sInitialized = false;
	static std::set<std::string> sExtensions;
	if( ! sInitialized ) {
		const char *buf = reinterpret_cast<const char*>( glGetString( GL_EXTENSIONS ) );
		if( 0 != buf ) {
			std::string extsStr = std::string( buf );
			size_t startPos = 0;
			size_t endPos = extsStr.find( ' ' );
			bool done = ( std::string::npos == endPos );
			while( ! done ) {
				size_t len = endPos - startPos;
				std::string s = extsStr.substr( startPos, len );
				std::transform( s.begin(), s.end(), s.begin(), static_cast<int(*)(int)>( tolower ) );
				sExtensions.insert( s );

				startPos = endPos + 1;
				endPos = extsStr.find( ' ', startPos );
				if( std::string::npos == endPos ) {
					endPos = extsStr.length();
				}

				done = ( startPos >= endPos );
			}
		}
		sInitialized = true;
	}

	// convert to lower case
	std::string extension = extName;
	std::transform( extension.begin(), extension.end(), extension.begin(), static_cast<int(*)(int)>( tolower ) );
	bool result = ( sExtensions.count( extension ) > 0 );
	return result;
}

bool EnvironmentEs::supportsFboMultiSample() const
{
	return false;
}

bool EnvironmentEs::supportsCoverageSample() const
{
	return false;
}

bool EnvironmentEs::supportsHardwareVao() const
{
#if defined( CINDER_GL_ES_2 )
	static bool result = isExtensionAvailable( "GL_OES_vertex_array_object" ) || isExtensionAvailable( "GL_ARB_vertex_array_object" );
	return result;
#else
	// Assumes OpenGL ES 3 or greater
	return true;
#endif	
}

bool EnvironmentEs::supportsInstancedArrays() const
{
#if defined( CINDER_GL_ES_2 )
	static bool result = isExtensionAvailable( "GL_EXT_draw_instanced" ) || isExtensionAvailable( "GL_NV_draw_instanced" );
	return result;
#else
	return true;
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

bool EnvironmentEs::supportsGeometryShader() const
{
	static bool result = isExtensionAvailable( "GL_EXT_geometry_shader" );
	return result;
}

bool EnvironmentEs::supportsTessellationShader() const
{
	static bool result = isExtensionAvailable( "GL_EXT_tessellation_shader" );
	return result;
}

GLenum EnvironmentEs::getPreferredIndexType() const
{
#if defined( CINDER_GL_ES_2 )
	return isExtensionAvailable( "GL_OES_element_index_uint" ) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
#else	
	return GL_UNSIGNED_INT;
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
	// Test at runtime for presence of 'glTexStorage2D' and just force mutable storage if it's not available
	// both ANGLE and iOS support EXT_texture_storage
  #if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
	static auto texStorage2DFn = (void (*)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height))nullptr;
  #else	
	static auto texStorage2DFn = glTexStorage2DEXT;
  #endif
#else
	static auto texStorage2DFn = glTexStorage2D;
#endif
	if( immutable && texStorage2DFn ) {
		texStorage2DFn( target, levels, internalFormat, width, height );
	} 
	else {
		GLenum dataFormat, dataType;
		TextureBase::getInternalFormatInfo( internalFormat, &dataFormat, &dataType );
		if( texImageDataType != -1 ) {
			dataType = texImageDataType;
		}
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
  #if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
	static auto texStorage2DFn = (void (*)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height))nullptr;
  #else	
	static auto texStorage2DFn = glTexStorage2DEXT;
  #endif	
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

class ShaderSource {
public:
	ShaderSource& operator<<( const std::string& s ) { mStream << s << "\n"; return *this; }
	std::string str() const { return mStream.str(); }
private:
	std::stringstream mStream;
};

std::string	EnvironmentEs::generateVertexShader( const ShaderDef &shader )
{
	ShaderSource ss;

#if ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3 )
  #if ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_3 )
	ss << "#version 300 es";
  #elif	( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_3_1 )
	ss << "#version 310 es";
  #elif ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_3_2 )
	ss << "#version 320 es";
  #endif

	ss << "uniform mat4 ciModelViewProjection;";

	if( shader.mLambert ) {
		ss <<  "uniform mat3	ciNormalMatrix;";
	}
	
	ss << "in vec4      ciPosition;";

	if( shader.mUniformBasedPosAndTexCoord ) {
		ss << "uniform vec2 uPositionOffset, uPositionScale;";
		if( shader.mTextureMapping ) {
			ss << "uniform vec2 uTexCoordOffset, uTexCoordScale;";
		}
	}

	if( shader.mTextureMapping ) {
		ss << "in vec2        ciTexCoord0;";
		ss << "out highp vec2 TexCoord;";
	}

	if( shader.mColor ) {
		ss << "in vec4        ciColor;";
		ss << "out lowp vec4  Color;";
	}

	if( shader.mLambert ) {
		ss << "in vec3        ciNormal;";
		ss << "out highp vec3 Normal;";
	}


	ss << "void main( void )";
	ss << "{";

	if( shader.mUniformBasedPosAndTexCoord ) {
		ss << "    gl_Position = ciModelViewProjection * ( vec4( uPositionOffset, 0, 0 ) + vec4( uPositionScale, 1, 1 ) * ciPosition );";
	}
	else {
		ss << "    gl_Position = ciModelViewProjection * ciPosition;";
	}

	if( shader.mTextureMapping ) {	
		if( shader.mUniformBasedPosAndTexCoord ) {
			ss << "    TexCoord = uTexCoordOffset + uTexCoordScale * ciTexCoord0;";
		}
		else {
			ss << "    TexCoord = ciTexCoord0;";
		}
	}

	if( shader.mColor ) {
		ss << "    Color = ciColor;";
	}

	if( shader.mLambert ) {
		ss << "    Normal = ciNormalMatrix * ciNormal;";
	}

	ss << "}";	

#else // OpenGL ES 2.0

	ss << "#version 100";

	ss << "uniform mat4   ciModelViewProjection;";

	if( shader.mLambert ) {
		ss <<  "uniform mat3	ciNormalMatrix;";
	}

	ss << "attribute vec4 ciPosition;";

	if( shader.mUniformBasedPosAndTexCoord ) {
		ss << "uniform vec2 uPositionOffset, uPositionScale;";
		if( shader.mTextureMapping ) {
			ss << "uniform vec2 uTexCoordOffset, uTexCoordScale;";
		}
	}

	if( shader.mTextureMapping ) {
		ss << "attribute vec2 ciTexCoord0;";
		ss << "varying highp vec2 TexCoord;";
	}

	if( shader.mColor ) {
		ss << "attribute vec4    ciColor;";
		ss << "varying lowp vec4 Color;";
	}

	if( shader.mLambert ) {
		ss << "attribute vec3     ciNormal;";
		ss << "varying highp vec3 Normal;";
	}


	ss << "void main( void )";
	ss << "{";

	if( shader.mUniformBasedPosAndTexCoord ) {
		ss << "    gl_Position = ciModelViewProjection * ( vec4( uPositionOffset, 0, 0 ) + vec4( uPositionScale, 1, 1 ) * ciPosition );";
	}
	else {
		ss << "    gl_Position = ciModelViewProjection * ciPosition;";
	}

	if( shader.mTextureMapping ) {	
		if( shader.mUniformBasedPosAndTexCoord ) {
			ss << "    TexCoord = uTexCoordOffset + uTexCoordScale * ciTexCoord0;";
		}
		else {
			ss << "    TexCoord = ciTexCoord0;";
		}
	}

	if( shader.mColor ) {
		ss << "    Color = ciColor;";
	}

	if( shader.mLambert ) {
		ss << "    Normal = ciNormalMatrix * ciNormal;";
	}

	ss << "}";
#endif

	return ss.str();
}

std::string	EnvironmentEs::generateFragmentShader( const ShaderDef &shader )
{
	ShaderSource ss;	

#if ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3 )
  #if ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_3 )
	ss << "#version 300 es";
  #elif	( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_3_1 )
	ss << "#version 310 es";
  #elif ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_3_2 )
	ss << "#version 320 es";
  #endif

  #if defined( CINDER_ANDROID )
	if( shader.mTextureMappingExternalOes) {
		ss << "#extension GL_OES_EGL_image_external : require";
	}
  #endif

	ss << "precision highp float;";

  #if defined( CINDER_ANDROID )
	if( shader.mTextureMapping ) {	
		if( shader.mTextureMappingExternalOes ) {
			ss << "uniform samplerExternalOES uTex0;";
			ss << "in highp vec2              TexCoord;";
		}
		else {
			ss << "uniform sampler2D uTex0;";
			ss << "in highp vec2     TexCoord;";
		}
	}
  #else	
	if( shader.mTextureMapping ) {
		ss << "uniform sampler2D uTex0;";
		ss << "in highp vec2     TexCoord;";
	}
  #endif
	if( shader.mColor ) {
		ss << "in lowp vec4 Color;";
	}

	if( shader.mLambert ) { 
		ss << "in highp vec3 Normal;";
	}

	ss << "out lowp vec4 outColor;";
	ss << "void main( void )";
	ss << "{\n";

	if( shader.mLambert ) {
		ss << "    const vec3 L = vec3( 0, 0, 1 );";
		ss << "    vec3 N = normalize( Normal );";
		ss << "    float lambert = max( 0.0, dot( N, L ) );";
	}
	
	std::string s = "outColor = vec4( 1 )";
	
	if( shader.mTextureMapping ) {
		s += " * texture( uTex0, TexCoord.st )";
	}
	
	if( shader.mColor ) {
		s += " * Color";
	}

	if( shader.mLambert ) {
		s += " * vec4( vec3( lambert ), 1.0 )";
	}

	s += ";";
	
	ss << "    " + s;
	
	ss << "}";

#else // OpenGL ES 2.0

	ss << "#version 100";

  #if defined( CINDER_ANDROID )
	if( shader.mTextureMappingExternalOes) {
		ss << "#extension GL_OES_EGL_image_external : require";
	}
  #endif

	ss << "precision highp float;";

  #if defined( CINDER_ANDROID )
	if( shader.mTextureMapping ) {	
		if( shader.mTextureMappingExternalOes ) {
			ss << "uniform samplerExternalOES uTex0;";
			ss << "varying highp vec2         TexCoord;";
		}
		else {
			ss << "uniform sampler2D  uTex0;";
			ss << "varying highp vec2 TexCoord;";
		}
	}
  #else	
	if( shader.mTextureMapping ) {
		ss << "uniform sampler2D  uTex0;";
		ss << "varying highp vec2 TexCoord;";
	}
  #endif
	if( shader.mColor ) {
		ss << "varying lowp vec4 Color;";
	}

	if( shader.mLambert ) { 
		ss << "varying highp vec3 Normal;";
	}

	ss << "void main( void )";
	ss << "{\n";

	if( shader.mLambert ) {
		ss << "    const vec3 L = vec3( 0, 0, 1 );";
		ss << "    vec3 N = normalize( Normal );";
		ss << "    float lambert = max( 0.0, dot( N, L ) );";
	}
	
	std::string s = "gl_FragColor = vec4( 1 )";
	
	if( shader.mTextureMapping ) {
		s += " * texture2D( uTex0, TexCoord.st )";
	}
	
	if( shader.mColor ) {
		s += " * Color";
	}

	if( shader.mLambert ) {
		s += " * vec4( vec3( lambert ), 1.0 )";
	}

	s += ";";

	ss << "    " << s;
	
	ss << "}";

#endif

	return ss.str();	
}


GlslProgRef	EnvironmentEs::buildShader( const ShaderDef &shader )
{
	GlslProg::Format fmt = GlslProg::Format().vertex( generateVertexShader( shader ) )
												.fragment( generateFragmentShader( shader ) )
												.attribLocation( "ciPosition", 0 )
												.preprocess( false );

	if( shader.mTextureMapping ) {
		fmt.attribLocation( "ciTexCoord0", 1 );
	}

	return GlslProg::create( fmt );
}

} } // namespace cinder::gl

#endif // defined( CINDER_GL_ES )