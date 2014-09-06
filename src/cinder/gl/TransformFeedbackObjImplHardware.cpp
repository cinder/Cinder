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

#include "cinder/gl/TransformFeedbackObj.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Vbo.h"

namespace cinder { namespace gl {
	
#if ! defined( CINDER_GL_ES )
	
class TransformFeedbackObjImplHardware : public TransformFeedbackObj {
  public:
	TransformFeedbackObjImplHardware();
	~TransformFeedbackObjImplHardware();
	
	void bindImpl( Context *context );
	void unbindImpl( Context *context );
	void setIndex( int index, BufferObjRef buffer );
	
  private:
	friend class Context;
};

TransformFeedbackObjRef createTransformFeedbackObjImplHardware()
{
	return TransformFeedbackObjRef( new TransformFeedbackObjImplHardware() );
}

TransformFeedbackObjImplHardware::TransformFeedbackObjImplHardware()
{
	glGenTransformFeedbacks( 1, &mId );
}

TransformFeedbackObjImplHardware::~TransformFeedbackObjImplHardware()
{
	glDeleteTransformFeedbacks( 1, &mId );
}

void TransformFeedbackObjImplHardware::bindImpl( Context *context )
{
	glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, mId );
}

void TransformFeedbackObjImplHardware::unbindImpl( Context *context )
{
	glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, 0 );
}

void TransformFeedbackObjImplHardware::setIndex( int index, BufferObjRef buffer )
{
	auto exists = mBufferBases.find( index );
	if( exists == mBufferBases.end() ) {
		mBufferBases.insert( std::pair<int, BufferObjRef>( index, buffer ) );
		glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, index, buffer->getId() );
	}
}
	
#endif
	
}} // gl // cinder
