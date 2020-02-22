/*
 Copyright (c) 2010, The Barbarian Group
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

#include "cinder/Cinder.h"
#include "cinder/Buffer.h"
#include "cinder/Exception.h"
#include "cinder/Filesystem.h"
#include "cinder/Noncopyable.h"
#if defined( CINDER_ANDROID )
  #include "cinder/app/android/AssetFileSystem.h"
#endif

#include <string>

namespace cinder {

class CI_API StreamBase : private Noncopyable {
 public:
	virtual ~StreamBase() {}
	
	enum Endianness { STREAM_BIG_ENDIAN, STREAM_LITTLE_ENDIAN };
 
	//! Returns the platform's endianness as a StreamBase::Endianness
	static uint8_t		getNativeEndianness()
	{
#ifdef CINDER_LITTLE_ENDIAN
		 return STREAM_LITTLE_ENDIAN;
#else
		 return STREAM_BIG_ENDIAN;
#endif
	}

	//! Returns the file name of the path from which a Stream originated when relevant. Empty string when undefined.
  	const fs::path&		getFileName() const { return mFileName; }
	//! Sets the file name of the path from which a Stream originated when relevant. Empty string when undefined.
	void				setFileName( const fs::path &aFileName ) { mFileName = aFileName; }

	//! Returns whether the Stream has been requested to destroy its source upon its own destruction. For example, IStreamFile will delete its source file. Ignored in some types of streams. Defaults to \c false.
	bool		getDeleteOnDestroy() const { return mDeleteOnDestroy; }
	//! Sets whether the Stream has been requested to destroy its source upon its own destruction. For example, IStreamFile will delete its source file. Ignored in some types of streams. Defaults to \c false.
	void		setDeleteOnDestroy( bool enable = true ) { mDeleteOnDestroy = enable; }

	//! Returns the current position of the stream measured in bytes **/
	virtual off_t		tell() const = 0;

	//! Sets the current position of the stream to byte \a absoluteOffset. A negative offset is relative to the end of the file.
	virtual void		seekAbsolute( off_t absoluteOffset ) = 0;
	
	//! Moves the current position of the stream by \a relativeOffset bytes
	virtual void		seekRelative( off_t relativeOffset ) = 0;
 
 protected:
	StreamBase() : mDeleteOnDestroy( false ) {}

	fs::path				mFileName;
	bool					mDeleteOnDestroy;
};

class CI_API OStream : public virtual StreamBase {
 public:
	virtual ~OStream() {}

	//! Writes null-terminated string, including terminator
	void		write( const std::string &s ) { writeData( s.c_str(), s.length() + 1 ); }
	template<typename T>
	void		write( T t ) { IOWrite( &t, sizeof(T) ); }
	template<typename T>
	void		writeEndian( T t, uint8_t endian ) { if ( endian == STREAM_BIG_ENDIAN ) writeBig( t ); else writeLittle( t ); }
	template<typename T>
	void		writeBig( T t );
	template<typename T>
	void		writeLittle( T t );

	void		write( const Buffer &buffer );
	void		writeData( const void *src, size_t size );

 protected:
	OStream() = default;
 
	virtual void		IOWrite( const void *t, size_t size ) = 0;
};


typedef std::shared_ptr<class OStream>	OStreamRef;

class CI_API IStreamCinder : public virtual StreamBase {
 public:
	virtual ~IStreamCinder() {};

	template<typename T>
	void		read( T *t ) { IORead( t, sizeof(T) ); }
	template<typename T>
	void		readEndian( T *t, uint8_t endian ) { if ( endian == STREAM_BIG_ENDIAN ) readBig( t ); else readLittle( t ); }
	template<typename T>
	void		readBig( T *t );
	template<typename T>
	void		readLittle( T *t );

	//! Reads characters until a null terminator
	void		read( std::string *s );
	void		read( fs::path *p );
	void		readFixedString( char *t, size_t maxSize, bool nullTerminate );
	void		readFixedString( std::string *t, size_t size );
	std::string	readLine();
	
	void			readData( void *dest, size_t size );
	virtual size_t	readDataAvailable( void *dest, size_t maxSize ) = 0;

	virtual off_t		size() const = 0;	
	virtual bool		isEof() const = 0;

 protected:
	IStreamCinder() = default;

	virtual void		IORead( void *t, size_t size ) = 0;
		
	static const int	MINIMUM_BUFFER_SIZE = 8; // minimum bytes of random access a stream must offer relative to the file start
};
typedef std::shared_ptr<IStreamCinder>		IStreamRef;


