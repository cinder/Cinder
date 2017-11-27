/*
 Copyright (c) 2015, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#include "cinder/app/Platform.h"
#include "cinder/Display.h"

#include <vector>
#include <string>

namespace cinder { namespace app {

class PlatformWinRt : public Platform {
  public:
	PlatformWinRt();

	DataSourceRef	loadResource( const fs::path &resourcePath ) override;

	fs::path getResourceDirectory() const override									{ return fs::path(); }
	fs::path getResourcePath( const fs::path &rsrcRelativePath ) const override		{ return fs::path(); }

	void getOpenFilePathAsync( const std::function<void(const fs::path&)> &callback, const fs::path &initialPath = fs::path(), const std::vector<std::string> &extensions = {} ) override;
	void getFolderPathAsync( const std::function<void(const fs::path&)> &callback, const fs::path &initialPath = "" ) override;
	void getSaveFilePathAsync( const std::function<void(const fs::path&)> &callback, const fs::path &initialPath, const std::vector<std::string> &extensions = {} ) override;

	// Unimplemented
	fs::path getOpenFilePath( const fs::path &initialPath = fs::path(), const std::vector<std::string> &extensions = std::vector<std::string>() ) override;
	fs::path getFolderPath( const fs::path &initialPath = fs::path() ) override;
	fs::path getSaveFilePath( const fs::path &initialPath = fs::path(), const std::vector<std::string> &extensions = std::vector<std::string>() ) override;

	void prepareAssetLoading() override;

	// Overridden to use OutputDebugString
	std::ostream&	console() override;

	std::map<std::string,std::string>	getEnvironmentVariables() override;

	fs::path	expandPath( const fs::path &path ) override;
	fs::path	getHomeDirectory() const override;
	fs::path	getDocumentsDirectory() const override;
	fs::path	getDefaultExecutablePath() const override;

	void launchWebBrowser( const Url &url ) override;

	void sleep( float milliseconds ) override;
	
	std::vector<std::string> stackTrace() override;

	void setThreadName( const std::string &name ) override;

	const std::vector<DisplayRef>&	getDisplays() override;
  
  private:

	std::unique_ptr<std::ostream>	mOutputStream;
	std::vector<DisplayRef>			mDisplays;
};

//! MSW-specific Exception for failed resource loading, reports windows resource id and type
class ResourceLoadExcMsw : public ResourceLoadExc {
  public:
	ResourceLoadExcMsw( int mswID, const std::string &mswType );
};

} } // namespace cinder::app

namespace cinder {

class DisplayWinRt : public Display {
	friend app::PlatformWinRt;
};

} // namespace cinder
