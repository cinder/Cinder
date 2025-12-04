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

#include "cinder/app/glfw/AppImplGlfw.h"
#include "cinder/app/glfw/AppGlfw.h"
#include "cinder/app/glfw/WindowImplGlfw.h"

#include "cinder/Log.h"

#include <iostream>
#if defined( CINDER_LINUX )
	#include <unistd.h>
#elif defined( CINDER_MAC )
	#include <thread>
	#include <chrono>
	#include "cinder/app/glfw/AppImplGlfwMac.h"
#endif

namespace cinder { namespace app {

class GlfwCallbacks {
public:

	static std::map<GLFWwindow*, std::pair<AppImplGlfw*,WindowRef>> sWindowMapping;
	static bool sCapsLockDown, sNumLockDown, sScrollLockDown;

	//! Stores key data from the most recent keyDown for correlation with subsequent char events
	struct LastKeyData {
		int			code = 0;
		int			scancode = 0;
		unsigned int modifiers = 0;
	};
	static std::map<GLFWwindow*, LastKeyData> sLastKeyData;

	static void registerWindowEvents( GLFWwindow *glfwWindow, AppImplGlfw* cinderAppImpl, const WindowRef& cinderWindow ) {
		sWindowMapping[glfwWindow] = std::make_pair( cinderAppImpl, cinderWindow );

		::glfwSetWindowSizeCallback( glfwWindow, GlfwCallbacks::onWindowSize );
		::glfwSetWindowPosCallback( glfwWindow, GlfwCallbacks::onWindowMove );
		::glfwSetKeyCallback( glfwWindow, GlfwCallbacks::onKeyboard );
		::glfwSetCharCallback( glfwWindow, GlfwCallbacks::onCharInput );
		::glfwSetCursorPosCallback( glfwWindow, GlfwCallbacks::onMousePos );
		::glfwSetMouseButtonCallback( glfwWindow, GlfwCallbacks::onMouseButton );
		::glfwSetScrollCallback( glfwWindow, GlfwCallbacks::onMouseWheel );
		::glfwSetDropCallback( glfwWindow, GlfwCallbacks::onFileDrop );
//		::glfwSetWindowRefreshCallback( glfwWindow, GlfwCallbacks::onWindowRefresh );
	}

	static void unregisterWindowEvents( GLFWwindow *glfwWindow ) {
		sWindowMapping.erase( glfwWindow );
		sLastKeyData.erase( glfwWindow );
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
			cinderAppImpl->forceDraw( cinderWindow );
		}
	}

	static void onWindowMove( GLFWwindow* glfwWindow, int xpos, int ypos ) {
		auto iter = sWindowMapping.find( glfwWindow );
		if( sWindowMapping.end() != iter ) {
			auto& cinderAppImpl = iter->second.first;
			auto& cinderWindow = iter->second.second;
			cinderAppImpl->setWindow( cinderWindow );

			cinderWindow->emitMove();
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
			char convertedChar = modifyChar( key, modifiers, sCapsLockDown );
			uint32_t charUtf32 = convertedChar ? (uint32_t)(unsigned char)convertedChar : 0;

			// Calculate KeyEvent from GLFW's parameters on both press and release
			KeyEvent keyEvent( cinderWindow, nativeKeyCode, charUtf32, convertedChar, modifiers, scancode );

			if( GLFW_PRESS == action || GLFW_REPEAT == action ) {
				// Store key data for correlation with subsequent char callback
				sLastKeyData[glfwWindow] = { nativeKeyCode, scancode, static_cast<unsigned int>( modifiers ) };
				cinderWindow->emitKeyDown( &keyEvent );
			}
			else if( GLFW_RELEASE == action ) {
				cinderWindow->emitKeyUp( &keyEvent );
			}
		}
	}

