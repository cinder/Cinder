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

namespace cinder { namespace gl { 

#if ! defined( CINDER_GL_ES ) || defined( CINDER_GL_ES_3 )

typedef std::shared_ptr<class Sync>		SyncRef;

class CI_API Sync {
  public:
	//! Analogous to glFenceSync().
	static SyncRef	create( GLenum condition = GL_SYNC_GPU_COMMANDS_COMPLETE, GLbitfield flags = 0 );
	~Sync();

	//! Analogous to glClientWaitSync(). Returns \c GL_ALREADY_SIGNALED, \c GL_TIMEOUT_EXPIRED, \c GL_CONDITION_SATISFIED, or \c GL_WAIT_FAILED
	GLenum	clientWaitSync( GLbitfield flags = GL_SYNC_FLUSH_COMMANDS_BIT, GLuint64 timeoutNanoseconds = GL_TIMEOUT_IGNORED );

	//! Analogous to glWaitSync(). As of this writing \a flags and \a timeout must be their default values, \c 0 and \c GL_TIMEOUT_IGNORED
	void	waitSync( GLbitfield flags = 0, GLuint64 timeout = GL_TIMEOUT_IGNORED );

	//! Returns the raw OpenGL Sync Object
	GLsync	getObject() { return mSync; }

  protected:
  	Sync( GLenum condition, GLbitfield flags );

	GLsync		mSync;
};

#endif // ! defined( CINDER_GL_ES )

} } // namespace cinder::gl