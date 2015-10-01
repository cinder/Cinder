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
	bool	supportsMapBuffer() const override;
	bool 	supportsMapBufferRange() const override;

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
	::gl_es_load();

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
	return sExtensions.count( extension ) > 0;
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
	return isExtensionAvailable( "GL_OES_vertex_array_object" ) || isExtensionAvailable( "GL_ARB_vertex_array_object" );
#else
	// Assumes OpenGL ES 3 or greater
	return true;
#endif	
}

bool EnvironmentEs::supportsInstancedArrays() const
{
	return false;
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

bool EnvironmentEs::supportsMapBuffer() const
{
	return isExtensionAvailable( "GL_OES_mapbuffer" );
}

bool EnvironmentEs::supportsMapBufferRange() const
{
#if defined( CINDER_GL_ES_2 )
	return isExtensionAvailable( "GL_EXT_map_buffer_range" ) || isExtensionAvailable( "GL_ARB_map_buffer_range" );
#else
	// Assumes OpenGL ES 3 or greater
	return true;
#endif	
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

std::string	EnvironmentEs::generateVertexShader( const ShaderDef &shader )
{
	std::string s;

#if defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3 )
	s +=		"#version 100\n";
#endif
	
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
		s +=	"attribute vec2     ciTexCoord0;\n"
				"varying highp vec2	TexCoord;\n"
				;
	}
	if( shader.mColor ) {
		s +=	"attribute vec4    ciColor;\n"
				"varying lowp vec4 Color;\n"
				;
	}
	if( shader.mLambert ) {
		s += "attribute vec3    ciNormal;\n"
			"varying highp vec3 Normal;\n"
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

#if defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3 )
	s +=		"#version 100\n";
#endif	

#if defined( CINDER_ANDROID )
	if( shader.mTextureMappingExternalOes) {
		s += 	"#extension GL_OES_EGL_image_external : require\n"
				;
	}
#endif		

	s +=		"precision highp float;\n";

#if defined( CINDER_ANDROID )
	if( shader.mTextureMapping ) {	
		if( shader.mTextureMappingExternalOes ) {
			s +=	"uniform samplerExternalOES	uTex0;\n"
					"varying highp vec2         TexCoord;\n"
					;
		}
		else {
			s +=	"uniform sampler2D  uTex0;\n"
					"varying highp vec2	TexCoord;\n"
					;
		}
	}
#else	
	if( shader.mTextureMapping ) {
		s +=	"uniform sampler2D  uTex0;\n"
				"varying highp vec2 TexCoord;\n"
				;
	}
#endif

	if( shader.mColor ) {
		s +=	"varying lowp vec4 Color;\n";
	}

	if( shader.mLambert ) { 
		s +=	"varying highp vec3 Normal;\n";
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
	
	s +=		"	gl_FragColor = vec4( 1 )";
	
	if( shader.mTextureMapping ) {
		s +=	" * texture2D( uTex0, TexCoord.st )";
	}
	
	if( shader.mColor ) {
		s +=	" * Color";
	}

	if( shader.mLambert ) {
		s +=	" * vec4( vec3( lambert ), 1.0 )";
	}

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

	if( shader.mTextureMapping ) {
		fmt.attribLocation( "ciTexCoord0", 1 );
	}

	return GlslProg::create( fmt );
}

} } // namespace cinder::gl

#endif // defined( CINDER_GL_ES )