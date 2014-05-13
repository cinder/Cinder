#pragma once

#include "cinder/audio2/Node.h"

struct InterleavedPassThruNode : public ci::audio2::Node {
	InterleavedPassThruNode() : Node( Format() )
	{
		setAutoEnabled();
		setChannelMode( ChannelMode::SPECIFIED );
		setNumChannels( 2 );
	}

	virtual void initialize() override
	{
		mBufferInterleaved = ci::audio2::BufferInterleaved( getContext()->getFramesPerBlock(), 2 );
	}

	void process( ci::audio2::Buffer *buffer ) override
	{
		CI_ASSERT( buffer->getNumChannels() == 2 );

		ci::audio2::dsp::interleaveBuffer( buffer, &mBufferInterleaved );
		ci::audio2::dsp::deinterleaveBuffer( &mBufferInterleaved, buffer );
	}

private:
	ci::audio2::BufferInterleaved mBufferInterleaved;
};