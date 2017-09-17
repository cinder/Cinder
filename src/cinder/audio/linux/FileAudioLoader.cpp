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
#include "cinder/audio/dsp/Converter.h"
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

// ----------------------------------------------------------------------------------------------------
// ProcessScopedMpg123
// ----------------------------------------------------------------------------------------------------
class ProcessScopedMpg123 {
public:
	~ProcessScopedMpg123() {
		mpg123_exit();
//std::cout << "ProcessScopedMpg123::~ProcessScopedMpg123()" << std::endl;
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
//std::cout << "ProcessScopedMpg123::ProcessScopedMpg123()" << std::endl;
	}

	static std::unique_ptr<ProcessScopedMpg123> sInstance;
};

std::unique_ptr<ProcessScopedMpg123> ProcessScopedMpg123::sInstance;

// ----------------------------------------------------------------------------------------------------
// IStreamSndFile
// ----------------------------------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------------------------------
// IStreamMpg123
// ----------------------------------------------------------------------------------------------------
struct IStreamMpg123 {
	static ssize_t read( void* userData, void* ptr, size_t count ) {
		ci::IStreamCinder* stream = static_cast<ci::IStreamCinder*>( userData );
		size_t pos0 = stream->tell();
		stream->readData( ptr, count );
		size_t pos1 = stream->tell();
		return pos1 - pos0;
	}

	static off_t seek( void* userData, off_t offset, int whence ) {
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

	// NOTE: 'cleanup' is not overridden, pass in nullptr to mpg123_replace_reader_handle to
	//       use the default one.
};

//!
FileType determineFileType( const std::string &extension, const ci::IStreamRef& stream )
{
	FileType result = FileType::UNKNOWN;

	// Try MP3 first
	if( extension == ".mp3" ) {
		ProcessScopedMpg123::initialize();

		stream->seekAbsolute( 0 );

		int ret = MPG123_OK;
		mpg123_handle* handle = mpg123_new( nullptr, &ret );
		if( ( MPG123_OK == ret ) && ( MPG123_OK == mpg123_replace_reader_handle( handle, IStreamMpg123::read, IStreamMpg123::seek, nullptr ) ) ) {
			if( MPG123_OK == mpg123_open_handle( handle, stream.get() ) ) {
				long rate = 0;
				int channels = 0;
				int encodings = -1;
				off_t len = MPG123_ERR;
				if( ( MPG123_OK == mpg123_getformat( handle, &rate, &channels, &encodings ) ) && ( len =mpg123_length( handle ) ) ) {
					if( ( MPG123_ERR != len ) && ( len > 0 ) ) {
						result = FileType::MP3;
					}
				}
			}		
			mpg123_delete( handle );
		}
	}

	// If not an .mp3, try libsndfile
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

	stream->seekAbsolute( 0 );

	return result;	
}

// ----------------------------------------------------------------------------------------------------
// FileLoader
// ----------------------------------------------------------------------------------------------------
class FileLoader {
  public:
	FileLoader( SourceFileAudioLoader* cinderParent, const ci::IStreamRef& stream );
	virtual ~FileLoader();

	size_t				getNumChannels() const { return mNumChannels; }
	size_t				getSampleRateNative() const  { return mSampleRate; }

	//! Returns the length in frames.
	virtual size_t		getNumFrames() const = 0;
	//! Reads frameCount (or less) frames into buffer. Returns the actual number of frames read
	virtual size_t 		read( void* buffer, size_t frameCount ) = 0;
	//! Seeks to the absolute position specified by readPositionFrames
	virtual void 		seek( int readPositionFrames ) = 0;

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

// ----------------------------------------------------------------------------------------------------
// FileLoaderMpg123
// ----------------------------------------------------------------------------------------------------
class FileLoaderMpg123 : public FileLoader {
  public:
	FileLoaderMpg123( SourceFileAudioLoader* cinderParent, const ci::IStreamRef& stream );
	virtual ~FileLoaderMpg123();

	//! Returns the length in frames.
	virtual size_t		getNumFrames() const override;
	//! Reads frameCount (or less) frames into buffer. Returns the actual number of frames read
	virtual size_t 		read( void* buffer, size_t frameCount ) override;
	//! Seeks to the absolute position specified by readPositionFrames
	virtual void 		seek( int readPositionFrames ) override;

