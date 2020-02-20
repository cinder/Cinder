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

#include "cinder/app/linux/AppImplLinux.h"
#include "cinder/app/linux/AppLinux.h"
#include "cinder/app/linux/WindowImplLinux.h"

#include "cinder/Log.h"

#include <iostream>
#include <unistd.h>

namespace cinder { namespace app {

class GlfwCallbacks {
public:
	
	static std::map<GLFWwindow*, std::pair<AppImplLinux*,WindowRef>> sWindowMapping;
	static KeyEvent sKeyEvent;
	static bool sCapsLockDown, sNumLockDown, sScrollLockDown;
		
	static void registerWindowEvents( GLFWwindow *glfwWindow, AppImplLinux* cinderAppImpl, const WindowRef& cinderWindow ) {
		sWindowMapping[glfwWindow] = std::make_pair( cinderAppImpl, cinderWindow );

		::glfwSetWindowSizeCallback( glfwWindow, GlfwCallbacks::onWindowSize );
		::glfwSetKeyCallback( glfwWindow, GlfwCallbacks::onKeyboard );
		::glfwSetCharCallback( glfwWindow, GlfwCallbacks::onCharInput );
		::glfwSetCursorPosCallback( glfwWindow, GlfwCallbacks::onMousePos );
		::glfwSetMouseButtonCallback( glfwWindow, GlfwCallbacks::onMouseButton );
		::glfwSetScrollCallback( glfwWindow, GlfwCallbacks::onMouseWheel );
		::glfwSetDropCallback( glfwWindow, GlfwCallbacks::onFileDrop );
	}

	static void unregisterWindowEvents( GLFWwindow *glfwWindow ) {
		sWindowMapping.erase( glfwWindow );
	}

	static void onError( int error, const char* description ) {
		std::cout << "(glfw:error): error=" << error << ", desc=" << description << std::endl;
	}

	static void onWindowSize( GLFWwindow* glfwWindow, int width, int height ) {
		auto iter = sWindowMapping.find( glfwWindow );
		if( sWindowMapping.end() != iter ) {
			auto& cinderAppImpl = iter->second.first;
			auto& cinderWindow = iter->second.second;
			cinderAppImpl->setWindow( cinderWindow );

			cinderWindow->emitResize();
		}
	}

	//! Translates GLFW \a mods into ci::KeyEvent modifier values
	static int extractKeyModifiers( int mods )
	{
		uint32_t modifiers = 0;
		if( mods & GLFW_MOD_SHIFT ) {
			modifiers |= KeyEvent::SHIFT_DOWN;
		}
		if( mods & GLFW_MOD_CONTROL ) {
			modifiers |= KeyEvent::CTRL_DOWN;
		}
		if( mods & GLFW_MOD_ALT ) {
			modifiers |= KeyEvent::ALT_DOWN;
		}
		if( mods & GLFW_MOD_SUPER ) {
			modifiers |= KeyEvent::META_DOWN;
		}
		return modifiers;
	}

	//! Extracts Cinder ci::MouseEvent modifier values from glfwGetKey() calls
	static int getGlfwKeyModifiersMouse( GLFWwindow *glfwWindow )
	{
		int modifiers = 0;
		if( ( GLFW_PRESS == glfwGetKey( glfwWindow, GLFW_KEY_LEFT_SHIFT ) ) || ( GLFW_PRESS == glfwGetKey( glfwWindow, GLFW_KEY_RIGHT_SHIFT ) ) ) {
			modifiers |= MouseEvent::SHIFT_DOWN;
		}
		if( ( GLFW_PRESS == glfwGetKey( glfwWindow, GLFW_KEY_LEFT_CONTROL ) ) || ( GLFW_PRESS == glfwGetKey( glfwWindow, GLFW_KEY_RIGHT_CONTROL ) ) ) {
			modifiers |= MouseEvent::CTRL_DOWN;
		}
		if( ( GLFW_PRESS == glfwGetKey( glfwWindow, GLFW_KEY_LEFT_ALT ) ) || ( GLFW_PRESS == glfwGetKey( glfwWindow, GLFW_KEY_RIGHT_ALT ) ) ) {
			modifiers |= MouseEvent::ALT_DOWN;
		}
		if( ( GLFW_PRESS == glfwGetKey( glfwWindow, GLFW_KEY_LEFT_SUPER ) ) || ( GLFW_PRESS == glfwGetKey( glfwWindow, GLFW_KEY_RIGHT_SUPER ) )  ) {
			modifiers |= MouseEvent::META_DOWN;
		}
		return modifiers;
	}