class CI_API IoStream : public IStreamCinder, public OStream {
 public:
	IoStream() : IStreamCinder(), OStream() {}
	virtual ~IoStream() {}
};
typedef std::shared_ptr<IoStream>		IoStreamRef;


typedef std::shared_ptr<class IStreamFile>	IStreamFileRef;

class CI_API IStreamFile : public IStreamCinder {
 public:
	//! Creates a new IStreamFileRef from a C-style file pointer \a FILE as returned by fopen(). If \a ownsFile the returned stream will destroy the stream upon its own destruction.
	static IStreamFileRef create( FILE *file, bool ownsFile = true, int32_t defaultBufferSize = 2048 );
	~IStreamFile();

	size_t		readDataAvailable( void *dest, size_t maxSize );
	
	void		seekAbsolute( off_t absoluteOffset );
	void		seekRelative( off_t relativeOffset );
	off_t		tell() const;
	off_t		size() const;
	
	bool		isEof() const;
	
	FILE*		getFILE() { return mFile; }

 protected:
	IStreamFile( FILE *aFile, bool aOwnsFile = true, int32_t aDefaultBufferSize = 2048 );

	virtual void		IORead( void *t, size_t size );
	size_t				readDataImpl( void *dest, size_t maxSize );
 
	FILE						*mFile;
	bool						mOwnsFile;
	size_t						mBufferSize, mDefaultBufferSize;
	std::shared_ptr<uint8_t>	mBuffer;
	off_t						mBufferOffset; // actual offset to do IO from; incremented by IO
	off_t						mBufferFileOffset; // beginning of the buffer in the file
	mutable off_t				mSize;
	mutable bool				mSizeCached;
};


#if defined( CINDER_ANDROID )
typedef std::shared_ptr<class IStreamAndroidAsset>	IStreamAndroidAssetRef;

class IStreamAndroidAsset : public IStreamCinder {
 public:
	static IStreamAndroidAssetRef create( AAsset *asset, bool ownsFile = true, int32_t defaultBufferSize = 2048 );
	~IStreamAndroidAsset();

	size_t		readDataAvailable( void *dest, size_t maxSize );
	
	void		seekAbsolute( off_t absoluteOffset );
	void		seekRelative( off_t relativeOffset );
	off_t		tell() const;
	off_t		size() const;
	
	bool		isEof() const;
	
	AAsset*		getAAsset() { return mAsset; }

 protected:
	IStreamAndroidAsset( AAsset *aFile, bool aOwnsFile = true, int32_t aDefaultBufferSize = 2048 );

	virtual void		IORead( void *t, size_t size );
	size_t				readDataImpl( void *dest, size_t maxSize );
 
	AAsset						*mAsset;
	bool						mOwnsFile;
	size_t						mBufferSize, mDefaultBufferSize;
	std::shared_ptr<uint8_t>	mBuffer;
	off_t						mBufferOffset; // actual offset to do IO from; incremented by IO
	off_t						mBufferFileOffset; // beginning of the buffer in the file
	mutable off_t				mSize;
	mutable bool				mSizeCached;
};
#endif


typedef std::shared_ptr<class OStreamFile>	OStreamFileRef;

class CI_API OStreamFile : public OStream {
  public:
	//! Creates a new OStreamFileRef from a C-style file pointer \a FILE as returned by fopen(). If \a ownsFile the returned stream will destroy the stream upon its own destruction.
	static OStreamFileRef	create( FILE *file, bool ownsFile = true );
	~OStreamFile();

	virtual off_t		tell() const;
	virtual void		seekAbsolute( off_t absoluteOffset );
	virtual void		seekRelative( off_t relativeOffset );

	FILE*				getFILE() { return mFile; }

	
  protected:
	OStreamFile( FILE *aFile, bool aOwnsFile = true );

	virtual void		IOWrite( const void *t, size_t size );

	FILE*				mFile;
	bool				mOwnsFile;
};


typedef std::shared_ptr<class IoStreamFile>		IoStreamFileRef;

class CI_API IoStreamFile : public IoStream {
 public:
	//! Creates a new IoStreamFileRef from a C-style file pointer \a FILE as returned by fopen(). If \a ownsFile the returned stream will destroy the stream upon its own destruction.
	static IoStreamFileRef create( FILE *file, bool ownsFile = true, int32_t defaultBufferSize = 2048 );
	~IoStreamFile();

	size_t		readDataAvailable( void *dest, size_t maxSize );
	
	void		seekAbsolute( off_t absoluteOffset );
	void		seekRelative( off_t relativeOffset );
	off_t		tell() const;
	off_t		size() const;
	
	bool		isEof() const;
	
