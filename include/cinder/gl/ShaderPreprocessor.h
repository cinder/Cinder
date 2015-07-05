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

#pragma once

#include "cinder/Exception.h"
#include "cinder/Filesystem.h"

#include <set>
#include <vector>

namespace cinder { namespace gl {

//! \brief Class for parsing and processing GLSL preprocessor directives.
//!
//! Detected files that need to be included, via the `#include` directive, are first searched for relative
//! to the directory of the provided source path (if provided), and then relative to the search directories
//! list. By default, the Platform's default assets path is added to this list, and you can add custom
//! search directories with `addSearchDirectory()`.
//!
//! If a recursive #include is detected, a `ShaderPreprocessorExc` will be thrown.
//!
//! Adding #define statements are also supported, and you can set the #version via `setVersion( int )`. If
//! you are on OpenGL ES, then `" es"` will be appended to the version string.
class ShaderPreprocessor {
  public:
	ShaderPreprocessor();
	//! \brief Parses and processes the shader source at \a sourcePath. If \a includedFiles is provided, this will be filled with paths to any files detected as `#include`ed. \return a preprocessed source string.
	std::string		parse( const fs::path &sourcePath, std::set<fs::path> *includedFiles = nullptr );
	//! Parses and processes the shader source \a source, which can be found at \a sourcePath. If \a includedFiles is provided, this will be filled with paths to any files detected as `#include`ed. \return a preprocessed source string.
	std::string		parse( const std::string &source, const fs::path &sourcePath, std::set<fs::path> *includedFiles = nullptr );

	//! Adds a custom search directory to the search list. The last directory added will be searched first.
	void	addSearchDirectory( const fs::path &directory );
	//! Removes a custom search directory from the search list.
	void	removeSearchDirectory( const fs::path &directory );

	//! Adds a define directive
	void	addDefine( const std::string &define );
	//! Adds a define directive
	void	addDefine( const std::string &define, const std::string &value );
	//! Specifies a series of define directives to add to the shader sources
	void	setDefineDirectives( const std::vector<std::string> &defines );
	//! Specifies the #version directive to add to the shader sources
	void	setVersion( int version )	{ mVersion = version; }

  private:
	std::string		parseTopLevel( const std::string &source, const fs::path &currentDirectory, std::set<fs::path> &includeTree );
	std::string		parseRecursive( const fs::path &path, const fs::path &currentDirectory, std::set<fs::path> &includeTree );
	std::string		parseDirectives( const std::string &source );
	fs::path		findFullPath( const fs::path &includePath, const fs::path &currentPath );

	int								mVersion;
	std::vector<std::string>		mDefineDirectives;
	std::vector<fs::path>			mSearchDirectories;
};

//! Exception thrown when there is an error preprocessing the shader source in `ShaderPreprocessor`.
class ShaderPreprocessorExc : public Exception {
  public:
	ShaderPreprocessorExc( const std::string &description ) : Exception( description )	{}
};

} } // namespace cinder::gl
