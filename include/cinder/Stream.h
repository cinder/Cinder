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

#include <boost/noncopyable.hpp>

#include <string>
#ifndef __OBJC__
#	include <boost/iostreams/concepts.hpp>
#	include <boost/iostreams/stream.hpp>
#endif

namespace cinder {

class StreamBase : private boost::noncopyable {
 public:
	virtual ~StreamBase() {}
	
	enum Endianness { STREAM_BIG_ENDIAN, STREAM_LITTLE_ENDIAN };
 
	/** Returns the platform's endianness as a StreamBase::Endianness **/
	static uint8_t		getNativeEndianness()
#ifdef CINDER_LITTLE_ENDIAN
		{ return STREAM_LITTLE_ENDIAN; }
#else
		{ return STREAM_BIG_ENDIAN; }
#endif
 
	//! Returns the file name of the path from which a Stream originated when relevant. Empty string when undefined.
  	const std::string&	getFileName() const { return mFileName; }
	//! Sets the file name of the path from which a Stream originated when relevant. Empty string when undefined.
	void				setFileName( const std::string &aFileName ) { mFileName = aFileName; }

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

	std::string				mFileName;
	bool					mDeleteOnDestroy;
};

class OStream : public virtual StreamBase {
 public:
	virtual ~OStream() {}

	template<typename T>
	void		write( T t );
	template<typename T>
	void		writeEndian( T t, uint8_t endian ) { if ( endian == STREAM_BIG_ENDIAN ) writeBig( t ); else writeLittle( t ); }
	template<typename T>
	void		writeBig( T t );
	template<typename T>
	void		writeLittle( T t );

	void		write( const Buffer &buffer );
	void		writeData( const void *src, size_t size );

 protected:
	OStream() : StreamBase() {}
 
	virtual void		IOWrite( const void *t, size_t size ) = 0;
};


typedef shared_ptr<class OStream>	OStreamRef;

class IStream : public virtual StreamBase {
 public:
	virtual ~IStream() {};

	template<typename T>
	void		read( T *t );
	template<typename T>
	void		readEndian( T *t, uint8_t endian ) { if ( endian == STREAM_BIG_ENDIAN ) readBig( t ); else readLittle( t ); }
	template<typename T>
	void		readBig( T *t );
	template<typename T>
	void		readLittle( T *t );
	
	void		readFixedString( char *t, size_t maxSize, bool nullTerminate );
	void		readFixedString( std::string *t, size_t size );
	std::string	readLine();
	
	void			readData( void *dest, size_t size );
	virtual size_t	readDataAvailable( void *dest, size_t maxSize ) = 0;

	virtual off_t		size() const = 0;	
	virtual bool		isEof() const = 0;

 protected:
	IStream() : StreamBase() {}

	virtual void		IORead( void *t, size_t size ) = 0;
		
	static const int	MINIMUM_BUFFER_SIZE = 8; // minimum bytes of random access a stream must offer relative to the file start
};
typedef shared_ptr<IStream>		IStreamRef;


class IoStream : public IStream, public OStream {
 public:
	IoStream() : IStream(), OStream() {}
	virtual ~IoStream() {}
};
typedef shared_ptr<IoStream>		IoStreamRef;


typedef shared_ptr<class IStreamFile>	IStreamFileRef;

class IStreamFile : public IStream {
 public:
	//! Creates a new IStreamFileRef from a C-style file pointer \a FILE as returned by fopen(). If \a ownsFile the returned stream will destroy the stream upon its own destruction.
	static IStreamFileRef createRef( FILE *file, bool ownsFile = true, int32_t defaultBufferSize = 2048 );
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
 
	FILE					*mFile;
	bool					mOwnsFile;
	size_t					mBufferSize, mDefaultBufferSize;
	shared_ptr<uint8_t>	mBuffer;
	off_t					mBufferOffset; // actual offset to do IO from; incremented by IO
	off_t					mBufferFileOffset; // beginning of the buffer in the file
	mutable off_t			mSize;
	mutable bool			mSizeCached;
};


typedef shared_ptr<class OStreamFile>	OStreamFileRef;

class OStreamFile : public OStream {
  public:
	//! Creates a new OStreamFileRef from a C-style file pointer \a FILE as returned by fopen(). If \a ownsFile the returned stream will destroy the stream upon its own destruction.
	static OStreamFileRef	createRef( FILE *file, bool ownsFile = true );
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


typedef shared_ptr<class IoStreamFile>		IoStreamFileRef;

class IoStreamFile : public IoStream {
 public:
	//! Creates a new IoStreamFileRef from a C-style file pointer \a FILE as returned by fopen(). If \a ownsFile the returned stream will destroy the stream upon its own destruction.
	static IoStreamFileRef createRef( FILE *file, bool ownsFile = true, int32_t defaultBufferSize = 2048 );
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
	virtual void		IOWrite( const void *t, size_t size );
 
