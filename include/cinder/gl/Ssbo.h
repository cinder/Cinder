#pragma once

#include "cinder/gl/BufferObj.h"

#if defined( CINDER_MSW ) || defined( CINDER_LINUX )

class Ssbo;
typedef std::shared_ptr<Ssbo>	SsboRef;

class Ssbo : public ci::gl::BufferObj {
public:
	//! Creates a shader storage buffer object with storage for \a allocationSize bytes, and filled with data \a data if it is not NULL.
	static inline SsboRef	create( GLsizeiptr allocationSize, const void *data = nullptr, GLenum usage = GL_STATIC_DRAW )
	{
		return SsboRef( new Ssbo( allocationSize, data, usage ) );
	}

	//! Bind base.
	inline void bindBase( GLuint index ) { glBindBufferBase( mTarget, index, mId );  mBase = index; }
	//! Unbinds the buffer.
	inline void unbindBase() { glBindBufferBase( mTarget, mBase, 0 ); mBase = 0; }
	//! Analogous to bufferStorage.
	inline void bufferStorage( GLsizeiptr size, const void *data, GLbitfield flags ) const { glBufferStorage( mTarget, size, data, flags ); }
protected:
	Ssbo( GLsizeiptr allocationSize, const void *data = nullptr, GLenum usage = GL_STATIC_DRAW )
		: BufferObj( GL_SHADER_STORAGE_BUFFER, allocationSize, data, usage ),
			mBase( 0 )
	{
	}
	GLuint mBase;
};

//! Represents an OpenGL Shader Storage Buffer Object
template<class T>
class SsboT : public Ssbo { 
public: 
	typedef std::shared_ptr<SsboT<T>>	Ref;

	//! Creates a shader storage buffer object with storage for \a allocationSize bytes, and filled with data \a data if it is not NULL.
	static inline Ref	create( const std::vector<T> &data, GLenum usage = GL_STATIC_DRAW )
	{
		return Ref( new SsboT<T>( data, usage ) );
	}

	static inline Ref	create( GLsizeiptr size, GLenum usage = GL_STATIC_DRAW )
	{
		return Ref( new SsboT<T>( size, usage ) );
	}

	//! Map to type.
	inline T *mapT( GLbitfield access ) const { return reinterpret_cast<T *>( map( access ) ); }
	//! Map buffer range to type
	inline T *mapBufferRangeT( GLintptr offset, GLsizeiptr length, GLbitfield access ) { return reinterpret_cast<T *>( mapBufferRange( offset, length, access ) ); }
	//! Analogous to bufferStorage.
	inline void bufferStorageT( const std::vector<T> &data, GLbitfield flags ) const { glBufferStorage( mTarget, data.size(), &( data[0] ), flags ); }

protected:
	SsboT( const std::vector<T> &data, GLenum usage = GL_STATIC_DRAW )
		: Ssbo( sizeof( T ) * data.size(), &( data[0] ), usage )
	{
	}
	SsboT( GLsizeiptr size, GLenum usage = GL_STATIC_DRAW )
		: Ssbo( sizeof( T ) * size, nullptr, usage )
	{
	}
};

#endif // #if defined( CINDER_MSW ) || defined( CINDER_LINUX )
