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

#include <bcm_host.h>
#include <EGL/egl.h>

namespace cinder { namespace app {

void checkGlStatus();

RendererGlLinux::RendererGlLinux( RendererGl *aRenderer )
	: mRenderer( aRenderer )
{
}

RendererGlLinux::~RendererGlLinux()
{
}


bool RendererGlLinux::initialize( void *window, RendererRef sharedRenderer )
{
	mDisplay = eglGetDisplay( EGL_DEFAULT_DISPLAY );

	if( mDisplay == EGL_NO_DISPLAY) {
		CI_LOG_E( "Failed to get EGL display from device!" );
		return false;
	}

	std::vector<EGLint> configAttribs;
	// OpenGL ES 3 also uses EGL_OPENGL_ES2_BIT
	configAttribs.push_back( EGL_SURFACE_TYPE	 ); configAttribs.push_back( EGL_WINDOW_BIT );
	configAttribs.push_back( EGL_RENDERABLE_TYPE ); configAttribs.push_back( EGL_OPENGL_ES2_BIT );
	configAttribs.push_back( EGL_RED_SIZE		 ); configAttribs.push_back( 8 );
	configAttribs.push_back( EGL_GREEN_SIZE		 ); configAttribs.push_back( 8 );
	configAttribs.push_back( EGL_BLUE_SIZE		 ); configAttribs.push_back( 8 );
	configAttribs.push_back( EGL_ALPHA_SIZE		 ); configAttribs.push_back( 8 );
	configAttribs.push_back( EGL_DEPTH_SIZE		 ); configAttribs.push_back( mRenderer->getOptions().getDepthBufferDepth() );
	configAttribs.push_back( EGL_STENCIL_SIZE	 ); configAttribs.push_back( mRenderer->getOptions().getStencil() ? 8 : EGL_DONT_CARE );
	configAttribs.push_back( EGL_SAMPLE_BUFFERS  ); configAttribs.push_back( 1 );
	configAttribs.push_back( EGL_NONE			 );


	EGLint majorVersion, minorVersion;
	if( ! eglInitialize( mDisplay, &majorVersion, &minorVersion ) ) {
		CI_LOG_E( "Failed to initialize EGL!" );
		return false;
	}

	eglBindAPI( EGL_OPENGL_ES_API );
	if( eglGetError() != EGL_SUCCESS ) {
		CI_LOG_E( "Failed to bind GL API!" );
		return false;
	}

	EGLint contextAttibutes[] = {
#if defined( CINDER_GL_ES_3 )
		EGL_CONTEXT_CLIENT_VERSION, 3,
#else
		EGL_CONTEXT_CLIENT_VERSION, 2,
#endif
		EGL_NONE
	};

	EGLint configCount;
	if( ! eglChooseConfig( mDisplay, configAttribs.data(), &mConfig, 1, &configCount ) || ( configCount != 1 ) ) {
		CI_LOG_E( "Failed to choose appropriate EGL config!" );
		return false;
	}

	// Create window surface
	EGL_DISPMANX_WINDOW_T* nativeWindow = reinterpret_cast<EGL_DISPMANX_WINDOW_T*>( window );

	DISPMANX_UPDATE_HANDLE_T	update	= vc_dispmanx_update_start( 0 );
	DISPMANX_DISPLAY_HANDLE_T	display = vc_dispmanx_display_open( 0 );
	int32_t						layer = 0;
	VC_RECT_T					dst_rect = { 0, 0, nativeWindow->width, nativeWindow->height };
	DISPMANX_RESOURCE_HANDLE_T	src = 0;
	VC_RECT_T					src_rect = { 0, 0, nativeWindow->width << 16, nativeWindow->height << 16 };
	DISPMANX_PROTECTION_T		protection = DISPMANX_PROTECTION_NONE;
	VC_DISPMANX_ALPHA_T			alpha = { DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS, 255, 0 };
	DISPMANX_CLAMP_T*			clamp = 0;
	DISPMANX_TRANSFORM_T		transform = (DISPMANX_TRANSFORM_T)0;

	nativeWindow->element = vc_dispmanx_element_add(
			update,
			display,
			layer,
			&dst_rect,
			src,
			&src_rect,
			protection,
			&alpha,
			clamp,
			transform
	);

	vc_dispmanx_update_submit_sync( update );

	mSurface = eglCreateWindowSurface( mDisplay, mConfig, nativeWindow, nullptr );
	auto err = eglGetError();
	if( err != EGL_SUCCESS ) {
		CI_LOG_E( "Failed to create EGL surface for rendering!" );
		return false;
	}

	// Create context
	mContext = eglCreateContext( mDisplay, mConfig, EGL_NO_CONTEXT, contextAttibutes );
	if( eglGetError() != EGL_SUCCESS ) {
		CI_LOG_E( "Failed to create EGL context!" );
		return false;
	}
	checkGlStatus();

	eglMakeCurrent( mDisplay, mSurface, mSurface, mContext );
	if( eglGetError() != EGL_SUCCESS ) {
		CI_LOG_E( "Failed to make current EGL context!" );
		return false;
	}
	checkGlStatus();

	gl::Environment::setEs();
	gl::env()->initializeFunctionPointers();
	checkGlStatus();

	std::shared_ptr<gl::PlatformDataLinux> platformData( new gl::PlatformDataLinux( mContext, mDisplay, mSurface, mConfig ) );
	platformData->mObjectTracking = mRenderer->getOptions().getObjectTracking();

	mCinderContext = gl::Context::createFromExisting( platformData );
	checkGlStatus();

	mCinderContext->makeCurrent();
	checkGlStatus();

	eglSwapInterval( mDisplay, 1 );
	checkGlStatus();

	return true;
}

void RendererGlLinux::kill()
{
	eglTerminate( mDisplay );
	eglMakeCurrent( mDisplay, mSurface, mSurface, nullptr );
	mCinderContext.reset();
	gl::Context::reflectCurrent( nullptr );
}

void RendererGlLinux::defaultResize() const
{
	EGLint width;
	EGLint height;
	eglQuerySurface( mDisplay, mSurface, EGL_WIDTH, &width );
	eglQuerySurface( mDisplay, mSurface, EGL_HEIGHT, &height );

	gl::viewport( 0, 0, width, height );
	gl::setMatricesWindow( width, height );
}

void RendererGlLinux::swapBuffers() const
{
	auto status = ::eglMakeCurrent( mDisplay, mSurface, mSurface, mContext );
	assert( status );
	EGLBoolean result = ::eglSwapBuffers( mDisplay, mSurface );
}

void RendererGlLinux::makeCurrentContext( bool force )
{
	mCinderContext->makeCurrent( force );
}

void checkGlStatus()
{
#if defined( DEBUG_GL )
	EGLint lastEglError = ci::gl::getEglError();
	if( lastEglError != EGL_SUCCESS ) {
		CI_LOG_E( "EGL ERROR: " << ci::gl::getEglErrorString( lastEglError ) );
	}

	GLenum lastGlError = ci::gl::getError();
	if( lastGlError != GL_NO_ERROR ) {
		CI_LOG_E( "GL ERROR: " << ci::gl::getErrorString( lastGlError ) );
	}
#endif // DEBUG_GL
}

}} // namespace cinder::app
