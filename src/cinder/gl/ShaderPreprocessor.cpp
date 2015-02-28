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
#include "cinder/app/App.h"
#include "cinder/Utilities.h"
#include "cinder/Log.h"

#include <regex>

using namespace std;

namespace cinder { namespace gl {

namespace {
	const regex sIncludeRegex = regex( "^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*" );
} // anonymous namespace

ShaderPreprocessor::ShaderPreprocessor()
	: mCachingEnabled( true )
{
	// TODO: we discussed keeping this ci assets agnostic, instead adding getAssetPath() from the outside
	// - also, we don't want GlslProg to rely on App at all.
	//    - can it do what it needs to with Platform?
	mSearchDirectories.push_back( app::getAssetPath( "" ) );
}

string ShaderPreprocessor::parse( const fs::path &path )
{
	set<fs::path> includeTree;

	return parseRecursive( path, fs::path(), includeTree );
}

string ShaderPreprocessor::parse( const std::string &source, const fs::path &sourceDirectory )
{
	return parseTopLevel( source, sourceDirectory );
}

string ShaderPreprocessor::parseTopLevel( const string &source, const fs::path &currentDirectory )
{
	set<fs::path> includeTree;

	stringstream output;
	istringstream input( source );

	// go through each line and process includes
	string line;
	smatch matches;

	size_t lineNumber = 1;

	while( getline( input, line ) ) {
		if( regex_search( line, matches, sIncludeRegex ) ) {
			output << parseRecursive( matches[1].str(), currentDirectory, includeTree );
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

	if( includeTree.count( fullPath ) )
		throw ShaderPreprocessorExc( "circular include found, path: " + fullPath.string() );

	includeTree.insert( fullPath );

	time_t timeLastWrite = 0;
	if( mCachingEnabled ) {
		timeLastWrite = fs::last_write_time( fullPath );

		auto cachedIt = mCachedSources.find( fullPath );
		if( cachedIt != mCachedSources.end() ) {
			if( cachedIt->second.mTimeLastWrite >= timeLastWrite ) {
				return cachedIt->second.mString;
			}
		}
	}

	stringstream output;

	ifstream input( fullPath.c_str() );
	if( ! input.is_open() )
		throw ShaderPreprocessorExc( "Failed to open file at path: " + fullPath.string() );

	// go through each line and process includes
	string line;
	smatch matches;

	size_t lineNumber = 1;

	while( getline( input, line ) ) {
		if( regex_search( line, matches, sIncludeRegex ) ) {
			output << parseRecursive( matches[1].str(), fullPath.parent_path(), includeTree );
			output << "#line " << lineNumber << endl;
		}
		else
			output << line;

		output << endl;
		lineNumber++;
	}

	input.close();

	if( mCachingEnabled ) {
		Source &source = mCachedSources[fullPath];
		source.mTimeLastWrite = timeLastWrite;
		source.mString = output.str();

		return source.mString;
	}
	else
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

void ShaderPreprocessor::clearCache()
{
	mCachedSources.clear();
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
