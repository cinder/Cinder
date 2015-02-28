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

#include <map>
#include <set>
#include <vector>

namespace cinder { namespace gl {

class ShaderPreprocessor {
  public:
	ShaderPreprocessor();

	std::string		parse( const fs::path &path );
	std::string		parse( const std::string &source, const fs::path &currentDirectory );

	//!
	void	addSearchDirectory( const fs::path &directory );
	//!
	void	removeSearchDirectory( const fs::path &directory );
	//! Sets whether or not parsed sources are cached and reused in future shader preprocessing. \default is true.
	void	setCachingEnabled( bool enable = true )		{ mCachingEnabled = enable; }
	//! Returns whether or not parsed sources are cached and reused in future shader preprocessing. \default is true.
	bool	isCachingEnabled() const					{ return mCachingEnabled; }
	//! Clears any cache sources.
	void	clearCache();

  private:
	std::string		parseTopLevel( const std::string &source, const fs::path &currentDirectory );
	std::string		parseRecursive( const fs::path &path, const fs::path &currentDirectory, std::set<fs::path> &includeTree );
	fs::path		findFullPath( const fs::path &includePath, const fs::path &currentPath );

	struct Source {
		std::string		mString;
		std::time_t		mTimeLastWrite;
	};

	std::vector<fs::path>			mSearchDirectories;
	std::map<fs::path, Source>		mCachedSources;
	bool							mCachingEnabled;
};

class ShaderPreprocessorExc : public Exception {
  public:
	ShaderPreprocessorExc( const std::string &description ) : Exception( description )	{}
};

} } // namespace cinder::gl
