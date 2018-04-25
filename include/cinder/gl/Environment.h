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

#pragma once

#include "cinder/gl/platform.h"
#include "cinder/gl/Context.h"

#if defined( CINDER_MAC )
	typedef struct _CGLContextObject       *CGLContextObj;
#elif defined( CINDER_COCOA_TOUCH )
	#if defined( __OBJC__ )
		@class	EAGLContext;
	#else
		class	EAGLContext;
	#endif
#elif defined( CINDER_GL_ANGLE )
	typedef void*		EGLContext;
	typedef void*		EGLDisplay;
	typedef void*		EGLSurface;
	typedef void*		EGLConfig;
#elif defined( CINDER_MSW )
	#include "cinder/msw/CinderWindowsFwd.h"
	struct HGLRC__;
	typedef HGLRC__* HGLRC;
#elif defined( CINDER_ANDROID )
	typedef void*		EGLContext;
	typedef void*		EGLDisplay;
	typedef void*		EGLSurface;
	typedef void*		EGLConfig;
#elif defined( CINDER_LINUX )
	#if defined( CINDER_LINUX_EGL_ONLY )
		typedef void*		EGLContext;
		typedef void*		EGLDisplay;
		typedef void*		EGLSurface;
		typedef void*		EGLConfig;	
	#elif defined( CINDER_HEADLESS_GL_OSMESA )
		typedef struct osmesa_context	*OSMesaContext;
	#else
		typedef struct GLFWwindow 	GLFWwindow;
	#endif
#endif

namespace cinder { namespace gl {

class ShaderDef;
class GlslProg;
typedef std::shared_ptr<GlslProg>		GlslProgRef;
class Context;
typedef std::shared_ptr<Context>		ContextRef;

class CI_API Environment {
  public:
	virtual void			initializeFunctionPointers() = 0;
	
	ContextRef				createSharedContext( const Context *sharedContext );
	//! NULL for \a context deactivates the current context
	void					makeContextCurrent( const Context *context );

	virtual bool			isExtensionAvailable( const std::string &extName ) const = 0;

	virtual bool 			supportsFboMultiSample() const = 0;
	virtual bool 			supportsCoverageSample() const = 0;
	virtual bool			supportsHardwareVao() const = 0;
	virtual bool 			supportsInstancedArrays() const = 0;
	virtual bool			supportsTextureLod() const = 0;
	virtual bool 			supportsGeometryShader() const = 0;
	virtual bool 			supportsTessellationShader() const = 0;

	virtual GLenum			getPreferredIndexType() const  = 0;

	virtual void			allocateTexStorage1d( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, bool immutable, GLint texImageDataType ) = 0;
	virtual void			allocateTexStorage2d( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, bool immutable, GLint texImageDataType ) = 0;
	virtual void			allocateTexStorage3d( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, bool immutable ) = 0;
	virtual void			allocateTexStorageCubeMap( GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, bool immutable ) = 0;	

	virtual void			objectLabel( GLenum identifier, GLuint name, GLsizei length, const char *label ) = 0;

	virtual std::string		generateVertexShader( const ShaderDef &shader ) = 0;
	virtual std::string		generateFragmentShader( const ShaderDef &shader ) = 0;
	virtual GlslProgRef		buildShader( const ShaderDef &shader ) = 0;

#if ! defined( CINDER_GL_ES )	
	static void				setCore();
#else
	static void				setEs();
#endif
};


#if defined( CINDER_COCOA_TOUCH )
struct PlatformDataIos : public Context::PlatformData {
	PlatformDataIos( EAGLContext *eaglContext )
		: mEaglContext( eaglContext )
	{}
	
	EAGLContext		*mEaglContext;
};

#elif defined( CINDER_MAC )
struct PlatformDataMac : public Context::PlatformData {
	PlatformDataMac( CGLContextObj cglContext )
		: mCglContext( cglContext )
	{}
	
	CGLContextObj		mCglContext;
};

#elif defined( CINDER_GL_ANGLE )
struct PlatformDataAngle : public Context::PlatformData {
	PlatformDataAngle( EGLContext context, EGLDisplay display, EGLSurface surface, EGLConfig eglConfig )
		: mContext( context ), mDisplay( display ), mSurface( surface ), mConfig( eglConfig )
	{}

	EGLContext		mContext;
	EGLDisplay		mDisplay;
	EGLSurface		mSurface;
	EGLConfig		mConfig;
};

#elif defined( CINDER_MSW ) // normal MSW desktop GL
struct CI_API PlatformDataMsw : public Context::PlatformData {
	PlatformDataMsw( HGLRC glrc, HDC dc )
		: mGlrc( glrc ), mDc( dc )
	{}
	PlatformDataMsw( const std::shared_ptr<Context::PlatformData> &sharedPlatformData, HGLRC glrc, HDC dc )
		: Context::PlatformData( *sharedPlatformData ), mGlrc( glrc ), mDc( dc )
	{}

	HGLRC	mGlrc;
	HDC		mDc;
};
#elif defined( CINDER_ANDROID )
struct PlatformDataAndroid : public Context::PlatformData {
	PlatformDataAndroid( EGLContext context, EGLDisplay display, EGLSurface surface, EGLConfig eglConfig )
		: mContext( context ), mDisplay( display ), mSurface( surface ), mConfig( eglConfig )
	{}

	EGLContext		mContext;
	EGLDisplay		mDisplay;
	EGLSurface		mSurface;
	EGLConfig		mConfig;
};
#elif defined( CINDER_LINUX )
  #if defined( CINDER_LINUX_EGL_ONLY )
	struct PlatformDataLinux : public Context::PlatformData {
		PlatformDataLinux( EGLContext context, EGLDisplay display, EGLSurface surface, EGLConfig eglConfig )
			: mContext( context ), mDisplay( display ), mSurface( surface ), mConfig( eglConfig )
		{}

		EGLContext		mContext;
		EGLDisplay		mDisplay;
		EGLSurface		mSurface;
		EGLConfig		mConfig;
	};
	#elif defined( CINDER_HEADLESS_GL_OSMESA )
		struct PlatformDataLinux : public Context::PlatformData {
			PlatformDataLinux( OSMesaContext context, const std::vector<unsigned char>& buffer, int bufferWidth, int bufferHeight )
				: mContext( context ), mBuffer( buffer ), mBufferWidth( bufferWidth ), mBufferHeight( bufferHeight )
			{}
			OSMesaContext			mContext;
			std::vector<unsigned char>	mBuffer;
			int				mBufferWidth{ 0 };
			int				mBufferHeight{ 0 };
		};
  #else
	struct PlatformDataLinux : public Context::PlatformData {
		PlatformDataLinux( GLFWwindow *context )
			: mContext( context )
		{}

		GLFWwindow 		*mContext = nullptr;
	};
  #endif
#endif

} } // namespace cinder::gl
