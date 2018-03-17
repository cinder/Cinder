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

#include "cinder/audio/Buffer.h"
#include "cinder/audio/Exception.h"
#include "cinder/Noncopyable.h"

#include <memory>
#include <atomic>
#include <set>

namespace cinder { namespace audio {

typedef std::shared_ptr<class Context>			ContextRef;
typedef std::shared_ptr<class Node>				NodeRef;

//! \brief Fundamental building block for creating an audio processing graph.
//!
//!	Node's allow for flexible combinations of synthesis, analysis, effects, file reading/writing, etc, and are designed so that
//! that it is easy for users to add their own custom subclasses to a processing graph.
//!
//! A Node is owned by a Context, and as such must by created using its makeNode() method, or one of the specializations for platform
//! specific Node's.
//!
//! Audio Node's are designed to operate on two different threads: a 'user' thread (i.e. main / UI) and an audio thread. Specifically,
//! methods for connecting and disconnecting are expected to come from the 'user' thread, while the Node's process() and internal pulling
//! methods are called from a hard real-time thread. A mutex is used (Context::getMutex()) to synchronize connection changes and pulling
//! the audio graph. Note that if the Node's initialize() method is heavy, it can be called before connected to anything, so as to not
//! block the audio graph. This must be done throught the Context::initializeNode() interface.
//!
//! Subclassing: implement process( Buffer *buffer ) to perform audio processing. A Node does not have access to its owning Context until
//! initialize() is called, uninitialize() is called before a Node is deallocated or channel counts change.
//!
//! \see InputNode, OutputNode
class CI_API Node : public std::enable_shared_from_this<Node>, private Noncopyable {
  public:
	//! Used to specify how the corresponding channels are to be resolved between two connected Node's,
	//! based on either a Node's input (the default), it's output, or specified by user.
	enum class ChannelMode {
		//! Number of channels has been specified by user or is non-settable.
		SPECIFIED,
		//! This Node matches it's channels with it's input.
		MATCHES_INPUT,
		//! This Node matches it's channels with it's output.
		MATCHES_OUTPUT
	};

	struct Format {
		Format()
			: mChannels( 0 ), mChannelMode( ChannelMode::MATCHES_INPUT ), mAutoEnable( true ), mAutoEnableSet( false )
		{}

		//! Sets the number of channels for the Node.
		Format& channels( size_t ch )							{ mChannels = ch; return *this; }
		//! Controls how channels will be matched between connected Node's, if necessary. \see ChannelMode.
		Format& channelMode( ChannelMode mode )					{ mChannelMode = mode; return *this; }
		//! Whether or not the Node will be auto-enabled when connection changes occur.  Default is true for base \a Node class, although sub-classes may choose a different default.
		Format& autoEnable( bool autoEnable = true )			{ setAutoEnable( autoEnable ); return *this; }

		size_t			getChannels() const						{ return mChannels; }
		ChannelMode		getChannelMode() const					{ return mChannelMode; }
		bool			getAutoEnable() const					{ return mAutoEnable; }
		//! Returns whether the user specified whether auto-enable is on or off. Some types of Nodes will choose
		//! a different default for auto-enable, ex. the base Node type will turn it on by default, whereas `InputNode`s and `OutputNode`s
		//! will turn it off and the user will have to manually set this property or call enable() on the Node themselves.
		bool			isAutoEnableSet() const					{ return mAutoEnableSet; }

		void setChannels( size_t ch )							{ mChannels = ch; }
		void setChannelMode( ChannelMode mode )					{ mChannelMode = mode; }
		void setAutoEnable( bool autoEnable	)					{ mAutoEnable = autoEnable; mAutoEnableSet = true; }

	  private:
		size_t			mChannels;
		ChannelMode		mChannelMode;
		bool			mAutoEnable;
		bool			mAutoEnableSet;
	};

	Node( const Format &format );
	virtual ~Node();

	//! Enables this Node for processing. Same as `setEnabled( true )`.
	void enable();
	//! Enables this Node for processing at \a when seconds, measured against Context::getNumProcessedSeconds(). Same as `setEnabled( true, when )`.
	void enable( double when );
	//! Disables this Node for processing. Same as `setEnabled( false )`.
	void disable();
	//! Disables this Node for processing at \a when seconds, measured against Context::getNumProcessedSeconds(). Same as `setEnabled( false, when )`.
	void disable( double when );
	//! Sets whether this Node is enabled for processing or not.
	void setEnabled( bool b );
	//! Sets whether this Node is enabled for processing or not at \a when seconds, measured against Context::getNumProcessedSeconds().
	void setEnabled( bool b, double when );
	//! Returns whether this Node is enabled for processing or not.
	bool isEnabled() const						{ return mEnabled; }

