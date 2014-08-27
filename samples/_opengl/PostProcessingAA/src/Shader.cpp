/*
 Copyright (c) 2014, Paul Houx - All rights reserved.
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

#include "Shader.h"

#include "cinder/Utilities.h"
#include "cinder/app/AppBasic.h"

// Required to parse contents of shader files.
#include <regex>

#if defined(CINDER_MSW)
#define FORMATTED_STRING( dst, size, format, ... ) sprintf_s(dst, size, format, ##__VA_ARGS__)
#else
#define FORMATTED_STRING( dst, size, format, ... ) snprintf(dst, size, format, ##__VA_ARGS__)
#endif

using namespace ci;

Shader::Shader( void )
	: mHasGeometryShader( false ), mIsLiveEditable( false ), mGlslVersion( 0 )
{
}

Shader::Shader( const std::string& name )
	: mName( name ), mVertexFile( name + ".vert" ), mFragmentFile( name + ".frag" ), mGeometryFile( name + ".geom" )
	, mHasGeometryShader( false ), mIsLiveEditable( false ), mGlslVersion( 0 )
{
	load();
}

Shader::Shader( const std::string& name, const std::string& vert, const std::string& frag, const std::string& geom )
	: mName( name ), mVertexFile( vert ), mFragmentFile( frag ), mGeometryFile( geom )
	, mHasGeometryShader( false ), mIsLiveEditable( false ), mGlslVersion( 0 )
{
	load();
}

Shader::~Shader( void )
{
}

ShaderRef Shader::create()
{
	return std::make_shared<Shader>();
}

ShaderRef Shader::create( const std::string& name )
{
	return std::make_shared<Shader>( name );
}

void Shader::load()
{
	// get a reference to our path (and create it in the process)
	const fs::path& path = getPath();
	if( path.empty() ) {
		char msg[512]; FORMATTED_STRING( msg, 512, "Could not find shader '%s'.", mName.c_str() );
		throw std::runtime_error( msg );
	}

	// check if all files are present
	if( !fs::exists( path / mFragmentFile ) ) {
		char msg[512]; FORMATTED_STRING( msg, 512, "Shader '%s' is not complete. Make sure you have both a vertex and fragment shader file.", mName.c_str() );
		throw std::runtime_error( msg );
	}

	// parse source
	std::string vertexSource = parseShader( path / mVertexFile );
	std::string fragmentSource = parseShader( path / mFragmentFile );

	try {
		mHasGeometryShader = fs::exists( path / mGeometryFile );
		if( mHasGeometryShader ) {
			std::string geometrySource = parseShader( path / mGeometryFile );
			mGlslProg = gl::GlslProg::create( gl::GlslProg::Format()
											  .vertex( vertexSource.c_str() )
											  .fragment( fragmentSource.c_str() )
											  .geometry( geometrySource.c_str() ) );
		}
		else {
			mGlslProg = gl::GlslProg::create( gl::GlslProg::Format()
											  .vertex( vertexSource.c_str() )
											  .fragment( fragmentSource.c_str() ) );
		}
	}
	catch( const std::exception& e ) {
		char msg[64 * 1024]; FORMATTED_STRING( msg, 64 * 1024, "Failed to compile shader '%s':\n%s", mName.c_str(), e.what() );
		throw std::runtime_error( msg );
	}
}

const fs::path& Shader::getPath() const
{
	// return path if already found
	if( !mPath.empty() ) return mPath;

	// find path:
	//  1. in assets folder
	//  2. in assets/shaders folder
	//  3. next to executable
	mPath = app::getAssetPath( "" );
	if( fs::exists( mPath / mVertexFile ) ) return mPath;

	mPath = app::getAssetPath( "" ) / "shaders";
	if( fs::exists( mPath / mVertexFile ) ) return mPath;

	mPath = app::getAppPath();
	if( fs::exists( mPath / mVertexFile ) ) return mPath;

	// not found
	mPath.clear();
	return mPath;
}

std::string Shader::parseShader( const fs::path& path, bool optional, int level )
{
	std::stringstream output;

	if( level > 32 ) {
		char msg[512]; FORMATTED_STRING( msg, 512, "Reached the maximum parse depth for shader '%s'. This is probably due to circular dependencies. Check your #includes and remove #include statements from comments.", mName.c_str() );
		throw std::runtime_error( msg );

		return std::string();
	}

	static const std::regex includeRegexp( "^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*" );
	static const std::regex commentRegexp( "(.*?)/\\*(.*?)\\*/(.*?)" );
	static const std::regex commentBeginRegexp( "(.*?)/\\*(.*?)" );
	static const std::regex commentEndRegexp( "(.*?)\\*/(.*?)" );

	std::ifstream input( path.c_str() );
	if( !input.is_open() ) {
		if( optional )
			return std::string();

		if( level == 0 ) {
			char msg[512]; FORMATTED_STRING( msg, 512, "Failed to open shader file '%s'.", path.c_str() );
			throw std::runtime_error( msg );
		}
		else {
			char msg[512]; FORMATTED_STRING( msg, 512, "Failed to open shader include file '%s'.", path.c_str() );
			throw std::runtime_error( msg );
		}

		return std::string();
	}

	// go through each line and process includes
	std::string line;
	std::smatch	matches;

	//bool isInsideComment = false;
	while( std::getline( input, line ) ) {
		/*
		if( isInsideComment ) {
		if( std::regex_search( line, matches, commentEndRegexp ) ) {
		isInsideComment = false;
		output << matches[2].str() << std::endl;
		}
		}
		else {
		// Remove single line comments.
		line = std::regex_replace( line, commentRegexp, "" );
		// Handle multi line comments.
		if( std::regex_search( line, matches, commentBeginRegexp ) ) {
		isInsideComment = true;
		output << matches[1].str();
		}
		else
		*/
		if( std::regex_search( line, matches, includeRegexp ) )
			output << parseShader( path.parent_path() / matches[1].str(), false, level + 1 );
		else
			output << line;

		output << std::endl;
		//}
	}

	input.close();

	// make sure #version is the first line of the shader
	if( level == 0 )
		return parseVersion( output.str() );
	else
		return output.str();
}

std::string Shader::parseVersion( const std::string& code )
{
	static const std::regex versionRegexp( "^[ ]*#[ ]*version[ ]+([123456789][0123456789][0123456789]).*$" );

	if( code.empty() )
		return std::string();

	mGlslVersion = 120;

	std::stringstream completeCode( code );
	std::ostringstream cleanedCode;

	std::string line;
	std::smatch matches;
	while( std::getline( completeCode, line ) ) {
		if( std::regex_match( line, matches, versionRegexp ) ) {
			unsigned int versionNum = ci::fromString< unsigned int >( matches[1] );
			mGlslVersion = std::max( versionNum, mGlslVersion );

			continue;
		}

		cleanedCode << line << std::endl;
	}

	std::stringstream vs;
	vs << "#version " << mGlslVersion << std::endl << cleanedCode.str();

	return vs.str();
}

#undef FORMATTED_STRING