#include "AudioFile.h"
#include <cstring>
#include <cmath>

namespace fli {

/*shared_ptr<AudioFile> loadAudio( const std::string &path )
{
	shared_ptr<IStreamFile> IStream( loadFile( path ) );
	return loadAudio( IStream );
}

shared_ptr<AudioFile> loadAudio( shared_ptr<IStream> stream )
{
	return shared_ptr<AudioFile>( new AudioFileWAV( stream ) );
}*/
	
/*AudioFile::AudioFile( shared_ptr<IStream> aStream ) : mStream( aStream )
{
	setup();
}*/
AudioFile::AudioFile( const std::string &aFilePath )
{
#if defined( FLI_MAC )
	FSRef fsref;
	OSStatus fileRefError = FSPathMakeRef( (const UInt8 *)aFilePath.c_str(), &fsref, NULL );
	if( fileRefError ) {
		//handle error
		std::cout << "Input file not found" << std::endl;
		return;
	}
	
	OSStatus audioFileOpenError = AudioFileOpen(&fsref, fsRdPerm, 0, &mNativeFileRef);
	if( audioFileOpenError ) {
		//handle error
		std::cout << "AudioFileOpen failed" << std::endl;
		return;
	}
	
	loadHeader();
	load();
#endif
}

AudioFile::~AudioFile()
{
	if( mData ) {
		free( mData );
	}
}

/*void AudioFile::load( char * data, int length, int offset )
{
	mStream->seekSet( mDataStart + offset );
	mStream->readData( data, length );
}*/

void AudioFile::load()
{
	OSStatus err = noErr;
	
	uint32_t bytesReturned = 0;
	uint32_t packets = mPacketCount;
	
	mData = malloc( mByteCount );
	memset( mData, 0, mByteCount );
	
	err = AudioFileReadPackets( mNativeFileRef, false, (UInt32 *)&bytesReturned, NULL, 0, (UInt32 *)&packets, mData );
	if( err ) {
		std::cout << "Error loading audio data" << std::endl;
	}
}

double AudioFile::getDuration() const
{
	//TODO
	return 0.0;
	//return (double)mDataLength / ( ( mSampleRate * mBitsPerSample ) / 8 );
}

void AudioFile::loadHeader()
{
#if defined( FLI_MAC )
	OSStatus err = noErr;
	AudioStreamBasicDescription nativeFormatDescription;
	UInt32 size = sizeof( AudioStreamBasicDescription );
	err = AudioFileGetProperty( mNativeFileRef, kAudioFilePropertyDataFormat, &size, &nativeFormatDescription );
	if( err ) {
		std::cout << "error getting file data format" << std::endl;
	}
	mSampleRate = nativeFormatDescription.mSampleRate;
	mNativeFormatID = nativeFormatDescription.mFormatID;
	mNativeFormatFlags = nativeFormatDescription.mFormatFlags;
	mBytesPerPacket = nativeFormatDescription.mBytesPerPacket;
	mFramesPerPacket = nativeFormatDescription.mFramesPerPacket;
	mBytesPerFrame = nativeFormatDescription.mBytesPerFrame;
	mChannelCount = nativeFormatDescription.mChannelsPerFrame;
	mBitsPerSample = nativeFormatDescription.mBitsPerChannel;
	mReserved = nativeFormatDescription.mReserved;
	
	size = sizeof( uint64_t );
	err = AudioFileGetProperty( mNativeFileRef, kAudioFilePropertyAudioDataPacketCount, &size, &mPacketCount );
	if( err ) {
		std::cout << "error getting file packet count" << std::endl;
	}
	
	size = sizeof( uint64_t );
	err = AudioFileGetProperty( mNativeFileRef, kAudioFilePropertyAudioDataByteCount, &size, &mByteCount );
	if( err ) {
		std::cout << "error getting file byte count" << std::endl;
	}
	
	size = sizeof( uint32_t );
	err = AudioFileGetProperty( mNativeFileRef, kAudioFilePropertyMaximumPacketSize, &size, &mMaxPacketSize );
	if( err ) {
		std::cout << "error getting file max packet size count" << std::endl;
	}
#endif
}

void AudioFile::getData( void ** aBuffer, uint32_t * size, AudioStreamPacketDescription * outPacketDescriptions, uint32_t startPacket, uint32_t packetCount, bool looping )
{
	
	if( startPacket < mPacketCount ) {
		if( ( startPacket + packetCount ) > mPacketCount ) {
			packetCount = mPacketCount - startPacket;
		}
	} else {
		packetCount = 0;
	}
	
	UInt32 bytesToCopy = packetCount * mMaxPacketSize;
	
	if( packetCount ) {
		*aBuffer = (void *) calloc( 1, bytesToCopy );
		AudioFileReadPackets( mNativeFileRef, false, &bytesToCopy, outPacketDescriptions, (SInt64)startPacket, (UInt32 *)&packetCount, *aBuffer );
		*size = bytesToCopy;
	} else {
		*size = 0;
		aBuffer = NULL;
	}
}


} //namespace