/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.

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

#include "cinder/gl/ShaderPreprocessor.h"
#include "cinder/app/Platform.h"
#include "cinder/gl/platform.h"
#include "cinder/Utilities.h"
#include "cinder/Log.h"

using namespace std;

namespace cinder { namespace gl {

namespace {
// due to null or the rest of the line being a comment (//)
bool isTerminated( const char *c )
{
	if( *c == '\0' )
		return true;
	const char *next = c + 1;
	if( *c == '/' && *next && *next == '/' )
		return true;
	return false;
}

bool isWhiteSpace( const char *c )
{
	return *c == ' ' || *c == '\t';
}

bool isInlineCommentStart( const char *c )
{
	if( (*c == '\0') || (*c != '/') )
		return false;
	const char *next = c + 1;
	if( *next && *next == '*' )
		return true;
	return false;
}

bool isInlineCommentEnd( const char *c )
{
	if( (*c == '\0') || (*c != '*') )
		return false;
	const char *next = c + 1;
	if( *next && *next == '/' )
		return true;
	return false;
}

// consumes tabs, spaces, and inline comments; sets 'c' to first character that is none of these, or the terminus
void consumeWhiteSpace( const char **c )
{
	while( ( ! isTerminated( *c ) ) && isWhiteSpace( *c ) )
		++*c;
	if( isTerminated( *c ) )
		return;
	// did we encounter an inline comment?
	while( isInlineCommentStart( *c ) || isWhiteSpace( *c ) ) {
		while( isWhiteSpace( *c ) )
			++*c;
		const char *startBeforeComment = *c;
		while( ! isTerminated( *c ) && ! isInlineCommentEnd( *c ) )
			++*c;
		if( isTerminated( *c ) ) {
			*c = startBeforeComment;
			break;
		}
		else
			*c += 2;
	}
	return;
}

bool findIncludeStatement( const std::string &line, std::string *out )
{
	const int INCLUDE_KEYWORD_LEN = 7;
	const char *resultStart = nullptr;
	const char *c = line.c_str();
	consumeWhiteSpace( &c );
	// leading '#'
	if( ( isTerminated( c ) ) || ( *c != '#' ) )
		return false;
	++c;
	consumeWhiteSpace( &c );
	if( isTerminated( c ) )
		return false;
	// include keyword
	if( strncmp( c, "include", INCLUDE_KEYWORD_LEN ) )
		return false;
	c += INCLUDE_KEYWORD_LEN;
	consumeWhiteSpace( &c );
	// leading quote or angle bracket
	if( ( isTerminated( c ) ) || ( *c != '\"' && *c != '<' ) )
		return false;
	++c;
	resultStart = c;
	while( ( ! isTerminated( c ) )  && *c != '\"' && *c != '>' )
		++c;
	if( isTerminated( c ) ) // we hit the terminus before the closing symbol
		return false;
	if( out )
		*out = std::string( resultStart, c );
	return true;
}

bool findVersionStatement( const std::string &line, std::string *out )
{
	const int VERSION_KEYWORD_LEN = 7;
	const char *resultStart = nullptr;
	const char *c = line.c_str();
	consumeWhiteSpace( &c );
	// leading '#'
	if( isTerminated( c ) || ( *c != '#' ) )
		return false;
	++c;
	consumeWhiteSpace( &c );
	if( isTerminated( c ) )
		return false;
	// version keyword
	if( strncmp( c, "version", VERSION_KEYWORD_LEN ) ) // 7 == strlen( "version" )
		return false;
	c += VERSION_KEYWORD_LEN;
	consumeWhiteSpace( &c );
	// leading digit
	if( isTerminated( c ) || (! isdigit( *c )) )
		return false;
	resultStart = c;
	while( ( ! isTerminated( c ) ) && isdigit( *c ) )
		++c;
	if( out )
		*out = std::string( resultStart, c );
	return true;
}
} // anonymous namespace

ShaderPreprocessor::ShaderPreprocessor()
{
	mSearchDirectories.push_back( app::Platform::get()->getAssetPath( "" ) );

	// set the default version
#if defined( CINDER_GL_ES_3 )
	mVersion = 300;
#elif defined( CINDER_GL_ES_2 )
	mVersion = 100;
#else // desktop
	mVersion = 150;
#endif
}

string ShaderPreprocessor::parse( const fs::path &sourcePath, std::set<fs::path> *includedFiles )
{
	set<fs::path> localIncludeTree;
	if( ! includedFiles )
		includedFiles = &localIncludeTree;
	else
		includedFiles->clear();

	return parseDirectives( parseRecursive( sourcePath, fs::path(), *includedFiles ) );
}

string ShaderPreprocessor::parse( const std::string &source, const fs::path &sourcePath, set<fs::path> *includedFiles )
{
	CI_ASSERT( ! fs::is_directory( sourcePath ) );

	set<fs::path> localIncludeTree;
	if( ! includedFiles )
		includedFiles = &localIncludeTree;
	else
		includedFiles->clear();


	return parseDirectives( parseTopLevel( source, sourcePath.parent_path(), *includedFiles ) );
}

std::string ShaderPreprocessor::parseDirectives( const std::string &source )
{
	stringstream output;
	istringstream input( source );
	
	string versionLine;
	
	// go through each line and find the #version directive
	string line;
	while( getline( input, line ) ) {
		if( findVersionStatement( line, nullptr ) )
			versionLine = line;
		else
			output << line;
		output << endl;
	}
	
	// if we don't have a version yet, add the default one
	if( versionLine.empty() ) {
#if defined( CINDER_GL_ES_3 )
		versionLine = "#version " + to_string( mVersion ) + " es\n";
#else
		versionLine = "#version " + to_string( mVersion ) + "\n";
#endif
	}
	else if( ! mDefineDirectives.empty() ) {
		versionLine += "\n";
	}

	// copy the preprocessor directives to a string starting with the version
	std::string directivesString = versionLine;
	for( auto define : mDefineDirectives ) {
		directivesString += "#define " + define + "\n";
	}
	
	return directivesString + output.str();
}

string ShaderPreprocessor::parseTopLevel( const string &source, const fs::path &currentDirectory, set<fs::path> &includedFiles )
{
	stringstream output;
	istringstream input( source );

	// go through each line and process includes
	string line;

	size_t lineNumber = 1;

	while( getline( input, line ) ) {
		std::string includeFilePath;
		if( findIncludeStatement( line, &includeFilePath ) ) {
			output << parseRecursive( includeFilePath, currentDirectory, includedFiles );
			output << "#line " << lineNumber << endl;
		}
		else
			output << line;

		output << endl;
		lineNumber++;
	}

	return output.str();
}

string ShaderPreprocessor::parseRecursive( const fs::path &path, const fs::path &currentDirectory, set<fs::path> &includeTree )
{
	const fs::path fullPath = findFullPath( path, currentDirectory );

	if( includeTree.count( fullPath ) ) {
		// circular include, skip it as it has already been appended.
		return "";
	}

	includeTree.insert( fullPath );

	stringstream output;
	ifstream input( fullPath.string().c_str() );
	if( ! input.is_open() )
		throw ShaderPreprocessorExc( "Failed to open file at path: " + fullPath.string() );

	// go through each line and process includes
	string line;

	size_t lineNumber = 1;

	while( getline( input, line ) ) {
		std::string includeFilePath;
		if( findIncludeStatement( line, &includeFilePath ) ) {
			output << parseRecursive( includeFilePath, fullPath.parent_path(), includeTree );
			output << "#line " << lineNumber << endl;
		}
		else
			output << line;

		output << endl;
		lineNumber++;
	}

	input.close();
	return output.str();
}


void ShaderPreprocessor::addSearchDirectory( const fs::path &directory )
{
	if( ! fs::is_directory( directory ) ) {
		CI_LOG_E( "Not a directory: " << directory );
		return;
	}

	fs::path dirCanonical = fs::canonical( directory );
	auto it = find( mSearchDirectories.begin(), mSearchDirectories.end(), dirCanonical );
	if( it == mSearchDirectories.end() )
		mSearchDirectories.push_back( dirCanonical );
}

void ShaderPreprocessor::removeSearchDirectory( const fs::path &directory )
{
	fs::path dirCanonical = fs::canonical( directory );
	mSearchDirectories.erase( remove( mSearchDirectories.begin(), mSearchDirectories.end(), dirCanonical ), mSearchDirectories.end() );
}


void ShaderPreprocessor::addDefine( const std::string &define )
{
	mDefineDirectives.push_back( define );
}

void ShaderPreprocessor::addDefine( const std::string &define, const std::string &value )
{
	mDefineDirectives.push_back( define + " " + value );
}
void ShaderPreprocessor::setDefineDirectives( const std::vector<std::string> &defines )
{
	mDefineDirectives = defines;
}
	
fs::path ShaderPreprocessor::findFullPath( const fs::path &includePath, const fs::path &currentDirectory )
{
	auto fullPath = currentDirectory / includePath;
	if( fs::exists( fullPath ) )
		return fs::canonical( fullPath );

	for( auto dirIt = mSearchDirectories.rbegin(); dirIt != mSearchDirectories.rend(); ++dirIt ) {
		fullPath = *dirIt / includePath;
		if( fs::exists( fullPath ) )
			return fs::canonical( fullPath );
	}

	throw ShaderPreprocessorExc( "could not find shader with include path: " + includePath.string() );
}

} } // namespace cinder::gl
