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

#include "cinder/audio/ChannelRouterNode.h"
#include "cinder/audio/Context.h"
#include "cinder/audio/dsp/Dsp.h"
#include "cinder/CinderMath.h"

#include <limits>

using namespace std;

namespace cinder { namespace audio {

ChannelRouterNode::RouteConnector ChannelRouterNode::route( size_t inputChannelIndex, size_t outputChannelIndex )
{
	return RouteConnector( static_pointer_cast<ChannelRouterNode>( shared_from_this() ), inputChannelIndex, outputChannelIndex );
}

ChannelRouterNode::RouteConnector ChannelRouterNode::route( size_t inputChannelIndex, size_t outputChannelIndex, size_t numChannels )
{
	return RouteConnector( static_pointer_cast<ChannelRouterNode>( shared_from_this() ), inputChannelIndex, outputChannelIndex, numChannels );
}

ChannelRouterNode::RouteConnector::RouteConnector( const ChannelRouterNodeRef &outputRouter, size_t inputChannelIndex, size_t outputChannelIndex )
: mOutputRouter( outputRouter ), mInputChannelIndex( inputChannelIndex ), mOutputChannelIndex( outputChannelIndex ), mNumChannels( numeric_limits<size_t>::max() )
{
}

ChannelRouterNode::RouteConnector::RouteConnector( const ChannelRouterNodeRef &outputRouter, size_t inputChannelIndex, size_t outputChannelIndex, size_t numChannels )
: mOutputRouter( outputRouter ), mInputChannelIndex( inputChannelIndex ), mOutputChannelIndex( outputChannelIndex ), mNumChannels( numChannels )
{
}

const ChannelRouterNodeRef& operator>>( const NodeRef &input, const ChannelRouterNode::RouteConnector &route )
{
	size_t numChannels = route.getNumChannels();
	if( numChannels == numeric_limits<size_t>::max() )
		numChannels = min( input->getNumChannels(), route.getOutputRouter()->getNumChannels() - route.getOutputChannelIndex() );

	route.getOutputRouter()->addInputRoute( input, route.getInputChannelIndex(), route.getOutputChannelIndex(), numChannels );
	return route.getOutputRouter();
}

bool ChannelRouterNode::supportsInputNumChannels( size_t /*numChannels*/ ) const
{
	return true;
}

bool ChannelRouterNode::supportsProcessInPlace() const
{
	return false;
}

void ChannelRouterNode::addInputRoute( const NodeRef &input, size_t inputChannelIndex, size_t outputChannelIndex, size_t numChannels )
{
	CI_ASSERT_MSG( input, "bad input" );
	CI_ASSERT_MSG( inputChannelIndex < input->getNumChannels(), "inputChannelIndex out of range." );
	CI_ASSERT_MSG( ( outputChannelIndex + numChannels ) <= getNumChannels(), "outputChannelIndex + numChannels out of range." );

	Route route;
	route.mInput = input;
	route.mInputChannelIndex = inputChannelIndex;
	route.mOutputChannelIndex = outputChannelIndex;
	route.mNumChannels = numChannels;

	input->connect( shared_from_this() );

	lock_guard<mutex> lock( getContext()->getMutex() );
	mRoutes.push_back( route );
}

void ChannelRouterNode::disconnectInput( const NodeRef &input )
{
	Node::disconnectInput( input );

	lock_guard<mutex> lock( getContext()->getMutex() );

	for( auto it = mRoutes.begin(); it != mRoutes.end(); ++it ) {
		if( it->mInput == input ) {
			mRoutes.erase( it );
			return;
		}
	}
}

void ChannelRouterNode::disconnectAllInputs()
{
	Node::disconnectAllInputs();

	lock_guard<mutex> lock( getContext()->getMutex() );
	mRoutes.clear();
}

void ChannelRouterNode::sumInputs()
{
	BufferDynamic *summingBuffer = getSummingBuffer();
	Buffer *internalBuffer = getInternalBuffer();

	const size_t numFrames = internalBuffer->getNumFrames();
	internalBuffer->zero(); // TODO: this will wipe out any feedback data. Avoid if possible.

	for( auto &route : mRoutes ) {
		NodeRef &input = route.mInput;

		summingBuffer->setNumChannels( input->getNumChannels() );
		input->pullInputs( summingBuffer );

		const Buffer *processedBuffer = input->getProcessesInPlace() ? summingBuffer : input->getInternalBuffer();

		for( size_t ch = 0; ch < route.mNumChannels; ch++ ) {
			float *destChannel = internalBuffer->getChannel( ch + route.mOutputChannelIndex );
			dsp::add( destChannel, processedBuffer->getChannel( ch + route.mInputChannelIndex ), destChannel, numFrames );
		}
	}
}

} } // namespace cinder::audio
