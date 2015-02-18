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

#include "cinder/app/cocoa/AppBasicMac.h"
#include "cinder/app/cocoa/AppImplMac.h"
#include "cinder/Log.h"

using namespace std;

namespace cinder { namespace app {

AppBasicMac::AppBasicMac()
	: AppBase()
{
	const Settings *settings = dynamic_cast<Settings *>( sSettingsFromMain );
	CI_ASSERT( settings );

	Platform::get()->setExecutablePath( getAppPath() );
	mImpl = [[AppImplMac alloc] init:this settings:*settings];

	enablePowerManagement( settings->isPowerManagementEnabled() ); // TODO: consider moving to common method
}

AppBasicMac::~AppBasicMac()
{
	[mImpl release];
}

void AppBasicMac::launch( const char *title, int argc, char * const argv[] )
{
	[[NSApplication sharedApplication] run];
}

WindowRef AppBasicMac::createWindow( const Window::Format &format )
{
	return [mImpl createWindow:format];
}

void AppBasicMac::quit()
{
	[mImpl quit];
}

float AppBasicMac::getFrameRate() const
{
	return [mImpl getFrameRate];
}

void AppBasicMac::setFrameRate( float frameRate )
{
	[mImpl setFrameRate:frameRate];
}

void AppBasicMac::disableFrameRate()
{
	[mImpl disableFrameRate];
}

bool AppBasicMac::isFrameRateEnabled() const
{
	return [mImpl isFrameRateEnabled];
}

fs::path AppBasicMac::getAppPath() const
{
	return fs::path( [[[NSBundle mainBundle] bundlePath] UTF8String] );
}

WindowRef AppBasicMac::getWindow() const
{
	return [mImpl getWindow];
}

WindowRef AppBasicMac::getWindowIndex( size_t index ) const
{
	return [mImpl getWindowIndex:index];
}

size_t AppBasicMac::getNumWindows() const
{
	return [mImpl getNumWindows];
}

WindowRef AppBasicMac::getForegroundWindow() const
{
	return [mImpl getForegroundWindow];
}

void AppBasicMac::hideCursor()
{
	[NSCursor hide];
}

void AppBasicMac::showCursor()
{
	[NSCursor unhide];
}

ivec2 AppBasicMac::getMousePos() const
{
	NSPoint loc = [NSEvent mouseLocation];
	return ivec2( loc.x, cinder::Display::getMainDisplay()->getHeight() - loc.y );
}

} } // namespace cinder::app
