/*
 Copyright (c) 2015, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#include "cinder/audio/linux/FileAudioLoader.h"
#include "cinder/audio/Exception.h" 
#include "cinder/audio/SampleType.h"

#include <mpg123.h>
#include <sndfile.h>

namespace audioloader {

using SampleType = cinder::audio::SampleType;
using SourceFileAudioLoader = cinder::audio::linux::SourceFileAudioLoader;
using AudioFileExc = cinder::audio::AudioFileExc;

enum class FileType {
	UNKNOWN,
	MP3,
	PCM
};

//! \class ProcessScopedMpg123
//!
//!
class ProcessScopedMpg123 {
public:
	~ProcessScopedMpg123() {
		mpg123_exit();
std::cout << "ProcessScopedMpg123::~ProcessScopedMpg123()" << std::endl;
	}

	static void initialize() {
		if( ! sInstance ) {
			sInstance.reset( new ProcessScopedMpg123() );
		}
	}

	static void destroy() {
		sInstance.reset();
	}
private:
	ProcessScopedMpg123() {
		mpg123_init();
std::cout << "ProcessScopedMpg123::ProcessScopedMpg123()" << std::endl;
	}

	static std::unique_ptr<ProcessScopedMpg123> sInstance;
};

std::unique_ptr<ProcessScopedMpg123> ProcessScopedMpg123::sInstance;

//! \class IStreamSndFile
//!
//!
struct IStreamSndFile {
	static sf_count_t get_filelen( void* userData ) {
		ci::IStreamCinder* stream = static_cast<ci::IStreamCinder*>( userData );
		return stream->size();
	}

	static sf_count_t seek( sf_count_t offset, int whence, void* userData ) {
		ci::IStreamCinder* stream = static_cast<ci::IStreamCinder*>( userData );
		switch( whence ) {
			case SEEK_SET:
				stream->seekAbsolute( offset );
			break;

			case SEEK_CUR:
				stream->seekRelative( offset );
			break;

			case SEEK_END: {
				size_t absOffset = stream->size() - offset;
				stream->seekAbsolute( absOffset );
			}
			break;

			default :
				stream->seekAbsolute( 0 );
			break;
		}
		return stream->tell();
	}

	static sf_count_t read( void* ptr, sf_count_t count, void* userData ) {
		ci::IStreamCinder* stream = static_cast<ci::IStreamCinder*>( userData );
		size_t pos0 = stream->tell();
		stream->readData( ptr, count );
		size_t pos1 = stream->tell();
		return pos1 - pos0;
	}

	static sf_count_t write( const void* ptr, sf_count_t count, void *userData ) {
		return 0;
	}

	static sf_count_t tell( void* userData ) {
		ci::IStreamCinder* stream = static_cast<ci::IStreamCinder*>( userData );
		return stream->tell();
	}
};

//!
FileType determineFileType( const ci::IStreamRef& stream )
{
	FileType result = FileType::UNKNOWN;

	// Try MP3 first
	if( FileType::UNKNOWN == result ) {
		ProcessScopedMpg123::initialize();

		stream->seekAbsolute( 0 );

		int ret = MPG123_OK;
		mpg123_handle* handle = mpg123_new( nullptr, &ret );
		if( ( MPG123_OK == ret ) && ( MPG123_OK == mpg123_open_feed( handle ) ) ) {
			const size_t kStreamSize = stream->size();

			const size_t kBufSize = 4096;
			char buf[kBufSize];

			size_t totalBytesRead = 0;
			do {
				size_t readSize = std::min<size_t>( kBufSize, ( kStreamSize - stream->tell() ) );
				size_t pos0 = stream->tell();
				stream->readData( static_cast<void*>( buf ), readSize );
				size_t pos1 = stream->tell();
				size_t amountRead = pos1 - pos0;
				if( 0 == amountRead ) {
					break;
				}
				totalBytesRead += amountRead;
				ret = mpg123_decode( handle, reinterpret_cast<const unsigned char*>( buf ), amountRead, nullptr, 0, nullptr );
			}
			while( ( MPG123_NEED_MORE == ret ) && ( totalBytesRead < 64*1024 ) );

			long rate = 0;
			int channels = 0;
			int encodings = -1;
			if( ( MPG123_NEW_FORMAT == ret ) && ( MPG123_OK == mpg123_getformat( handle, &rate, &channels, &encodings ) ) ) {
				if( ( MPG123_OK == mpg123_format_none( handle ) ) && ( MPG123_OK == mpg123_format( handle, rate, channels, MPG123_ENC_SIGNED_16 ) ) ) {
					result = FileType::MP3;
				}
			}

			mpg123_delete( handle );
		}
	}

	// WAV next
	if( FileType::UNKNOWN == result ) {
		stream->seekAbsolute( 0 );

		SF_VIRTUAL_IO vio;
		vio.get_filelen	= IStreamSndFile::get_filelen;
		vio.seek		= IStreamSndFile::seek;
		vio.read		= IStreamSndFile::read;
		vio.write		= IStreamSndFile::write;
		vio.tell		= IStreamSndFile::tell;

		SF_INFO info;
		SNDFILE* handle = sf_open_virtual( &vio, SFM_READ, &info, static_cast<void*>( stream.get() ) );
		if( nullptr != handle ) {
			result = FileType::PCM;
			sf_close( handle );
		}
	}

	return result;	
}

//! \class FileLoader
//!
//!
class FileLoader {
  public:
	FileLoader( SourceFileAudioLoader* cinderParent, const ci::IStreamRef& stream );
	virtual ~FileLoader();

	size_t				getNumChannels() const { return mNumChannels; }
	size_t				getSampleRateNative() const  { return mSampleRate; }

	//! Returns the length in frames.
	virtual size_t		getNumFrames() const = 0;

  protected:
	SourceFileAudioLoader*	mCinderParent = nullptr;
	ci::IStreamRef			mStream;

	size_t					mSampleRate						= 0;
	size_t					mNumChannels					= 0;
	size_t					mBytesPerSample					= 0;
	SampleType				mSampleType						= SampleType::FLOAT_32;
	double					mSeconds						= 0.0;
	bool					mCanSeek						= false;
	size_t					mReadBufferPos					= 0;
	size_t					mFramesRemainingInReadBuffer	= 0;
};

FileLoader::FileLoader( SourceFileAudioLoader* cinderParent, const ci::IStreamRef& stream )
	: mCinderParent( cinderParent ), mStream( stream )
{
	if( mStream ) {
		mStream->seekAbsolute( 0 );
	}
}

FileLoader::~FileLoader()
{
}

//! \class FileLoaderMpg123
//!
//!
class FileLoaderMpg123 : public FileLoader {
  public:
	FileLoaderMpg123( SourceFileAudioLoader* cinderParent, const ci::IStreamRef& stream );
	virtual ~FileLoaderMpg123();

	//! Returns the length in frames.
	virtual size_t	getNumFrames() const override;

  private:
  	mpg123_handle*	mHandle = nullptr;
  	int 			mEncodings = -1;
};

FileLoaderMpg123::FileLoaderMpg123( SourceFileAudioLoader* cinderParent, const ci::IStreamRef& stream )
	: FileLoader( cinderParent, stream )
{
	// Determine format - for now use the information from the first block.
	// This is a bit redudant and there should definitely be a better way.
	{
		int ret = MPG123_OK;
		mpg123_handle* handle = mpg123_new( nullptr, &ret );
		if( ( MPG123_OK == ret ) && ( MPG123_OK == mpg123_open_feed( handle ) ) ) {
			const size_t kStreamSize = mStream->size();

			const size_t kBufSize = 4096;
			char buf[kBufSize];

			size_t totalBytesRead = 0;
			do {
				size_t readSize = std::min<size_t>( kBufSize, ( kStreamSize - mStream->tell() ) );
				size_t pos0 = mStream->tell();
				mStream->readData( static_cast<void*>( buf ), readSize );
				size_t pos1 = mStream->tell();
				size_t amountRead = pos1 - pos0;
				if( 0 == amountRead ) {
					break;
				}
				totalBytesRead += amountRead;
				ret = mpg123_decode( handle, reinterpret_cast<const unsigned char*>( buf ), amountRead, nullptr, 0, nullptr );
			}
			while( ( MPG123_NEED_MORE == ret ) && ( totalBytesRead < 64*1024 ) );

			long rate = 0;
			int channels = 0;
			int encodings = -1;
			if( ( MPG123_NEW_FORMAT == ret ) && ( MPG123_OK == mpg123_getformat( handle, &rate, &channels, &encodings ) ) ) {
				mSampleRate = rate;
				mNumChannels = channels;
				mEncodings = encodings;
				mpg123_delete( handle );
			}
			else {
				mpg123_delete( handle );
				throw AudioFileExc( "Could not determine MP3 format" );
			}

			// Reset the stream once we're done
			mStream->seekAbsolute( 0 );

			switch( mEncodings ) {
				case MPG123_ENC_8			: std::cout << "MPG123_ENC_8" << std::endl; break;
				case MPG123_ENC_16			: std::cout << "MPG123_ENC_16" << std::endl; break;
				case MPG123_ENC_24			: std::cout << "MPG123_ENC_24" << std::endl; break;
				case MPG123_ENC_32			: std::cout << "MPG123_ENC_32" << std::endl; break;
				case MPG123_ENC_SIGNED		: std::cout << "MPG123_ENC_SIGNED" << std::endl; break;
				case MPG123_ENC_FLOAT		: std::cout << "MPG123_ENC_FLOAT" << std::endl; break;
				case MPG123_ENC_SIGNED_16	: std::cout << "MPG123_ENC_SIGNED_16" << std::endl; break;
				case MPG123_ENC_UNSIGNED_16	: std::cout << "MPG123_ENC_UNSIGNED_16" << std::endl; break;
				case MPG123_ENC_UNSIGNED_8	: std::cout << "MPG123_ENC_UNSIGNED_8" << std::endl; break;
				case MPG123_ENC_SIGNED_8	: std::cout << "MPG123_ENC_SIGNED_8" << std::endl; break;
				case MPG123_ENC_ULAW_8		: std::cout << "MPG123_ENC_ULAW_8" << std::endl; break;
				case MPG123_ENC_ALAW_8		: std::cout << "MPG123_ENC_ALAW_8" << std::endl; break;
				case MPG123_ENC_SIGNED_32	: std::cout << "MPG123_ENC_SIGNED_32" << std::endl; break;
				case MPG123_ENC_UNSIGNED_32	: std::cout << "MPG123_ENC_UNSIGNED_32" << std::endl; break;
				case MPG123_ENC_SIGNED_24	: std::cout << "MPG123_ENC_SIGNED_24" << std::endl; break;
				case MPG123_ENC_UNSIGNED_24	: std::cout << "MPG123_ENC_UNSIGNED_24" << std::endl; break;
				case MPG123_ENC_FLOAT_32	: std::cout << "MPG123_ENC_FLOAT_32" << std::endl; break;
				case MPG123_ENC_FLOAT_64	: std::cout << "MPG123_ENC_FLOAT_64" << std::endl; break;
				case MPG123_ENC_ANY			: std::cout << "MPG123_ENC_ANY" << std::endl; break;
			}			
		}
	}

	// Now open it for actual porcessing
	int ret = MPG123_OK;
	mHandle = mpg123_new( nullptr, &ret );
	if( MPG123_OK != ret ) {
		throw AudioFileExc( "Could not allocate mpg123 handle" );
	}

	// Helps with determining track length
	mpg123_set_filesize( mHandle, static_cast<off_t>( mStream->size() ) );

	if( MPG123_OK != mpg123_open_feed( mHandle ) ) {
		mpg123_delete( mHandle );
		throw AudioFileExc( "Could not open feed for stream" );
	}
}

FileLoaderMpg123::~FileLoaderMpg123()
{
	if( nullptr != mHandle ) {
		mpg123_delete( mHandle );
	}
}

size_t FileLoaderMpg123::getNumFrames() const
{
	size_t result = 0;
	if( nullptr != mHandle) {
		off_t len = static_cast<size_t>( mpg123_length( mHandle ) );
		std::cout << "len: "<< len << std::endl;
		if( MPG123_ERR != len ) {
			result = len;
		}
	}
	return result;
}

//! \class FileLoaderSndFile
//!
//!
class FileLoaderSndFile : public FileLoader {
  public:
	FileLoaderSndFile( SourceFileAudioLoader* cinderParent, const ci::IStreamRef& stream );
	virtual ~FileLoaderSndFile();

	//! Returns the length in frames.
	virtual size_t	getNumFrames() const override { return 0; }

  private:
};

FileLoaderSndFile::FileLoaderSndFile( SourceFileAudioLoader* cinderParent, const ci::IStreamRef& stream )
	: FileLoader( cinderParent, stream )
{
}

FileLoaderSndFile::~FileLoaderSndFile()
{
}

} // namespace audioloader

namespace cinder { namespace audio { namespace linux {

SourceFileAudioLoader::SourceFileAudioLoader()
	: SourceFile( 0 )
{
}

SourceFileAudioLoader::SourceFileAudioLoader( const DataSourceRef &dataSource, size_t sampleRate )
	: SourceFile( sampleRate ), mDataSource( dataSource )
{
	init();
}

SourceFileAudioLoader::~SourceFileAudioLoader()
{
}

void SourceFileAudioLoader::init()
{
	if( mDataSource->isFilePath() ) {
		std::cout << "Loading into stream: " << mDataSource->getFilePath().string() << std::endl;
		mStream = ci::loadFileStream( mDataSource->getFilePath() );
	}
	else {
		mStream = mDataSource->createStream();
	}

	audioloader::FileType fileType = audioloader::determineFileType( mStream );

	switch( fileType ) {
		case audioloader::FileType::UNKNOWN:
			std::cout << "fileType: UNKNOWN" << std::endl;
		break;

		case audioloader::FileType::MP3:
			std::cout << "fileType: MP3" << std::endl;
		break;

		case audioloader::FileType::PCM:
			std::cout << "fileType: PCM" << std::endl;
		break;
	}

	if( audioloader::FileType::MP3 == fileType ) {
		mFileLoader.reset( new audioloader::FileLoaderMpg123( this, mStream ) );
	}
	else if( audioloader::FileType::PCM == fileType ) {
		mFileLoader.reset( new audioloader::FileLoaderSndFile( this, mStream ) );
	}
	else {
		if( mDataSource->isFilePath() ) {
			throw AudioFileExc( "Unable to determine file type for " + mDataSource->getFilePath().string()  );
		}
		else {
			throw AudioFileExc( "Unable to determine file type from stream" );
		}
	}

	mNumFrames = mFileLoader->getNumFrames();

	std::cout << "Sample rate  : " << getSampleRateNative() << std::endl;
	std::cout << "Num channels : " << getNumChannels() << std::endl;
	std::cout << "Num frames   : " << getNumFrames() << std::endl;
	std::cout << "Num seconds  : " << getNumSeconds() << std::endl;
}

SourceFileRef SourceFileAudioLoader::cloneWithSampleRate( size_t sampleRate ) const
{
	SourceFileRef result;
	return result;
}

size_t SourceFileAudioLoader::getNumChannels() const
{
	return mFileLoader->getNumChannels();
}

size_t SourceFileAudioLoader::getSampleRateNative() const
{
	return mFileLoader->getSampleRateNative();
}

size_t SourceFileAudioLoader::performRead( Buffer *buffer, size_t bufferFrameOffset, size_t numFramesNeeded )
{
	return 0;
}

void SourceFileAudioLoader::performSeek( size_t readPositionFrames )
{

}

std::vector<std::string> SourceFileAudioLoader::getSupportedExtensions()
{
	std::vector<std::string> result;
	return result;
}

} } } // namespace cinder::audio::linux