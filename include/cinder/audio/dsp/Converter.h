/*
 Copyright (c) 2014, The Cinder Project

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

#pragma once

#include "cinder/Export.h"
#include "cinder/audio/Buffer.h"

#include <memory>
#include <algorithm>

namespace cinder { namespace audio { namespace dsp {

//! A platform-specific converter that supports samplerate and channel conversion.
class CI_API Converter {
  public:
	//! If \a destSampleRate is 0, it is set to match \a sourceSampleRate. If \a destNumChannels is 0, it is set to match \a sourceNumChannels.
	static std::unique_ptr<Converter> create( size_t sourceSampleRate, size_t destSampleRate, size_t sourceNumChannels, size_t destNumChannels, size_t sourceMaxFramesPerBlock );

	virtual ~Converter() {}

	//! Converts up to getSourceMaxFramesPerBlock() frames of audio data from \a sourceBuffer into \a destBuffer. Returns a \a std::pair<num source frames used, num dest frames written>
	//! \note destBuffer must be large enough to complete the conversion, which is calculated as: \code minNumDestFrames = min( sourceBuffer->getNumFrames, getSourceMaxFramesPerBlock() ) * getDestSampleRate() / getSourceSampleRate() \endcode
	virtual std::pair<size_t, size_t> convert( const Buffer *sourceBuffer, Buffer *destBuffer ) = 0;

	//! Clears the state of the converter, discarding / flushing accumulated samples. Optional for implementations.
	virtual void clear()	{}

	size_t getSourceSampleRate()		const		{ return mSourceSampleRate; }
	size_t getDestSampleRate()			const		{ return mDestSampleRate; }
	size_t getSourceNumChannels()		const		{ return mSourceNumChannels; }
	size_t getDestNumChannels()			const		{ return mDestNumChannels; }
	size_t getSourceMaxFramesPerBlock()	const		{ return mSourceMaxFramesPerBlock; }
	size_t getDestMaxFramesPerBlock()	const		{ return mDestMaxFramesPerBlock; }

  protected:
	Converter( size_t sourceSampleRate, size_t destSampleRate, size_t sourceNumChannels, size_t destNumChannels, size_t sourceMaxFramesPerBlock );

	size_t mSourceSampleRate, mDestSampleRate, mSourceNumChannels, mDestNumChannels, mSourceMaxFramesPerBlock, mDestMaxFramesPerBlock;
};

//! Mixes \a numFrames frames of \a sourceBuffer to \a destBuffer's layout, replacing its content. Channel up or down mixing is applied if necessary.
CI_API void mixBuffers( const Buffer *sourceBuffer, Buffer *destBuffer, size_t numFrames );
//! Mixes \a sourceBuffer to \a destBuffer's layout, replacing its content. Channel up or down mixing is applied if necessary. Unequal frame counts are permitted (the minimum size will be used).
inline void mixBuffers( const Buffer *sourceBuffer, Buffer *destBuffer )	{ mixBuffers( sourceBuffer, destBuffer, std::min( sourceBuffer->getNumFrames(), destBuffer->getNumFrames() ) ); }

//! Sums \a numFrames frames of \a sourceBuffer into \a destBuffer. Channel up or down mixing is applied if necessary.
CI_API void sumBuffers( const Buffer *sourceBuffer, Buffer *destBuffer, size_t numFrames );
//! Sums \a sourceBuffer into \a destBuffer. Channel up or down mixing is applied if necessary. Unequal frame counts are permitted (the minimum size will be used).
inline void sumBuffers( const Buffer *sourceBuffer, Buffer *destBuffer )	{ sumBuffers( sourceBuffer, destBuffer, std::min( sourceBuffer->getNumFrames(), destBuffer->getNumFrames() ) ); }

//! Converts between two arrays of different precision (ex. float to double). \a length samples are converted.
template <typename SourceT, typename DestT>
void convert( const SourceT *sourceArray, DestT *destArray, size_t length )
{
	for( size_t i = 0; i < length; i++ )
		destArray[i] = static_cast<DestT>( sourceArray[i] );
}

//! Converts a float or double array to int16_t
template<typename FloatT>
void convert( const FloatT *sourceArray, int16_t *destArray, size_t length )
{
	const FloatT intNormalizer = 32768;

	for( size_t i = 0; i < length; i++ )
		destArray[i] = int16_t( sourceArray[i] * intNormalizer );
}

//! Converts an int16_t array to float or double
template<typename FloatT>
void convert( const int16_t *sourceArray, FloatT *destArray, size_t length )
{
	const FloatT floatNormalizer = (FloatT)3.0517578125e-05;	// 1.0 / 32768.0

	for( size_t i = 0; i < length; i++ )
		destArray[i] = (FloatT)sourceArray[i] * floatNormalizer;
}

//! Converts between two BufferT's of different precision (ex. float to double).  The number of frames converted is the lesser of the two. The number of channels converted is the lesser of the two.
template <typename SourceT, typename DestT>
void convertBuffer( const BufferT<SourceT> *sourceBuffer, BufferT<DestT> *destBuffer )
{
	size_t numFrames = std::min( sourceBuffer->getNumFrames(), destBuffer->getNumFrames() );
	size_t numChannels = std::min( sourceBuffer->getNumChannels(), destBuffer->getNumChannels() );

	for( size_t ch = 0; ch < numChannels; ch++ )
		convert( sourceBuffer->getChannel( ch ), destBuffer->getChannel( ch ), numFrames );
}

//! Converts the 24-bit int \a sourceArray to floating point precision, placing the result in \a destArray. \a length samples are converted.
template<typename FloatT>
void convertInt24ToFloat( const char *sourceArray, FloatT *destArray, size_t length )
{
	const FloatT floatNormalizer = (FloatT)1 / (FloatT)8388607;

	for( size_t i = 0; i < length; i++ ) {
		int32_t sample = (int32_t)( ( (int32_t)sourceArray[2] ) << 16 ) | ( ( (int32_t)(uint8_t)sourceArray[1] ) << 8 ) | ( (int32_t)(uint8_t)sourceArray[0] );
		destArray[i] = (FloatT)sample * floatNormalizer;
		sourceArray += 3;
	}
}

//! Converts the floating point \a sourceArray to 24-bit int precision, placing the result in \a destArray. \a length samples are converted.
template<typename FloatT>
void convertFloatToInt24( const FloatT *sourceArray, char *destArray, size_t length )
{
	const FloatT intNormalizer = 8388607;

	for( size_t i = 0; i < length; i++ ) {
		int32_t sample = int32_t( sourceArray[i] * intNormalizer );
		*(destArray++) = (char)( sample & 255 );
		*(destArray++) = (char)( ( sample >> 8 ) & 255 );
		*(destArray++) = (char)( ( sample >> 16 ) & 255 );
	}
}

//! Interleaves \a numCopyFrames of \a nonInterleavedSourceArray, placing the result in \a interleavedDestArray. \a numFramesPerChannel and \a numChannels describe the layout of the non-interleaved array.
template<typename T>
void interleave( const T *nonInterleavedSourceArray, T *interleavedDestArray, size_t numFramesPerChannel, size_t numChannels, size_t numCopyFrames )
{
	for( size_t ch = 0; ch < numChannels; ch++ ) {
		size_t x = ch;
		const T *sourceChannel = &nonInterleavedSourceArray[ch * numFramesPerChannel];
		for( size_t i = 0; i < numCopyFrames; i++ ) {
			interleavedDestArray[x] = sourceChannel[i];
			x += numChannels;
		}
	}
}

//! Interleaves \a numCopyFrames of \a nonInterleavedFloatSourceArray and converts from floating point to 16-bit int precision at the same time, placing the result in \a interleavedInt16DestArray. \a numFramesPerChannel and \a numChannels describe the layout of the non-interleaved array.
template<typename FloatT>
void interleave( const FloatT *nonInterleavedFloatSourceArray, int16_t *interleavedInt16DestArray, size_t numFramesPerChannel, size_t numChannels, size_t numCopyFrames )
{
	const FloatT intNormalizer = 32768;

	for( size_t ch = 0; ch < numChannels; ch++ ) {
		size_t x = ch;
		const FloatT *sourceChannel = &nonInterleavedFloatSourceArray[ch * numFramesPerChannel];
		for( size_t i = 0; i < numCopyFrames; i++ ) {
			interleavedInt16DestArray[x] = int16_t( sourceChannel[i] * intNormalizer );
			x += numChannels;
		}
	}
}

//! De-interleaves \a numCopyFrames of \a interleavedSourceArray, placing the result in \a nonInterleavedDestArray. \a numFramesPerChannel and \a numChannels describe the layout of the non-interleaved array.
template<typename T>
void deinterleave( const T *interleavedSourceArray, T *nonInterleavedDestArray, size_t numFramesPerChannel, size_t numChannels, size_t numCopyFrames )
{
	for( size_t ch = 0; ch < numChannels; ch++ ) {
		size_t x = ch;
		T *destChannel = &nonInterleavedDestArray[ch * numFramesPerChannel];
		for( size_t i = 0; i < numCopyFrames; i++ ) {
			destChannel[i] = interleavedSourceArray[x];
			x += numChannels;
		}
	}
}

//! De-interleaves \a numCopyFrames of \a interleavedInt16SourceArray and converts from 16-bit int to floating point precision at the same time, placing the result in \a nonInterleavedFloatDestArray. \a numFramesPerChannel and \a numChannels describe the layout of the non-interleaved array.
template<typename FloatT>
void deinterleave( const int16_t *interleavedInt16SourceArray, FloatT *nonInterleavedFloatDestArray, size_t numFramesPerChannel, size_t numChannels, size_t numCopyFrames )
{
	const FloatT floatNormalizer = (FloatT)3.0517578125e-05;	// 1.0 / 32768.0

	for( size_t ch = 0; ch < numChannels; ch++ ) {
		size_t x = ch;
		FloatT *destChannel = &nonInterleavedFloatDestArray[ch * numFramesPerChannel];
		for( size_t i = 0; i < numCopyFrames; i++ ) {
			destChannel[i] = (FloatT)interleavedInt16SourceArray[x] * floatNormalizer;
			x += numChannels;
		}
	}
}

//! De-interleaves \a numCopyFrames of \a interleavedInt24SourceArray and converts from 24-bit int to floating point precision at the same time, placing the result in \a nonInterleavedFloatDestArray. \a numFramesPerChannel and \a numChannels describe the layout of the non-interleaved array.
template<typename FloatT>
void deinterleaveInt24ToFloat( const char *interleavedInt24SourceArray, FloatT *nonInterleavedFloatDestArray, size_t numFramesPerChannel, size_t numChannels, size_t numCopyFrames )
{
	const FloatT floatNormalizer = (FloatT)1 / (FloatT)8388607;

	for( size_t ch = 0; ch < numChannels; ch++ ) {
		size_t x = ch;
		FloatT *destChannel = &nonInterleavedFloatDestArray[ch * numFramesPerChannel];
		for( size_t i = 0; i < numCopyFrames; i++ ) {
			int32_t sample = (int32_t)( ( (int32_t)interleavedInt24SourceArray[2] ) << 16 ) | ( ( (int32_t)(uint8_t)interleavedInt24SourceArray[1] ) << 8 ) | ( (int32_t)(uint8_t)interleavedInt24SourceArray[0] );
			destChannel[i] = (FloatT)sample * floatNormalizer;
			x += numChannels;
		}
	}
}
//! Interleaves \a nonInterleavedSource, placing the result in \a interleavedDest.
template<typename T>
void interleaveBuffer( const BufferT<T> *nonInterleavedSource, BufferInterleavedT<T> *interleavedDest )
{
	CI_ASSERT( interleavedDest->getNumChannels() == nonInterleavedSource->getNumChannels() );
	CI_ASSERT( interleavedDest->getSize() <= nonInterleavedSource->getSize() );

	interleave( nonInterleavedSource->getData(), interleavedDest->getData(), interleavedDest->getNumFrames(), interleavedDest->getNumChannels(), interleavedDest->getNumFrames() );
}

//! De-interleaves \a interleavedSource, placing the result in \a nonInterleavedDest.
template<typename T>
void deinterleaveBuffer( const BufferInterleavedT<T> *interleavedSource, BufferT<T> *nonInterleavedDest )
{
	CI_ASSERT( interleavedSource->getNumChannels() == nonInterleavedDest->getNumChannels() );
	CI_ASSERT( nonInterleavedDest->getSize() <= interleavedSource->getSize() );

	deinterleave( interleavedSource->getData(), nonInterleavedDest->getData(), nonInterleavedDest->getNumFrames(), nonInterleavedDest->getNumChannels(), nonInterleavedDest->getNumFrames() );
}

//! Interleaves \a nonInterleavedSource, placing the result in \a interleavedDest. This method is only slightly faster than interleaveBuffer(), which can handle an arbitrary number of channels.
template<typename T>
void interleaveStereoBuffer( const BufferT<T> *nonInterleavedSource, BufferInterleavedT<T> *interleavedDest )
{
	CI_ASSERT( interleavedDest->getNumChannels() == 2 && nonInterleavedSource->getNumChannels() == 2 );
	CI_ASSERT( interleavedDest->getSize() <= nonInterleavedSource->getSize() );

	size_t numFrames = interleavedDest->getNumFrames();
	const T *left = nonInterleavedSource->getChannel( 0 );
	const T *right = nonInterleavedSource->getChannel( 1 );

	T *mixed = interleavedDest->getData();

	size_t i, j;
	for( i = 0, j = 0; i < numFrames; i++, j += 2 ) {
		mixed[j] = left[i];
		mixed[j + 1] = right[i];
	}
}

//! De-interleaves \a interleavedSource, placing the result in \a nonInterleavedDest. This method is only slightly faster than deinterleaveStereoBuffer(), which can handle an arbitrary number of channels.
template<typename T>
void deinterleaveStereoBuffer( const BufferInterleavedT<T> *interleavedSource, BufferT<T> *nonInterleavedDest )
{
	CI_ASSERT( interleavedSource->getNumChannels() == 2 && nonInterleavedDest->getNumChannels() == 2 );
	CI_ASSERT( nonInterleavedDest->getSize() <= interleavedSource->getSize() );

	size_t numFrames = nonInterleavedDest->getNumFrames();
	T *left = nonInterleavedDest->getChannel( 0 );
	T *right = nonInterleavedDest->getChannel( 1 );
	const T *mixed = interleavedSource->getData();

	size_t i, j;
	for( i = 0, j = 0; i < numFrames; i++, j += 2 ) {
		left[i] = mixed[j];
		right[i] = mixed[j + 1];
	}
}

} } } // namespace cinder::audio::dsp
