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

#if defined __OBJC__
	@class NSBundle;
	@class NSAutoreleasePool;
#else
	class NSBundle;
	class NSAutoreleasePool;
#endif

namespace cinder { namespace app {

class PlatformCocoa : public Platform {
  public:
	PlatformCocoa();

	void prepareLaunch() override;
	void cleanupLaunch() override;

	DataSourceRef	loadResource( const fs::path &resourcePath ) override;

	fs::path getResourcePath() const override;
	fs::path getResourcePath( const fs::path &rsrcRelativePath ) const override;

	//! Implemented on desktop, no-op on iOS (returns empty path ).
	fs::path getOpenFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions ) override;
	//! Implemented on desktop, no-op on iOS (returns empty path ).
	fs::path getFolderPath( const fs::path &initialPath ) override;
	//! Implemented on desktop, no-op on iOS (returns empty path ).
	fs::path getSaveFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions ) override;

	//! Sets the NSBundle that should be associated with this global platform object. By default this is `[NSBundle mainBundle]`.
	void			setBundle( NSBundle *bundle );
	//! Returns the NSBundle associated with this global platform object. By default this is `[NSBundle mainBundle]`.
	NSBundle*		getBundle() const;

	void prepareAssetLoading() override;

	void launchWebBrowser( const Url &url ) override;
	
  private:
	NSAutoreleasePool*		mAutoReleasePool;
	mutable NSBundle*		mBundle;
};

} } // namespace cinder::app