	//! Character input callback - emits keyChar events for text input
	static void onCharInput( GLFWwindow *glfwWindow, unsigned int codepoint ) {
		auto iter = sWindowMapping.find( glfwWindow );
		if( sWindowMapping.end() != iter ) {
			auto& cinderAppImpl = iter->second.first;
			auto& cinderWindow = iter->second.second;
			cinderAppImpl->setWindow( cinderWindow );

			// Retrieve stored key data from preceding keyDown (if any)
			int keyCode = 0;
			int scancode = 0;
			unsigned int modifiers = 0;
			auto keyDataIter = sLastKeyData.find( glfwWindow );
			if( keyDataIter != sLastKeyData.end() ) {
				keyCode = keyDataIter->second.code;
				scancode = keyDataIter->second.scancode;
				modifiers = keyDataIter->second.modifiers;
			}

			// Convert UTF-32 codepoint to char (ASCII only, 0 otherwise)
			char charValue = ( codepoint < 128 ) ? static_cast<char>( codepoint ) : 0;

			KeyEvent charEvent( cinderWindow, keyCode, codepoint, charValue, modifiers, scancode );
			cinderWindow->emitKeyChar( &charEvent );
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
		auto iter = sWindowMapping.find( glfwWindow );
		if( sWindowMapping.end() != iter ) {
			auto& cinderAppImpl = iter->second.first;
			auto& cinderWindow = iter->second.second;
			cinderAppImpl->setWindow( cinderWindow );

			std::vector<fs::path> files;
			for( int i = 0; i < count; i++ ) {
				files.push_back( paths[i] );
			}

			// Get the cursor position at the time of the drop so the event lands in the right spot
			double xpos, ypos;
			::glfwGetCursorPos( glfwWindow, &xpos, &ypos );
			vec2 dropPoint = { static_cast<float>(xpos), static_cast<float>(ypos) };

			// Get current modifier key state
			// On macOS, use global modifier state since the drag may originate from an external app
#if defined( CINDER_MAC )
			unsigned int modifiers = getGlobalModifierState();
#else
			unsigned int modifiers = getGlfwKeyModifiersMouse( glfwWindow );
#endif

			FileDropEvent dropEvent( cinderWindow, static_cast<int>( dropPoint.x ), static_cast<int>( dropPoint.y ), files, modifiers );
			cinderWindow->emitFileDrop( &dropEvent );
		}
	}
	
	static void onWindowRefresh( GLFWwindow* glfwWindow )
	{
		auto iter = sWindowMapping.find( glfwWindow );
		if( sWindowMapping.end() != iter ) {
			auto &cinderAppImpl = iter->second.first;
			auto &cinderWindow  = iter->second.second;

			cinderAppImpl->setWindow( cinderWindow );
			cinderWindow->emitDraw();
		}
	}
};

std::map<GLFWwindow*, std::pair<AppImplGlfw*,WindowRef>> GlfwCallbacks::sWindowMapping;
std::map<GLFWwindow*, GlfwCallbacks::LastKeyData> GlfwCallbacks::sLastKeyData;
bool GlfwCallbacks::sCapsLockDown = false;
bool GlfwCallbacks::sNumLockDown = false;
bool GlfwCallbacks::sScrollLockDown = false;

////////////////////////////////////////////////////////////////////////////////
// AppImplGlfw
////////////////////////////////////////////////////////////////////////////////
AppImplGlfw::AppImplGlfw( AppGlfw *aApp, const AppGlfw::Settings &settings )
	: mApp( aApp )
{
	// Set error callback
	::glfwSetErrorCallback( GlfwCallbacks::onError );

	// Must be called before we can do anything with GLFW
 	if( ! ::glfwInit() ) {
  		std::cerr << std::string( "::glfwInit failed!" ) << std::endl;
		std::exit( 1 );
	}

	// Initialize settings before creating delegate (which needs mQuitOnLastWindowClosed)
	mFrameRate = settings.getFrameRate();
	mFrameRateEnabled = settings.isFrameRateEnabled();
	mQuitOnLastWindowClosed = settings.isQuitOnLastWindowCloseEnabled();
	mMultiTouchEnabled = settings.isMultiTouchEnabled();

#if defined( CINDER_MAC )
	// Setup macOS-specific NSApplicationDelegate wrapper
	setupMacOSDelegate( mApp, &mQuitOnLastWindowClosed );
#endif

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

AppImplGlfw::~AppImplGlfw()
{
	mWindows.clear();
	mMainWindow.reset();
	::glfwTerminate();
}

AppGlfw *AppImplGlfw::getApp()
{
	return mApp;
}

void AppImplGlfw::sleepUntilNextFrame()
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
#if defined( CINDER_LINUX )
		usleep(sleepMicroSecs);
#elif defined( CINDER_MAC )
		std::this_thread::sleep_for( std::chrono::microseconds( sleepMicroSecs ) );
#endif
	}
}

void AppImplGlfw::run()
{
	mApp->privateSetup__();
	mSetupHasBeenCalled = true;

	// quit() was called from setup()
	if( mShouldQuit )
		goto terminate;

	// issue initial app activation event
	mActive = true;
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
		for( auto windowIt = mWindows.begin(); windowIt != mWindows.end(); ) {
			if( ::glfwWindowShouldClose( (*windowIt)->getNative() ) ) {
				closeAndEraseWindow( windowIt );
			}
			else {
				++windowIt;
			}
		}

		if( mWindows.empty() && mQuitOnLastWindowClosed )
			mShouldQuit = true;
	}

  terminate:
	mApp->emitCleanup();
	// Don't destroy windows here - defer until AppImplGlfw destructor
	// so GL context stays alive for app cleanup in ~AppGlfw()
}

