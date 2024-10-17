#pragma once

#include "cinder/audio/Node.h"

struct InterleavedPassThruNode : public ci::audio::Node {
	InterleavedPassThruNode() : Node( Format() )
	{
		setAutoEnabled();
		setChannelMode( ChannelMode::SPECIFIED );
		setNumChannels( 2 );
	}

	void initialize() override
	{
		mBufferInterleaved = ci::audio::BufferInterleaved( getContext()->getFramesPerBlock(), 2 );
	}

	void process( ci::audio::Buffer *buffer ) override
	{
		CI_ASSERT( buffer->getNumChannels() == 2 );

		ci::audio::dsp::interleaveBuffer( buffer, &mBufferInterleaved );
		ci::audio::dsp::deinterleaveBuffer( &mBufferInterleaved, buffer );
	}

private:
	ci::audio::BufferInterleaved mBufferInterleaved;
};