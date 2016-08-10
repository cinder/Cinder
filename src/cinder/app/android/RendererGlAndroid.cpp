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

#include "cinder/app/android/RendererGlAndroid.h"
#include "cinder/app/android/AppAndroid.h"
#include "cinder/app/RendererGl.h"
#include "cinder/app/AppBase.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Environment.h"

#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "cinder", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "cinder", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"cinder", __VA_ARGS__))

namespace cinder { namespace app {

void checkGlStatus();

ivec2 RendererGlAndroid::sSurfaceSize = ivec2( 0, 0 );

RendererGlAndroid::RendererGlAndroid( RendererGl *aRenderer )
    : mRenderer( aRenderer )
{
}

RendererGlAndroid::~RendererGlAndroid()
{
}

bool RendererGlAndroid::initialize( ANativeWindow *nativeWindow, RendererRef sharedRenderer )
{
	std::vector<EGLint> configAttribs;

	// OpenGL ES 3 also uses EGL_OPENGL_ES2_BIT
	configAttribs.push_back( EGL_RENDERABLE_TYPE ); configAttribs.push_back( EGL_OPENGL_ES2_BIT );
	configAttribs.push_back( EGL_SURFACE_TYPE    ); configAttribs.push_back( EGL_WINDOW_BIT );
	configAttribs.push_back( EGL_RED_SIZE        ); configAttribs.push_back( 8 );
	configAttribs.push_back( EGL_GREEN_SIZE      ); configAttribs.push_back( 8 );
	configAttribs.push_back( EGL_BLUE_SIZE       ); configAttribs.push_back( 8 );
	configAttribs.push_back( EGL_ALPHA_SIZE      ); configAttribs.push_back( EGL_DONT_CARE );
	configAttribs.push_back( EGL_DEPTH_SIZE      ); configAttribs.push_back( mRenderer->getOptions().getDepthBufferDepth() );
	configAttribs.push_back( EGL_STENCIL_SIZE    ); configAttribs.push_back( mRenderer->getOptions().getStencil() ? 8 : EGL_DONT_CARE );
	configAttribs.push_back( EGL_SAMPLE_BUFFERS  ); configAttribs.push_back( 1 );
	configAttribs.push_back( EGL_NONE            );

	EGLint surfaceAttribList[] =
	{
		EGL_NONE, EGL_NONE
	};

	mDisplay = eglGetDisplay( EGL_DEFAULT_DISPLAY );
	if( mDisplay == EGL_NO_DISPLAY) {
		return false;
	}

	EGLint majorVersion, minorVersion;
	if( ! eglInitialize( mDisplay, &majorVersion, &minorVersion ) ) {
		return false;
	}

	eglBindAPI( EGL_OPENGL_ES_API );
	if( eglGetError() != EGL_SUCCESS ) {
		return false;
	}

	EGLint configCount;
	if( ! eglChooseConfig( mDisplay, configAttribs.data(), &mConfig, 1, &configCount ) || (configCount != 1) ) {
		return false;
	}

	//
	// EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	// guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	// As soon as we picked a EGLConfig, we can safely reconfigure the
	// ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
	//
	EGLint format;
	eglGetConfigAttrib( mDisplay, mConfig, EGL_NATIVE_VISUAL_ID, &format );
	ANativeWindow_setBuffersGeometry( nativeWindow, 0, 0, format );	

	mSurface = eglCreateWindowSurface( mDisplay, mConfig, nativeWindow, NULL );

	auto err = eglGetError();
	if( err != EGL_SUCCESS ) {
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

	mContext = eglCreateContext( mDisplay, mConfig, NULL, contextAttibutes );
	if( eglGetError() != EGL_SUCCESS ) {
		return false;
	}
	checkGlStatus();

	eglMakeCurrent( mDisplay, mSurface, mSurface, mContext );
	if( eglGetError() != EGL_SUCCESS ) {
		return false;
	}
	checkGlStatus();

	gl::Environment::setEs();
	gl::env()->initializeFunctionPointers();
	checkGlStatus();

	std::shared_ptr<gl::PlatformDataAndroid> platformData( new gl::PlatformDataAndroid( mContext, mDisplay, mSurface, mConfig ) );
	platformData->mObjectTracking = mRenderer->getOptions().getObjectTracking();

	mCinderContext = gl::Context::createFromExisting( platformData );
	checkGlStatus();

	// Get device screen size
	{
		EGLint width;
		EGLint height;
		eglQuerySurface( mDisplay, mSurface, EGL_WIDTH, &width );
		eglQuerySurface( mDisplay, mSurface, EGL_HEIGHT, &height );
		RendererGlAndroid::sSurfaceSize = ivec2( width, height );
	}

	mCinderContext->makeCurrent();
	checkGlStatus();

	// NOTE: 'interval' value of 0 causes the Samsung S6 to not render correctly on startup.
	//
	eglSwapInterval( mDisplay, 1 );
	checkGlStatus();

	return true;
}

void RendererGlAndroid::kill()
{
}

void RendererGlAndroid::defaultResize() const
{
	EGLint width;
	EGLint height;
	eglQuerySurface( mDisplay, mSurface, EGL_WIDTH, &width );
	eglQuerySurface( mDisplay, mSurface, EGL_HEIGHT, &height );

	gl::viewport( 0, 0, width, height );
	gl::setMatricesWindow( width, height );
}

void RendererGlAndroid::swapBuffers() const
{
	auto status = ::eglMakeCurrent( mDisplay, mSurface, mSurface, mContext );
	assert( status );
	EGLBoolean result = ::eglSwapBuffers( mDisplay, mSurface );
	// @TODO: Is this really necessary?
	//assert( result );
}

void RendererGlAndroid::makeCurrentContext( bool force )
{
	mCinderContext->makeCurrent( force );
}

void RendererGlAndroid::reinitializeWindowSurface( ANativeWindow *nativeWindow )
{
	mSurface = eglCreateWindowSurface( mDisplay, mConfig, nativeWindow, NULL );
	makeCurrentContext();
}

EGLint getEglError()
{
	return eglGetError();
}

std::string getEglErrorString( EGLint err )
{
	switch( err ) {
		case EGL_SUCCESS: return "EGL_SUCCESS";
		case EGL_NOT_INITIALIZED: return "EGL_NOT_INITIALIZED";
		case EGL_BAD_ACCESS: return "EGL_BAD_ACCESS";
		case EGL_BAD_ALLOC: return "EGL_BAD_ALLOC";
		case EGL_BAD_ATTRIBUTE: return "EGL_BAD_ATTRIBUTE";
		case EGL_BAD_CONTEXT: return "EGL_BAD_CONTEXT";
		case EGL_BAD_CONFIG: return "EGL_BAD_CONFIG";
		case EGL_BAD_CURRENT_SURFACE: return "EGL_BAD_CURRENT_SURFACE";
		case EGL_BAD_DISPLAY: return "EGL_BAD_DISPLAY";
		case EGL_BAD_SURFACE: return "EGL_BAD_SURFACE";
		case EGL_BAD_MATCH: return "EGL_BAD_MATCH";
		case EGL_BAD_PARAMETER: return "EGL_BAD_PARAMETER";
		case EGL_BAD_NATIVE_PIXMAP: return "EGL_BAD_NATIVE_PIXMAP";
		case EGL_BAD_NATIVE_WINDOW: return "EGL_BAD_NATIVE_WINDOW";
		case EGL_CONTEXT_LOST: return "EGL_CONTEXT_LOST";
		default: return "unknown error code";
	}
}

void checkGlStatus()
{
#if DEBUG_GL
	EGLint lastEglError = getEglError();
	if( lastEglError != EGL_SUCCESS ) {
		ci::app::console() << "EGL ERROR: " << getEglErrorString( lastEglError ) << std::endl;
		CI_BREAK();
	}

	GLenum lastGlError = ci::gl::getError();
	if( lastGlError != GL_NO_ERROR ) {
		ci::app::console() << "GL ERROR: " << ci::gl::getErrorString( lastGlError ) << std::endl;
	}
#endif // DEBUG_GL
}

} } // namespace cinder::app