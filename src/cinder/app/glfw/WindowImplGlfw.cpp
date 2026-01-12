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

#include "cinder/app/glfw/RendererImplGlfw.h"
#include "cinder/app/RendererGl.h"
#include "cinder/app/glfw/WindowImplGlfw.h"
#include "cinder/app/glfw/AppImplGlfw.h"

#if defined( CINDER_LINUX )
	#include "cinder/app/linux/PlatformLinux.h"
#endif

#if defined( CINDER_MAC )
	#include "cinder/app/glfw/AppImplGlfwMac.h"
#endif

namespace cinder { namespace app {

WindowImplGlfw::WindowImplGlfw( const Window::Format &format, WindowImplGlfw *sharedRendererWindow, AppImplGlfw *appImpl )
	: mAppImpl( appImpl )
{
	mFullScreen = format.isFullScreen();
	mDisplay = format.getDisplay();

	if( ! mDisplay )
		mDisplay = Display::getMainDisplay();

	mRenderer = format.getRenderer();

	GLFWwindow *sharedGlfwWindow = nullptr;
	if( sharedRendererWindow )
		sharedGlfwWindow = reinterpret_cast<GLFWwindow*>( sharedRendererWindow->getNative() );

	// Get the renderer impl and have it prepare GLFW window hints
	auto rendererGl = std::dynamic_pointer_cast<RendererGl>( mRenderer );
	if( rendererGl ) {
		auto rendererImpl = rendererGl->getGlfwRendererImpl();
		if( rendererImpl ) {
			rendererImpl->prepareWindowHints();
		}
	}

	// Set window-specific hints (not renderer-specific)
	::glfwWindowHint( GLFW_DECORATED, format.isBorderless() ? GL_FALSE : GL_TRUE );
	::glfwWindowHint( GLFW_RESIZABLE, format.isResizable() ? GL_TRUE : GL_FALSE );
	::glfwWindowHint( GLFW_FLOATING, format.isAlwaysOnTop() ? GL_TRUE : GL_FALSE );
#if defined( CINDER_MAC )
	// Set retina framebuffer hint based on app's high density display setting
	::glfwWindowHint( GLFW_COCOA_RETINA_FRAMEBUFFER, mAppImpl->getApp()->isHighDensityDisplayEnabled() ? GL_TRUE : GL_FALSE );
#endif

	if( mFullScreen ) {
#if defined( CINDER_LINUX )
		cinder::app::DisplayLinux* displayLinux = dynamic_cast<cinder::app::DisplayLinux*>( mDisplay.get() );
		auto windowSize = displayLinux->getSize();
		mGlfwWindow = ::glfwCreateWindow( windowSize.x, windowSize.y, format.getTitle().c_str(), displayLinux->getGlfwMonitor(), sharedGlfwWindow );
#elif defined( CINDER_MAC )
		// On macOS, get the primary monitor for fullscreen
		GLFWmonitor* primaryMonitor = ::glfwGetPrimaryMonitor();
		auto windowSize = mDisplay->getSize();
		mGlfwWindow = ::glfwCreateWindow( windowSize.x, windowSize.y, format.getTitle().c_str(), primaryMonitor, sharedGlfwWindow );
#endif
		mWindowedSize = format.getSize();
		mWindowedPos = format.getPos();
	}
	else {
		auto windowSize = format.getSize();
		mGlfwWindow = ::glfwCreateWindow( windowSize.x, windowSize.y, format.getTitle().c_str(), nullptr, sharedGlfwWindow );
	}

	mRenderer->setup( mGlfwWindow, sharedRendererWindow ? sharedRendererWindow->getRenderer() : nullptr );

	// set WindowRef and its impl pointer to this
	mWindowRef = Window::privateCreate__( this, mAppImpl->getApp() );

	mAppImpl->registerWindowEvents( this );

#if defined( CINDER_MAC )
	// Enable resize/move tracking on macOS
	enableResizeTrackingForWindow( mGlfwWindow, this );
#endif

	setBorderless( format.isBorderless() );
	setAlwaysOnTop( format.isAlwaysOnTop() );
}

WindowImplGlfw::~WindowImplGlfw()
{
	if( mAppImpl && mGlfwWindow ) {
		mAppImpl->unregisterWindowEvents( this );
	}

	if( mGlfwWindow ) {
#if defined( CINDER_MAC )
		// Clean up resize tracking before destroying window
		disableResizeTrackingForWindow( mGlfwWindow );
#endif
		::glfwDestroyWindow( mGlfwWindow );
		mGlfwWindow = nullptr;
	}
}

void WindowImplGlfw::setFullScreen( bool fullScreen, const app::FullScreenOptions &options )
{
	if( fullScreen == mFullScreen )
		return;

	if( fullScreen ) {
		// preserve these for potential return to windowed later
		int x, y;
		::glfwGetWindowPos( mGlfwWindow, &x, &y );
		mWindowedPos = ivec2( x, y );
		::glfwGetWindowSize( mGlfwWindow, &x, &y );
		mWindowedSize = ivec2( x, y );

		if( options.getDisplay() )
			mDisplay = options.getDisplay();

#if defined( CINDER_LINUX )
		cinder::app::DisplayLinux* displayLinux = dynamic_cast<cinder::app::DisplayLinux*>( mDisplay.get() );
		::glfwSetWindowMonitor( mGlfwWindow, displayLinux->getGlfwMonitor(), 0, 0, mDisplay->getWidth(), mDisplay->getHeight(), GLFW_DONT_CARE );
#elif defined( CINDER_MAC )
		GLFWmonitor* primaryMonitor = ::glfwGetPrimaryMonitor();
		::glfwSetWindowMonitor( mGlfwWindow, primaryMonitor, 0, 0, mDisplay->getWidth(), mDisplay->getHeight(), GLFW_DONT_CARE );
#endif
	}
	else {
		::glfwSetWindowMonitor( mGlfwWindow, NULL, mWindowedPos.x, mWindowedPos.y, mWindowedSize.x, mWindowedSize.y, GLFW_DONT_CARE );
	}

	mFullScreen = fullScreen;
}

ivec2 WindowImplGlfw::getSize() const
{
	int xsize, ysize;
	::glfwGetWindowSize( mGlfwWindow, &xsize, &ysize );
	return ivec2( xsize, ysize );
}

void WindowImplGlfw::setSize( const ivec2 &size )
{
	::glfwSetWindowSize( mGlfwWindow, size.x, size.y );
}

ivec2 WindowImplGlfw::getPos() const
{
	int xpos, ypos;
	::glfwGetWindowPos( mGlfwWindow, &xpos, &ypos );
	return ivec2( xpos, ypos );
}

void WindowImplGlfw::setPos( const ivec2 &pos )
{
	::glfwSetWindowPos( mGlfwWindow, pos.x, pos.y );
}

void WindowImplGlfw::close()
{
	::glfwSetWindowShouldClose( mGlfwWindow, 1 );
}

void WindowImplGlfw::setTitle( const std::string &title )
{
    ::glfwSetWindowTitle( mGlfwWindow, title.c_str() );
}

void WindowImplGlfw::hide()
{
	::glfwHideWindow( mGlfwWindow );
}

void WindowImplGlfw::show()
{
	::glfwShowWindow( mGlfwWindow );
}

float WindowImplGlfw::getContentScale() const
{
	float xscale = 1.0f, yscale = 1.0f;
	::glfwGetWindowContentScale( mGlfwWindow, &xscale, &yscale );
	// Return the larger of the two scales (they should typically be the same)
	return std::max( xscale, yscale );
}

const std::vector<TouchEvent::Touch>& WindowImplGlfw::getActiveTouches() const
{
	return mActiveTouches;
}

void WindowImplGlfw::setBorderless( bool borderless )
{
	if( mBorderless == borderless )
		return;

	if( mGlfwWindow ) {
		::glfwSetWindowAttrib( mGlfwWindow, GLFW_DECORATED, borderless ? GLFW_FALSE : GLFW_TRUE );
	}

	mBorderless = borderless;
}

void WindowImplGlfw::setAlwaysOnTop( bool alwaysOnTop )
{
	if( mAlwayOnTop == alwaysOnTop )
		return;

	if( mGlfwWindow ) {
		::glfwSetWindowAttrib( mGlfwWindow, GLFW_FLOATING, alwaysOnTop ? GLFW_TRUE : GLFW_FALSE );
	}

	mAlwayOnTop = alwaysOnTop;
}

void WindowImplGlfw::keyDown( const KeyEvent &event )
{
	KeyEvent localEvent( event );
	mAppImpl->setWindow( mWindowRef );
	mWindowRef->emitKeyDown( &localEvent );
}

void WindowImplGlfw::draw()
{
	mAppImpl->setWindow( mWindowRef );
	mRenderer->startDraw();
	mWindowRef->emitDraw();
	mRenderer->finishDraw();
}

void WindowImplGlfw::resize()
{
	mAppImpl->setWindow( mWindowRef );
	mWindowRef->emitResize();
}

void WindowImplGlfw::hideCursor()
{
	::glfwSetInputMode( mGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );
}

void WindowImplGlfw::showCursor()
{
	::glfwSetInputMode( mGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
}

ivec2 WindowImplGlfw::getMousePos() const
{
	double xpos, ypos;
	::glfwGetCursorPos( mGlfwWindow, &xpos, &ypos );
	return ivec2( static_cast<int>( xpos ), static_cast<int>( ypos ) );
}

void WindowImplGlfw::setResizing( bool resizing )
{
	if( mWindowRef ) {
		mWindowRef->setIsResizing( resizing );
	}
}

void WindowImplGlfw::handlePostResize()
{
	if( mWindowRef ) {
		mWindowRef->emitPostResize();
	}
}

}} // namespace cinder::app
