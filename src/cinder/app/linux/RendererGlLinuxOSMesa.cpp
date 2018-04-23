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

#include "cinder/gl/platform.h"
#include "cinder/app/linux/RendererGlLinux.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Environment.h"
#include "cinder/Log.h"

#if ! defined( GLAPIENTRY )
	#define GLAPIENTRY
#endif
#include "GL/osmesa.h"

namespace cinder { namespace app {

void checkGlStatus();

RendererGlLinux::RendererGlLinux( RendererGl *aRenderer )
	: mRenderer( aRenderer )
{
}

RendererGlLinux::~RendererGlLinux()
{
}


bool RendererGlLinux::initialize( ci::ivec2 renderSize, RendererRef sharedRenderer )
{
	// Create context
	int ctxattribs[] = { OSMESA_FORMAT, OSMESA_RGBA, 
						 OSMESA_DEPTH_BITS, 32,
						 OSMESA_STENCIL_BITS, 8,
						 OSMESA_ACCUM_BITS, 16,
						 OSMESA_PROFILE, OSMESA_CORE_PROFILE,
						 0 };
	mContext = OSMesaCreateContextAttribs( ctxattribs, nullptr );
	if( ! mContext ) {
		CI_LOG_E( "Failed to create OSMESA context!" );
		checkGlStatus();
		return false;
	}

	mBufferWidth = renderSize.x;
	mBufferHeight = renderSize.y;
	mBuffer = malloc( renderSize.x * renderSize.y * 4 * sizeof( GL_UNSIGNED_BYTE ) );
	if( ! mBuffer ) {
		CI_LOG_E( "Failed to allocate draw buffer!" );
		return false;
	}

	if( ! OSMesaMakeCurrent( mContext, mBuffer, GL_UNSIGNED_BYTE, renderSize.x, renderSize.y ) ) {
		CI_LOG_E( "Failed to make current EGL context!" );
		checkGlStatus();
		return false;
	}
	CI_LOG_I( "Rendering with OSMesa" );

#if ! defined( CINDER_GL_ES )
	gl::Environment::setCore();
#else
	gl::Environment::setEs();
#endif
	gl::env()->initializeFunctionPointers();
	checkGlStatus();
	std::shared_ptr<gl::PlatformDataLinux> platformData( new gl::PlatformDataLinux( mContext, mBuffer, mBufferWidth, mBufferHeight ) );
	platformData->mObjectTracking = mRenderer->getOptions().getObjectTracking();

	mCinderContext = gl::Context::createFromExisting( platformData );
	checkGlStatus();

	mCinderContext->makeCurrent();
	checkGlStatus();

	CI_LOG_I( "Renderer: " << glGetString( GL_RENDERER ) );
	CI_LOG_I( "Vendor: " << glGetString( GL_VENDOR ) );
	CI_LOG_I( "OpenGL Version: " << glGetString( GL_VERSION ) );

	return true;
}

void RendererGlLinux::kill()
{
	OSMesaDestroyContext( mContext );
	mCinderContext.reset();
	gl::Context::reflectCurrent( nullptr );
}

void RendererGlLinux::defaultResize() const
{
}

void RendererGlLinux::swapBuffers() const
{
}

void RendererGlLinux::makeCurrentContext( bool force )
{
	mCinderContext->makeCurrent( force );
}

void checkGlStatus()
{
#if defined( DEBUG_GL )
	GLenum lastGlError = ci::gl::getError();
	if( lastGlError != GL_NO_ERROR ) {
		CI_LOG_E( "GL ERROR: " << ci::gl::getErrorString( lastGlError ) );
	}
#endif // DEBUG_GL
}

}} // namespace cinder::app
