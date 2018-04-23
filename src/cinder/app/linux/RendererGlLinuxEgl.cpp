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

#if ! defined( CINDER_HEADLESS_GL_EGL )
#include <bcm_host.h>
#include <EGL/egl.h>
#else
#include <EGL/egl.h>
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>
#endif

namespace cinder { namespace app {

void checkGlStatus();

RendererGlLinux::RendererGlLinux( RendererGl *aRenderer )
	: mRenderer( aRenderer )
{
}

RendererGlLinux::~RendererGlLinux()
{
}


#if ! defined( CINDER_HEADLESS_GL_EGL )
bool RendererGlLinux::initialize( void *window, RendererRef sharedRenderer )
#else
bool RendererGlLinux::initialize( ci::ivec2 renderSize, RendererRef sharedRenderer )
#endif
{
#if ! defined( CINDER_HEADLESS_GL_EGL )
	mDisplay = eglGetDisplay( EGL_DEFAULT_DISPLAY );
#else
	const char* s = eglQueryString( EGL_NO_DISPLAY, EGL_EXTENSIONS );
	std::string egl_extensions( s );
	CI_LOG_I( "Supported EGL extensions : " << egl_extensions );

	auto hasExtension = [ & ] ( const std::string extension ) -> bool {
		size_t pos = egl_extensions.find( extension );
		// Check to see if we have exact extension match.
		if( pos != std::string::npos ) {
			if( ( ( pos == 0 ) || ! std::isalpha( egl_extensions[ pos - 1 ] ) )
					&& ! std::isalpha( egl_extensions[ pos + extension.size() ] ) )
				return true;
		}
		return false;
	};

	if( hasExtension( "EGL_EXT_device_base" )
			&& hasExtension( "EGL_EXT_platform_device" )
			&& hasExtension( "EGL_EXT_platform_base" ) ) {

		int numDevices = 0;
		EGLDeviceEXT* devices = nullptr;

		PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT = (PFNEGLQUERYDEVICESEXTPROC)eglGetProcAddress( "eglQueryDevicesEXT" );
		PFNEGLQUERYDEVICESTRINGEXTPROC eglQueryDeviceStringEXT = (PFNEGLQUERYDEVICESTRINGEXTPROC)eglGetProcAddress( "eglQueryDeviceStringEXT" );
		PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress( "eglGetPlatformDisplayEXT" );

		// Check that we have at least one device.
		if( ! eglQueryDevicesEXT || ! eglQueryDevicesEXT( 0, nullptr, &numDevices ) || numDevices < 1 )
			return false;
		if( ( devices = (EGLDeviceEXT*)malloc( sizeof( EGLDeviceEXT ) * numDevices ) ) == nullptr )
			return false;
		if( ! eglQueryDevicesEXT( numDevices, devices, &numDevices ) || numDevices < 1 )
			return false;

		// Get a device as display.
		mDisplay = eglGetPlatformDisplayEXT( EGL_PLATFORM_DEVICE_EXT, devices[0], nullptr );
	}
	else {
		CI_LOG_E( "No available EGL Device extension support!" );
		return false;
	}
#endif // CINDER_HEADLESS_GL_EGL
	if( mDisplay == EGL_NO_DISPLAY) {
		CI_LOG_E( "Failed to get EGL display from device!" );
		return false;
	}

	std::vector<EGLint> configAttribs;
#if ! defined( CINDER_HEADLESS_GL_EGL )
	// OpenGL ES 3 also uses EGL_OPENGL_ES2_BIT
	configAttribs.push_back( EGL_SURFACE_TYPE	 ); configAttribs.push_back( EGL_WINDOW_BIT );
	configAttribs.push_back( EGL_RENDERABLE_TYPE ); configAttribs.push_back( EGL_OPENGL_ES2_BIT );
#else
	configAttribs.push_back( EGL_RENDERABLE_TYPE ); configAttribs.push_back( EGL_OPENGL_BIT );
	configAttribs.push_back( EGL_SURFACE_TYPE	 ); configAttribs.push_back( EGL_PBUFFER_BIT );
#endif
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

#if ! defined( CINDER_HEADLESS_GL_EGL ) || defined( CINDER_GL_ES )
	eglBindAPI( EGL_OPENGL_ES_API );
#else
	eglBindAPI( EGL_OPENGL_API );
#endif
	if( eglGetError() != EGL_SUCCESS ) {
		CI_LOG_E( "Failed to bind GL API!" );
		return false;
	}

	EGLint contextAttibutes[] = {
#if ! defined( CINDER_GL_ES )
		EGL_CONTEXT_MAJOR_VERSION, mRenderer->getOptions().getVersion().first,
		EGL_CONTEXT_MINOR_VERSION, mRenderer->getOptions().getVersion().second,
		EGL_CONTEXT_OPENGL_PROFILE_MASK,
		EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
#elif defined( CINDER_GL_ES_3 )
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

#if ! defined( CINDER_HEADLESS_GL_EGL )
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
#else
	// Create Pixel Buffer surface for offscreen rendering
	int pbattribs[] = { EGL_WIDTH, renderSize.x, EGL_HEIGHT, renderSize.y, EGL_NONE };
	mSurface = eglCreatePbufferSurface( mDisplay, mConfig, pbattribs );
#endif // CINDER_HEADLESS_GL_EGL
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

#if ! defined( CINDER_GL_ES )
	gl::Environment::setCore();
#else
	gl::Environment::setEs();
#endif
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

	CI_LOG_I( "Renderer: " << glGetString( GL_RENDERER ) );
	CI_LOG_I( "Vendor: " << glGetString( GL_VENDOR ) );
	CI_LOG_I( "OpenGL Version: " << glGetString( GL_VERSION ) );

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
