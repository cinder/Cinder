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

#include "cinder/audio/dsp/Converter.h"

#include <vector>
#include <memory>

namespace r8b {
	class CDSPResampler24;
}

namespace cinder { namespace audio { namespace dsp {

//! \a Converter implementation using the r8brain samplerate conversion library by designed by Aleksey Vaneev.
class ConverterImplR8brain : public Converter {
  public:

	ConverterImplR8brain( size_t sourceSampleRate, size_t destSampleRate, size_t sourceNumChannels, size_t destNumChannels, size_t sourceMaxFramesPerBlock );
	virtual ~ConverterImplR8brain();

	std::pair<size_t, size_t>	convert( const Buffer *sourceBuffer, Buffer *destBuffer )	override;
	void						clear()														override;

  private:
	std::pair<size_t, size_t> convertImpl( const Buffer *sourceBuffer, Buffer *destBuffer, int readCount );
	std::pair<size_t, size_t> convertImplUpMixing( const Buffer *sourceBuffer, Buffer *destBuffer, int readCount );
	std::pair<size_t, size_t> convertImplDownMixing( const Buffer *sourceBuffer, Buffer *destBuffer, int readCount );

	std::vector<std::unique_ptr<r8b::CDSPResampler24> > mResamplers;
	BufferT<double> mBufferd;
	Buffer mMixingBuffer;
};

} } } // namespace cinder::audio::dsp
