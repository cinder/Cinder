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

#pragma once

#include "cinder/app/emscripten/AppEmscripten.h"

// Use the system version, not the one that ships with Cinder.
#include <GLFW/glfw3.h>
#include <list>
#include <unistd.h>
namespace cinder { namespace app {

class AppEmscripten;
class WindowImplEmscripten;

class AppImplEmscripten {
 public:

	AppImplEmscripten( AppEmscripten *aApp, const AppEmscripten::Settings &settings );
	virtual ~AppImplEmscripten();

	AppEmscripten					*getApp();
	
 protected:
	RendererRef					findSharedRenderer( const RendererRef &searchRenderer );

	WindowRef					createWindow( Window::Format format );
	void						quit();

	float						getFrameRate() const;
	void						setFrameRate( float aFrameRate );
	void 						disableFrameRate();
	bool 						isFrameRateEnabled() const;


	WindowRef					getWindow() const;
	void						setWindow( WindowRef window );
	size_t 						getNumWindows() const;
	WindowRef 					getWindowIndex( size_t index ) const;
	WindowRef 					getForegroundWindow() const;
	void						setForegroundWindow( WindowRef window );

	void						hideCursor();
	void						showCursor();
	ivec2						getMousePos() const;


private:
	AppEmscripten				*mApp = nullptr;
	WindowRef					mMainWindow;

	std::list<WindowImplEmscripten*>	mWindows;
	WindowRef							mActiveWindow;
	WindowRef 							mForegroundWindow;

	float 						mFrameRate;
	bool						mFrameRateEnabled;
	bool 						mShouldQuit = false;

	bool						mSetupHasBeenCalled = false;
	
	double						mNextFrameTime;

	void 						sleepUntilNextFrame();
	void 						run();

	void 						registerWindowEvents( WindowImplEmscripten* window );
	void 						unregisterWindowEvents( WindowImplEmscripten* window );

	friend class AppEmscripten;
	friend class WindowImplEmscripten;
	friend class GlfwCallbacks;

public:
	// This has to be static so we can call it from the callback.
	void 						updateAndDraw();	
};

}} // namespace cinder::app
