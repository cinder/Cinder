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

#include "cinder/audio/InputNode.h"
#include "cinder/audio/Context.h"

using namespace std;

namespace cinder { namespace audio {

// ----------------------------------------------------------------------------------------------------
// InputNode
// ----------------------------------------------------------------------------------------------------

InputNode::InputNode( const Format &format )
	: Node( format )
{
	if( getChannelMode() != ChannelMode::SPECIFIED )
		setChannelMode( ChannelMode::MATCHES_OUTPUT );

	if( ! format.isAutoEnableSet() )
		setAutoEnabled( false );
}

InputNode::~InputNode()
{
}

void InputNode::connectInput( const NodeRef & /*input*/ )
{
	CI_ASSERT_MSG( 0, "InputNode does not support inputs" );
}

// ----------------------------------------------------------------------------------------------------
// InputDeviceNode
// ----------------------------------------------------------------------------------------------------

InputDeviceNode::InputDeviceNode( const DeviceRef &device, const Format &format )
	: InputNode( format ), mDevice( device ), mLastOverrun( 0 ), mLastUnderrun( 0 )
{
	if( ! mDevice ) {
		string errorMsg = "Empty DeviceRef.";
		if( ! audio::Device::getDefaultInput() )
			errorMsg += " Also, no default input Device so perhaps there is no available hardware input.";

		throw AudioDeviceExc( errorMsg );
	}

	size_t deviceNumChannels = mDevice->getNumInputChannels();

	// If number of channels hasn't been specified, default to 2.
	if( getChannelMode() != ChannelMode::SPECIFIED ) {
		setChannelMode( ChannelMode::SPECIFIED );
		setNumChannels( std::min( deviceNumChannels, (size_t)2 ) );
	}

	// TODO: this doesn't always mean a failing cause, need Device::supportsNumInputChannels( mNumChannels ) to be sure
	//	- on iOS, the RemoteIO audio unit can have 2 input channels, while the AVAudioSession reports only 1 input channel.
//	if( deviceNumChannels < mNumChannels )
//		throw AudioFormatExc( string( "Device can not accommodate " ) + to_string( deviceNumChannels ) + " output channels." );
}

InputDeviceNode::~InputDeviceNode()
{
}

uint64_t InputDeviceNode::getLastUnderrun() const
{
	uint64_t result = mLastUnderrun;
	mLastUnderrun = 0;
	return result;
}

uint64_t InputDeviceNode::getLastOverrun() const
{
	uint64_t result = mLastOverrun;
	mLastOverrun = 0;
	return result;
}

void InputDeviceNode::markUnderrun()
{
	auto ctx = getContext();
	CI_ASSERT( ctx );

	mLastUnderrun = ctx->getNumProcessedFrames();
}

void InputDeviceNode::markOverrun()
{

	auto ctx = getContext();
	CI_ASSERT( ctx );

	mLastOverrun = getContext()->getNumProcessedFrames();
}

void InputDeviceNode::setRingBufferPaddingFactor( float factor )
{
	mRingBufferPaddingFactor = max<float>( 1, factor );
}

string InputDeviceNode::getName() const
{
	return Node::getName() + " (" + getDevice()->getName() + ")";
}

// ----------------------------------------------------------------------------------------------------
// CallbackProcessorNode
// ----------------------------------------------------------------------------------------------------

CallbackProcessorNode::CallbackProcessorNode( const CallbackProcessorFn &callbackFn, const Format &format )
	: InputNode( format ), mCallbackFn( callbackFn )
{
}

void CallbackProcessorNode::process( Buffer *buffer )
{
	if( mCallbackFn )
		mCallbackFn( buffer, getSampleRate() );
}

} } // namespace cinder::audio
