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

#include "cinder/Cinder.h"
#if defined( CINDER_UWP ) || ( _WIN32_WINNT >= 0x0600 ) // requires Windows Vista+

#include "cinder/audio/Context.h"

namespace cinder { namespace audio { namespace msw {

struct WasapiRenderClientImpl;
struct WasapiCaptureClientImpl;

class OutputDeviceNodeWasapi : public OutputDeviceNode {
  public:
	OutputDeviceNodeWasapi( const DeviceRef &device, bool exclusiveMode, const Format &format );
	~OutputDeviceNodeWasapi();

protected:
	void initialize()				override;
	void uninitialize()				override;
	void enableProcessing()			override;
	void disableProcessing()		override;
	bool supportsProcessInPlace() const	override	{ return false; }

  private:
	void renderInputs();

	std::unique_ptr<WasapiRenderClientImpl>		mRenderImpl;
	std::vector<char>							mSampleBuffer;

	friend WasapiRenderClientImpl;
};

class InputDeviceNodeWasapi : public InputDeviceNode {
public:
	InputDeviceNodeWasapi( const DeviceRef &device, bool exclusiveMode, const Format &format = Format() );
	virtual ~InputDeviceNodeWasapi();

protected:
	void initialize()				override;
	void uninitialize()				override;
	void enableProcessing()			override;
	void disableProcessing()		override;
	void process( Buffer *buffer )	override;

private:
	std::unique_ptr<WasapiCaptureClientImpl>	mCaptureImpl;

	friend WasapiCaptureClientImpl;
};

class ContextWasapi : public Context {
  public:
	ContextWasapi();
	OutputDeviceNodeRef	createOutputDeviceNode( const DeviceRef &device, const Node::Format &format = Node::Format() )	override;
	InputDeviceNodeRef	createInputDeviceNode( const DeviceRef &device, const Node::Format &format = Node::Format() )	override;

	//! Sets whether 'Exclusive-Mode Streams' are used for OutputDeviceNode and InputDeviceNode instances. Default is false ('shared mode').
	//! \note when exclusive mode is enabled, the channel count for OutputDeviceNode and InputDeviceNode always matches the Device's number of outputs or inputs.
	void setExclusiveModeEnabled( bool enable = true )	{ mExclusiveMode = enable; }
	//! Returns whether 'Exclusive-Mode Streams' are used for OutputDeviceNode and InputDeviceNode instances. Default is false ('shared mode').
	bool isExclusiveModeEnabled() const					{ return mExclusiveMode; }

  private:
	bool	mExclusiveMode = false;
};

class WasapiExc : public AudioExc {
  public:
	WasapiExc( const std::string &description );
	WasapiExc( const std::string &description, int32_t hr );
};

} } } // namespace cinder::audio::msw

#endif // defined( CINDER_UWP ) || ( _WIN32_WINNT >= 0x0600 )
