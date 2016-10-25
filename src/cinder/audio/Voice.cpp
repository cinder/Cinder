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

#include "cinder/audio/Voice.h"
#include "cinder/audio/Context.h"
#include "cinder/audio/GainNode.h"
#include "cinder/audio/PanNode.h"

#include <map>

using namespace std;
using namespace ci;

namespace cinder { namespace audio {

// ----------------------------------------------------------------------------------------------------
// MixerImpl
// ----------------------------------------------------------------------------------------------------

// A private mixer implementation, which adds gain and panning to each voice with Node composition.
// This will hopefully be replaced with a yet-to-be-determined MixerNode class, which has a custom pullInputs
// and performs the gain / pan as a post-processing step

class MixerImpl {
public:

	static MixerImpl *get();

	//! returns the number of connected busses.
	void	setBusVolume( size_t busId, float volume );
	float	getBusVolume( size_t busId );
	void	setBusPan( size_t busId, float pos );
	float	getBusPan( size_t busId );
	NodeRef getOutputNode( size_t busId ) const;

	void	addVoice( const VoiceRef &source, const Voice::Options &options );
	void	removeVoice( size_t busId );

	BufferRef loadBuffer( const SourceFileRef &sourceFile );
	// clears the cache of all previously loaded audio file buffers stored in mBufferCache
	void clearBufferCache();

private:
	MixerImpl();

	struct Bus {
		weak_ptr<Voice>		mVoice; // stored as weak reference so that when the user's VoiceRef goes out of scope, it will be removed.
		GainNodeRef			mGain;
		Pan2dNodeRef		mPan;
	};

	size_t getFirstAvailableBusId() const;

	map<size_t, Bus> mBusses;							// key is bus id
	map<SourceFileRef, BufferRef> mBufferCache;		// key is the source file
};

MixerImpl* MixerImpl::get()
{
	static unique_ptr<MixerImpl> sMixer;
	if( ! sMixer )
		sMixer.reset( new MixerImpl );

	return sMixer.get();
}

MixerImpl::MixerImpl()
{
	Context::master()->enable();
}

void MixerImpl::addVoice( const VoiceRef &source, const Voice::Options &options )
{
	Context *ctx = Context::master();

	source->mBusId = getFirstAvailableBusId();

	MixerImpl::Bus &bus = mBusses[source->mBusId];

	bus.mVoice = source;
	bus.mGain = ctx->makeNode( new GainNode( Node::Format().channels( options.getChannels() ) ) );
	bus.mPan = ctx->makeNode( new Pan2dNode() );

	source->getInputNode() >> bus.mGain >> bus.mPan;

	if( options.getConnectToMaster() )
		bus.mPan >> ctx->getOutput();
}

void MixerImpl::removeVoice( size_t busId )
{
	auto it = mBusses.find( busId );
	CI_ASSERT( it != mBusses.end() );

	it->second.mPan->disconnectAllOutputs();
	mBusses.erase( it );
}

BufferRef MixerImpl::loadBuffer( const SourceFileRef &sourceFile )
{
	auto cached = mBufferCache.find( sourceFile );
	if( cached != mBufferCache.end() )
		return cached->second;
	else {
		BufferRef result = sourceFile->loadBuffer();
		mBufferCache.insert( make_pair( sourceFile, result ) );
		return result;
	}
}
	
void MixerImpl::clearBufferCache()
{
	mBufferCache.clear();
}

size_t MixerImpl::getFirstAvailableBusId() const
{
	size_t result = 0;
	for( const auto &bus : mBusses ) {
		if( bus.first != result )
			break;

		result++;
	}

	return result;
}

void MixerImpl::setBusVolume( size_t busId, float volume )
{
	mBusses[busId].mGain->setValue( volume );
}

float MixerImpl::getBusVolume( size_t busId )
{
	return mBusses[busId].mGain->getValue();
}

void MixerImpl::setBusPan( size_t busId, float pos )
{
	auto pan = mBusses[busId].mPan;
	if( pan )
		pan->setPos( pos );
}

float MixerImpl::getBusPan( size_t busId )
{
	auto pan = mBusses[busId].mPan;
	if( pan )
		return mBusses[busId].mPan->getPos();

	return 0;
}

NodeRef MixerImpl::getOutputNode( size_t busId ) const
{
	const auto &bus = mBusses.at( busId );

	if( bus.mPan )
		return bus.mPan;
	else
		return bus.mGain;
}

// ----------------------------------------------------------------------------------------------------
// Voice
// ----------------------------------------------------------------------------------------------------

VoiceRef Voice::create( const CallbackProcessorFn &callbackFn, const Options &options )
{
	VoiceRef result( new VoiceCallbackProcessor( callbackFn, options ) );
	MixerImpl::get()->addVoice( result, options );

	return result;
}

VoiceSamplePlayerNodeRef Voice::create( const SourceFileRef &sourceFile, const Options &options )
{
	VoiceSamplePlayerNodeRef result( new VoiceSamplePlayerNode( sourceFile, options ) );
	MixerImpl::get()->addVoice( result, options );

	return result;
}

Voice::~Voice()
{
	MixerImpl::get()->removeVoice( mBusId );
}
	
void Voice::clearBufferCache()
{
	MixerImpl::get()->clearBufferCache();
}

float Voice::getVolume() const
{
	return MixerImpl::get()->getBusVolume( mBusId );
}

float Voice::getPan() const
{
	return MixerImpl::get()->getBusPan( mBusId );
}

void Voice::setVolume( float volume )
{
	MixerImpl::get()->setBusVolume( mBusId, volume );
}

void Voice::setPan( float pan )
{
	MixerImpl::get()->setBusPan( mBusId, pan );
}

void Voice::start()
{
	getInputNode()->enable();
}

void Voice::pause()
{
	getInputNode()->disable();
}

void Voice::stop()
{
	getInputNode()->disable();
}

bool Voice::isPlaying() const
{
	return getInputNode()->isEnabled();
}

NodeRef Voice::getOutputNode() const
{
	return MixerImpl::get()->getOutputNode( mBusId );
}

// ----------------------------------------------------------------------------------------------------
// VoiceSamplePlayerNode
// ----------------------------------------------------------------------------------------------------

VoiceSamplePlayerNode::VoiceSamplePlayerNode( const SourceFileRef &sourceFile, const Options &options )
	: Voice()
{
	size_t requiredSampleRate = audio::master()->getSampleRate();
	SourceFileRef sf = requiredSampleRate == sourceFile->getSampleRate() ? sourceFile : sourceFile->cloneWithSampleRate( requiredSampleRate );

	if( sf->getNumFrames() <= options.getMaxFramesForBufferPlayback() ) {
		BufferRef buffer = MixerImpl::get()->loadBuffer( sf );
		mNode = Context::master()->makeNode( new BufferPlayerNode( buffer ) );
	} else
		mNode = Context::master()->makeNode( new FilePlayerNode( sf ) );
}

void VoiceSamplePlayerNode::start()
{
	if( mNode->isEof() )
		mNode->start();

	mNode->enable();
}

void VoiceSamplePlayerNode::stop()
{
	mNode->stop();
}

// ----------------------------------------------------------------------------------------------------
// VoiceCallbackProcessor
// ----------------------------------------------------------------------------------------------------

VoiceCallbackProcessor::VoiceCallbackProcessor( const CallbackProcessorFn &callbackFn, const Options &options )
	: Voice()
{
	mNode = Context::master()->makeNode( new CallbackProcessorNode( callbackFn, Node::Format().channels( options.getChannels() ) ) );
}

} } // namespace cinder::audio
