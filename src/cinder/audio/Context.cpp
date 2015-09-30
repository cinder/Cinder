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

#include "cinder/audio/Context.h"
#include "cinder/audio/InputNode.h"
#include "cinder/audio/Utilities.h"
#include "cinder/audio/dsp/Converter.h"

#include "cinder/Cinder.h"
#include "cinder/app/AppBase.h"

#include <sstream>

#if defined( CINDER_COCOA )
	#include "cinder/audio/cocoa/ContextAudioUnit.h"
	#if defined( CINDER_MAC )
		#include "cinder/audio/cocoa/DeviceManagerCoreAudio.h"
	#else // CINDER_COCOA_TOUCH
		#include "cinder/audio/cocoa/DeviceManagerAudioSession.h"
	#endif
#elif defined( CINDER_MSW ) && ( _WIN32_WINNT >= 0x0600 ) // Windows Vista+
	#define CINDER_AUDIO_WASAPI
	#include "cinder/audio/msw/ContextWasapi.h"
	#include "cinder/audio/msw/DeviceManagerWasapi.h"
#else
	#define CINDER_AUDIO_DISABLED
#endif

#if ! defined( CINDER_AUDIO_DISABLED )

using namespace std;

namespace cinder { namespace audio {

std::shared_ptr<Context>		Context::sMasterContext;
std::unique_ptr<DeviceManager>	Context::sDeviceManager;

bool sIsRegisteredForCleanup = false;

// static
void Context::registerClearStatics()
{
	sIsRegisteredForCleanup = true;

	// A signal is registered for app cleanup in order to ensure that all Node's and their
	// dependencies are destroyed before static memory goes down - this avoids a crash at cleanup
	// in r8brain's static processing containers.
	// TODO: consider leaking the master context by default and providing a public clear function.
	app::AppBase::get()->getSignalCleanup().connect( [] {
		sDeviceManager.reset();
		sMasterContext.reset();
	} );
}

// static
Context* Context::master()
{
	if( ! sMasterContext ) {
#if defined( CINDER_COCOA )
		sMasterContext.reset( new cocoa::ContextAudioUnit() );
#elif defined( CINDER_MSW )
	#if( _WIN32_WINNT >= 0x0600 ) // requires Windows Vista+
		sMasterContext.reset( new msw::ContextWasapi() );
	#else
		sMasterContext.reset( new msw::ContextXAudio() );
	#endif
#endif
		if( ! sIsRegisteredForCleanup )
			registerClearStatics();
	}
	return sMasterContext.get();
}

// static
DeviceManager* Context::deviceManager()
{
	if( ! sDeviceManager ) {
#if defined( CINDER_MAC )
		sDeviceManager.reset( new cocoa::DeviceManagerCoreAudio() );
#elif defined( CINDER_COCOA_TOUCH )
		sDeviceManager.reset( new cocoa::DeviceManagerAudioSession() );
#elif defined( CINDER_MSW )
	#if( _WIN32_WINNT > 0x0600 ) // requires Windows Vista+
		sDeviceManager.reset( new msw::DeviceManagerWasapi() );
	//#else
	//	CI_ASSERT( 0 && "TODO: simple DeviceManagerXp" );
	#endif
#endif
		if( ! sIsRegisteredForCleanup )
			registerClearStatics();
	}
	return sDeviceManager.get();
}

// static
void Context::setMaster( Context *masterContext, DeviceManager *deviceManager )
{
	sMasterContext.reset( masterContext );
	sDeviceManager.reset( deviceManager );
}

Context::Context()
	: mEnabled( false ), mAutoPullRequired( false ), mAutoPullCacheDirty( false ), mNumProcessedFrames( 0 )
{
}

Context::~Context()
{
	disable();
	lock_guard<mutex> lock( mMutex );
	uninitializeAllNodes();
}

void Context::enable()
{
	if( mEnabled )
		return;

	const auto &output = getOutput();

	// output may not yet be initialized if no Node's are connected to it.
	if( ! output->isInitialized() )
		output->initializeImpl();

	mEnabled = true;
	getOutput()->enable();
}

void Context::disable()
{
	if( ! mEnabled )
		return;

	mEnabled = false;
	getOutput()->disable();
}

void Context::setEnabled( bool b )
{
	if( b )
		enable();
	else
		disable();
}

void Context::initializeAllNodes()
{
	set<NodeRef> traversedNodes;
	initRecursisve( mOutput, traversedNodes );

	for( const auto& node : mAutoPulledNodes )
		initRecursisve( node, traversedNodes );
}

void Context::uninitializeAllNodes()
{
	set<NodeRef> traversedNodes;
	uninitRecursive( mOutput, traversedNodes );

	for( const auto& node : mAutoPulledNodes )
		uninitRecursive( node, traversedNodes );
}

void Context::disconnectAllNodes()
{
	set<NodeRef> traversedNodes;
	disconnectRecursive( mOutput, traversedNodes );

	for( const auto& node : mAutoPulledNodes )
		disconnectRecursive( node, traversedNodes );
}

void Context::setOutput( const OutputNodeRef &output )
{
	mOutput = output;
}

const OutputNodeRef& Context::getOutput()
{
	if( ! mOutput )
		mOutput = createOutputDeviceNode();
	return mOutput;
}

void Context::initializeNode( const NodeRef &node )
{
	node->initializeImpl();
}

void Context::uninitializeNode( const NodeRef &node )
{
	node->uninitializeImpl();
}

void Context::disconnectRecursive( const NodeRef &node, set<NodeRef> &traversedNodes )
{
	if( ! node || traversedNodes.count( node ) )
		return;

	traversedNodes.insert( node );

	for( auto &input : node->getInputs() )
		disconnectRecursive( input, traversedNodes );

	node->disconnectAllInputs();
}

void Context::initRecursisve( const NodeRef &node, set<NodeRef> &traversedNodes )
{
	if( ! node || traversedNodes.count( node ) )
		return;

	traversedNodes.insert( node );

	for( auto &input : node->getInputs() )
		initRecursisve( input, traversedNodes );

	node->configureConnections();
}

void Context::uninitRecursive( const NodeRef &node, set<NodeRef> &traversedNodes )
{
	if( ! node || traversedNodes.count( node ) )
		return;

	traversedNodes.insert( node );

	for( auto &input : node->getInputs() )
		uninitRecursive( input, traversedNodes );

	node->uninitializeImpl();
}

void Context::addAutoPulledNode( const NodeRef &node )
{
	mAutoPulledNodes.insert( node );
	mAutoPullRequired = true;
	mAutoPullCacheDirty = true;

	// if not done already, allocate a buffer for auto-pulling that is large enough for stereo processing
	size_t framesPerBlock = getFramesPerBlock();
	if( mAutoPullBuffer.getNumFrames() < framesPerBlock )
		mAutoPullBuffer.setSize( framesPerBlock, 2 );
}

void Context::removeAutoPulledNode( const NodeRef &node )
{
	size_t result = mAutoPulledNodes.erase( node );
	CI_VERIFY( result );

	mAutoPullCacheDirty = true;
	if( mAutoPulledNodes.empty() )
		mAutoPullRequired = false;
}

void Context::schedule( double when, const NodeRef &node, bool enable, const std::function<void ()> &func )
{
	const uint64_t framesPerBlock = (uint64_t)getFramesPerBlock();
	uint64_t eventFrameThreshold = timeToFrame( when, getSampleRate() );

	// Place the threshold back one block so we can process the block first, guarding against wrap around
	if( eventFrameThreshold >= framesPerBlock )
		eventFrameThreshold -= framesPerBlock;

	lock_guard<mutex> lock( mMutex );
	mScheduledEvents.push_back( ScheduledEvent( eventFrameThreshold, node, enable, func ) );
}

bool Context::isAudioThread() const
{
	return mAudioThreadId == std::this_thread::get_id();
}

void Context::preProcess()
{
	mAudioThreadId = std::this_thread::get_id();

	preProcessScheduledEvents();
}

void Context::postProcess()
{
	processAutoPulledNodes();
	postProcessScheduledEvents();
	incrementFrameCount();
}

void Context::incrementFrameCount()
{
	mNumProcessedFrames += getFramesPerBlock();
}

void Context::processAutoPulledNodes()
{
	if( ! mAutoPullRequired )
		return;

	for( Node *node : getAutoPulledNodes() ) {
		mAutoPullBuffer.setNumChannels( node->getNumChannels() );
		node->pullInputs( &mAutoPullBuffer );
		if( ! node->getProcessesInPlace() )
			dsp::mixBuffers( node->getInternalBuffer(), &mAutoPullBuffer );
	}
}

void Context::preProcessScheduledEvents()
{
	const uint64_t framesPerBlock = (uint64_t)getFramesPerBlock();
	const uint64_t numProcessedFrames = mNumProcessedFrames;

	for( auto &event : mScheduledEvents ) {
		if( numProcessedFrames >= event.mEventFrameThreshold ) {
			uint64_t frameOffset = numProcessedFrames - event.mEventFrameThreshold;
			if( event.mEnable ) {
				event.mNode->mProcessFramesRange.first = size_t( framesPerBlock - frameOffset );
				event.mFunc();
			}
			else {
				// set the process range but don't call its function until postProcess() (which should be disable()'ing the Node)
				event.mNode->mProcessFramesRange.second = (size_t)frameOffset;
			}

			event.mFinished = true;
		}
	}
}

void Context::postProcessScheduledEvents()
{
	for( auto eventIt = mScheduledEvents.begin(); eventIt != mScheduledEvents.end(); /* */ ) {
		if( eventIt->mFinished ) {
			if( ! eventIt->mEnable )
				eventIt->mFunc();

			// reset process frame range
			auto &range = eventIt->mNode->mProcessFramesRange;
			range.first = 0;
			range.second = getFramesPerBlock();

			eventIt = mScheduledEvents.erase( eventIt );
		}
		else
			++eventIt;
	}
}

const std::vector<Node *>& Context::getAutoPulledNodes()
{
	if( mAutoPullCacheDirty ) {
		mAutoPullCache.clear();
		for( const NodeRef &node : mAutoPulledNodes )
			mAutoPullCache.push_back( node.get() );
	}
	return mAutoPullCache;
}

namespace {

void printRecursive( ostream &stream, const NodeRef &node, size_t depth, set<NodeRef> &traversedNodes )
{
	if( ! node )
		return;
	for( size_t i = 0; i < depth; i++ )
		stream << "-- ";

	if( traversedNodes.count( node ) ) {
		stream << node->getName() << "\t[ ** already printed ** ]" << endl;
		return;
	}

	traversedNodes.insert( node );

	string channelMode;
	switch( node->getChannelMode() ) {
		case Node::ChannelMode::SPECIFIED: channelMode = "specified"; break;
		case Node::ChannelMode::MATCHES_INPUT: channelMode = "matches input"; break;
		case Node::ChannelMode::MATCHES_OUTPUT: channelMode = "matches output"; break;
	}

	stream << node->getName() << "\t[ " << ( node->isEnabled() ? "enabled" : "disabled" );
	stream << ", ch: " << node->getNumChannels();
	stream << ", ch mode: " << channelMode;
	stream << ", " << ( node->getProcessesInPlace() ? "in-place" : "sum" );
	stream << " ]" << endl;

	for( const auto &input : node->getInputs() )
		printRecursive( stream, input, depth + 1, traversedNodes );
};

} // anonymous namespace

string Context::printGraphToString()
{
	stringstream stream;
	set<NodeRef> traversedNodes;

	printRecursive( stream, getOutput(), 0, traversedNodes );

	if( ! mAutoPulledNodes.empty() ) {
		stream << "(auto-pulled:)" << endl;
		for( const auto& node : mAutoPulledNodes )
			printRecursive( stream, node, 0, traversedNodes );
	}

	return stream.str();
}

// ----------------------------------------------------------------------------------------------------
// MARK: - ScopedEnableContext
// ----------------------------------------------------------------------------------------------------

ScopedEnableContext::ScopedEnableContext( Context *context )
	: mContext( context )
{
	mWasEnabled = ( mContext ? mContext->isEnabled() : false );
}

ScopedEnableContext::ScopedEnableContext( Context *context, bool enable )
	: mContext( context )
{
	if( mContext ) {
		mWasEnabled = mContext->isEnabled();
		mContext->setEnabled( enable );
	}
	else
		mWasEnabled = false;
}

ScopedEnableContext::~ScopedEnableContext()
{
	if( mContext )
		mContext->setEnabled( mWasEnabled );
}

} } // namespace cinder::audio

#endif // ! defined( CINDER_AUDIO_DISABLED )