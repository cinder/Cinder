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
	string source = loadString( loadFile( sourcePath ) );
	return parse( source, fs::path(), includedFiles );
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
	size_t lineNumberStart;

	parseDirectives( source, sourcePath, &directives, &sourceBody, &lineNumberStart );
	if( directives.empty() ) {
		// There were no directives added, parse original source for includes
		return parseTopLevel( source, sourcePath, lineNumberStart, *includedFiles );
	}
	else {
		// Parse the remaining source and then append it to the directives string
		string result = parseTopLevel( sourceBody, sourcePath, lineNumberStart, *includedFiles );
		return directives + result;
	}
}

void printString( const std::string &str )
{
	ci::app::Platform::get()->console() << str << endl;
}

// - returns directives string and remaining source separately, so that parseTopLevel can start after the directives we've added
// - lineNumberStart indicates what value a #line directive should have after parsing #include statements
void ShaderPreprocessor::parseDirectives( const std::string &source, const fs::path &sourcePath, std::string *directives, std::string *sourceBody, size_t *lineNumberStart )
{		
	// go through each line and find the #version directive
	string line;
	size_t lineNumber = 0;
	bool hasVersionLine = false;

	size_t lineStartPos = 0;
	while( lineStartPos < source.size() ) {
		size_t lineEndPos = source.find( '\n', lineStartPos );
		if( lineEndPos == std::string::npos )
			break;


		// TODO: we don't need to copy out each line, just pass in a const char* to the correct starting position in the line to findVersionStatement
		// - also remove extra argument that isn't used for anything
		string line = source.substr( lineStartPos, lineEndPos - lineStartPos );

		if( findVersionStatement( line, nullptr ) ) {
			// if no defines, return leaving the directive and sourceBody strings empty,
			// thereby indicating to use the original source without modification;
			if( mDefineDirectives.empty() ) {
				*lineNumberStart = 0;
				return;
			}

			// Copy #version line and everything before it to the directives string, the rest to sourceBody
			*directives = source.substr( 0, lineEndPos + 1 );
			*sourceBody = source.substr( lineEndPos + 1, source.size() - lineEndPos );
			*lineNumberStart = lineNumber + 1;
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
		*lineNumberStart = lineNumber;

	}

	// append any #defines to the directives string
	for( const auto &define : mDefineDirectives ) {
		*directives += "#define " + define + "\n";
		*lineNumberStart += 1;
	}

	// TODO: update this comment
	// if we've made any modifications, add a #line directive to ensure debug error statements are correct.
	// - if no version line but we have directives, lineStatementNumber should be 0
	// - if no version line and no directives, line number should be the one parsed
	// - if both, line statement number should be the original version line + 1
	if( ! mDefineDirectives.empty() || ! hasVersionLine ) {
		*directives += getLineDirective( sourcePath, *lineNumberStart );
		*lineNumberStart += 1;
	}
}

string ShaderPreprocessor::parseTopLevel( const string &source, const fs::path &sourcePath, size_t lineNumberStart, set<fs::path> &includedFiles )
{
	stringstream output;
	istringstream input( source );

	// go through each line and process includes
	string line;
	fs::path currentDirectory = sourcePath.parent_path();
	size_t lineNumber = lineNumberStart;

	while( getline( input, line ) ) {
		std::string includeFilePath;
		if( findIncludeStatement( line, &includeFilePath ) ) {
			output << parseRecursive( includeFilePath, currentDirectory, includedFiles );
			output << getLineDirective( sourcePath, lineNumber );
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
	string output;
	string signalIncludeResult;

	output = getLineDirective( path, 0 );

	if( mSignalInclude.emit( path, &signalIncludeResult ) ) {

		if( includeTree.count( path ) ) {
			// circular include, skip it as it has already been appended.
			return "";
		}

		includeTree.insert( path );
		istringstream input( signalIncludeResult );
		output += readStream( input, path, includeTree );
	}
	else {
		const fs::path fullPath = findFullPath( path, currentDirectory );

		if( includeTree.count( fullPath ) ) {
			// circular include, skip it as it has already been appended.
			return "";
		}

		includeTree.insert( fullPath );

		ifstream input( fullPath.string().c_str() );
		if( ! input.is_open() )
			throw ShaderPreprocessorExc( "Failed to open file at path: " + fullPath.string() );
		output += readStream( input, fullPath, includeTree );
		input.close();
	}

	return output;
}

std::string ShaderPreprocessor::readStream( std::istream &input, const fs::path &path, set<fs::path> &includeTree )
{
	// go through each line and process includes
	string line;
	size_t lineNumber = 1;
	stringstream output;

	while( getline( input, line ) ) {
		std::string includeFilePath;
		if( findIncludeStatement( line, &includeFilePath ) ) {
			output << parseRecursive( includeFilePath, path.parent_path(), includeTree );
			//output << "#line " << lineNumber << endl;
			output << getLineDirective( path, lineNumber );
		}
		else
			output << line;

		output << endl;
		lineNumber++;
	}

	return output.str();
}

std::string ShaderPreprocessor::getLineDirective( const fs::path &path, size_t lineNumber ) const
{
	return "#line " + to_string( lineNumber ) + " \"" + path.string() + "\"\n";
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

void ShaderPreprocessor::clearDefineDirectives()
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

	throw ShaderPreprocessorExc( "could not find shader with include path: " + includePath.string() );
}

} } // namespace cinder::gl