WindowImplGlfw* AppImplGlfw::findSharedRendererWindow( const RendererRef &searchRenderer )
{
	if( ! searchRenderer ) {
		return nullptr;
	}

	for( const auto &win : mWindows ) {
		RendererRef renderer = win->getRenderer();
		auto rendererPtr = renderer.get();
		auto searchRendererPtr = searchRenderer.get();
		if( renderer && ( typeid(*rendererPtr) == typeid(*searchRendererPtr) ) ) {
			return win.get();
		}
	}

	return nullptr;
}

WindowRef AppImplGlfw::createWindow( Window::Format format )
{
	if( ! format.getRenderer() ) {
		format.setRenderer( mApp->getDefaultRenderer()->clone() );
	}

	auto windowImpl = std::make_unique<WindowImplGlfw>( format, findSharedRendererWindow( format.getRenderer() ), this );
	auto *windowImplPtr = windowImpl.get();
	mWindows.push_back( std::move( windowImpl ) );

#if defined( CINDER_MAC )
	if( getApp() )
		enableMultiTouchForWindow( mWindows.back()->getNative(), windowImplPtr );
#endif
		
	// emit initial resize if we have fired setup
	if( mSetupHasBeenCalled ) {
		windowImplPtr->getWindow()->emitResize();
	}

	return windowImplPtr->getWindow();
}

void AppImplGlfw::quit()
{
	for( auto &window : mWindows ) {
		::glfwSetWindowShouldClose( window->getNative(), true );
	}
	mShouldQuit = true;
}

float AppImplGlfw::getFrameRate() const
{
	return mFrameRate;
}

void AppImplGlfw::setFrameRate( float frameRate )
{
	mFrameRate = frameRate;
	mFrameRateEnabled = true;
	mNextFrameTime = mApp->getElapsedSeconds();
}

void AppImplGlfw::disableFrameRate()
{
	mFrameRateEnabled = false;
}

bool AppImplGlfw::isFrameRateEnabled() const
{
	return mFrameRateEnabled;
}

WindowRef AppImplGlfw::getWindow() const
{
	return mActiveWindow;
}

void AppImplGlfw::setWindow( WindowRef window )
{
	mActiveWindow = window;
}

size_t AppImplGlfw::getNumWindows() const
{
	return mWindows.size();
}

WindowRef AppImplGlfw::getWindowIndex( size_t index ) const
{
	if( index >= mWindows.size() ) {
		return cinder::app::WindowRef();
	}

	auto winIt = mWindows.begin();
	std::advance( winIt, index );
	return (*winIt)->mWindowRef;
}

WindowRef AppImplGlfw::getForegroundWindow() const
{
	return mForegroundWindow;
}

void AppImplGlfw::setForegroundWindow( WindowRef window )
{
	mForegroundWindow = window;
}

void AppImplGlfw::hideCursor()
{
	mActiveWindow->getImpl()->hideCursor();
}

void AppImplGlfw::showCursor()
{
	mActiveWindow->getImpl()->showCursor();
}

ivec2 AppImplGlfw::getMousePos() const
{
	return mActiveWindow->getImpl()->getMousePos();
}

void AppImplGlfw::closeAndEraseWindow( std::list<std::unique_ptr<WindowImplGlfw>>::iterator &windowIt )
{
	auto &window = *windowIt;
	WindowRef windowRef = window->getWindow();

	if( windowRef ) {
		windowRef->emitClose();
	}

	unregisterWindowEvents( window.get() );

	if( windowRef ) {
		windowRef->mValid = false;
		windowRef->setImpl( nullptr );
		windowRef->setApp( nullptr );
	}

	if( window->mGlfwWindow ) {
		::glfwDestroyWindow( window->mGlfwWindow );
		window->mGlfwWindow = nullptr;
	}

	window->mWindowRef.reset();

	windowIt = mWindows.erase( windowIt );

	if( windowRef ) {
		if( mActiveWindow == windowRef ) {
			if( ! mWindows.empty() )
				mActiveWindow = mWindows.front()->getWindow();
			else
				mActiveWindow.reset();
		}
		if( mForegroundWindow == windowRef ) {
			if( ! mWindows.empty() )
				mForegroundWindow = mWindows.front()->getWindow();
			else
				mForegroundWindow.reset();
		}
		if( mMainWindow == windowRef ) {
			if( ! mWindows.empty() )
				mMainWindow = mWindows.front()->getWindow();
			else
				mMainWindow.reset();
		}
	}

	windowRef.reset();
}

void AppImplGlfw::registerWindowEvents( WindowImplGlfw* window )
{
	GlfwCallbacks::registerWindowEvents( window->getNative(), this, window->getWindow() );
}

void AppImplGlfw::unregisterWindowEvents( WindowImplGlfw* window )
{
	GlfwCallbacks::unregisterWindowEvents( window->getNative() );
}

void AppImplGlfw::forceDraw( cinder::app::WindowRef window )
{
	mApp->privateUpdate__();
	dynamic_cast<WindowImplGlfw*>( window->getImpl() )->draw();
}

}} // namespace cinder::app
