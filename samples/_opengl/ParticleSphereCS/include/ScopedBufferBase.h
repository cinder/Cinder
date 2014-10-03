
#pragma once
#include <boost/noncopyable.hpp>
#include <cinder/gl/BufferObj.h>

#if defined( CINDER_MSW ) || defined( CINDER_LINUX )

struct ScopedBufferBase : public boost::noncopyable {
	ScopedBufferBase( const ci::gl::BufferObjRef &bufferObj, GLuint index )
		: mIndex( index ), mTarget( bufferObj->getTarget() )
	{
		glBindBufferBase( mTarget, mIndex, bufferObj->getId() );
	}

	ScopedBufferBase( GLenum target, GLuint index, GLuint id )
		: mTarget( target ), mIndex( index )
	{
		glBindBufferBase( mTarget, mIndex, id );
	}

	~ScopedBufferBase()
	{
		glBindBufferBase( mTarget, mIndex, 0 );
	}

private:
	GLenum		mTarget;
	GLuint		mIndex;
};

#endif