	//! Connects this Node to \a output.
	virtual void connect( const NodeRef &output );
	//! Disconnects this Node from \a output.
	virtual void disconnect( const NodeRef &output );
	//! Disconnects this Node from all inputs and outputs.
	virtual void disconnectAll();
	//! Disconnects this Node from all outputs.
	virtual void disconnectAllOutputs();
	//! Disconnects all of this Node's inputs.
	virtual void disconnectAllInputs();

	//! Returns the number of inputs connected to this Node.
	size_t		getNumConnectedInputs() const;
	//! Returns the number of outputs this Node is connected to.
	size_t		getNumConnectedOutputs() const;
	//! Returns true if \a input is connected to this Node as an input, false otherwise.
	bool		isConnectedToInput( const NodeRef &input ) const;
	//! Returns true if \a output is connected to this Node as an output, false otherwise.
	bool		isConnectedToOutput( const NodeRef &output ) const;
	//! Returns the \a Context associated with this \a Node. \note Cannot be called from within a \a Node's constructor. Use initialize instead.
	ContextRef	getContext() const					{ return mContext.lock(); }
	//! Returns the number of channels this Node will process.
	size_t		getNumChannels() const				{ return mNumChannels; }
	//! Returns the channel mode. \see ChannelMode.
	ChannelMode getChannelMode() const				{ return mChannelMode; }
	//! Returns the maximum number of channels any input has.
	size_t		getMaxNumInputChannels() const;
	//! Returns the samplerate of this Node, which is governed by the Context's OutputNode.
	size_t		getSampleRate() const;
	//! Returns the number of frames processed in one block by this Node, which is governed by the Context's OutputNode.
	size_t		getFramesPerBlock() const;
	//! Returns whether this Node is automatically enabled / disabled when connected
	bool		isAutoEnabled() const				{ return mAutoEnabled; }
	//! Sets whether this Node is automatically enabled / disabled when connected
	void		setAutoEnabled( bool b = true )		{ mAutoEnabled = b; }
	//! Returns whether this Node is in an initialized state and is capable of processing audio.
	bool		isInitialized() const				{ return mInitialized; }
	//! Returns whether this Node will process audio with an in-place Buffer.
	bool		getProcessesInPlace() const			{ return mProcessInPlace; }
	//! Returns whether it is possible to connect to \a input, example reasons of failure would be this == Node, or Node is already an input.
	bool		canConnectToInput( const NodeRef &input );
	//! Returns true if there is an unmanageable cycle betweeen \a sourceNode and \a destNode. If any Node's in the traversal returns true for supportsCycles(), this method will return false.
	bool		checkCycle( const NodeRef &sourceNode, const NodeRef &destNode ) const;

	//! Returns an immutable reference to the inputs container.
	const std::set<NodeRef>&	getInputs() const		{ return mInputs; }
	//! Returns a copy of the NodeRef's referenced by the this Node as outputs.  The copy is necessary because outputs are stored internally with weak_ptr's.
	std::vector<NodeRef>		getOutputs() const;

	//! Returns a string representing the name of this Node type. Default returns a demangled, compiler-specific class name.
	virtual std::string getName() const;
	//! Sets this Node's name to a user-specified string.
	void				setName( const std::string &name )	{ mName = name; }

	//! Usually used internally by a Node subclass, returns a pointer to the internal buffer storage.
	Buffer*			getInternalBuffer()			{ return &mInternalBuffer; }
	//! Usually used internally by a Node subclass, returns a pointer to the internal buffer storage.
	const Buffer*	getInternalBuffer() const	{ return &mInternalBuffer; }
	//! Usually called internally by the Node, in special cases sub-classes may need to call this on other Node's.
	void			pullInputs( Buffer *inPlaceBuffer );

  protected:

	//! Called before audio buffers need to be used. There is always a valid Context at this point. Default implementation is empty.
	virtual void initialize()				{}
	//! Called once the contents of initialize are no longer relevant, i.e. connections have changed. Default implementation is empty.
	//! \note Not guaranteed to be called at Node destruction.
	virtual void uninitialize()				{}
	//! Callled when this Node should enable processing. Initiated from Node::enable(). Default implementation is empty.
	virtual void enableProcessing()			{}
	//! Callled when this Node should disable processing. Initiated from Node::disable(). Default implementation is empty.
	virtual void disableProcessing()		{}
	//! Override to perform audio processing on \t buffer. Default implementation is empty.
	virtual void process( Buffer *buffer );
	//! Override to customize how input Nodes are summed into the internal summing buffer. You usually don't need to do this.
	virtual void sumInputs();

