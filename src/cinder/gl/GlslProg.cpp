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

#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"

using boost::shared_ptr;
using namespace std;

namespace cinder { namespace gl {

GlslProg::Obj::~Obj()
{
	if( mHandle )
		glDeleteProgram( (GLuint)mHandle );
}

//////////////////////////////////////////////////////////////////////////
// GlslProg
GlslProg::GlslProg( DataSourceRef vertexShader, DataSourceRef fragmentShader, DataSourceRef geometryShader )
	: mObj( shared_ptr<Obj>( new Obj ) )
{
	mObj->mHandle = glCreateProgram();
	
	if ( vertexShader )
		loadShader( vertexShader->getBuffer(), GL_VERTEX_SHADER_ARB );
	if( fragmentShader )
		loadShader( fragmentShader->getBuffer(), GL_FRAGMENT_SHADER_ARB );
	if( geometryShader )
		loadShader( geometryShader->getBuffer(), GL_GEOMETRY_SHADER_EXT );

	link();
}

GlslProg::GlslProg( const char *vertexShader, const char *fragmentShader, const char *geometryShader )
	: mObj( shared_ptr<Obj>( new Obj ) )
{
	mObj->mHandle = glCreateProgram();
	
	if ( vertexShader )
		loadShader( vertexShader, GL_VERTEX_SHADER_ARB );
	if( fragmentShader )
		loadShader( fragmentShader, GL_FRAGMENT_SHADER_ARB );
	if( geometryShader )
		loadShader( geometryShader, GL_GEOMETRY_SHADER_EXT );

	link();
}

void GlslProg::loadShader( Buffer shaderSourceBuffer, GLint shaderType )
{
	// we need to duplicate the contents of the buffer and append a null-terminator
	shared_ptr<char> sourceBlock( new char[shaderSourceBuffer.getDataSize() + 1], checked_array_deleter<char>() );
	memcpy( sourceBlock.get(), shaderSourceBuffer.getData(), shaderSourceBuffer.getDataSize() );
	sourceBlock.get()[shaderSourceBuffer.getDataSize()] = 0; // null terminate
	const char *sourceBlockPtr = sourceBlock.get();
	loadShader( sourceBlockPtr, shaderType );
}

void GlslProg::loadShader( const char *shaderSource, GLint shaderType )
{
	GLuint handle = glCreateShader( shaderType );
	glShaderSource( handle, 1, reinterpret_cast<const GLchar**>( &shaderSource ), NULL );
	glCompileShader( handle );
	
	GLint status;
	glGetShaderiv( (GLuint) handle, GL_COMPILE_STATUS, &status );
	if( status != GL_TRUE ) {
		std::string log = getShaderLog( (GLuint)handle );
		throw GlslProgCompileExc( log, shaderType );
	}
	glAttachShader( mObj->mHandle, handle );
}

void GlslProg::link()
{
	glLinkProgram( mObj->mHandle );	
}

void GlslProg::bind() const
{
	glUseProgram( mObj->mHandle );
}

void GlslProg::unbind()
{
	glUseProgram( 0 );
}

std::string GlslProg::getShaderLog( GLuint handle ) const
{
	std::string log;
	
	GLchar *debugLog;
	GLint debugLength = 0, charsWritten = 0;
	glGetShaderiv( handle, GL_INFO_LOG_LENGTH, &debugLength );

	if( debugLength > 0 ) {
		debugLog = new GLchar[debugLength];
		glGetShaderInfoLog( handle, debugLength, &charsWritten, debugLog );
		log.append( debugLog, 0, debugLength );
		delete [] debugLog;
	}
	
	return log;
}

void GlslProg::uniform( const std::string &name, int data )
{
	GLint loc = getUniformLocation( name );
	glUniform1i( loc, data );
}

void GlslProg::uniform( const std::string &name, const Vec2i &data )
{
	GLint loc = getUniformLocation( name );
	glUniform2i( loc, data.x, data.y );
}

void GlslProg::uniform( const std::string &name, const int *data, int count )
{
	GLint loc = getUniformLocation( name );
	glUniform1iv( loc, count, data );
}

void GlslProg::uniform( const std::string &name, const Vec2i *data, int count )
{
	GLint loc = getUniformLocation( name );
	glUniform2iv( loc, count, &data[0].x );
}

void GlslProg::uniform( const std::string &name, float data )
{
	GLint loc = getUniformLocation( name );
	glUniform1f( loc, data );
}

void GlslProg::uniform( const std::string &name, const Vec2f &data )
{
	GLint loc = getUniformLocation( name );
	glUniform2f( loc, data.x, data.y );
}

void GlslProg::uniform( const std::string &name, const Vec3f &data )
{
	GLint loc = getUniformLocation( name );
	glUniform3f( loc, data.x, data.y, data.z );
}

void GlslProg::uniform( const std::string &name, const Vec4f &data )
{
	GLint loc = getUniformLocation( name );
	glUniform4f( loc, data.x, data.y, data.z, data.w );
}

void GlslProg::uniform( const std::string &name, const float *data, int count )
{
	GLint loc = getUniformLocation( name );
	glUniform1fv( loc, count, data );
}

void GlslProg::uniform( const std::string &name, const Vec2f *data, int count )
{
	GLint loc = getUniformLocation( name );
	glUniform2fv( loc, count, &data[0].x );
}

void GlslProg::uniform( const std::string &name, const Vec3f *data, int count )
{
	GLint loc = getUniformLocation( name );
	glUniform3fv( loc, count, &data[0].x );
}

void GlslProg::uniform( const std::string &name, const Vec4f *data, int count )
{
	GLint loc = getUniformLocation( name );
	glUniform4fv( loc, count, &data[0].x );
}

void GlslProg::uniform( const std::string &name, const Matrix44f &data, bool transpose )
{
	GLint loc = getUniformLocation( name );
	glUniformMatrix4fv( loc, 1, ( transpose ) ? GL_TRUE : GL_FALSE, data.m );
}

GLint GlslProg::getUniformLocation( const std::string &name )
{
	map<string,int>::const_iterator uniformIt = mObj->mUniformLocs.find( name );
	if( uniformIt == mObj->mUniformLocs.end() ) {
		GLint loc = glGetUniformLocation( mObj->mHandle, name.c_str() );
		mObj->mUniformLocs[name] = loc;
		return loc;
	}
	else
		return uniformIt->second;
}

GLint GlslProg::getAttribLocation( const std::string &name )
{
	return glGetAttribLocation( mObj->mHandle, name.c_str() );
}

//////////////////////////////////////////////////////////////////////////
// GlslProgCompileExc
GlslProgCompileExc::GlslProgCompileExc( const std::string &log, GLint aShaderType ) throw()
	: mShaderType( aShaderType )
{
	if( mShaderType == GL_VERTEX_SHADER_ARB )
		strncpy( mMessage, "VERTEX: ", 1000 );
	else if( mShaderType == GL_FRAGMENT_SHADER_ARB )
		strncpy( mMessage, "FRAGMENT: ", 1000 );
	else if( mShaderType == GL_GEOMETRY_SHADER_EXT )
		strncpy( mMessage, "GEOMETRY: ", 1000 );
	else
		strncpy( mMessage, "UNKNOWN: ", 1000 );
	strncat( mMessage, log.c_str(), 15000 );
}
	
} } // namespace cinder::gl
