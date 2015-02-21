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

#if defined __OBJC__
	@class NSBundle;
	@class NSAutoreleasePool;
	#if defined( CINDER_MAC )
		@class NSScreen;
	#else
		@class UIScreen;
	#endif
#else
	class NSBundle;
	class NSAutoreleasePool;
	#if defined( CINDER_MAC )
		class NSScreen;
	#else
		class UIScreen;
	#endif
#endif
typedef uint32_t CGDirectDisplayID;

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

	const std::vector<DisplayRef>& getDisplays( bool forceRefresh = false ) override;

  private:
	NSAutoreleasePool*		mAutoReleasePool;
	mutable NSBundle*		mBundle;
	
	bool					mDisplaysInitialized;
	std::vector<DisplayRef>	mDisplays;
};

} } // namespace cinder::app

namespace cinder {
#if defined( CINDER_MAC )

//! Represents a monitor/display on OS X
class DisplayMac : public Display {
  public:
	~DisplayMac();

	NSScreen*			getNsScreen() const { return mScreen; }
	CGDirectDisplayID	getCgDirectDisplayId() const { return mDirectDisplayID; }

	static DisplayRef			findFromCgDirectDisplayId( CGDirectDisplayID displayID );
	static DisplayRef			findFromNsScreen( NSScreen *nsScreen );

  protected:	
	NSScreen			*mScreen;
	CGDirectDisplayID	mDirectDisplayID;
	
	friend app::PlatformCocoa;
};

#else

//! Represents a monitor/display on iOS
class DisplayCocoaTouch : public Display {
  public:
	~DisplayCocoaTouch();
	
	UIScreen*	getUiScreen() const { return mUiScreen; }
	//! Returns a vector of resolutions the Display supports
	const std::vector<ivec2>&	getSupportedResolutions() const { return mSupportedResolutions; }
	//! Sets the resolution of the Display. Rounds to the nearest supported resolution.
	void						setResolution( const ivec2 &resolution );

	//! Returns the signal emitted when a display is connected or disconnected
	signals::Signal<void()>&	getSignalDisplaysChanged() { return mSignalDisplaysChanged; }

  protected:
	

	UIScreen				*mUiScreen;
	std::vector<ivec2>		mSupportedResolutions;
	signals::Signal<void()>	mSignalDisplaysChanged;
	
	friend app::PlatformCocoa;	
};
#endif

} // namespace cinder
