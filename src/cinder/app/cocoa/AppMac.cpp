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

	Platform::get()->setExecutablePath( getAppPath() );
	mImpl = [[AppImplMac alloc] init:this settings:*settings];

	enablePowerManagement( settings->isPowerManagementEnabled() ); // TODO: consider moving to common method
}

AppMac::~AppMac()
{
	[mImpl release];
}

void AppMac::launch()
{
	[[NSApplication sharedApplication] run];
}

WindowRef AppMac::createWindow( const Window::Format &format )
{
	return [mImpl createWindow:format];
}

void AppMac::quit()
{
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

fs::path AppMac::getAppPath() const
{
	return fs::path( [[[NSBundle mainBundle] bundlePath] UTF8String] ).parent_path();
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