	//! Default implementation returns true if numChannels matches our format.
	virtual bool supportsInputNumChannels( size_t numChannels ) const	{ return mNumChannels == numChannels; }
	//! Default implementation returns false, return true if it makes sense for the Node to be processed in a cycle (eg. Delay).
	virtual bool supportsCycles() const									{ return false; }
	//! Default implementation returns true, subclasses should return false if they must process out-of-place (summing).
	virtual bool supportsProcessInPlace() const							{ return true; }

	//! \note Connection methods \must be called on a non-audio thread and synchronized with the Context's mutex.
	virtual void connectInput( const NodeRef &input );
	virtual void disconnectInput( const NodeRef &input );
	virtual void disconnectOutput( const NodeRef &output );
	virtual void configureConnections();

	void setupProcessWithSumming();
	void notifyConnectionsDidChange();
	bool inputChannelsAreUnequal() const;

	//! Only Node subclasses can specify num channels directly - users specify via Format at construction time.
	void setNumChannels( size_t numChannels );
	//! Only Node subclasses can specify channel mode directly - users specify via Format at construction time.
	void setChannelMode( ChannelMode mode );

	//! \brief Returns a pair of frame indices for Nodes that wish to support sample accurate enable and disable.
	//!
	//! The first index is where processing should start, the second is where it should	end. Should only be called on the audio thread from within a Node's process() method.
	//! Unless scheduled (with Context::schedule()), this will be [0, getFramesPerBlock()]
	const std::pair<size_t, size_t>& getProcessFramesRange() const	{ return mProcessFramesRange; }

	void initializeImpl();
	void uninitializeImpl();

	BufferDynamic*			getSummingBuffer()			{ return &mSummingBuffer; }
	const BufferDynamic*	getSummingBuffer() const	{ return &mSummingBuffer; }

  private:
	// The owning Context calls this.
	void setContext( const ContextRef &context )	{ mContext = context; }

	std::weak_ptr<Context>	mContext;
	std::atomic<bool>		mEnabled;
	std::atomic<bool>		mEventScheduled;
	bool					mInitialized;
	bool					mAutoEnabled;
	bool					mProcessInPlace;
	ChannelMode				mChannelMode;
	size_t					mNumChannels;

	std::pair<size_t, size_t>	mProcessFramesRange;

	uint64_t				mLastProcessedFrame;
	std::string				mName;
	BufferDynamic			mInternalBuffer, mSummingBuffer;

	std::set<std::shared_ptr<Node> >	mInputs;
	std::vector<std::weak_ptr<Node> >	mOutputs;

	friend class Context;
	friend class Param;
};

//! Enable connection syntax: `input >> output`, which is equivelant to `input->connect( output )`. Enables chaining.  \return the connected \a output
inline const NodeRef& operator>>( const NodeRef &input, const NodeRef &output )
{
	input->connect( output );
	return output;
}

//! a Node that can be pulled without being connected to any outputs.
class CI_API NodeAutoPullable : public Node {
  public:
	virtual ~NodeAutoPullable();

	void connect( const NodeRef &output )					override;
	void connectInput( const NodeRef &input )				override;
	void disconnectInput( const NodeRef &input )			override;
	//! Overridden to remove from Context's auto-pulled list if there are no more outputs
	void disconnectOutput( const NodeRef &output )			override;
	//! Overridden to also remove from Context's auto-pulled list
	void disconnectAllOutputs()								override;

  protected:
	NodeAutoPullable( const Format &format );
	void updatePullMethod();

	bool mIsPulledByContext;
};

//! RAII-style utility class to set a \a Node's enabled state and have it restored at the end of the current scope block.
struct CI_API ScopedEnableNode {
	//! Constructs an object that will store \a node's enabled state and restore it at the end of the current scope.
	ScopedEnableNode( const NodeRef &node );
	//! Constructs an object that will set \a node's enabled state to \a enable and restore it to the original state at the end of the current scope.
	ScopedEnableNode( const NodeRef &node, bool enable );
	~ScopedEnableNode();
  private:
	NodeRef		mNode;
	bool		mWasEnabled;
};

class CI_API NodeCycleExc : public AudioExc {
  public:
	NodeCycleExc( const NodeRef &sourceNode, const NodeRef &destNode );
};

} } // namespace cinder::audio
