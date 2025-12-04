/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.

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

namespace cinder { namespace app {

class AppGlfw;
class WindowImplGlfw;

// Setup macOS-specific NSApplicationDelegate wrapper
// This should be called during AppImplGlfw initialization on macOS
void setupMacOSDelegate( AppGlfw *app, bool *quitOnLastWindowClosed );

// Enable multitouch on a GLFW window
// Client apps should call this if they want to receive touch events
void enableMultiTouchForWindow( void *nativeWindow, WindowImplGlfw *windowImpl );

// Enable resize tracking on a GLFW window
// This sets up notification observers to track window resize operations
void enableResizeTrackingForWindow( void *nativeWindow, WindowImplGlfw *windowImpl );

// Disable resize tracking on a GLFW window
// This removes notification observers and cleans up - call before window destruction
void disableResizeTrackingForWindow( void *nativeWindow );

// Get current global modifier key state (works even when window doesn't have focus)
// This is useful for file drop events where the drag originates from an external app
unsigned int getGlobalModifierState();

}} // namespace cinder::app