	//! Returns one or fewer ci::MouseEvent mask value to reflect initiating mouse button
	static int getGlfwMouseInitiator( GLFWwindow *glfwWindow )
	{
		int initiator = 0;
		if( GLFW_PRESS == glfwGetMouseButton( glfwWindow, GLFW_MOUSE_BUTTON_LEFT ) ) {
			initiator = MouseEvent::LEFT_DOWN;
		}
		else if( GLFW_PRESS == glfwGetMouseButton( glfwWindow, GLFW_MOUSE_BUTTON_MIDDLE ) ) {
			initiator = MouseEvent::MIDDLE_DOWN;
		}
		else if( GLFW_PRESS == glfwGetMouseButton( glfwWindow, GLFW_MOUSE_BUTTON_RIGHT ) ) {
			initiator = MouseEvent::RIGHT_DOWN;
		}
		return initiator;
	}

	//! Returns MouseEvent bitmask to reflect pressed buttons as ci::MouseEvent mask values
	static int getGlfwMouseButtons( GLFWwindow *glfwWindow )
	{
		int modifiers = 0;
		if( GLFW_PRESS == glfwGetMouseButton( glfwWindow, GLFW_MOUSE_BUTTON_LEFT ) ) {
			modifiers |= MouseEvent::LEFT_DOWN;
		}
		if( GLFW_PRESS == glfwGetMouseButton( glfwWindow, GLFW_MOUSE_BUTTON_MIDDLE ) ) {
			modifiers |= MouseEvent::MIDDLE_DOWN;
		}
		if( GLFW_PRESS == glfwGetMouseButton( glfwWindow, GLFW_MOUSE_BUTTON_RIGHT ) ) {
			modifiers |= MouseEvent::RIGHT_DOWN;
		}
		return modifiers;
	}

	static int8_t modifyChar( int key, uint32_t modifiers, bool capsLockIsDown )
	{
		// Limit char8 to ASCII input for now.
		int8_t ret = ( key <= 127 ) ? (char)key : 0;
		if( ret ) {
			// a letter key
			bool shiftIsDown = modifiers & KeyEvent::SHIFT_DOWN;
			if( ret > 64 && ret < 91 && ! ( shiftIsDown ^ capsLockIsDown ) ) {
					ret += 32;
			}
			// other modifiable keys
			else if( shiftIsDown ) {
				switch ( ret ) {
					case '1': ret = '!'; break;
					case '2': ret = '@'; break;
					case '3': ret = '#'; break;
					case '4': ret = '$'; break;
					case '5': ret = '%'; break;
					case '6': ret = '^'; break;
					case '7': ret = '&'; break;
					case '8': ret = '*'; break;
					case '9': ret = '('; break;
					case '0': ret = ')'; break;
					case '`': ret = '~'; break;
					case '-': ret = '_'; break;
					case '=': ret = '+'; break;
					case '[': ret = '{'; break;
					case ']': ret = '}'; break;
					case '\\': ret = '|'; break;
					case ';': ret = ':'; break;
					case '\'': ret = '"'; break;
					case ',': ret = '<'; break;
					case '.': ret = '>'; break;
					case '/': ret = '?'; break;
					default: break;
				};
			}
		}
		return ret;
	}

	static void onKeyboard( GLFWwindow *glfwWindow, int key, int scancode, int action, int mods ) {
		auto iter = sWindowMapping.find( glfwWindow );
		if( sWindowMapping.end() != iter ) {
			auto& cinderAppImpl = iter->second.first;
			auto& cinderWindow = iter->second.second;
			cinderAppImpl->setWindow( cinderWindow );

			int32_t nativeKeyCode = KeyEvent::translateNativeKeyCode( key );
			
			if( glfwGetKey( glfwWindow, GLFW_KEY_CAPS_LOCK ) )
				sCapsLockDown = ! sCapsLockDown;
			if( glfwGetKey( glfwWindow, GLFW_KEY_NUM_LOCK ) )
				sNumLockDown = ! sNumLockDown;
			if( glfwGetKey( glfwWindow, GLFW_KEY_SCROLL_LOCK ) )
				sScrollLockDown = !sScrollLockDown;

			auto modifiers = extractKeyModifiers( mods );
			auto convertedChar = modifyChar( key, modifiers, sCapsLockDown );

			if( GLFW_PRESS == action ) {
				sKeyEvent = KeyEvent( cinderWindow, nativeKeyCode, 0, 0, modifiers, scancode );
				// if convertedChar != 0, we'll get the unicode char value and wait until onCharInput is called.
				// if convertedChar == 0, that means it is a non-unicode key (ex ctrl), so we'll emit the keydown here.
				if( convertedChar == 0 ) {
					cinderWindow->emitKeyDown( &sKeyEvent );
				}
			}
			else if( GLFW_RELEASE == action ) {
				KeyEvent event( cinderWindow, sKeyEvent.getCode(), sKeyEvent.getCharUtf32(), sKeyEvent.getChar(), modifiers, scancode );
				sKeyEvent = {};
				cinderWindow->emitKeyUp( &event );
			}
		}
	}

