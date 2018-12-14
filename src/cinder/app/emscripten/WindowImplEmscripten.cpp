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
#include "cinder/app/emscripten/WindowImplEmscripten.h"
#include "cinder/app/emscripten/AppImplEmscripten.h"

namespace cinder { namespace app {

WindowImplEmscripten::WindowImplEmscripten( const Window::Format &format, RendererRef sharedRenderer, AppImplEmscripten *appImpl )
	: mAppImpl( appImpl )
{
	mDisplay = format.getDisplay();
	mRenderer = format.getRenderer();

#if defined( CINDER_GL_ES )
	::glfwDefaultWindowHints();
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
#endif

	auto windowSize = format.getSize();
	mGlfwWindow = ::glfwCreateWindow( windowSize.x, windowSize.y, format.getTitle().c_str(), NULL, NULL );

	mRenderer->setup( mGlfwWindow, sharedRenderer );

	// set WindowRef and its impl pointer to this
	mWindowRef = Window::privateCreate__( this, mAppImpl->getApp() );

	mAppImpl->registerWindowEvents( this );
}

WindowImplEmscripten::~WindowImplEmscripten()
{
	mAppImpl->unregisterWindowEvents( this );
}

void WindowImplEmscripten::setFullScreen( bool fullScreen, const app::FullScreenOptions &options )
{
	// TODO: Find a way to do this w/o recreating 
}

ivec2 WindowImplEmscripten::getSize() const
{
	int xsize, ysize;
	::glfwGetWindowSize( mGlfwWindow, &xsize, &ysize );
	return ivec2( xsize, ysize );	
}

void WindowImplEmscripten::setSize( const ivec2 &size )
{
	::glfwSetWindowSize( mGlfwWindow, size.x, size.y );
}

ivec2 WindowImplEmscripten::getPos() const
{
	int xpos, ypos;
	::glfwGetWindowPos( mGlfwWindow, &xpos, &ypos );
	return ivec2( xpos, ypos );
}

void WindowImplEmscripten::setPos( const ivec2 &pos )
{	
	::glfwSetWindowPos( mGlfwWindow, pos.x, pos.y );
}

void WindowImplEmscripten::close()
{
}

void WindowImplEmscripten::setTitle( const std::string &title )
{
}

void WindowImplEmscripten::hide()
{
	::glfwHideWindow( mGlfwWindow );
}

void WindowImplEmscripten::show()
{
	::glfwShowWindow( mGlfwWindow );
}

const std::vector<TouchEvent::Touch>& WindowImplEmscripten::getActiveTouches() const
{
	return mActiveTouches;
}

void WindowImplEmscripten::setBorderless( bool borderless )
{
	// TODO: Find a way to do this w/o recreating 
}

void WindowImplEmscripten::setAlwaysOnTop( bool alwaysOnTop )
{
	// TODO: Find a way to do this w/o recreating 
}

void WindowImplEmscripten::keyDown( const KeyEvent &event )
{
	KeyEvent localEvent( event );
	mAppImpl->setWindow( mWindowRef );
	mWindowRef->emitKeyDown( &localEvent );
}

void WindowImplEmscripten::draw()
{
	mAppImpl->setWindow( mWindowRef );
	mRenderer->startDraw();
	mWindowRef->emitDraw();
	mRenderer->finishDraw();
}

void WindowImplEmscripten::resize()
{
	mAppImpl->setWindow( mWindowRef );
	mWindowRef->emitResize();
}

void WindowImplEmscripten::hideCursor()
{
	::glfwSetInputMode( mGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );
}

void WindowImplEmscripten::showCursor()
{
	::glfwSetInputMode( mGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
}

ivec2 WindowImplEmscripten::getMousePos() const
{
	double xpos, ypos;
	::glfwGetCursorPos( mGlfwWindow, &xpos, &ypos );
	return ivec2( static_cast<int>( xpos ), static_cast<int>( ypos ) );
}

}} // namespace cinder::app
