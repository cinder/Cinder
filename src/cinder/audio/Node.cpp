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


#include "cinder/audio/Node.h"
#include "cinder/audio/DelayNode.h"
#include "cinder/audio/Context.h"
#include "cinder/audio/dsp/Dsp.h"
#include "cinder/audio/dsp/Converter.h"
#include "cinder/CinderAssert.h"
#include "cinder/System.h"

#include <limits>

using namespace std;

namespace cinder { namespace audio {

// ----------------------------------------------------------------------------------------------------
// Node
// ----------------------------------------------------------------------------------------------------

Node::Node( const Format &format )
	: mInitialized( false ), mEnabled( false ), mEventScheduled( false ), mChannelMode( format.getChannelMode() ),
		mNumChannels( 1 ), mAutoEnabled( true ), mProcessInPlace( true ), mLastProcessedFrame( numeric_limits<uint64_t>::max() )
{
	if( format.getChannels() ) {
		mNumChannels = format.getChannels();
		mChannelMode = ChannelMode::SPECIFIED;
	}

	if( format.isAutoEnableSet() )
		setAutoEnabled( format.getAutoEnable() );
}

Node::~Node()
{
}

void Node::connect( const NodeRef &output )
{
	// make a reference to ourselves so that we aren't deallocated in the case of the last owner
	// disconnecting us, which we may need later anyway
	NodeRef thisRef = shared_from_this();

	if( ! output || ! output->canConnectToInput( thisRef ) )
		return;

	if( checkCycle( thisRef, output ) )
		throw NodeCycleExc( thisRef, output );

	mOutputs.push_back( output ); // set output first, so that it is visible in configureConnections()
	output->connectInput( thisRef );

	output->notifyConnectionsDidChange();
}

void Node::disconnect( const NodeRef &output )
{
	if( ! output )
		return;

	for( auto weakOutIt = mOutputs.begin(); weakOutIt != mOutputs.end(); ++weakOutIt ) {
		if( weakOutIt->lock() == output ) {
			mOutputs.erase( weakOutIt );
			break;
		}
	}

	output->disconnectInput( shared_from_this() );
	output->notifyConnectionsDidChange();
}

void Node::disconnectAll()
{
	disconnectAllInputs();
	disconnectAllOutputs();
}

void Node::disconnectAllOutputs()
{
	NodeRef thisRef = shared_from_this();

	auto outputs = getOutputs(); // first make a copy of only the still-alive NodeRef's
	for( const auto &output : outputs )
		disconnect( output );
}

void Node::disconnectAllInputs()
{
	NodeRef thisRef = shared_from_this();

	for( auto &input : mInputs )
		input->disconnectOutput( thisRef );

	mInputs.clear();
	notifyConnectionsDidChange();
}

void Node::connectInput( const NodeRef &input )
{
	auto ctx = getContext();
	if( ! ctx )
		return;

	lock_guard<mutex> lock( ctx->getMutex() );

	mInputs.insert( input );
	configureConnections();
}

void Node::disconnectInput( const NodeRef &input )
{
	auto ctx = getContext();
	if( ! ctx )
		return;

	lock_guard<mutex> lock( ctx->getMutex() );

	for( auto inIt = mInputs.begin(); inIt != mInputs.end(); ++inIt ) {
		if( *inIt == input ) {
			mInputs.erase( inIt );
			break;
		}
	}
}

void Node::disconnectOutput( const NodeRef &output )
{
	auto ctx = getContext();
	if( ! ctx )
		return;

	lock_guard<mutex> lock( ctx->getMutex() );

	for( auto outIt = mOutputs.begin(); outIt != mOutputs.end(); ++outIt ) {
		if( outIt->lock() == output ) {
			mOutputs.erase( outIt );
			break;
		}
	}
}

vector<NodeRef> Node::getOutputs() const
{
	vector<NodeRef> result;
	for( const weak_ptr<Node> &outputWeak : mOutputs ) {
		NodeRef output = outputWeak.lock();
		if( output )
			result.push_back( output );
	}

	return result;
}

void Node::enable()
{
	if( ! mInitialized )
		initializeImpl();

	// Need to cancel events regardless if node is already enabled as one might be disabling us
	if( mEventScheduled ) {
		auto context = getContext();
		if( context && ! context->isAudioThread() ) {
			context->cancelScheduledEvents( shared_from_this() );
		}
	}

	if( mEnabled )
		return;

	mEnabled = true;
	enableProcessing();
}

void Node::disable()
{
	// Need to cancel events regardless if node is already disabled as one might be enabling us
	if( mEventScheduled ) {
		auto context = getContext();
		if( context && ! context->isAudioThread() ) {
			context->cancelScheduledEvents( shared_from_this() );
		}
	}

	if( ! mEnabled )
		return;

	mEnabled = false;
	disableProcessing();
}

void Node::enable( double when )
{
	getContext()->scheduleEvent( when, shared_from_this(), true, [this] { enable(); } );
}

void Node::disable( double when )
{
	getContext()->scheduleEvent( when, shared_from_this(), false, [this] { disable(); } );
}

void Node::setEnabled( bool b )
{
	if( b )
		enable();
	else
		disable();
}


void Node::setEnabled( bool b, double when )
{
	if( b )
		enable( when );
	else
		disable( when );
}

size_t Node::getNumConnectedInputs() const
{
	return mInputs.size();
}

size_t Node::getNumConnectedOutputs() const
{
	return mOutputs.size();
}

bool Node::isConnectedToInput( const NodeRef &input ) const
{
	return ( mInputs.count( input ) != 0 );
}

bool Node::isConnectedToOutput( const NodeRef &output ) const
{
	for( const weak_ptr<Node> &outputWeak : mOutputs ) {
		NodeRef out = outputWeak.lock();
		if( out && out == output )
			return true;
	}

	return false;
}

void Node::initializeImpl()
{
	if( mInitialized )
		return;

	if( mProcessInPlace && ! supportsProcessInPlace() )
		setupProcessWithSumming();

	mProcessFramesRange.first = 0;
	mProcessFramesRange.second = getFramesPerBlock();

	initialize();
	mInitialized = true;

	if( mAutoEnabled )
		enable();
}


void Node::uninitializeImpl()
{
	if( ! mInitialized )
		return;

	if( mAutoEnabled )
		disable();

	uninitialize();
	mInitialized = false;
}

void Node::setNumChannels( size_t numChannels )
{
	if( mNumChannels == numChannels )
		return;

	uninitializeImpl();
	mNumChannels = numChannels;
}

void Node::setChannelMode( ChannelMode mode )
{
	CI_ASSERT_MSG( ! mInitialized, "setting the channel mode must be done before initializing" );

	if( mChannelMode == mode )
		return;

	mChannelMode = mode;
}

size_t Node::getMaxNumInputChannels() const
{
	size_t result = 0;
	for( auto &in : mInputs )
		result = max( result, in->getNumChannels() );

	return result;
}

size_t Node::getSampleRate() const
{
	return getContext()->getSampleRate();
}

size_t Node::getFramesPerBlock() const
{
	return getContext()->getFramesPerBlock();
}

bool Node::inputChannelsAreUnequal() const
{
	if( ! mInputs.empty() ) {
		size_t numChannels = (*mInputs.begin() )->getNumChannels();
		for( auto &in : mInputs ) {
			if( numChannels != in->getNumChannels() )
				return true;
		}
	}

	return false;
}

// TODO: Checking for DelayNode below is a kludge and will not work for other types that want to support feedback.
//		 With more investigation it might be possible to avoid this, or at least define some interface that
//       specifies whether this input needs to be summed.
void Node::configureConnections()
{
	CI_ASSERT( getContext() );

	mProcessInPlace = supportsProcessInPlace();

	if( getNumConnectedInputs() > 1 || getNumConnectedOutputs() > 1 )
		mProcessInPlace = false;

	bool isDelay = ( dynamic_cast<DelayNode *>( this ) != nullptr ); // see note above
	bool inputChannelsUnequal = inputChannelsAreUnequal();

	for( auto &input : mInputs ) {
		bool inputProcessInPlace = true;

		size_t inputNumChannels = input->getNumChannels();
		if( ! supportsInputNumChannels( inputNumChannels ) ) {
			if( mChannelMode == ChannelMode::MATCHES_INPUT )
				setNumChannels( getMaxNumInputChannels() );
			else if( input->getChannelMode() == ChannelMode::MATCHES_OUTPUT ) {
				input->setNumChannels( mNumChannels );
				input->configureConnections();
			}
			else {
				mProcessInPlace = false;
				inputProcessInPlace = false;
			}
		}

		// inputs with more than one output cannot process in-place, so make them sum
		if( input->getProcessesInPlace() && input->getNumConnectedOutputs() > 1 )
			inputProcessInPlace = false;

		// when there are multiple inputs and their channel counts don't match, they must be summed
		if( inputChannelsUnequal )
			inputProcessInPlace = false;

		// if we're unable to process in-place and we're a DelayNode, its possible that the input may be part of a feedback loop, in which case input must sum.
		if( ! mProcessInPlace && isDelay )
			inputProcessInPlace = false;

		if( ! inputProcessInPlace )
			input->setupProcessWithSumming();

		input->initializeImpl();
	}

	for( auto &out : mOutputs ) {
		NodeRef output = out.lock();
		if( ! output )
			continue;
		if( ! output->supportsInputNumChannels( mNumChannels ) ) {
			if( output->getChannelMode() == ChannelMode::MATCHES_INPUT ) {
				output->setNumChannels( mNumChannels );
				output->configureConnections();
			}
			else
				mProcessInPlace = false;
		}
	}

	if( ! mProcessInPlace )
		setupProcessWithSumming();

	initializeImpl();
}

void Node::pullInputs( Buffer *inPlaceBuffer )
{
	CI_ASSERT( getContext() );

	if( mProcessInPlace ) {
		if( mInputs.empty() ) {
			// Fastest route: no inputs and process in-place. inPlaceBuffer must be cleared so that samples left over
			// from InputNode's that aren't filling the entire buffer are zero.
			inPlaceBuffer->zero();
			if( mEnabled )
				process( inPlaceBuffer );
		}
		else {
			// First pull the input (can only be one when in-place), then run process() if input did any processing.
			const NodeRef &input = *mInputs.begin();
			input->pullInputs( inPlaceBuffer );

			if( ! input->getProcessesInPlace() )
				dsp::mixBuffers( input->getInternalBuffer(), inPlaceBuffer );

			if( mEnabled )
				process( inPlaceBuffer );
		}
	}
	else {
		// Pull and sum all enabled inputs. Only do this once per processing block, which is checked by the current number of processed frames.
		uint64_t numProcessedFrames = getContext()->getNumProcessedFrames();
		if( mLastProcessedFrame != numProcessedFrames ) {
			mLastProcessedFrame = numProcessedFrames;

			mSummingBuffer.zero();
			sumInputs();
		}
	}
}

void Node::process( Buffer * /*buffer*/ )
{
}

void Node::sumInputs()
{
	// Pull all inputs, summing the results from the buffer that input used for processing.
	// mInternalBuffer is not zero'ed before pulling inputs to allow for feedback.
	for( auto &input : mInputs ) {
		input->pullInputs( &mInternalBuffer );
		const Buffer *processedBuffer = input->getProcessesInPlace() ? &mInternalBuffer : input->getInternalBuffer();
		dsp::sumBuffers( processedBuffer, &mSummingBuffer );
	}

	// Process the summed results if enabled.
	if( mEnabled )
		process( &mSummingBuffer );

	// copy summed buffer back to internal so downstream can get it.
	dsp::mixBuffers( &mSummingBuffer, &mInternalBuffer );
}

void Node::setupProcessWithSumming()
{
	CI_ASSERT( getContext() );

	mProcessInPlace = false;
	size_t framesPerBlock = getFramesPerBlock();

	mInternalBuffer.setSize( framesPerBlock, mNumChannels );
	mSummingBuffer.setSize( framesPerBlock, mNumChannels );
}

bool Node::checkCycle( const NodeRef &sourceNode, const NodeRef &destNode ) const
{
	if( sourceNode == destNode )
		return true;

	if( sourceNode->supportsCycles() || destNode->supportsCycles() )
		return false;

	for( const auto &input : sourceNode->getInputs() ) {
		if( checkCycle( input, destNode ) )
			return true;
	}

	return false;
}

void Node::notifyConnectionsDidChange()
{
	auto ctx = getContext();
	if( ! ctx )
		return;

	ctx->connectionsDidChange( shared_from_this() );
}

bool Node::canConnectToInput( const NodeRef &input )
{
	if( ! input || input == shared_from_this() )
		return false;

	if( isConnectedToInput( input ) )
		return false;

	return true;
}

std::string Node::getName() const
{
	return ! mName.empty() ? mName : System::demangleTypeName( typeid( *this ).name() );
}

// ----------------------------------------------------------------------------------------------------
// NodeAutoPullable
// ----------------------------------------------------------------------------------------------------

NodeAutoPullable::NodeAutoPullable( const Format &format )
	: Node( format ), mIsPulledByContext( false )
{
}

NodeAutoPullable::~NodeAutoPullable()
{
}

void NodeAutoPullable::connect( const NodeRef &output )
{
	Node::connect( output );
	updatePullMethod();
}

void NodeAutoPullable::connectInput( const NodeRef &input )
{
	Node::connectInput( input );
	updatePullMethod();
}

void NodeAutoPullable::disconnectInput( const NodeRef &input )
{
	Node::disconnectInput( input );
	updatePullMethod();
}

void NodeAutoPullable::disconnectOutput( const NodeRef &output )
{
	// make sure we live past disconnection, as output could be the last guy with a strong reference to us
	auto thisRef = shared_from_this();
	Node::disconnectOutput( output );
	updatePullMethod();
}

void NodeAutoPullable::disconnectAllOutputs()
{
	// make sure we live past disconnection, as output could be the last guy with a strong reference to us
	auto thisRef = shared_from_this();
	Node::disconnectAllOutputs();

	// no need to query getOutputs() as we know it is empty now, so just remove from auto pull list if needed
	if( mIsPulledByContext ) {
		mIsPulledByContext = false;
		getContext()->removeAutoPulledNode( shared_from_this() );
	}
}

void NodeAutoPullable::updatePullMethod()
{
	bool hasOutputs = ! getOutputs().empty();
	if( ! hasOutputs && ! mIsPulledByContext ) {
		mIsPulledByContext = true;
		getContext()->addAutoPulledNode( shared_from_this() );
	}
	else if( hasOutputs && mIsPulledByContext ) {
		mIsPulledByContext = false;
		getContext()->removeAutoPulledNode( shared_from_this() );
	}
}

// ----------------------------------------------------------------------------------------------------
// ScopedEnableNode
// ----------------------------------------------------------------------------------------------------

ScopedEnableNode::ScopedEnableNode( const NodeRef &node )
	: mNode( node )
{
	mWasEnabled = ( mNode ? mNode->isEnabled() : false );
}

ScopedEnableNode::ScopedEnableNode( const NodeRef &node, bool enable )
	: mNode( node )
{
	if( mNode ) {
		mWasEnabled = mNode->isEnabled();
		mNode->setEnabled( enable );
	}
	else
		mWasEnabled = false;
}

ScopedEnableNode::~ScopedEnableNode()
{
	if( mNode )
		mNode->setEnabled( mWasEnabled );
}

// ----------------------------------------------------------------------------------------------------
// Exceptions
// ----------------------------------------------------------------------------------------------------

NodeCycleExc::NodeCycleExc( const NodeRef &sourceNode, const NodeRef &destNode )
	: AudioExc( string( "cyclical connection between source: " ) + sourceNode->getName() + string( " and dest: " ) + destNode->getName() )
{
}

} } // namespace cinder::audio

