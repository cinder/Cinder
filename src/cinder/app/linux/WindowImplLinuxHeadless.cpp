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

#include "cinder/app/RendererGl.h"
#include "cinder/app/linux/WindowImplLinux.h"
#include "cinder/app/linux/AppImplLinux.h"

namespace cinder { namespace app {


WindowImplLinux::WindowImplLinux( const Window::Format &format, WindowImplLinux *sharedRendererWindow, AppImplLinux *appImpl )
	: mAppImpl( appImpl )
{
	mDisplay = format.getDisplay();
	mRenderer = format.getRenderer();

	RendererRef sharedRenderer = sharedRendererWindow ? sharedRendererWindow->getRenderer() : nullptr;
	mWindowedSize = format.getSize();
	mRenderer->setup( mWindowedSize, sharedRenderer );
	RendererGlRef rendererGl = std::dynamic_pointer_cast<RendererGl>( mRenderer );

	// set WindowRef and its impl pointer to this
	mWindowRef = Window::privateCreate__( this, mAppImpl->getApp() );

	mAppImpl->registerWindowEvents( this );
}

WindowImplLinux::~WindowImplLinux()
{
	mAppImpl->unregisterWindowEvents( this );
}

void WindowImplLinux::setFullScreen( bool fullScreen, const app::FullScreenOptions &options )
{
	/*Not implemented*/
}

ivec2 WindowImplLinux::getSize() const
{
	return mWindowedSize;
}

void WindowImplLinux::setSize( const ivec2 &size )
{
	/*Not implemented*/
}

ivec2 WindowImplLinux::getPos() const
{
	return ivec2( 0, 0 );
}

void WindowImplLinux::setPos( const ivec2 &pos )
{
	/*Not implemented*/
}

void WindowImplLinux::close()
{
	/*Not implemented*/
}

void WindowImplLinux::setTitle( const std::string &title )
{
	/*Not implemented*/
}

void WindowImplLinux::hide()
{
	/*Not implemented*/
}

void WindowImplLinux::show()
{
	/*Not implemented*/
}

const std::vector<TouchEvent::Touch>& WindowImplLinux::getActiveTouches() const
{
	return mActiveTouches;
}

void* WindowImplLinux::getNative()
{
	return nullptr;
}

void* WindowImplLinux::getNative() const
{
	return nullptr;
}

void WindowImplLinux::setBorderless( bool borderless )
{
	/*Not implemented*/
}

void WindowImplLinux::setAlwaysOnTop( bool alwaysOnTop )
{
	/*Not implemented*/
}

void WindowImplLinux::keyDown( const KeyEvent &event )
{
	KeyEvent localEvent( event );
	mAppImpl->setWindow( mWindowRef );
	mWindowRef->emitKeyDown( &localEvent );
}

void WindowImplLinux::draw()
{
	mAppImpl->setWindow( mWindowRef );
	mRenderer->startDraw();
	mWindowRef->emitDraw();
	mRenderer->finishDraw();
}

void WindowImplLinux::resize()
{
	mAppImpl->setWindow( mWindowRef );
	mWindowRef->emitResize();
}

void WindowImplLinux::hideCursor()
{
	/*Not implemented*/
}

void WindowImplLinux::showCursor()
{
	/*Not implemented*/
}

ivec2 WindowImplLinux::getMousePos() const
{
	return ivec2( 0, 0 );
}

}} // namespace cinder::app
