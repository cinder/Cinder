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

#include "cinder/gl/Sync.h"

namespace cinder { namespace gl { 

#if ! defined( CINDER_GL_ES ) || defined( CINDER_GL_ES_3 )

SyncRef	Sync::create( GLenum condition, GLbitfield flags )
{
	return std::shared_ptr<Sync>( new Sync( condition, flags ) );
}

Sync::Sync( GLenum condition, GLbitfield flags )
{
	mSync = glFenceSync( condition, flags );	
}

Sync::~Sync()
{
	glDeleteSync( mSync );
}

GLenum Sync::clientWaitSync( GLbitfield flags, GLuint64 timeoutNanoseconds )
{
	return glClientWaitSync( mSync, flags, timeoutNanoseconds );
}

void Sync::waitSync( GLbitfield flags, GLuint64 timeoutNanoseconds )
{
	glWaitSync( mSync, flags, timeoutNanoseconds );
}

#endif // ! defined( CINDER_GL_ES )

} } // namespace cinder::gl