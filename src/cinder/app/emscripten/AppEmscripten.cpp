/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.

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

#include "cinder/app/emscripten/AppEmscripten.h"
#include "cinder/app/emscripten/AppImplEmscripten.h"

namespace cinder { namespace app {

AppEmscripten::AppEmscripten()
	: AppBase()
{
	const Settings *settings = dynamic_cast<Settings *>( sSettingsFromMain );
	CI_ASSERT( settings );

	enablePowerManagement( settings->isPowerManagementEnabled() ); // TODO: consider moving to common method

	mImpl =  new AppImplEmscripten( this, *settings );
}

AppEmscripten::~AppEmscripten()
{
	delete mImpl;
	mImpl = nullptr;
}

void AppEmscripten::launch()
{
	mImpl->run();
}

WindowRef AppEmscripten::createWindow( const Window::Format &format )
{
	return mImpl->createWindow( format );
}

void AppEmscripten::quit()
{
	mImpl->quit();
}

float AppEmscripten::getFrameRate() const
{
	return mImpl->getFrameRate();
}

void AppEmscripten::setFrameRate( float frameRate )
{
	mImpl->setFrameRate( frameRate );
}

void AppEmscripten::disableFrameRate()
{
	mImpl->disableFrameRate();
}

bool AppEmscripten::isFrameRateEnabled() const
{
	return mImpl->isFrameRateEnabled();
}

WindowRef AppEmscripten::getWindow() const
{
	return mImpl->getWindow();
}

WindowRef AppEmscripten::getWindowIndex( size_t index ) const
{
	return mImpl->getWindowIndex( index );
}

size_t AppEmscripten::getNumWindows() const
{
	return mImpl->getNumWindows();
}

WindowRef AppEmscripten::getForegroundWindow() const
{
	return mImpl->getForegroundWindow();
}

void AppEmscripten::hideCursor()
{
	mImpl->hideCursor();
}

void AppEmscripten::showCursor()
{
	mImpl->showCursor();
}

ivec2 AppEmscripten::getMousePos() const
{
	return mImpl->getMousePos();
}

}} // namespace cinder::app
