/*
 Copyright (c) 2014, The Cinder Project, All rights reserved.

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

#include "cinder/app/cocoa/AppMac.h"
#include "cinder/app/cocoa/AppImplMac.h"
#include "cinder/Log.h"

using namespace std;

namespace cinder { namespace app {

AppMac::AppMac()
	: AppBase()
{
	const Settings *settings = dynamic_cast<Settings *>( sSettingsFromMain );
	CI_ASSERT( settings );

	mImpl = [[AppImplMac alloc] init:this settings:*settings];

	enablePowerManagement( settings->isPowerManagementEnabled() ); // TODO: consider moving to common method
}

AppMac::~AppMac()
{
	[mImpl release];
}

void AppMac::initialize( Settings *settings, const RendererRef &defaultRenderer, const char *title, int argc, char * const argv[] )
{
	AppBase::initialize( settings, defaultRenderer, title, argc, argv );

	// Xcode automatically adds a pair of command line args, `NSDocumentRevisionsDebugMode YES` if "Allow debugging when using document Versions Browser" is not explicitly unchecked.
	// We strip this known flag here
	vector<string> replacedArgs;
	const vector<string>& currentArgs = settings->getCommandLineArgs();
	for( size_t i = 0; i < currentArgs.size(); ++i ) {
		if( ( currentArgs[i] == "-NSDocumentRevisionsDebugMode" ) && ( i + 1 < currentArgs.size() ) )
			i += 2; // skip this arg and the successor, which will be 'YES' or 'NO'
		else
			replacedArgs.push_back( currentArgs[i] );
	}
	settings->setCommandLineArgs( replacedArgs );
}

void AppMac::launch()
{
	[[NSApplication sharedApplication] run];
	emitCleanup();
}

void AppMac::enablePowerManagement( bool powerManagement )
{
	[mImpl setPowerManagementEnabled:powerManagement ? YES : NO];
}

bool AppMac::isPowerManagementEnabled() const
{
	return 	[mImpl isPowerManagementEnabled];
}

WindowRef AppMac::createWindow( const Window::Format &format )
{
	return [mImpl createWindow:format];
}

void AppMac::quit()
{
	// if launch() has not yet been called, we note the request rather than issue the call to AppImpl's quit
	if( ! getLaunchCalled() )
		setQuitRequested();
	else
		[mImpl quit];
}

float AppMac::getFrameRate() const
{
	return [mImpl getFrameRate];
}

void AppMac::setFrameRate( float frameRate )
{
	[mImpl setFrameRate:frameRate];
}

void AppMac::disableFrameRate()
{
	[mImpl disableFrameRate];
}

bool AppMac::isFrameRateEnabled() const
{
	return [mImpl isFrameRateEnabled];
}

WindowRef AppMac::getWindow() const
{
	return [mImpl getWindow];
}

WindowRef AppMac::getWindowIndex( size_t index ) const
{
	return [mImpl getWindowIndex:index];
}

size_t AppMac::getNumWindows() const
{
	return [mImpl getNumWindows];
}

WindowRef AppMac::getForegroundWindow() const
{
	return [mImpl getForegroundWindow];
}

void AppMac::hideCursor()
{
	[NSCursor hide];
}

void AppMac::showCursor()
{
	[NSCursor unhide];
}

ivec2 AppMac::getMousePos() const
{
	NSPoint loc = [NSEvent mouseLocation];
	return ivec2( loc.x, cinder::Display::getMainDisplay()->getHeight() - loc.y );
}

} } // namespace cinder::app
