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

#include "cinder/audio/msw/MswUtil.h"
#include "cinder/CinderAssert.h"

#include <Audioclient.h>

namespace cinder { namespace audio { namespace msw {

::WAVEFORMATEXTENSIBLE makeWaveFormat( SampleType sampleType, size_t sampleRate, size_t numChannels, size_t bitsPerSample, bool useExtensible )
{
	::WAVEFORMATEXTENSIBLE wfx;

	if( bitsPerSample == 0 ) {
		switch( sampleType ) {
			case SampleType::INT_16:	bitsPerSample = 16; break;
			case SampleType::INT_24:	bitsPerSample = 24; break;
			case SampleType::FLOAT_32:	bitsPerSample = 32; break;
			default:					CI_ASSERT_NOT_REACHABLE();
		}
	}

	wfx.Samples.wValidBitsPerSample		= bitsPerSample;
	wfx.SubFormat						= sampleType == SampleType::FLOAT_32 ? KSDATAFORMAT_SUBTYPE_IEEE_FLOAT : KSDATAFORMAT_SUBTYPE_PCM;
	wfx.dwChannelMask					= 0; // use the default channel order ('no speaker location is desired on any of the mono streams')

	wfx.Format.wFormatTag				= useExtensible ? WAVE_FORMAT_EXTENSIBLE : WAVE_FORMAT_PCM;
	wfx.Format.nSamplesPerSec			= (DWORD)sampleRate;
	wfx.Format.nChannels				= (WORD)numChannels;
	wfx.Format.wBitsPerSample			= (WORD)bitsPerSample;
	wfx.Format.nBlockAlign				= wfx.Format.nChannels * wfx.Format.wBitsPerSample / 8;
	wfx.Format.nAvgBytesPerSec			= wfx.Format.nSamplesPerSec * wfx.Format.nBlockAlign;
	wfx.Format.cbSize					= sizeof( ::WAVEFORMATEXTENSIBLE ) - sizeof( ::WAVEFORMATEX );

	return wfx;
}

void copyWaveFormat( const ::WAVEFORMATEX &source, ::WAVEFORMATEX *dest )
{
	size_t sizeBytes = source.wFormatTag == WAVE_FORMAT_EXTENSIBLE ? sizeof( ::WAVEFORMATEXTENSIBLE ) : sizeof( ::WAVEFORMATEX );
	memcpy( dest, &source, sizeBytes );
}

std::string	waveFormatToString( const ::WAVEFORMATEX &wfx )
{
	std::string result;
	result += ".Format.wFormatTag: "; 
	if( wfx.wFormatTag == WAVE_FORMAT_EXTENSIBLE )
		result += "WAVE_FORMAT_EXTENSIBLE";
	else if( wfx.wFormatTag == WAVE_FORMAT_PCM )
		result += "WAVE_FORMAT_PCM";
	else
		result += "(unknown)";

	result += ", .Format.nSamplesPerSec: " + std::to_string( (int)wfx.nSamplesPerSec );
	result += ", .Format.nChannels: " +  std::to_string( (int)wfx.nChannels );
	result += ", .Format.wBitsPerSample: " +  std::to_string( (int)wfx.wBitsPerSample );

	if( wfx.wFormatTag == WAVE_FORMAT_EXTENSIBLE ) {
		const auto &wfxextensible = (const ::WAVEFORMATEXTENSIBLE &)wfx; 
		result += ", .SubFormat: "; 
		if( wfxextensible.SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT )
			result += "KSDATAFORMAT_SUBTYPE_IEEE_FLOAT";
		else if( wfxextensible.SubFormat == KSDATAFORMAT_SUBTYPE_PCM )
			result += "KSDATAFORMAT_SUBTYPE_PCM";
		else
			result += "(unknown)";

		result += ", .Samples.wValidBitsPerSample: " + std::to_string( (int)wfxextensible.Samples.wValidBitsPerSample );
		result += ", .Samples.wValidBitsPerSample: " + std::to_string( (int)wfxextensible.Samples.wValidBitsPerSample );
	}

	return result;
}

std::string	waveFormatToString( const ::WAVEFORMATEXTENSIBLE &wfx )
{
	return waveFormatToString( wfx.Format );
}

::REFERENCE_TIME framesToHundredNanoSeconds( size_t frames, size_t sampleRate )
{
	return (::REFERENCE_TIME)( ( 10000.0 * 1000.0 / (double)sampleRate * (double)frames ) + 0.5 );
}

size_t hundredNanoSecondsToFrames( ::REFERENCE_TIME timeHundredNs, size_t sampleRate )
{
	return lround( (double)sampleRate * (double)timeHundredNs * 0.0000001 );
}

const char* hresultToString( ::HRESULT hr )
{
	switch( hr ) {
		case S_OK: return "S_OK";
		case S_FALSE: return "S_FALSE";
		case E_POINTER: return "E_POINTER";
		case E_INVALIDARG: return "E_INVALIDARG";
		case E_OUTOFMEMORY: return "E_OUTOFMEMORY";
		case AUDCLNT_E_DEVICE_INVALIDATED: return "AUDCLNT_E_DEVICE_INVALIDATED";
		case AUDCLNT_E_SERVICE_NOT_RUNNING: return "AUDCLNT_E_SERVICE_NOT_RUNNING";
		case AUDCLNT_E_ALREADY_INITIALIZED: return "AUDCLNT_E_ALREADY_INITIALIZED";
		case AUDCLNT_E_WRONG_ENDPOINT_TYPE: return "AUDCLNT_E_WRONG_ENDPOINT_TYPE";
		case AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED: return "AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED";
		case AUDCLNT_E_BUFFER_SIZE_ERROR: return "AUDCLNT_E_BUFFER_SIZE_ERROR";
		case AUDCLNT_E_CPUUSAGE_EXCEEDED: return "AUDCLNT_E_CPUUSAGE_EXCEEDED";
		case AUDCLNT_E_DEVICE_IN_USE: return "AUDCLNT_E_DEVICE_IN_USE";
		case AUDCLNT_E_ENDPOINT_CREATE_FAILED: return "AUDCLNT_E_ENDPOINT_CREATE_FAILED";
		case AUDCLNT_E_INVALID_DEVICE_PERIOD: return "AUDCLNT_E_INVALID_DEVICE_PERIOD";
		case AUDCLNT_E_UNSUPPORTED_FORMAT: return "AUDCLNT_E_UNSUPPORTED_FORMAT";
		case AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED: return "AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED";
		case AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL: return "AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL";
		case AUDCLNT_E_NOT_INITIALIZED: return "AUDCLNT_E_NOT_INITIALIZED";
		default: break;
	}

	return "(unknown)";
}

} } } // namespace cinder::audio::msw
