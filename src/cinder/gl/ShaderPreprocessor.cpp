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

#include <cstring>
#include <algorithm>

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

bool findVersionStatement( const char *lineStart, int *versionNumberOut )
{
	const int VERSION_KEYWORD_LEN = 7;
	const char *c = lineStart;
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
	if( versionNumberOut )
		*versionNumberOut = (int)std::strtol( c, nullptr, 0 );
	return true;
}
} // anonymous namespace

ShaderPreprocessor::ShaderPreprocessor()
	: mUseFilenameInLineDirective( false )
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
	const fs::path fullPath = findFullPath( sourcePath, "" );
	if( fullPath.empty() ) {
		throw ShaderPreprocessorExc( "could not find shader at source path: " + sourcePath.string() );
	}

	string source = loadString( loadFile( fullPath ) );
	return parse( source, sourcePath, includedFiles );
}

string ShaderPreprocessor::parse( const std::string &source, const fs::path &sourcePath, set<fs::path> *includedFiles )
{
	set<fs::path> localIncludeTree; // even if user didn't ask for includedFiles, keep track of them to detect recursion
	if( ! includedFiles )
		includedFiles = &localIncludeTree;
	else
		includedFiles->clear();

	string directives;
	string sourceBody;
	int versionNumber = -1;
	int lineNumberStart;

	parseDirectives( source, sourcePath, &directives, &sourceBody, &versionNumber, &lineNumberStart );
	if( directives.empty() ) {
		// There were no directives added, parse original source for includes
		return parseTopLevel( source, sourcePath, lineNumberStart, versionNumber, *includedFiles );
	}
	else {
		// Parse the remaining source and then append it to the directives string
		string result = parseTopLevel( sourceBody, sourcePath, lineNumberStart, versionNumber, *includedFiles );
		return directives + result;
	}
}

// - returns directives string and remaining source separately, so that parseTopLevel can start after the directives we've added
// - lineNumberStart indicates what value a #line directive should have after parsing #include statements
void ShaderPreprocessor::parseDirectives( const std::string &source, const fs::path &sourcePath, std::string *directives, std::string *sourceBody, int *versionNumber, int *lineNumberStart )
{		
	// go through each line and find the #version directive
	int lineNumber = 1;
	bool hasVersionLine = false;

	for( size_t lineStartPos = 0; lineStartPos < source.size(); /* */ ) {
		size_t lineEndPos = source.find( '\n', lineStartPos );
		if( lineEndPos == std::string::npos )
			break;

		if( findVersionStatement( source.c_str() + lineStartPos, versionNumber ) ) {
			// if no defines, return leaving the directive and sourceBody strings empty,
			// thereby indicating to use the original source without modification;
			if( mDefineDirectives.empty() ) {
				*lineNumberStart = 1;
				return;
			}

			// Copy #version line and everything before it to the directives string, the rest to sourceBody
			*directives = source.substr( 0, lineEndPos + 1 );
			*sourceBody = source.substr( lineEndPos + 1 );
			*lineNumberStart = lineNumber;
			hasVersionLine = true;

			break;
		}

		lineStartPos = lineEndPos + 1;
		lineNumber++;
	}

	// if we don't have a version yet, add a default one that will be at the top
	if( ! hasVersionLine ) {
#if defined( CINDER_GL_ES_3 )
		*directives += "#version " + to_string( mVersion ) + " es\n";
#else
		*directives += "#version " + to_string( mVersion ) + "\n";
#endif

		*sourceBody = source; // sourceBody is the entire source
		*lineNumberStart = 0;
		*versionNumber = mVersion;
	}

	// append any #defines to the directives string
	for( const auto &define : mDefineDirectives ) {
		*directives += "#define " + define.first;
		if( ! define.second.empty() )
			*directives += " " + define.second;

		*directives += "\n";
	}

	// if we've made any modifications, add a #line directive to ensure debug error statements are correct.
	if( ! mDefineDirectives.empty() || ! hasVersionLine ) {
		*directives += getLineDirective( sourcePath, *lineNumberStart, 0, *versionNumber );
		*lineNumberStart += 1;
	}
}

string ShaderPreprocessor::parseTopLevel( const string &source, const fs::path &sourcePath, int lineNumberStart, int versionNumber, set<fs::path> &includedFiles )
{
	istringstream input( source );
	return readStream( input, sourcePath, lineNumberStart, versionNumber, includedFiles );
}