	FILE*		getFILE() { return mFile; }

 protected:
	IoStreamFile( FILE *aFile, bool aOwnsFile = true, int32_t aDefaultBufferSize = 2048 );
	
	virtual void		IORead( void *t, size_t size );
	size_t				readDataImpl( void *dest, size_t maxSize );
	virtual void		IOWrite( const void *t, size_t size );
 
	FILE						*mFile;
	bool						mOwnsFile;
	int32_t						mBufferSize, mDefaultBufferSize;
	std::shared_ptr<uint8_t>	mBuffer;
	off_t						mBufferOffset; // actual offset to do IO from; incremented by IO
	off_t						mBufferFileOffset; // beginning of the buffer in the file
	mutable off_t				mSize;
	mutable bool				mSizeCached;
};

typedef std::shared_ptr<class IStreamMem>	IStreamMemRef;
class CI_API IStreamMem : public IStreamCinder {
 public:
	//! Creates a new IStreamMemRef from the memory pointed to by \a data which is of size \a size bytes.
	static IStreamMemRef		create( const void *data, size_t size );
	~IStreamMem();

	size_t		readDataAvailable( void *dest, size_t maxSize );
	
	void		seekAbsolute( off_t absoluteOffset );
	void		seekRelative( off_t relativeOffset );
	//! Returns the current offset into the stream in bytes
	off_t		tell() const;
	//! Returns the total length of stream in bytes
	off_t		size() const { return static_cast<off_t>( mDataSize ); }

	//! Returns whether the stream is currently pointed at the end of the file	
	bool		isEof() const;
	
	//! Returns a pointer to the data which the stream wraps
	const void*	getData() { return reinterpret_cast<const void*>( mData ); }

 protected:
 	IStreamMem( const void *aData, size_t aDataSize );

	virtual void	IORead( void *t, size_t size );
 
	const uint8_t	*mData;
	size_t			mDataSize;
	size_t			mOffset;
};


typedef std::shared_ptr<class OStreamMem>		OStreamMemRef;

class CI_API OStreamMem : public OStream {
 public:
	static OStreamMemRef		create( size_t bufferSizeHint = 4096 ) { return std::shared_ptr<OStreamMem>( new OStreamMem( bufferSizeHint ) ); }

	~OStreamMem();

	virtual off_t		tell() const { return static_cast<off_t>( mOffset ); }
	virtual void		seekAbsolute( off_t absoluteOffset );
	virtual void		seekRelative( off_t relativeOffset );

	void*				getBuffer() { return mBuffer; }
	
 protected:
	OStreamMem( size_t bufferSizeHint );

	virtual void		IOWrite( const void *t, size_t size );

	void*			mBuffer;
	size_t			mDataSize;
	size_t			mOffset;
};


// This class is a utility to save and restore a stream's state
class CI_API IStreamStateRestore {
 public:
	IStreamStateRestore( IStreamCinder &aStream ) : mStream( aStream ), mOffset( aStream.tell() ) {}
	~IStreamStateRestore()
	{
		mStream.seekAbsolute( mOffset );
	}
	
 private:
	IStreamCinder&	mStream;
	off_t			mOffset;
};

//! Opens the file lcoated at \a path for read access as a stream.
CI_API IStreamFileRef	loadFileStream( const fs::path &path );
//! Opens the file located at \a path for write access as a stream, and creates it if it does not exist. Optionally creates any intermediate directories when \a createParents is true.
CI_API OStreamFileRef	writeFileStream( const fs::path &path, bool createParents = true );
//! Opens a path for read-write access as a stream.
CI_API IoStreamFileRef readWriteFileStream( const fs::path &path );

//! Loads the contents of a stream into a contiguous block of memory, pointed to by \a resultData. The size of this block is stored in \a resultDataSize.
CI_API void loadStreamMemory( IStreamRef is, std::shared_ptr<uint8_t> *resultData, size_t *resultDataSize );
//! Loads the contents of a stream into a ref counted Buffer
CI_API BufferRef loadStreamBuffer( IStreamRef is );


#if defined( CINDER_ANDROID )
//! Opens the file lcoated at \a path for read access as a stream.
IStreamAndroidAssetRef	loadAndroidAssetStream( const fs::path &path );
#endif // defined( CINDER_ANDROID )


// Stream exception
class CI_API StreamExc : public Exception {
  public:
	StreamExc() throw() {}
	StreamExc( const std::string &fontName ) throw();
	virtual const char* what() const throw() { return mMessage; }	
  private:
	char mMessage[2048];		
};

class CI_API StreamExcOutOfMemory : public StreamExc {
};


} // namespace cinder