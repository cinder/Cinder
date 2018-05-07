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

#pragma once

#include "cinder/app/AppBase.h"
#if defined( CINDER_LINUX_EGL_ONLY )
	typedef void *EGLConfig;
	typedef void *EGLContext;
	typedef void *EGLDisplay;
	typedef void *EGLSurface;
#elif defined( CINDER_HEADLESS_GL_OSMESA )
	typedef struct osmesa_context *OSMesaContext;
#else
	typedef struct GLFWwindow GLFWwindow;
#endif

namespace cinder { namespace gl {

class Context;
using ContextRef = std::shared_ptr<Context>;

}} // namespace cinder::gl

namespace cinder { namespace app {

class RendererGlLinux {
 public:

	RendererGlLinux( class RendererGl *aRenderer );
	virtual ~RendererGlLinux();

#if defined( CINDER_HEADLESS )
	virtual bool		initialize( ci::ivec2 renderSize, RendererRef sharedRenderer );
#else
	virtual bool		initialize( void *window, RendererRef sharedRenderer );
#endif
	virtual void		kill();
	virtual void		defaultResize() const;
	virtual void		swapBuffers() const;
	virtual void		makeCurrentContext( bool force = false );

 private:
	class RendererGl	*mRenderer = nullptr;
	gl::ContextRef		mCinderContext;

#if defined( CINDER_LINUX_EGL_ONLY )
	EGLContext			mContext;
	EGLDisplay			mDisplay;
	EGLSurface			mSurface;
	EGLConfig			mConfig;
#elif defined( CINDER_HEADLESS_GL_OSMESA )
	OSMesaContext			mContext;
	std::vector<unsigned char>	mBuffer;
	int				mBufferWidth{ 0 };
	int				mBufferHeight{ 0 };
#else
	GLFWwindow			*mContext = nullptr;
#endif
};

}} // namespace cinder::app
