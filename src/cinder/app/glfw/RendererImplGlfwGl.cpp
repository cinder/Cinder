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

#include "cinder/app/glfw/RendererImplGlfwGl.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Environment.h"
#include "cinder/Log.h"

#if defined( CINDER_MAC )
	#define GLFW_EXPOSE_NATIVE_COCOA
	#define GLFW_EXPOSE_NATIVE_NSGL
	#include <glfw/glfw3.h>
	#include <glfw/glfw3native.h>
#endif

#include "glad/glad.h"
#include "glfw/glfw3.h"

#if defined( CINDER_LINUX )
	#include "cinder/app/linux/PlatformLinux.h"
#endif

namespace cinder { namespace app {

RendererImplGlfwGl::RendererImplGlfwGl( RendererGl *aRenderer )
	: mRenderer( aRenderer ), mGlfwWindow( nullptr )
{
}

void RendererImplGlfwGl::prepareWindowHints()
{
	const auto& options = mRenderer->getOptions();

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

#elif defined( CINDER_MAC )
	::glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	::glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
	::glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	if( ! options.getCoreProfile() )
		CI_LOG_E( "Ignoring request for compatibility profile. Unsupported on macOS" );
#else // Desktop Linux/Windows
	int32_t majorVersion = options.getVersion().first;
	int32_t minorVersion = options.getVersion().second;

	// If version is explicitly specified, use that version
	// Otherwise don't set version hints - GLFW will give us the highest available
	if( majorVersion != 0 || minorVersion != 0 ) {
		::glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, majorVersion );
		::glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, minorVersion );
	}

	// Only set profile if explicitly specified by user
	if( options.getProfileSpecified() ) {
		if( options.getCoreProfile() )
			::glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
		else
			::glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE );
	}

	if( options.getDebug() )
		::glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE );
#endif

	::glfwWindowHint( GLFW_SAMPLES, options.getMsaa() );
}

bool RendererImplGlfwGl::initialize( GLFWwindow *window, RendererRef sharedRenderer )
{
	mGlfwWindow = window;

	::glfwMakeContextCurrent( mGlfwWindow );

#if defined( CINDER_GL_ES )
	gl::Environment::setEs();
#else
	gl::Environment::setCore();
#endif
	gl::env()->initializeFunctionPointers();

	// Debug: Check if OpenGL functions were loaded
	if( ! glGenVertexArrays ) {
		CI_LOG_E( "FATAL: glGenVertexArrays is NULL after initializeFunctionPointers!" );
		return false;
	}

#if defined( CINDER_LINUX )
	std::shared_ptr<gl::PlatformDataLinux> platformData( new gl::PlatformDataLinux( mGlfwWindow ) );
#elif defined( CINDER_MAC )
	CGLContextObj contextObj = nullptr;
	if( mGlfwWindow ) {
		id nsContext = glfwGetNSGLContext( mGlfwWindow );
		if( nsContext ) {
			contextObj = [(NSOpenGLContext*)nsContext CGLContextObj];
			::CGLRetainContext( contextObj );  // Take our own reference to prevent use-after-free
		}
	}
	auto platformData = std::shared_ptr<cinder::gl::Context::PlatformData>( new cinder::gl::PlatformDataMac( contextObj ) );
#endif

#if ! defined( CINDER_GL_ES )
	platformData->mDebug = mRenderer->getOptions().getDebug();
	platformData->mDebugLogSeverity = mRenderer->getOptions().getDebugLogSeverity();
	platformData->mDebugBreakSeverity = mRenderer->getOptions().getDebugBreakSeverity();
	platformData->mObjectTracking = mRenderer->getOptions().getObjectTracking();
#endif

	mCinderContext = gl::Context::createFromExisting( platformData );
	mCinderContext->makeCurrent();

	::glfwSwapInterval( 1 );

	return true;
}

void RendererImplGlfwGl::kill()
{
}

void RendererImplGlfwGl::defaultResize() const
{
	// Viewport uses framebuffer size (pixels)
	int fbWidth = 0, fbHeight = 0;
	::glfwGetFramebufferSize( mGlfwWindow, &fbWidth, &fbHeight );
	gl::viewport( 0, 0, fbWidth, fbHeight );

	// Matrices use window size (points) so app code works in points
	int winWidth = 0, winHeight = 0;
	::glfwGetWindowSize( mGlfwWindow, &winWidth, &winHeight );
	gl::setMatricesWindow( winWidth, winHeight );
}

void RendererImplGlfwGl::swapBuffers() const
{
	::glfwSwapBuffers( mGlfwWindow );
}

void RendererImplGlfwGl::makeCurrentContext( bool force )
{
	mCinderContext->makeCurrent( force );
}

}} // namespace cinder::app