  private:
  	mpg123_handle*	mHandle = nullptr;
  	int 			mEncodings = -1;
};

FileLoaderMpg123::FileLoaderMpg123( SourceFileAudioLoader* cinderParent, const ci::IStreamRef& stream )
	: FileLoader( cinderParent, stream )
{
	int ret = MPG123_OK;
	mHandle = mpg123_new( nullptr, &ret );
	if( MPG123_OK == ret ) {
		if( MPG123_OK != mpg123_replace_reader_handle( mHandle, IStreamMpg123::read, IStreamMpg123::seek, nullptr ) ) {
			throw AudioFileExc( "Could not replace mpg123 reader handle" );
		}

		if( MPG123_OK != mpg123_param( mHandle, MPG123_ADD_FLAGS, MPG123_FORCE_FLOAT, 0 ) ) {
			mpg123_delete( mHandle );
			throw AudioFileExc( "Could not force float output" );					
		}

		if( MPG123_OK != mpg123_open_handle( mHandle, mStream.get() ) ) {
			mpg123_delete( mHandle );
			throw AudioFileExc( "Could not determine MP3 format: failed to open handle" );		
		}

		long rate = 0;
		int channels = 0;
		int encodings = -1;
		if( MPG123_OK != mpg123_getformat( mHandle, &rate, &channels, &encodings ) ) {
			mpg123_delete( mHandle );
			throw AudioFileExc( "Could not determine MP3 format: failed to get rate, channel and encoding" );
		}		

		mSampleRate = rate;
		mNumChannels = channels;
		mEncodings = encodings;

		/*
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
		*/		
	}

	mBytesPerSample = sizeof( float );
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
		// mpg123_length seems to return the number of frames - not samples as stated in the docs.
		off_t len = mpg123_length( mHandle );
		if( MPG123_ERR != len ) {
			result = len ;
		}
	}
	return result;
}

size_t FileLoaderMpg123::read( void* buffer, size_t frameCount )
{
	if( nullptr == mHandle ) {
		// @TODO: Should probably throw here?
		return 0;
	}

	const size_t byteCount = frameCount * getNumChannels() * mBytesPerSample;

	size_t numDecodedBytes = 0;
	int ret = mpg123_read( mHandle, static_cast<unsigned char*>( buffer ), byteCount, &numDecodedBytes );
	if( MPG123_OK != ret ) {
		// Handle error
	}

	size_t numDecodedFrames = numDecodedBytes / (getNumChannels() * mBytesPerSample);
	return numDecodedFrames;
}

void FileLoaderMpg123::seek( int readPositionFrames )
{
	if( nullptr == mHandle ) {
		return;
	}

	off_t sampleoff = readPositionFrames*getNumChannels();
	mpg123_seek( mHandle, sampleoff, SEEK_SET );
}

// ----------------------------------------------------------------------------------------------------
// FileLoaderSndFile
// ----------------------------------------------------------------------------------------------------
class FileLoaderSndFile : public FileLoader {
  public:
	FileLoaderSndFile( SourceFileAudioLoader* cinderParent, const ci::IStreamRef& stream );
	virtual ~FileLoaderSndFile();

	//! Returns the length in frames.
	virtual size_t		getNumFrames() const override;
	//! Reads frameCount (or less) frames into buffer. Returns the actual number of frames read
	virtual size_t 		read( void* buffer, size_t frameCount ) override;
	//! Seeks to the absolute position specified by readPositionFrames
	virtual void 		seek( int readPositionFrames ) override;

