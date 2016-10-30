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

#include "cinder/audio/NodeMath.h"
#include "cinder/audio/dsp/Dsp.h"
#include "cinder/CinderMath.h"

namespace cinder { namespace audio {

// ----------------------------------------------------------------------------------------------------
// AddNode
// ----------------------------------------------------------------------------------------------------

MathNode::MathNode( float initialValue, const Format &format )
	: Node( format ), mParam( this, initialValue )
{
}

void AddNode::process( Buffer *buffer )
{
	if( mParam.eval() ) {
		for( size_t ch = 0; ch < getNumChannels(); ch++ ) {
			float *channel = buffer->getChannel( ch );
			dsp::add( channel, mParam.getValueArray(), channel, buffer->getNumFrames() );
		}
	}
	else
		dsp::add( buffer->getData(), mParam.getValue(), buffer->getData(), buffer->getSize() );
}

void SubtractNode::process( Buffer *buffer )
{
	if( mParam.eval() ) {
		for( size_t ch = 0; ch < getNumChannels(); ch++ ) {
			float *channel = buffer->getChannel( ch );
			dsp::sub( channel, mParam.getValueArray(), channel, buffer->getNumFrames() );
		}
	}
	else
		dsp::sub( buffer->getData(), mParam.getValue(), buffer->getData(), buffer->getSize() );
}

void MultiplyNode::process( Buffer *buffer )
{
	if( mParam.eval() ) {
		for( size_t ch = 0; ch < getNumChannels(); ch++ ) {
			float *channel = buffer->getChannel( ch );
			dsp::mul( channel, mParam.getValueArray(), channel, buffer->getNumFrames() );
		}
	}
	else
		dsp::mul( buffer->getData(), mParam.getValue(), buffer->getData(), buffer->getSize() );
}

void DivideNode::process( Buffer *buffer )
{
	if( mParam.eval() ) {
		for( size_t ch = 0; ch < getNumChannels(); ch++ ) {
			float *channel = buffer->getChannel( ch );
			dsp::divide( channel, mParam.getValueArray(), channel, buffer->getNumFrames() );
		}
	}
	else
		dsp::divide( buffer->getData(), mParam.getValue(), buffer->getData(), buffer->getSize() );
}

} } // namespace cinder::audio
