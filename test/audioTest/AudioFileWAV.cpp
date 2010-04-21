#include "AudioFileWAV.h"
#include <cstring>
#include <cmath>

namespace fli {

shared_ptr<AudioFileWAV> loadAudio( const std::string &path )
{
	shared_ptr<IStreamFile> IStream( loadFile( path ) );
	return loadAudio( IStream );
}

shared_ptr<AudioFileWAV> loadAudio( shared_ptr<IStream> stream )
{
	return shared_ptr<AudioFileWAV>( new AudioFileWAV( stream ) );
}
	
AudioFileWAV::AudioFileWAV( shared_ptr<IStream> aStream ) : mStream( aStream )
{
	loadHeader();
}

AudioFileWAV::~AudioFileWAV()
{
	if( mData ) {
		free( mData );
	}
}

void AudioFileWAV::load( char * data, int length, int offset )
{
	mStream->seekSet( mDataStart + offset );
	mStream->readData( data, length );
}

double AudioFileWAV::getDuration() const
{
	return (double)mDataLength / ( ( mSampleRate * mBitsPerSample ) / 8 );
}

void AudioFileWAV::loadHeader()
{
	uint32_t fileSize = 0;
	
	char chunkName[5] = { 0 };
	uint32_t chunkSize = 0;
	
	char riffType[5] = { 0 };
	
	mStream->readData( chunkName, 4 );
	mStream->readLittle( &fileSize );
	fileSize = fileSize + 4 + sizeof( int );
	
	mStream->readData( riffType, 4 );
	if( strcmp( "RIFF", chunkName ) != 0 || strcmp( "WAVE", riffType ) != 0  ) {
		throw AudioFileInvalidExc();
	}
	
	
	uint32_t chunkEnd = 0;
	
	while( mStream->tell() < fileSize) {
		mStream->readData( chunkName, 4 );
		mStream->readLittle( &chunkSize );
		chunkEnd = mStream->tell() + chunkSize;
		
		std::cout << chunkName << ":" << chunkSize << std::endl;
		
		if( strcmp( "fmt ", chunkName ) == 0 ) {
			readFormatChunk();
		} else if( strcmp( "data", chunkName ) == 0 ) {
			mDataLength = chunkSize;
			mDataStart = mStream->tell();
		}
		 
		mStream->seekSet( chunkEnd );
	}
	
	mSampleCount = mDataLength / ( mBitsPerSample / 8 );
	
	//Pull all of the data
	mData = (int16_t *)calloc( 1, mDataLength );
	mStream->seekSet( mDataStart );
	mStream->readData( mData, mDataLength );
}

void AudioFileWAV::readFormatChunk()
{
	mAudioFormat = 0;
	mChannelCount = 0;
	mSampleRate = 0;
	mByteRate = 0;
	mBlockAlign = 0;
	mBitsPerSample = 0;

	//load actual data
	mStream->readLittle( &mAudioFormat );
	mStream->readLittle( &mChannelCount );
	mStream->readLittle( &mSampleRate );
	mStream->readLittle( &mByteRate );
	mStream->readLittle( &mBlockAlign );
	mStream->readLittle( &mBitsPerSample );
}

void AudioFileWAV::getData( void ** aBuffer, uint32_t * size, uint32_t startSample, uint32_t sampleCount )
{
	//std::cout << "Render Callback : " << startSample << ", " << sampleCount << std::endl;
 	//float ** buffers = (float **)aBuffer;
	//int sampleIdx = startSample;
	
	//float divisor = ( pow( 2.0f, mBitsPerSample ) / 2 )  - 1;
	if( startSample < mSampleCount ) {
		if( startSample + sampleCount > mSampleCount ) {
			sampleCount = mSampleCount - startSample;
		}
	} else {
		sampleCount = 0;
	}
	
	int bytesToCopy = sampleCount * mBlockAlign;
	
	//mStream->seekSet( mDataStart + startSample );
	if( sampleCount ) {
		*aBuffer = (void *) calloc( 1, bytesToCopy );
		//mStream->readData( *aBuffer, bytesToCopy );
		memcpy( *aBuffer, mData + startSample, bytesToCopy );
		*size = bytesToCopy;
	} else {
		*size = 0;
		aBuffer = NULL;
	}
	
	
	//mStream->readData( &aBuffer[0], mBitsPerSample * sampleCount );
    /*for ( int i = 0; i < sampleCount; i++ ){
		
		for( int j = 0; j < mChannelCount; j++ ) {
			//buffers[j][i] = sin( twoPi * now * mult );
			if( sampleIdx < mSampleCount ) {
				//int16_t pt = 0;
				//mStream->readLittle( &pt );
				int8_t pt = 0;
				mStream->read( &pt );
				
				buffers[j][i] = ( pt / divisor );
				//std::cout << sampleIdx << " : " << pt << " : " << buffers[j][i] << std::endl;
				
			} else {
				buffers[j][i] = 0.0;
			}
			sampleIdx += 1;
			
		}
    }*/
}


} //namespace