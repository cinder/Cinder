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

#include "cinder/app/android/WindowImplAndroid.h"
#include "cinder/app/android/AppImplAndroid.h"
#include "cinder/app/android/RendererGlAndroid.h"
#include "cinder/app/RendererGl.h"

namespace cinder { namespace app { 

WindowImplAndroid::WindowImplAndroid( ANativeWindow *nativeWindow, const Window::Format &format, RendererRef sharedRenderer, AppImplAndroid *appImpl )
	: mAppImpl( appImpl ), mNativeWindow( nativeWindow )
{
	mDisplay = format.getDisplay();
	if( ! mDisplay ) {
		mDisplay = Display::getMainDisplay();
	}

	mRenderer = format.getRenderer();

	mRenderer->setup( mNativeWindow, sharedRenderer );

	// set WindowRef and its impl pointer to this
	mWindowRef = Window::privateCreate__( this, mAppImpl->getApp() );	
}

WindowImplAndroid::~WindowImplAndroid()
{
}

void WindowImplAndroid::setFullScreen( bool fullScreen, const app::FullScreenOptions &options )
{
	// Do nothing
}

ivec2 WindowImplAndroid::getSize() const
{
	// Returns the device's screen size
	return RendererGlAndroid::sSurfaceSize;
}

void WindowImplAndroid::setSize( const ivec2 &size )
{
	// Do nothing
}

ivec2 WindowImplAndroid::getPos() const 
{
	return ivec2( 0, 0 );
}

void WindowImplAndroid::setPos( const ivec2 &pos )
{
	// Do nothing
}

void WindowImplAndroid::close()
{
	getAppImpl()->closeWindow( this );
	// at this point 'this' is invalid so do nothing after this line	
}

void WindowImplAndroid::setTitle( const std::string &title )
{
}

void WindowImplAndroid::hide()
{
}

void WindowImplAndroid::show()
{
}

const std::vector<TouchEvent::Touch>& WindowImplAndroid::getActiveTouches() const
{
	return getAppImpl()->getActiveTouches();	
}

void WindowImplAndroid::setBorderless( bool borderless )
{
}

void WindowImplAndroid::setAlwaysOnTop( bool alwaysOnTop )
{
}

void WindowImplAndroid::keyDown( const KeyEvent &event )
{
	KeyEvent localEvent( event );
	mAppImpl->setWindow( mWindowRef );
	mWindowRef->emitKeyDown( &localEvent );
}

void WindowImplAndroid::draw()
{
	mAppImpl->setWindow( mWindowRef );
	mRenderer->startDraw();
	mWindowRef->emitDraw();
	mRenderer->finishDraw();
}

void WindowImplAndroid::resize()
{
	mAppImpl->setWindow( mWindowRef );
	mWindowRef->emitResize();
}

void WindowImplAndroid::privateClose()
{
}

void WindowImplAndroid::reinitializeWindowSurface( ANativeWindow *nativeWindow )
{
	mNativeWindow = nativeWindow;
	
	RendererGlRef rendererGl = std::dynamic_pointer_cast<RendererGl>( mRenderer );
	if( rendererGl ) {
		rendererGl->getImpl()->reinitializeWindowSurface( mNativeWindow );
	}
}

} } // namespace cinder::app