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
#include "cinder/gl/gl.h"

#if defined( CINDER_GL_ES )

#include "cinder/gl/Shader.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Vao.h"

#if defined( CINDER_COCOA_TOUCH )
	#include <OpenGLES/ES2/glext.h>
#endif

namespace cinder { namespace gl {

class EnvironmentEs2 : public Environment {
  public:
	virtual void	initializeFunctionPointers() override;

	virtual bool	isExtensionAvailable( const std::string &extName ) override;
	virtual bool	supportsHardwareVao() override;
	virtual void	objectLabel( GLenum identifier, GLuint name, GLsizei length, const char *label );

	virtual std::string		generateVertexShader( const ShaderDef &shader ) override;
	virtual std::string		generateFragmentShader( const ShaderDef &shader ) override;
	virtual GlslProgRef		buildShader( const ShaderDef &shader ) override;
};

Environment* allocateEnvironmentEs2()
{
	return new EnvironmentEs2;
}

void EnvironmentEs2::initializeFunctionPointers()
{
}

bool EnvironmentEs2::isExtensionAvailable( const std::string &extName )
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

bool EnvironmentEs2::supportsHardwareVao()
{
#if defined( CINDER_COCOA_TOUCH )
	return true;
#else
	return false;
#endif
}

void EnvironmentEs2::objectLabel( GLenum identifier, GLuint name, GLsizei length, const char *label )
{
#if defined( CINDER_COCOA_TOUCH )
	glLabelObjectEXT( identifier, name, length, label );
#endif
}

std::string	EnvironmentEs2::generateVertexShader( const ShaderDef &shader )
{
	std::string s;
	
	s +=		"uniform mat4	ciModelViewProjection;\n"
				"\n"
				"attribute vec4		ciPosition;\n"
				;

	if( shader.mUniformBasedPosAndTexCoord ) {
		s +=	"uniform vec2	uPositionOffset, uPositionScale;\n";
		if( shader.mTextureMapping ) {
			s+= "uniform vec2	uTexCoordOffset, uTexCoordScale;\n";
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
	
	s +=		"}\n";
	
	return s;
}

std::string	EnvironmentEs2::generateFragmentShader( const ShaderDef &shader )
{
	std::string s;

	s +=		"precision highp float;\n"
				;

	if( shader.mTextureMapping ) {	
		s +=	"uniform sampler2D	uTex0;\n"
				"varying highp vec2	TexCoord;\n"
				;
	}
	if( shader.mColor ) {
		s +=	"varying lowp vec4	Color;\n"
				;
	}

	s +=		"void main( void )\n"
				"{\n"
				;
	
	if( shader.mTextureMapping && shader.mColor ) {
		s +=	"	gl_FragColor = texture2D( uTex0, TexCoord.st ) * Color;\n"
				;
	}
	else if( shader.mTextureMapping ) {
		s +=	"	gl_FragColor = texture2D( uTex0, TexCoord.st );\n"
				;
	}
	else if( shader.mColor ) {
		s +=	"	gl_FragColor = Color;\n"
				;
	}
	
	s +=		"}\n"
				;
	
	return s;
}


GlslProgRef	EnvironmentEs2::buildShader( const ShaderDef &shader )
{
	GlslProg::Format fmt = GlslProg::Format().vertex( generateVertexShader( shader ).c_str() )
												.fragment( generateFragmentShader( shader ).c_str() )
												.attribLocation( "ciPosition", 0 );
	if( shader.mTextureMapping )
		fmt.attribLocation( "ciTexCoord0", 1 );
	return GlslProg::create( fmt );
}

} } // namespace cinder::gl

#endif // defined( CINDER_GL_ES )