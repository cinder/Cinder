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

#include "cinder/audio/Node.h"

#include <list>

namespace cinder { namespace audio {

typedef std::shared_ptr<class ChannelRouterNode>	ChannelRouterNodeRef;

//! \brief Node for mapping input channels to output channels.
//!
//! ChannelRouterNode allows one to route the channels of connected inputs to specific output channels, which
//! can be useful for, among other things:
//! * combining x number of Node's into an n-channel output signal that can be consumed by a multichannel output-device via OutputDeviceNode.
//! * splitting a stereo Node into multiple outputs
//!
//! The class is designed to be connected using the overloaded `operator>>`, similar to how typical Node's are connected but
//! via one of the route() variants. The first variant takes an input channel index and an output channel index, and routes either all of
//! the input's channels or as many as many channels are available based on the ChannelRouterNode's specified channel count (via its Node::Format() at construction).

//! For example,
//!
//! \code
//! samplePlayer >> router->route( 0, 3 );		// routes channel 0 of samplePlayer to router's channel 3, taking as many channels from samplePlayer as possible.
//! samplePlayer >> router->route( 1, 0, 1 );	// routes channel 1 of samplePlayer to router's channel 0, taking only one channel.
//! \endcode
//!
class CI_API ChannelRouterNode : public Node {
  public:
	//! Constructs a ChannelRouterNode object, with an optional \a format.
	ChannelRouterNode( const Format &format = Format() ) : Node( format )	{}

	//! Used in conjunction with `operator>>` and the route() methods to make connections that conduct channel routing. This class is usually not directly constructed by the user.
	struct RouteConnector {
		//! Constructs a RouteConnector with no number of channels information, so the maximum possible will be used.
		RouteConnector( const ChannelRouterNodeRef &outputRouter, size_t inputChannelIndex, size_t outputChannelIndex );
		//! Constructs a RouteConnector that will route \a numChannels from the input to the output ChannelRouterNode.
		RouteConnector( const ChannelRouterNodeRef &outputRouter, size_t inputChannelIndex, size_t outputChannelIndex, size_t numChannels );

		//! Returns the output ChannelRouterNode.
		const ChannelRouterNodeRef& getOutputRouter() const		{ return mOutputRouter; }
		//! Returns the channel index to start routing input channels from.
		size_t	getInputChannelIndex() const				{ return mInputChannelIndex; }
		//! Returns the channel index to route input channels to.
		size_t	getOutputChannelIndex() const				{ return mOutputChannelIndex; }
		//! Returns the number of channels to route in the connection.
		size_t	getNumChannels() const						{ return mNumChannels; }

	  private:
		ChannelRouterNodeRef mOutputRouter;
		size_t	mInputChannelIndex, mOutputChannelIndex, mNumChannels;
	};

	//! Used in conjunction with `operator>>`, specified that the routing is to map the input's \a inputChannelIndex to the output's (this ChannelRouterNode) \a outputChannelIndex, routing as many channels as possible.
	RouteConnector route( size_t inputChannelIndex, size_t outputChannelIndex );
	//! Used in conjunction with `operator>>`, specified that the routing is to map the input's \a inputChannelIndex to the output's (this ChannelRouterNode) \a outputChannelIndex, routing \a numChannels number of channels.
	RouteConnector route( size_t inputChannelIndex, size_t outputChannelIndex, size_t numChannels );

	//! Adds \a input to the route list, routing \a numChannels starting at \a inputChannelIndex of \a input to \a outputChannelIndex.
	void addInputRoute( const NodeRef &input, size_t inputChannelIndex, size_t outputChannelIndex, size_t numChannels );

	void disconnectAllInputs()									override;
	
  protected:
	bool supportsInputNumChannels( size_t numChannels ) const	override;
	bool supportsProcessInPlace() const							override;
	void sumInputs()											override;
	void disconnectInput( const NodeRef &input )				override;

	struct Route {
		NodeRef	mInput;
		size_t	mInputChannelIndex, mOutputChannelIndex, mNumChannels;
	};

	std::list<Route>	mRoutes;
};

//! Enable routing connection syntax: \code input >> output->route( inputChannelIndex, outputChannelIndex, numChannels ); \endcode.  \return the output ChannelRouterNode after connection is made.
CI_API const ChannelRouterNodeRef& operator>>( const NodeRef &input, const ChannelRouterNode::RouteConnector &route );

} } // namespace cinder::audio
