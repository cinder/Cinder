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

#include "cinder/app/linux/RendererGlLinux.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Environment.h"

#include "glad/glad.h"
#include "glfw/glfw3.h"

namespace cinder { namespace app {

RendererGlLinux::RendererGlLinux( RendererGl *aRenderer )
	: mRenderer( aRenderer )
{
}

RendererGlLinux::~RendererGlLinux()
{
}


bool RendererGlLinux::initialize( void *window, RendererRef sharedRenderer )
{
	mContext = reinterpret_cast<GLFWwindow*>( window );

	::glfwMakeContextCurrent( mContext );

#if defined( CINDER_GL_ES )
	gl::Environment::setEs();
#else
	gl::Environment::setCore();
#endif
	gl::env()->initializeFunctionPointers();

	std::shared_ptr<gl::PlatformDataLinux> platformData( new gl::PlatformDataLinux( mContext ) );

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

void RendererGlLinux::kill()
{
}

void RendererGlLinux::defaultResize() const
{
	int width = 0;
	int height = 0;
	glfwGetFramebufferSize( mContext, &width, &height );

	gl::viewport( 0, 0, width, height );
	gl::setMatricesWindow( width, height );	
}

void RendererGlLinux::swapBuffers() const
{
	::glfwSwapBuffers( mContext );
}

void RendererGlLinux::makeCurrentContext( bool force )
{
	mCinderContext->makeCurrent( force );
}

}} // namespace cinder::app
