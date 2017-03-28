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
#include "cinder/Signals.h"

#include <set>
#include <vector>

namespace cinder { namespace gl {

typedef std::shared_ptr<class ShaderPreprocessor>	ShaderPreprocessorRef;

//! The Signal type used for for ShaderPreprocessor::getSignalInclude().
//! The Connection interprets a path and, if it can handle the file then sets the contents of the string and returns true. Returns false if it cannot handle the specified path.
typedef signals::Signal<bool ( const fs::path &,std::string * )>	SignalIncludeHandler;

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
class CI_API ShaderPreprocessor {
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
	//! Adds a define directive in the form of `define=value`
	void	addDefine( const std::string &define, const std::string &value );
	//! Specifies all define directives to add to the shader sources, overwriting any existing defines.
	void	setDefines( const std::vector<std::pair<std::string, std::string>> &defines );
	//! Returns all of the define directives to add to the shader sources.
	const std::vector<std::pair<std::string,std::string>>&	getDefines() const	{ return mDefineDirectives; }
	//! Returns all of the define directives to add to the shader sources, modifiable version.
	std::vector<std::pair<std::string, std::string>>&		getDefines()		{ return mDefineDirectives; }
	//! Removes a define directive
	void	removeDefine( const std::string &define );
	//! Clears all define directives
	void	clearDefines();

	//! Specifies the #version directive to add to the shader sources (if it doesn't explicitly contain a `#version` directive).
	void	setVersion( int version )	{ mVersion = version; }
	//! Returns the version used for #version directives that was added with setVersion().
	int		getVersion() const			{ return mVersion; }

	//! If set to true, the name of the currently processed file will be used in `#line` directives. Not to glsl spec, but works on some graphics cards like NVidia.
	void	setUseFilenameInLineDirectiveEnabled( bool enable )	{ mUseFilenameInLineDirective = enable; }
	//! If true, the name of the currently processed file will be used in `#line` directives. Not to glsl spec, but works on some graphics cards like NVidia.
	bool	isUseFilenameInLineDirectiveEnabled() const		{ return mUseFilenameInLineDirective; }

	//! Returns a Signal that the user can connect to in order to handle custom includes.
	SignalIncludeHandler& getSignalInclude()	{ return mSignalInclude; }
	
  private:
	void			parseDirectives( const std::string &source, const fs::path &sourcePath, std::string *directives, std::string *sourceBody, int *versionNumber, int *lineNumberStart );
	std::string		parseTopLevel( const std::string &source, const fs::path &currentDirectory, int lineNumberStart, int versionNumber, std::set<fs::path> &includeTree );
	std::string		parseRecursive( const fs::path &path, const fs::path &currentDirectory, int versionNumber, std::set<fs::path> &includeTree );
	std::string		readStream( std::istream &stream, const fs::path &sourcePath, int lineNumberStart, int versionNumber, std::set<fs::path> &includeTree );
	std::string		getLineDirective( const fs::path &sourcePath, int lineNumber, int sourceStringNumber, int versionNumber ) const;
	fs::path		findFullPath( const fs::path &includePath, const fs::path &currentPath );
	
	int								mVersion;
	std::vector<std::pair<std::string,std::string>>		mDefineDirectives; // [macro, value]
	std::vector<fs::path>			mSearchDirectories;
	SignalIncludeHandler			mSignalInclude;

	bool mUseFilenameInLineDirective;
};

//! Exception thrown when there is an error preprocessing the shader source in `ShaderPreprocessor`.
class CI_API ShaderPreprocessorExc : public Exception {
  public:
	ShaderPreprocessorExc( const std::string &description ) : Exception( description )	{}
};

} } // namespace cinder::gl