string ShaderPreprocessor::parseRecursive( const fs::path &includePath, const fs::path &currentDirectory, int versionNumber, set<fs::path> &includeTree )
{	
	string output;
	string signalIncludeResult;
	const int lineNumberStart = 1;

	output = getLineDirective( includePath, 0, (int)includeTree.size() + 1, versionNumber );

	if( mSignalInclude.emit( includePath, &signalIncludeResult ) ) {

		if( includeTree.count( includePath ) ) {
			// circular include, skip it as it has already been appended.
			return "";
		}

		includeTree.insert( includePath );
		istringstream input( signalIncludeResult );
		output += readStream( input, includePath, lineNumberStart, versionNumber, includeTree );
	}
	else {
		const fs::path fullPath = findFullPath( includePath, currentDirectory );
		if( fullPath.empty() ) {
			throw ShaderPreprocessorExc( "could not find shader with include path: " + includePath.string() );
		}

		if( includeTree.count( fullPath ) ) {
			// circular include, skip it as it has already been appended.
			return "";
		}

		includeTree.insert( fullPath );

		ifstream input( fullPath.string().c_str() );
		if( ! input.is_open() )
			throw ShaderPreprocessorExc( "Failed to open file at include path: " + fullPath.string() );

		try {
			output += readStream( input, fullPath, lineNumberStart, versionNumber, includeTree );
		}
		catch( ShaderPreprocessorExc &exc ) {
			// append currently processed glsl file.
			throw ShaderPreprocessorExc( string( exc.what() ) + ", while parsing file: " + fullPath.string() );
		}

		input.close();
	}

	return output;
}

std::string ShaderPreprocessor::readStream( std::istream &input, const fs::path &sourcePath, int lineNumberStart, int versionNumber, set<fs::path> &includeTree )
{
	// go through each line and process includes
	string line;
	int lineNumber = lineNumberStart;
	stringstream output;

	while( getline( input, line ) ) {
		std::string includeFilePath;
		if( findIncludeStatement( line, &includeFilePath ) ) {
			int numIncludesBefore = (int)includeTree.size();
			output << parseRecursive( includeFilePath, sourcePath.parent_path(), versionNumber, includeTree );
			output << getLineDirective( sourcePath, lineNumber, numIncludesBefore, versionNumber );
		}
		else
			output << line << "\n";

		lineNumber++;
	}

	return output.str();
}

std::string ShaderPreprocessor::getLineDirective( const fs::path &sourcePath, int lineNumber, int sourceStringNumber, int versionNumber ) const
{
	// in glsl 330 and up, the #line directive indicates what the next line should be. Before that, it is the current line.
	if( versionNumber >= 330 )
		lineNumber += 1;

	string fileId = mUseFilenameInLineDirective ? "\"" + sourcePath.filename().string() + "\"" : to_string( sourceStringNumber ); 
	return "#line " + to_string( lineNumber ) + " " + fileId + "\n";
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
	// Check if there are any existing defines with this symbol name, and if so replace it.
	string defineUntilSpace = define.substr( 0, define.find( ' ' ) );
	for( auto &dd : mDefineDirectives ) {

		if( dd.first == define ) {
			dd.first = define;
			dd.second = {};
			return;
		}
	}

	// define is unique, add it to list
	mDefineDirectives.push_back( { define, {} } );
}

void ShaderPreprocessor::addDefine( const std::string &define, const std::string &value )
{
//	string defineLine = define + " " + value;

	// Check if there are any existing defines with this symbol name, and if so replace it.
	for( auto &dd : mDefineDirectives ) {
		if( dd.first == define ) {
			dd.first = define;
			dd.second = value;
			return;
		}
	}

	// define is unique, add it to list
	mDefineDirectives.push_back( { define, value } );
}

void ShaderPreprocessor::setDefines( const std::vector<std::pair<std::string, std::string>> &defines )
{
	mDefineDirectives = defines;
}

void ShaderPreprocessor::removeDefine( const std::string &define )
{
	mDefineDirectives.erase( remove_if( mDefineDirectives.begin(), mDefineDirectives.end(),
		[&define]( const pair<string,string> &o ) {
			return o.first == define;
		} ), mDefineDirectives.end() );
}

void ShaderPreprocessor::clearDefines()
{
	mDefineDirectives.clear();
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

	return {};
}

} } // namespace cinder::gl
