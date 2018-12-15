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



#include "cinder/app/emscripten/RendererImplGlEmscripten.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Environment.h"

#include <GLFW/glfw3.h>

namespace cinder { namespace app {

RendererImplGlEmscripten::RendererImplGlEmscripten( RendererGl *aRenderer )
	: mRenderer( aRenderer )
{
}

RendererImplGlEmscripten::~RendererImplGlEmscripten()
{
}


bool RendererImplGlEmscripten::initialize( void *window, RendererRef sharedRenderer )
{
	mContext = reinterpret_cast<GLFWwindow*>( window );

	::glfwMakeContextCurrent( mContext );

	gl::Environment::setEs();
	gl::env()->initializeFunctionPointers();

	std::shared_ptr<gl::PlatformDataEmscripten> platformData( new gl::PlatformDataEmscripten( mContext ) );
	platformData->mObjectTracking = mRenderer->getOptions().getObjectTracking();

	mCinderContext = gl::Context::createFromExisting( platformData );
	mCinderContext->makeCurrent();

	//::glfwSwapInterval( 1 );

	return true;
}

void RendererImplGlEmscripten::kill()
{
}

void RendererImplGlEmscripten::defaultResize() const
{
	int width = 0;
	int height = 0;
	glfwGetFramebufferSize( mContext, &width, &height );

	gl::viewport( 0, 0, width, height );
	gl::setMatricesWindow( width, height );
}

void RendererImplGlEmscripten::swapBuffers() const
{
	::glfwSwapBuffers( mContext );
}

void RendererImplGlEmscripten::makeCurrentContext( bool force )
{
	mCinderContext->makeCurrent( force );
}

}} // namespace cinder::app
