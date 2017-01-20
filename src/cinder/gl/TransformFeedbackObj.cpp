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

#if defined( CINDER_GL_HAS_TRANSFORM_FEEDBACK )

#include "cinder/gl/Vbo.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Environment.h"

namespace cinder { namespace gl {

namespace {

class TransformFeedbackObjImplHardware : public TransformFeedbackObj {
  public:
	TransformFeedbackObjImplHardware()
	{
		glGenTransformFeedbacks( 1, &mId );
	}
	~TransformFeedbackObjImplHardware()
	{
		glDeleteTransformFeedbacks( 1, &mId );
	}
	
	void bindImpl( Context * /*context*/ )
	{
		glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, mId );
	}

	void unbindImpl( Context * /*context*/ )
	{
		glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, 0 );
	}
	void setIndex( int index, BufferObjRef buffer )
	{
		auto exists = mBufferBases.find( index );
		if( exists == mBufferBases.end() ) {
			mBufferBases.insert( std::pair<int, BufferObjRef>( index, buffer ) );
			glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, index, buffer->getId() );
		}
	}
};

class TransformFeedbackObjImplSoftware : public TransformFeedbackObj {
  public:
	TransformFeedbackObjImplSoftware()
	{
		mId = 0;
	}
	~TransformFeedbackObjImplSoftware()
	{
	}
	
	void bindImpl( class Context * /*context*/ )
	{
		for( auto bufferIt = mBufferBases.begin(); bufferIt != mBufferBases.end(); bufferIt++ ) {
			glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, bufferIt->first, bufferIt->second->getId() );
		}
	}
	
	void unbindImpl( class Context * /*context*/ )
	{
	}

	void setIndex( int index, BufferObjRef buffer )
	{
		bool changed = false;
		
		auto exists = mBufferBases.find( index );
		if( exists == mBufferBases.end() ) {
			mBufferBases.insert( std::pair<int, BufferObjRef>( index, buffer ) );
			changed = true;
		}
		else {
			if( exists->second != buffer ) {
				exists->second = buffer;
				changed = true;
			}
		}
		
		if( changed ) {
			glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, index, buffer->getId() );
		}
	}
};

} // anonymous namespace

TransformFeedbackObjRef TransformFeedbackObj::create()
{
	if( ! glGenTransformFeedbacks ) {
		return TransformFeedbackObjRef( new TransformFeedbackObjImplSoftware() );
	}
	else {
		return TransformFeedbackObjRef( new TransformFeedbackObjImplHardware() );
	}
}

void TransformFeedbackObj::bind()
{
	gl::context()->bindTransformFeedbackObj( shared_from_this() );
}

void TransformFeedbackObj::unbind()
{
	gl::context()->bindTransformFeedbackObj( nullptr );
}

void TransformFeedbackObj::begin( GLenum primitiveMode )
{
	glBeginTransformFeedback( primitiveMode );
}

void TransformFeedbackObj::pause()
{
	glPauseTransformFeedback();
}

void TransformFeedbackObj::resume()
{
	glResumeTransformFeedback();
}

void TransformFeedbackObj::end()
{
	glEndTransformFeedback();
}

void TransformFeedbackObj::setLabel( const std::string &label )
{
	mLabel = label;
	env()->objectLabel( GL_TRANSFORM_FEEDBACK, mId, (GLsizei)label.size(), label.c_str() );
}

} } // cinder::gl

#endif // defined( CINDER_GL_HAS_TRANSFORM_FEEDBACK )
