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

#include <bcm_host.h>

namespace cinder { namespace app {

struct WindowImplLinux::NativeWindow {
	EGL_DISPMANX_WINDOW_T window;
	NativeWindow( const ivec2& size, uint32_t element ) {
		window.width   = size.x;
		window.height  = size.y;
		window.element = element;
	}
};

WindowImplLinux::WindowImplLinux( const Window::Format &format, RendererRef sharedRenderer, AppImplLinux *appImpl )
	: mAppImpl( appImpl )
{
	mDisplay = format.getDisplay();
	mRenderer = format.getRenderer();

	// NativeWindow->window will get updated by the mRenderer
	auto windowSize = format.getSize();
	mNativeWindow = std::unique_ptr<NativeWindow>( new NativeWindow( windowSize, 0 ) );
	mRenderer->setup( reinterpret_cast<void*>( &(mNativeWindow->window) ), sharedRenderer );

	RendererGlRef rendererGl = std::dynamic_pointer_cast<RendererGl>( mRenderer );

	// set WindowRef and its impl pointer to this
	mWindowRef = Window::privateCreate__( this, mAppImpl->getApp() );

	mAppImpl->registerInput( this );
}

WindowImplLinux::~WindowImplLinux()
{
	mAppImpl->unregisterInput( this );
}

void WindowImplLinux::setFullScreen( bool fullScreen, const app::FullScreenOptions &options )
{
	// TODO: Find a way to do this w/o recreating 
}

ivec2 WindowImplLinux::getSize() const
{
	return mAppImpl->getDefaultDisplaySize();
}

void WindowImplLinux::setSize( const ivec2 &size )
{
}

ivec2 WindowImplLinux::getPos() const
{
	return ivec2( 0, 0 );
}

void WindowImplLinux::setPos( const ivec2 &pos )
{	
}

void WindowImplLinux::close()
{
}

void WindowImplLinux::setTitle( const std::string &title )
{
}

void WindowImplLinux::hide()
{
}

void WindowImplLinux::show()
{
}

const std::vector<TouchEvent::Touch>& WindowImplLinux::getActiveTouches() const
{
	return mActiveTouches;
}

EGLNativeWindowType	WindowImplLinux::getNative() 
{ 
	return reinterpret_cast<EGLNativeWindowType>( mNativeWindow.get() );
}

EGLNativeWindowType WindowImplLinux::getNative() const 
{ 
	return reinterpret_cast<EGLNativeWindowType>( mNativeWindow.get() );
}

void WindowImplLinux::setBorderless( bool borderless )
{
	// TODO: Find a way to do this w/o recreating 
}

void WindowImplLinux::setAlwaysOnTop( bool alwaysOnTop )
{
	// TODO: Find a way to do this w/o recreating 
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
}

void WindowImplLinux::showCursor()
{
}

ivec2 WindowImplLinux::getMousePos() const
{
	return ivec2( 0, 0 );
}

}} // namespace cinder::app
