/*
 Copyright (c) 2014, The Cinder Project: http://libcinder.org
 All rights reserved.

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

#if defined( CINDER_GL_HAS_TRANSFORM_FEEDBACK )

#include <map>
#include <string>

namespace cinder { namespace gl {

typedef std::shared_ptr<class BufferObj>	BufferObjRef;

typedef std::shared_ptr<class TransformFeedbackObj> TransformFeedbackObjRef;

class CI_API TransformFeedbackObj : public std::enable_shared_from_this<TransformFeedbackObj> {
  public:
	
	static TransformFeedbackObjRef create();
	virtual ~TransformFeedbackObj() {}
	
	//! Binds this Transform Feedback Object to the system.
	void bind();
	//! Unbinds this Transform Feedback Object from the system.
	void unbind();
	
	//! Notifies the system to begin capturing Vertices
	void begin( GLenum primitiveMode );
	//! Notifies the system to pause capturing Vertices
	void pause();
	//! Notifies the system to begin capturing Vertices after a pause
	void resume();
	//! Notifies the sytstem you are finished capturing Vertices with this object
	void end();
	
	//! Returns the gl system id for this Transform Feedback Object
	GLuint	getId() const { return mId; }
	
	//! Returns the debugging label associated with the Transform Feedback Object.
	const std::string&	getLabel() const { return mLabel; }
	//! Sets the debugging label associated with the Transform Feedback Object. Calls glObjectLabel() when available.
	void				setLabel( const std::string &label );	
	
  protected:
	TransformFeedbackObj() {}
	
	virtual void bindImpl( class Context *context ) = 0;
	virtual void unbindImpl( class Context *context ) = 0;
	
	virtual void setIndex( int index, BufferObjRef buffer ) = 0;
	
	GLuint								mId;
	std::map<GLuint, gl::BufferObjRef>	mBufferBases;
	
	std::string							mLabel; // debug label
	
	friend class Context;
};

} } // cinder::gl

#endif // defined( CINDER_GL_HAS_TRANSFORM_FEEDBACK )