	FILE					*mFile;
	bool					mOwnsFile;
	int32_t					mBufferSize, mDefaultBufferSize;
	shared_ptr<uint8_t>		mBuffer;
	off_t					mBufferOffset; // actual offset to do IO from; incremented by IO
	off_t					mBufferFileOffset; // beginning of the buffer in the file
	mutable off_t			mSize;
	mutable bool			mSizeCached;
};


typedef shared_ptr<class IStreamMem>	IStreamMemRef;
class IStreamMem : public IStream {
 public:
	//! Creates a new IStreamMemRef from the memory pointed to by \a data which is of size \a size bytes.
	static IStreamMemRef		createRef( const void *data, size_t size );
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


class OStreamMem : public OStream {
 public:
	~OStreamMem();

	virtual off_t		tell() const { return static_cast<off_t>( mOffset ); }
	virtual void		seekAbsolute( off_t absoluteOffset );
	virtual void		seekRelative( off_t relativeOffset );

	void*				getBuffer() { return mBuffer; }
	
 protected:
	OStreamMem( size_t bufferSizeHint );

	virtual void		IOWrite( const void *t, size_t size );

	void			*mBuffer;
	size_t			mDataSize;
	size_t			mOffset;
};

typedef shared_ptr<OStreamMem>		OStreamMemRef;


// This class is a utility to save and restore a stream's state
class IStreamStateRestore {
 public:
	IStreamStateRestore( IStream &aStream ) : mStream( aStream ), mOffset( aStream.tell() ) {}
	~IStreamStateRestore() {
		mStream.seekAbsolute( mOffset );
	}
	
 private:
	IStream		&mStream;
	off_t		mOffset;
};

//! Opens the file lcoated at \a path for read access as a stream.
IStreamFileRef	loadFileStream( const std::string &path );
//! Opens the file located at \a path for write access as a stream, and creates it if it does not exist. Optionally creates any intermediate directories when \a createParents is true.
OStreamFileRef	writeFileStream( const std::string &path, bool createParents = true );
//! Opens a path for read-write access as a stream.
IoStreamFileRef readWriteFileStream( const std::string &path );

//! Loads the contents of a stream into a contiguous block of memory, pointed to by \a resultData. The size of this block is stored in \a resultDataSize.
void loadStreamMemory( IStreamRef is, shared_ptr<uint8_t> *resultData, size_t *resultDataSize );
//! Loads the contents of a stream into a Buffer
Buffer loadStreamBuffer( IStreamRef is );


// Stream exception
class StreamExc : public Exception {
};

class StreamExcOutOfMemory : public StreamExc {
};

#ifndef __OBJC__
class cinder_stream_source {
 public:
	typedef char char_type;
    typedef boost::iostreams::source_tag category;

	cinder_stream_source( cinder::IStreamRef aStream ) : mStream( aStream ) {}

	std::streamsize read( char *s, std::streamsize n )
	{
		if( mStream->isEof() )
			return -1;
		
		return (std::streamsize)mStream->readDataAvailable( s, (size_t)n );
	}

 protected:
	IStreamRef		mStream; // a little kludgy but this is for convenience
};

typedef boost::iostreams::stream<cinder_stream_source> cinder_istream;

class cinder_stream_sink {
 public:
	typedef char char_type;
    typedef boost::iostreams::sink_tag category;

	cinder_stream_sink( OStreamRef aStream ) : mStream( aStream ) {}

	std::streamsize write( const char *s, std::streamsize n )
	{
		mStream->writeData( s, (size_t)n );
		return n;
	}

 protected:
	OStreamRef		mStream;
};

typedef boost::iostreams::stream<cinder_stream_sink> cinder_ostream;

class cinder_stream_bidirectional_device {
 public:
	typedef char char_type;
    typedef boost::iostreams::seekable_device_tag category;

	cinder_stream_bidirectional_device( cinder::IoStreamRef aStream ) : mStream( aStream ) {}

	std::streamsize read( char *s, std::streamsize n )
	{
		return static_cast<std::streamsize>( mStream->readDataAvailable( s, (size_t)n ) );
	}

	std::streamsize write( const char *s, std::streamsize n )
	{
		mStream->writeData( s, (size_t)n );
		return n;
	}

	boost::iostreams::stream_offset seek( boost::iostreams::stream_offset off, std::ios_base::seekdir way)
	{
		if( way == std::ios_base::beg ) {
			mStream->seekAbsolute( (off_t)off );
		}
		else if( way == std::ios_base::cur ) {
			mStream->seekRelative( (off_t)off );
		}
		else { // way == std::ios_base::end
			mStream->seekAbsolute( -(off_t)off );
		}
		return mStream->tell();
	}

 protected:
	IoStreamRef		mStream;
};

typedef boost::iostreams::stream<cinder_stream_bidirectional_device> cinder_iostream;

#endif // ! __OBJC__

} // namespace cinder