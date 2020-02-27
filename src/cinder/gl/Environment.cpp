/*
 Copyright (c) 2014, The Cinder Project
 All rights reserved.

 This code is designed for use with the Cinder C++ library, http://libcinder.org

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

#include "cinder/gl/Environment.h"
#include "cinder/gl/Context.h"
#include "cinder/Utilities.h"

#if defined( CINDER_COCOA ) && ( ! defined( __OBJC__ ) )
	#error "This file must be compiled as Objective-C++ on the Mac"
#endif

#if defined( CINDER_MAC )
	#include <OpenGL/OpenGL.h>
#elif defined( CINDER_COCOA_TOUCH )
	#import <OpenGLES/EAGL.h>
#elif defined( CINDER_GL_ANGLE )
	#include "EGL/egl.h"
#elif defined( CINDER_MSW )
	#include <windows.h>
	#include "glad/glad_wgl.h"
#elif defined( CINDER_ANDROID )
	#include "EGL/egl.h"
#elif defined( CINDER_LINUX )
	#if defined( CINDER_LINUX_EGL_ONLY )
		#include "EGL/egl.h"
	#elif defined( CINDER_HEADLESS_GL_OSMESA )
		#if ! defined( GLAPIENTRY )
			#define GLAPIENTRY
		#endif
		#include "GL/osmesa.h"
	#else
		#include "glad/glad.h"
		#include "glfw/glfw3.h"
	#endif
#endif

#include "cinder/Log.h"

using namespace std;

namespace cinder { namespace gl {

extern Environment* allocateEnvironmentCore();
extern Environment* allocateEnvironmentEs();
static Environment *sEnvironment = NULL;

#if ! defined( CINDER_GL_ES )
void Environment::setCore()
{
	if( ! sEnvironment ) {
		sEnvironment = allocateEnvironmentCore();
	}
}

#else

void Environment::setEs()
{
	if( ! sEnvironment ) {
		sEnvironment = allocateEnvironmentEs();
	}
}

#endif

Environment* env()
{
	assert( sEnvironment );
	return sEnvironment;
}

namespace {
void destroyPlatformData( Context::PlatformData *data )
{
#if defined( CINDER_MAC )
	auto platformData = dynamic_cast<PlatformDataMac*>( data );
	::CGLDestroyContext( platformData->mCglContext );
#elif defined( CINDER_COCOA_TOUCH )
	auto platformData = dynamic_cast<PlatformDataIos*>( data );
	[(EAGLContext*)platformData->mEaglContext release];
#elif defined( CINDER_GL_ANGLE )
	auto platformData = dynamic_cast<PlatformDataAngle*>( data );
	::eglDestroyContext( platformData->mDisplay, platformData->mContext );
#elif defined( CINDER_MSW )
	auto platformData = dynamic_cast<PlatformDataMsw*>( data );
	::wglMakeCurrent( NULL, NULL );
	::wglDeleteContext( platformData->mGlrc );
#elif defined( CINDER_ANDROID )
	auto platformData = dynamic_cast<PlatformDataAndroid*>( data );
#elif defined( CINDER_LINUX )
	auto platformData = dynamic_cast<PlatformDataLinux*>( data );
	#if defined( CINDER_LINUX_EGL_ONLY )
		::eglDestroyContext( platformData->mDisplay, platformData->mContext );
	#elif defined( CINDER_HEADLESS_GL_OSMESA )
		OSMesaDestroyContext( platformData->mContext );
	#else
		::glfwDestroyWindow( platformData->mContext );
	#endif
#endif

	delete data;
}
} // anonymous namespace

ContextRef Environment::createSharedContext( const Context *sharedContext )
{
#if defined( CINDER_MAC )
	auto sharedContextPlatformData = dynamic_pointer_cast<PlatformDataMac>( sharedContext->getPlatformData() );
	CGLContextObj prevContext = ::CGLGetCurrentContext();
	CGLContextObj sharedContextCgl = sharedContextPlatformData->mCglContext;
	CGLPixelFormatObj sharedContextPixelFormat = ::CGLGetPixelFormat( sharedContextCgl );
	CGLContextObj cglContext;
	if( ::CGLCreateContext( sharedContextPixelFormat, sharedContextCgl, (CGLContextObj*)&cglContext ) != kCGLNoError ) {
		throw ExcContextAllocation();
	}

	::CGLSetCurrentContext( cglContext );
	shared_ptr<Context::PlatformData> platformData = shared_ptr<Context::PlatformData>( new PlatformDataMac( cglContext ), destroyPlatformData );
#elif defined( CINDER_COCOA_TOUCH )
	auto sharedContextPlatformData = dynamic_pointer_cast<PlatformDataIos>( sharedContext->getPlatformData() );
	EAGLContext *prevContext = [EAGLContext currentContext];
	EAGLContext *sharedContextEagl = sharedContextPlatformData->mEaglContext;
	EAGLSharegroup *sharegroup = sharedContextEagl.sharegroup;
	EAGLContext *eaglContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup:sharegroup];
	[EAGLContext setCurrentContext:eaglContext];
	shared_ptr<Context::PlatformData> platformData = shared_ptr<Context::PlatformData>( new PlatformDataIos( eaglContext ), destroyPlatformData );
#elif defined( CINDER_GL_ANGLE )
	auto sharedContextPlatformData = dynamic_pointer_cast<PlatformDataAngle>( sharedContext->getPlatformData() );
	EGLContext prevEglContext = ::eglGetCurrentContext();
	EGLDisplay prevEglDisplay = ::eglGetCurrentDisplay();
	EGLSurface prevEglSurface = ::eglGetCurrentSurface( EGL_DRAW );

	EGLint surfaceAttribList[] = { EGL_NONE, EGL_NONE };
	EGLContext eglContext = ::eglCreateContext( prevEglDisplay, sharedContextPlatformData->mConfig, prevEglContext, surfaceAttribList );

	shared_ptr<Context::PlatformData> platformData( new PlatformDataAngle( eglContext, sharedContextPlatformData->mDisplay, sharedContextPlatformData->mSurface, sharedContextPlatformData->mConfig ), destroyPlatformData );
#elif defined( CINDER_MSW )
	// save the current context so we can restore it
	HGLRC prevContext = ::wglGetCurrentContext();
	HDC prevDc = ::wglGetCurrentDC();
	auto sharedContextPlatformData = dynamic_pointer_cast<PlatformDataMsw>( sharedContext->getPlatformData() );
	HGLRC sharedContextRc = sharedContextPlatformData->mGlrc;
	HDC sharedContextDc = sharedContextPlatformData->mDc;
	::wglMakeCurrent( NULL, NULL );
	int attribList[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, sharedContextPlatformData->mVersion.first,
		WGL_CONTEXT_MINOR_VERSION_ARB, sharedContextPlatformData->mVersion.second,
		WGL_CONTEXT_FLAGS_ARB, sharedContextPlatformData->mDebug ? WGL_CONTEXT_DEBUG_BIT_ARB : 0,
		WGL_CONTEXT_PROFILE_MASK_ARB, ( sharedContextPlatformData->mCoreProfile ) ? WGL_CONTEXT_CORE_PROFILE_BIT_ARB : WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0, 0,
		0
	};
	if( sharedContextPlatformData->mMultiGpuEnabledNV ) {
		attribList[8] = WGL_CONTEXT_MULTIGPU_ATTRIB_NV;
		attribList[9] = sharedContextPlatformData->mMultiGpuModeNV;
	}
	HGLRC rc = wglCreateContextAttribsARB( sharedContextDc, sharedContextRc, attribList );
	::wglMakeCurrent( sharedContextDc, rc );
	shared_ptr<Context::PlatformData> platformData = shared_ptr<Context::PlatformData>( new PlatformDataMsw( sharedContextPlatformData, rc, sharedContextDc ), destroyPlatformData );
#elif defined( CINDER_ANDROID )
	auto sharedContextPlatformData = dynamic_pointer_cast<PlatformDataAndroid>( sharedContext->getPlatformData() );
	EGLContext prevEglContext = ::eglGetCurrentContext();
	EGLDisplay prevEglDisplay = ::eglGetCurrentDisplay();
	EGLSurface prevEglSurface = ::eglGetCurrentSurface( EGL_DRAW );

	EGLint surfaceAttribList[] = { EGL_NONE, EGL_NONE };
	EGLContext eglContext = ::eglCreateContext( prevEglDisplay, sharedContextPlatformData->mConfig, prevEglContext, surfaceAttribList );

	shared_ptr<Context::PlatformData> platformData( new PlatformDataAndroid( eglContext, sharedContextPlatformData->mDisplay, sharedContextPlatformData->mSurface, sharedContextPlatformData->mConfig ), destroyPlatformData );
#elif defined( CINDER_LINUX )
	#if defined( CINDER_LINUX_EGL_ONLY )
		auto sharedContextPlatformData = dynamic_pointer_cast<PlatformDataLinux>( sharedContext->getPlatformData() );
		EGLContext prevEglContext = ::eglGetCurrentContext();
		EGLDisplay prevEglDisplay = ::eglGetCurrentDisplay();
		EGLint offscreenSurfaceAttribList[] = { EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE, EGL_NONE };
		EGLSurface offScreenSurface = ::eglCreatePbufferSurface( prevEglDisplay, sharedContextPlatformData->mConfig, offscreenSurfaceAttribList );

		EGLint surfaceAttribList[] = { EGL_NONE, EGL_NONE };
		EGLContext offscreenContext = ::eglCreateContext( prevEglDisplay, sharedContextPlatformData->mConfig, prevEglContext, surfaceAttribList );

		shared_ptr<Context::PlatformData> platformData( new PlatformDataLinux( offscreenContext, sharedContextPlatformData->mDisplay, offScreenSurface, sharedContextPlatformData->mConfig ), destroyPlatformData );
	#elif defined( CINDER_HEADLESS_GL_OSMESA )
		auto sharedContextPlatformData = dynamic_pointer_cast<PlatformDataLinux>( sharedContext->getPlatformData() );
		const int ctxattribs[] = { OSMESA_FORMAT, OSMESA_RGBA, OSMESA_DEPTH_BITS, 32, OSMESA_STENCIL_BITS, 8, OSMESA_ACCUM_BITS, 16, OSMESA_PROFILE, OSMESA_CORE_PROFILE, 0 };
		OSMesaContext sharedCtx  = OSMesaCreateContextAttribs( ctxattribs, sharedContextPlatformData->mContext );
		shared_ptr<Context::PlatformData> platformData( new PlatformDataLinux( sharedContextPlatformData->mContext, sharedContextPlatformData->mBuffer, sharedContextPlatformData->mBufferWidth, sharedContextPlatformData->mBufferHeight ) );
	#else
		auto sharedContextPlatformData = dynamic_pointer_cast<PlatformDataLinux>( sharedContext->getPlatformData() );
		// Create a shared context GLFW style
		glfwWindowHint( GLFW_VISIBLE, GL_FALSE );
		GLFWwindow* sharedGlfwContext = ::glfwCreateWindow( 1, 1, "", NULL, sharedContextPlatformData->mContext );
		shared_ptr<Context::PlatformData> platformData( new PlatformDataLinux( sharedGlfwContext ), destroyPlatformData );
	#endif
#endif

	ContextRef result( new Context( platformData ) );
	env()->initializeFunctionPointers();

#if defined( CINDER_MAC )
	::CGLSetCurrentContext( prevContext );
#elif defined( CINDER_COCOA_TOUCH )
	[EAGLContext setCurrentContext:prevContext];
#elif defined( CINDER_GL_ANGLE )
	EGLBoolean status = ::eglMakeCurrent( prevEglDisplay, prevEglSurface, prevEglSurface, prevEglContext );
	assert( status );
#elif defined( CINDER_MSW )
	::wglMakeCurrent( prevDc, prevContext );
#endif

	return result;
}

void Environment::makeContextCurrent( const Context *context )
{
#if defined( CINDER_MAC )
	if( context ) {
		auto platformData = dynamic_pointer_cast<PlatformDataMac>( context->getPlatformData() );
		::CGLSetCurrentContext( platformData->mCglContext );
	}
	else {
		::CGLSetCurrentContext( NULL );
	}
#elif defined( CINDER_COCOA_TOUCH )
	if( context ) {
		auto platformData = dynamic_pointer_cast<PlatformDataIos>( context->getPlatformData() );
		[EAGLContext setCurrentContext:platformData->mEaglContext];
	}
	else {
		[EAGLContext setCurrentContext:nil];
	}
#elif defined( CINDER_GL_ANGLE )
	if( context ) {
		auto platformData = dynamic_pointer_cast<PlatformDataAngle>( context->getPlatformData() );
		EGLBoolean status = ::eglMakeCurrent( platformData->mDisplay, platformData->mSurface, platformData->mSurface, platformData->mContext );
		assert( status );
	}
	else {
		// currently not implemented
		// eglMakeCurrent( , EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
	}
#elif defined( CINDER_MSW )
	if( context ) {
		auto platformData = dynamic_pointer_cast<PlatformDataMsw>( context->getPlatformData() );
		int attemptCount = 10;
		// wglMakeCurrent() can fail (vewry infrequently) if it's called from two threads simultaneously (ostensibly due to driver bugs)
		// this reattempts 10 times before throwing
		while( ! ::wglMakeCurrent( platformData->mDc, platformData->mGlrc ) ) {
			DWORD error = GetLastError();
			if( attemptCount-- <= 0 )
				throw ExcContextMakeCurrent();
			ci::sleep( 1 );
		}
	}
	else {
		::wglMakeCurrent( NULL, NULL );
	}
#elif defined( CINDER_ANDROID )
	// NOTE: Does not work as advertised on Android. Disabling for now.
	//
	// if( context ) {
	//	auto platformData = dynamic_pointer_cast<PlatformDataAndroid>( context->getPlatformData() );
	//	EGLBoolean status = ::eglMakeCurrent( platformData->mDisplay, platformData->mSurface, platformData->mSurface, platformData->mContext );
	//	assert( status );
	// }
#elif defined( CINDER_LINUX )
	#if defined( CINDER_LINUX_EGL_ONLY )
		if( context ) {
			auto platformData = dynamic_pointer_cast<PlatformDataLinux>( context->getPlatformData() );
			EGLBoolean status = ::eglMakeCurrent( platformData->mDisplay, platformData->mSurface, platformData->mSurface, platformData->mContext );
			assert( status );
		} else {
			EGLBoolean status = eglMakeCurrent( ::eglGetCurrentDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
		}
	#elif defined( CINDER_HEADLESS_GL_OSMESA )
		if( context ) {
			auto platformData = dynamic_pointer_cast<PlatformDataLinux>( context->getPlatformData() );
			OSMesaMakeCurrent( platformData->mContext, platformData->mBuffer.data(), GL_UNSIGNED_BYTE, platformData->mBufferWidth, platformData->mBufferHeight );
		}
	#else
		if( context ) {
			auto platformData = dynamic_pointer_cast<PlatformDataLinux>( context->getPlatformData() );
			glfwMakeContextCurrent( platformData->mContext );
		}
		else {
			glfwMakeContextCurrent( nullptr );
		}
	#endif
#endif
}

} } // namespace cinder::gl