	static void onCharInput( GLFWwindow *glfwWindow, unsigned int codepoint )
	{
		auto iter = sWindowMapping.find( glfwWindow );
		if( sWindowMapping.end() != iter ) {
			auto& cinderAppImpl = iter->second.first;
			auto& cinderWindow = iter->second.second;
			cinderAppImpl->setWindow( cinderWindow );

			// create new sKeyEvent that includes same info as previous but also the utf32 char. emit key down signal with that
			char asciiChar = codepoint < 256 ? (char)codepoint : 0;
			sKeyEvent = KeyEvent( cinderWindow, sKeyEvent.getCode(), codepoint, asciiChar, sKeyEvent.getModifiers(), sKeyEvent.getNativeKeyCode() );
			cinderWindow->emitKeyDown( &sKeyEvent );
		}
	}

	static void onMousePos( GLFWwindow* glfwWindow, double mouseX, double mouseY ) {
		auto iter = sWindowMapping.find( glfwWindow );
		if( sWindowMapping.end() != iter ) {
			auto& cinderAppImpl = iter->second.first;
			auto& cinderWindow = iter->second.second;
			cinderAppImpl->setWindow( cinderWindow );

			int initiator = getGlfwMouseInitiator( glfwWindow );
			int modifiers = getGlfwMouseButtons( glfwWindow ) | getGlfwKeyModifiersMouse( glfwWindow );
			MouseEvent event( getWindow(), initiator, (int)mouseX, (int)mouseY, modifiers, 0.0f, 0 );
			if( 0 != initiator ) {
				cinderWindow->emitMouseDrag( &event );
			}
			else {
				cinderWindow->emitMouseMove( &event );
			}
		}
	}

	static void onMouseButton(GLFWwindow* glfwWindow, int button, int action, int mod ) {
		auto iter = sWindowMapping.find( glfwWindow );
		if( sWindowMapping.end() != iter ) {
			auto& cinderAppImpl = iter->second.first;
			auto& cinderWindow = iter->second.second;
			cinderAppImpl->setWindow( cinderWindow );

			double mouseX, mouseY;
			::glfwGetCursorPos( glfwWindow, &mouseX, &mouseY );

			int initiator = 0;
			switch( button ) {
				case GLFW_MOUSE_BUTTON_LEFT   : initiator = MouseEvent::LEFT_DOWN;   break;
				case GLFW_MOUSE_BUTTON_MIDDLE : initiator = MouseEvent::MIDDLE_DOWN; break;
				case GLFW_MOUSE_BUTTON_RIGHT  : initiator = MouseEvent::RIGHT_DOWN;  break;
			}
			int modifiers = getGlfwMouseButtons( glfwWindow ) | getGlfwKeyModifiersMouse( glfwWindow );

			if( 0 != initiator ) {
				MouseEvent event( getWindow(), initiator, (int)mouseX, (int)mouseY, modifiers, 0.0f, 0 );
				if( GLFW_PRESS == action ) {
					cinderWindow->emitMouseDown( &event );	
				}
				else if( GLFW_RELEASE == action ) {
					cinderWindow->emitMouseUp( &event );	
				}
			}
		}
	}