  private:
  	SF_INFO		mInfo;
  	SNDFILE*	mHandle = nullptr;
};

FileLoaderSndFile::FileLoaderSndFile( SourceFileAudioLoader* cinderParent, const ci::IStreamRef& stream )
	: FileLoader( cinderParent, stream )
{
	SF_VIRTUAL_IO vio;
	vio.get_filelen	= IStreamSndFile::get_filelen;
	vio.seek		= IStreamSndFile::seek;
	vio.read		= IStreamSndFile::read;
	vio.write		= IStreamSndFile::write;
	vio.tell		= IStreamSndFile::tell;

	std::memset( &mInfo, 0, sizeof( SF_INFO ) );
	mHandle = sf_open_virtual( &vio, SFM_READ, &mInfo, static_cast<void*>( stream.get() ) );
	if( nullptr == mHandle ) {
		throw AudioFileExc( "Could not open handle for sndfile stream" );
	}

	mSampleRate = mInfo.samplerate;
	mNumChannels = mInfo.channels;
}

FileLoaderSndFile::~FileLoaderSndFile()
{
	if( nullptr != mHandle ) {
		sf_close( mHandle );
		mHandle = nullptr;
	}
}

size_t FileLoaderSndFile::getNumFrames() const
{
	return mInfo.frames;
}

size_t FileLoaderSndFile::read( void* buffer, size_t frameCount )
{
	if( nullptr == mHandle ) {
		return 0;
	}

	size_t numFramesRead = sf_readf_float( mHandle, static_cast<float*>( buffer ), frameCount );
	int errorNum = sf_error( mHandle );
	if( SF_ERR_NO_ERROR != errorNum ) {
		throw AudioFileExc( std::string( "sndfile I/O error: " ) + sf_error_number( errorNum ) );
	}

	return numFramesRead;
}

void FileLoaderSndFile::seek( int readPositionFrames )
{
	if( nullptr == mHandle ) {
		return;
	}

	sf_seek( mHandle, readPositionFrames, SEEK_SET );
}

} // namespace audioloader

namespace cinder { namespace audio { namespace linux {

// ----------------------------------------------------------------------------------------------------
// SourceFileAudioLoader
// ----------------------------------------------------------------------------------------------------
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
		//std::cout << "Loading into stream: " << mDataSource->getFilePath().string() << std::endl;
		mStream = ci::loadFileStream( mDataSource->getFilePath() );
	}
	else {
		mStream = mDataSource->createStream();
	}

	auto extension = mDataSource->getFilePathHint().extension().string();
	audioloader::FileType fileType = audioloader::determineFileType( extension, mStream );

	/*
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
	*/

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

	mNumFrames = mFileNumFrames = mFileLoader->getNumFrames();

/*
	std::cout << "Sample rate  : " << getSampleRateNative() << std::endl;
	std::cout << "Num channels : " << getNumChannels() << std::endl;
	std::cout << "Num frames   : " << getNumFrames() << std::endl;
	std::cout << "Num seconds  : " << getNumSeconds() << std::endl;
*/

	mAudioData.setSize( getMaxFramesPerRead(), mFileLoader->getNumChannels() );
}

SourceFileRef SourceFileAudioLoader::cloneWithSampleRate( size_t sampleRate ) const
{
	auto result = std::make_shared<SourceFileAudioLoader>( mDataSource, sampleRate );
	result->setupSampleRateConversion();

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
	CI_ASSERT( buffer->getNumFrames() >= bufferFrameOffset + numFramesNeeded );
//std::cout << "numFramesNeeded: " << numFramesNeeded << std::endl;

	size_t readCount = 0;

	const size_t numChannels = mFileLoader->getNumChannels();

	mReadBuffer.setSize( getMaxFramesPerRead(), numChannels );

	while( readCount < numFramesNeeded ) {
		// Read the audio audio data
		const size_t maxFrames = std::min<size_t>( mAudioData.getNumFrames(), getNumFrames() - readCount );
		size_t numFramesRead = mFileLoader->read( mAudioData.getData(), maxFrames );
		if( 0 == numFramesRead ) {
			break;
		}

		// Deinterleave the audio data
		if( 1 == numChannels ) {
			std::memcpy( mReadBuffer.getData(), mAudioData.getData(), numFramesRead * sizeof( float ) );
		}
		else {
			dsp::deinterleave( mAudioData.getData(), mReadBuffer.getData(), mReadBuffer.getNumFrames(), numChannels, numFramesRead );
		}

		// Copy to channels
		for( size_t ch = 0; ch < numChannels; ch++ ) {
			float *readChannel = mReadBuffer.getChannel( ch );
			float *resultChannel = buffer->getChannel( ch );
			std::memcpy( resultChannel + readCount, readChannel, numFramesRead * sizeof( float ) );
		}		

		readCount += numFramesRead;
	}
	
//std::cout << "readCount: " << readCount << std::endl;	
	return static_cast<size_t>( readCount );
}

void SourceFileAudioLoader::performSeek( size_t readPositionFrames )
{
	mFileLoader->seek( readPositionFrames );
}

std::vector<std::string> SourceFileAudioLoader::getSupportedExtensions()
{
	std::vector<std::string> result;
	return result;
}

} } } // namespace cinder::audio::linux