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

#include "cinder/audio/InputNode.h"
#include "cinder/audio/SamplePlayerNode.h"
#include "cinder/audio/Source.h"

#include <memory>

namespace cinder { namespace audio {

typedef std::shared_ptr<class Voice> VoiceRef;
typedef std::shared_ptr<class VoiceSamplePlayerNode> VoiceSamplePlayerNodeRef;

//! \brief Interface for performing high-level audio playback tasks.
//!
//! A Voice is an abstraction of something you can hear, and as such they send audio
//! to your computers output device. Currently supports file playback and processing audio with a callback function.
//! Each Voice has controls for setting the volume and pan of its signal, and supports
//! start(), stop(), and pause() functionality.
//!
//! Underneath, playback is managed by a Node, which can be retrieved via the virtual getNode() method to
//! perform more complex tasks.
//!
class CI_API Voice {
  public:
	//! Optional parameters passed into Voice::create() methods.
	struct Options {
		Options()
			: mChannels( 0 ), mMaxFramesForBufferPlayback( 96000 ), mConnectToMaster( true )
		{}

		//! Sets the number of channels for the Voice.
		Options& channels( size_t ch )							{ mChannels = ch; return *this; }

		//! \brief Sets the maximum number of frames acceptable for a VoiceSamplePlayerNode to use in-memory buffer playback via BufferPlayerNode
		//!
		//! If the file is larger than this, it will be streamed from disk using a FilePlayerNode. Default = 96,000.
		Options& maxFramesForBufferPlayback( size_t frames )	{ mMaxFramesForBufferPlayback = frames; return *this; }

		//! \brief Sets whether the Voice's output is automatically connected to master()->getOutput().
		//!
		//! If set to false, the user must manage the connection of its output node. Default = true, automaitically connects to master output.
		Options& connectToMaster( bool shouldConnect )			{ mConnectToMaster = shouldConnect; return *this; }

		//! Returns the number of configured channels. \see channels()
		size_t			getChannels() const						{ return mChannels; }
		//! Returns the configured frame allowance for buffer playback. \see maxFramesForBufferPlayback()
		size_t			getMaxFramesForBufferPlayback() const	{ return mMaxFramesForBufferPlayback; }
		//! Returns whether or not the Voice will be automatically connected to master()->getOutput().
		bool			getConnectToMaster() const				{ return mConnectToMaster; }

	  protected:
		size_t			mChannels, mMaxFramesForBufferPlayback;
		bool			mConnectToMaster;
	};

	virtual ~Voice();

	//! Creates a Voice that manages sample playback of an audio file pointed at with \a sourceFile.
	static VoiceSamplePlayerNodeRef create( const SourceFileRef &sourceFile, const Options &options = Options() );
	//! Creates a Voice that continuously calls \a callbackFn to process a Buffer of samples.
	static VoiceRef create( const CallbackProcessorFn &callbackFn, const Options &options = Options() );
	//! Clears all audio file buffers that that are cached in the Mixer
	static void clearBufferCache();

	//! Starts the Voice. Does nothing if currently playing. \note In the case of a VoiceSamplePlayerNode and the sample has reached EOF, start() will start from the beginning.
	virtual void start();
	//! Stops the Voice, resetting its state to the same as when it was created.
	virtual void stop();
	//! Pauses the Voice inits current state. start() will resume from here.
	virtual void pause();
	//! Returns whether the Voice is currently playing or not.
	virtual bool isPlaying() const;
	//! Returns the Node that is the source of this Voice's audio production. The Node type is determined by the Voice subclassed.
	virtual NodeRef getInputNode() const = 0;
	//! Returns the Node that is used for connected the Voice to the greater audio graph (by default, it is connected to Context::master()).
	virtual NodeRef getOutputNode() const;

	void setVolume( float volume );
	void setPan( float pos );

	float getVolume() const;
	float getPan() const;

  protected:
	Voice() : mBusId( 0 ) {}

  private:
	size_t mBusId;
	friend class MixerImpl;
};

//! \brief Concrete Voice for sample playback.
//!
//! Depending on the size of the specified file, playback will either be done in-memory (with BufferPlayerNode)
//! or streaming (with FilePlayerNode). The maximum frames for in-memory playback can be specified with Voice::Options::maxFramesForBufferPlayback()
//! Create with Voice::create( const SourceFileRef &sourceFile, const Options &options )
class CI_API VoiceSamplePlayerNode : public Voice {
  public:

	NodeRef getInputNode() const override			{ return mNode; }
	//! Returns a shared_ptr of the owned SamplePlayerNode.
	SamplePlayerNodeRef getSamplePlayerNode() const			{ return mNode; }

	void start() override;
	void stop() override;

  protected:
	VoiceSamplePlayerNode( const SourceFileRef &sourceFile, const Options &options );
	SamplePlayerNodeRef mNode;

	friend class Voice;
};

//! Concrete Voice for processing audio with a callback function. \see CallbackProcessorFn
class CI_API VoiceCallbackProcessor : public Voice {
  public:
	NodeRef getInputNode() const override			{ return mNode; }

  protected:
	VoiceCallbackProcessor( const CallbackProcessorFn &callbackFn, const Options &options );

	CallbackProcessorNodeRef mNode;
	friend class Voice;
};

} } // namespace cinder::audio