	static void onMouseWheel(GLFWwindow* glfwWindow, double xoffset, double yoffset ) {
		auto iter = sWindowMapping.find( glfwWindow );
		if( sWindowMapping.end() != iter ) {
			auto& cinderAppImpl = iter->second.first;
			auto& cinderWindow = iter->second.second;
			cinderAppImpl->setWindow( cinderWindow );

			double mouseX, mouseY;
			::glfwGetCursorPos( glfwWindow, &mouseX, &mouseY );

			int modifiers = getGlfwMouseButtons( glfwWindow ) | getGlfwKeyModifiersMouse( glfwWindow );

			float wheelDelta = xoffset + yoffset;
			MouseEvent event( getWindow(), 0, (int)mouseX, (int)mouseY, modifiers, wheelDelta , 0 );
			cinderWindow->emitMouseWheel( &event );	
		}
	}

	static void onFileDrop( GLFWwindow *glfwWindow, int count, const char **paths )
	{
		std::vector<fs::path> files;
		for( int i = 0; i < count; i++ ) {
			files.push_back( paths[i] );
		}

		vec2 dropPoint = { 0, 0 }; // note: doesn't appear to be any way to get the drop position.
		FileDropEvent dropEvent( getWindow(), dropPoint.x, dropPoint.y, files );
		getWindow()->emitFileDrop( &dropEvent );
	}
};

std::map<GLFWwindow*, std::pair<AppImplLinux*,WindowRef>> GlfwCallbacks::sWindowMapping;
KeyEvent GlfwCallbacks::sKeyEvent;
bool GlfwCallbacks::sCapsLockDown = false;
bool GlfwCallbacks::sNumLockDown = false;
bool GlfwCallbacks::sScrollLockDown = false;

////////////////////////////////////////////////////////////////////////////////
// AppImplLinux
////////////////////////////////////////////////////////////////////////////////
AppImplLinux::AppImplLinux( AppLinux *aApp, const AppLinux::Settings &settings )
	: mApp( aApp )
{
	// Set error callback
	::glfwSetErrorCallback( GlfwCallbacks::onError );

	// Must be called before we can do anything with GLFW
 	if( ! ::glfwInit() ) {
  		std::cerr << std::string( "::glfwInit failed!" ) << std::endl;
		std::exit( 1 );
	}

	mFrameRate = settings.getFrameRate();
	mFrameRateEnabled = settings.isFrameRateEnabled();
	mQuitOnLastWindowClosed = settings.isQuitOnLastWindowCloseEnabled();
	
	auto formats = settings.getWindowFormats();
	if( formats.empty() ) {
		formats.push_back( settings.getDefaultWindowFormat() );
	}

	for( auto& format : formats ) {
		if( ! format.isTitleSpecified() ) {
			format.setTitle( settings.getTitle() );
		}

		// We need to keep tabs on the main window since it's how
		// we will communicate with GLFW.
		if( ! mMainWindow ) {
			mMainWindow = this->createWindow( format );
		}
		else {
			this->createWindow( format );
		}
	}

	// Set the active window
	if( ! mWindows.empty() ) {
		setWindow( mWindows.back()->getWindow() );
	}
}

AppImplLinux::~AppImplLinux()
{
	::glfwTerminate();
}

AppLinux *AppImplLinux::getApp()
{
	return mApp;
}

void AppImplLinux::sleepUntilNextFrame()
{
	// get current time in seconds
	double currentSeconds = getElapsedSeconds();

	// calculate time per frame in seconds
	double secondsPerFrame = 1.0 / mFrameRate;

	// determine if application was frozen for a while and adjust next frame time
	double elapsedSeconds = currentSeconds - mNextFrameTime;
	if( elapsedSeconds > 1.0 ) {
		int numSkipFrames = (int)(elapsedSeconds / secondsPerFrame);
		mNextFrameTime += (numSkipFrames * secondsPerFrame);
	}

	// determine when next frame should be drawn
	mNextFrameTime += secondsPerFrame;

	// sleep and process messages until next frame
	if( ( mFrameRateEnabled ) && ( mNextFrameTime > currentSeconds ) ) {
		double sleepTime = std::max( mNextFrameTime - currentSeconds, 0.0 );
		unsigned long sleepMicroSecs = sleepTime*1000000L;
		usleep(sleepMicroSecs);
	}
}

void AppImplLinux::run()
{
	mApp->privateSetup__();
	mSetupHasBeenCalled = true;

	// quit() was called from setup()
	if( mShouldQuit )
		goto terminate;

	// issue initial app activation event
	mApp->emitDidBecomeActive();
	
	// isse initial resize revent
	for( auto &window : mWindows ) {
		window->resize();
	}

	// initialize our next frame time
	mNextFrameTime = getElapsedSeconds();	

	while( ! mShouldQuit ) {
		// update and draw
		mApp->privateUpdate__();
		for( auto &window : mWindows ) {
			if( mShouldQuit ) // test for quit() issued from update() or draw()
				goto terminate;
			window->draw();
		}

		glfwPollEvents();

		// Sleep until the next frame
		sleepUntilNextFrame();	

		// Check if a window should be closed / destroyed or if we should exit.
		auto shouldCloseWindow = [ this ] ( WindowImplLinux* window ) {
			if( ::glfwWindowShouldClose( window->getNative() ) ) {
				window->getWindow()->emitClose();
				::glfwDestroyWindow( window->getNative() );
				return true;
			}
			return false;
		};

        	mWindows.remove_if( shouldCloseWindow );
		if( mWindows.empty() && mQuitOnLastWindowClosed )
			mShouldQuit = true;
	}

  terminate:
	mApp->emitCleanup();
	// Destroy the main window - this should resolve to
	// a call for ::glfwDestroyWindow( ... );
	mMainWindow.reset();
}

WindowImplLinux* AppImplLinux::findSharedRendererWindow( const RendererRef &searchRenderer )
{
	if( ! searchRenderer ) {
		return nullptr;
	}

	for( const auto &win : mWindows ) {
		RendererRef renderer = win->getRenderer();
		auto rendererPtr = renderer.get();
		auto searchRendererPtr = searchRenderer.get();
		if( renderer && ( typeid(*rendererPtr) == typeid(*searchRendererPtr) ) ) {
			return win;
		}
	}

	return nullptr;
}

WindowRef AppImplLinux::createWindow( Window::Format format )
{
	if( ! format.getRenderer() ) {
		format.setRenderer( mApp->getDefaultRenderer()->clone() );
	}

	mWindows.push_back( new WindowImplLinux( format, findSharedRendererWindow( format.getRenderer() ), this ) );

	// emit initial resize if we have fired setup
	if( mSetupHasBeenCalled ) {
		mWindows.back()->getWindow()->emitResize();
	}

	return mWindows.back()->getWindow();
}

void AppImplLinux::quit()
{
	for( auto &window : mWindows ) {
		::glfwSetWindowShouldClose( window->getNative(), true );	
	}
	mShouldQuit = true;
}

float AppImplLinux::getFrameRate() const 
{ 
	return mFrameRate; 
}

void AppImplLinux::setFrameRate( float frameRate )
{
	mFrameRate = frameRate;
	mFrameRateEnabled = true;
	mNextFrameTime = mApp->getElapsedSeconds();
}

void AppImplLinux::disableFrameRate()
{
	mFrameRateEnabled = false;
}

bool AppImplLinux::isFrameRateEnabled() const
{
	return mFrameRateEnabled;
}

WindowRef AppImplLinux::getWindow() const 
{ 
	return mActiveWindow; 
}

void AppImplLinux::setWindow( WindowRef window ) 
{ 
	mActiveWindow = window; 
}

size_t AppImplLinux::getNumWindows() const
{
	return mWindows.size();
}

WindowRef AppImplLinux::getWindowIndex( size_t index ) const
{
	if( index >= mWindows.size() ) {
		return cinder::app::WindowRef();
	}
	
	auto winIt = mWindows.begin();
	std::advance( winIt, index );
	return (*winIt)->mWindowRef;
}

WindowRef AppImplLinux::getForegroundWindow() const
{
	return mForegroundWindow;
}

void AppImplLinux::setForegroundWindow( WindowRef window )
{
	mForegroundWindow = window;
}

void AppImplLinux::hideCursor()
{
	mActiveWindow->getImpl()->hideCursor();
}

void AppImplLinux::showCursor()
{
	mActiveWindow->getImpl()->showCursor();
}

ivec2 AppImplLinux::getMousePos() const
{
	return mActiveWindow->getImpl()->getMousePos();	
}

void AppImplLinux::registerWindowEvents( WindowImplLinux* window )
{
	GlfwCallbacks::registerWindowEvents( window->getNative(), this, window->getWindow() );
}

void AppImplLinux::unregisterWindowEvents( WindowImplLinux* window )
{
	GlfwCallbacks::unregisterWindowEvents( window->getNative() );
}

}} // namespace cinder::app
