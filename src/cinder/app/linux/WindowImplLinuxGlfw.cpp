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
#include "cinder/app/linux/PlatformLinux.h"
#include "cinder/Log.h"

namespace cinder { namespace app {

WindowImplLinux::WindowImplLinux( const Window::Format &format, WindowImplLinux *sharedRendererWindow, AppImplLinux *appImpl )
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

	const auto& options = std::dynamic_pointer_cast<RendererGl>( mRenderer )->getOptions();
#if defined( CINDER_GL_ES )
	::glfwDefaultWindowHints();
	::glfwWindowHint( GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API );
	::glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_ES_API );
  #if CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3_2
	::glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	::glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );	
	std::cout << "Rendering with OpenGL ES 3.2" << std::endl;		
  #elif CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3_1
	::glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	::glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );	
	std::cout << "Rendering with OpenGL ES 3.1" << std::endl;	
  #elif CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3
	::glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	::glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
	std::cout << "Rendering with OpenGL ES 3.0" << std::endl;	
  #elif CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_2
	::glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );
	::glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
	std::cout << "Rendering with OpenGL ES 2.0" << std::endl;	
  #endif

#else // Desktop
	int32_t majorVersion = options.getVersion().first;
	int32_t minorVersion = options.getVersion().second;
	::glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, majorVersion );
	::glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, minorVersion );
	::glfwWindowHint( GLFW_DECORATED, format.isBorderless() ? GL_FALSE : GL_TRUE );
	::glfwWindowHint( GLFW_RESIZABLE, format.isResizable() ? GL_TRUE : GL_FALSE );
	if( options.getCoreProfile() ) {
		::glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
		std::cout << "Rendering with OpenGL Core Profile " << majorVersion << "." << minorVersion << std::endl;
	}
	else {
		std::cout << "Rendering with OpenGL " << majorVersion << "." << minorVersion << std::endl;		
	}
	if( options.getDebug() )
		::glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE );
#endif

	::glfwWindowHint( GLFW_SAMPLES, options.getMsaa() );

	if( mFullScreen ) {
		cinder::app::DisplayLinux* displayLinux = dynamic_cast<cinder::app::DisplayLinux*>( mDisplay.get() );
		auto windowSize = displayLinux->getSize();
		mGlfwWindow = ::glfwCreateWindow( windowSize.x, windowSize.y, format.getTitle().c_str(), displayLinux->getGlfwMonitor(), sharedGlfwWindow );
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
}

WindowImplLinux::~WindowImplLinux()
{
	mAppImpl->unregisterWindowEvents( this );
}

void WindowImplLinux::setFullScreen( bool fullScreen, const app::FullScreenOptions &options )
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
		cinder::app::DisplayLinux* displayLinux = dynamic_cast<cinder::app::DisplayLinux*>( mDisplay.get() );
		::glfwSetWindowMonitor( mGlfwWindow, displayLinux->getGlfwMonitor(), 0, 0, mDisplay->getWidth(), mDisplay->getHeight(), GLFW_DONT_CARE );
	}
	else {
		::glfwSetWindowMonitor( mGlfwWindow, NULL, mWindowedPos.x, mWindowedPos.y, mWindowedSize.x, mWindowedSize.y, GLFW_DONT_CARE );
	}

	mFullScreen = fullScreen;
}

ivec2 WindowImplLinux::getSize() const
{
	int xsize, ysize;
	::glfwGetWindowSize( mGlfwWindow, &xsize, &ysize );
	return ivec2( xsize, ysize );	
}

void WindowImplLinux::setSize( const ivec2 &size )
{
	::glfwSetWindowSize( mGlfwWindow, size.x, size.y );
}

ivec2 WindowImplLinux::getPos() const
{
	int xpos, ypos;
	::glfwGetWindowPos( mGlfwWindow, &xpos, &ypos );
	return ivec2( xpos, ypos );
}

void WindowImplLinux::setPos( const ivec2 &pos )
{	
	::glfwSetWindowPos( mGlfwWindow, pos.x, pos.y );
}

void WindowImplLinux::close()
{
	::glfwSetWindowShouldClose( mGlfwWindow, 1 );
}

void WindowImplLinux::setTitle( const std::string &title )
{
    ::glfwSetWindowTitle( mGlfwWindow, title.c_str() );
}

void WindowImplLinux::hide()
{
	::glfwHideWindow( mGlfwWindow );
}

void WindowImplLinux::show()
{
	::glfwShowWindow( mGlfwWindow );
}

const std::vector<TouchEvent::Touch>& WindowImplLinux::getActiveTouches() const
{
	return mActiveTouches;
}

void WindowImplLinux::setBorderless( bool borderless )
{
	CI_LOG_W( "Window::setBorderless() currently unimplemented in GLFW" );
	// TODO: Find a way to do this w/o recreating 
}

void WindowImplLinux::setAlwaysOnTop( bool alwaysOnTop )
{
	CI_LOG_W( "Window::setAlwaysOnTop() currently unimplemented in GLFW" );
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
	::glfwSetInputMode( mGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );
}

void WindowImplLinux::showCursor()
{
	::glfwSetInputMode( mGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
}

ivec2 WindowImplLinux::getMousePos() const
{
	double xpos, ypos;
	::glfwGetCursorPos( mGlfwWindow, &xpos, &ypos );
	return ivec2( static_cast<int>( xpos ), static_cast<int>( ypos ) );
}

}} // namespace cinder::app
