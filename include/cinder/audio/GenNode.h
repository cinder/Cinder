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
#include "cinder/audio/WaveTable.h"

namespace cinder { namespace audio {

//! Typedef for a shared_ptr to the base GenNode. If all you need to set on the GenNode is the frequency, you can reference the Node with this.
typedef std::shared_ptr<class GenNode>				GenNodeRef;
// Typedefs for shared_ptr's to all available GenNode types.
typedef std::shared_ptr<class GenNoiseNode>			GenNoiseNodeRef;
typedef std::shared_ptr<class GenPhasorNode>		GenPhasorNodeRef;
typedef std::shared_ptr<class GenSineNode>			GenSineNodeRef;
typedef std::shared_ptr<class GenTriangleNode>		GenTriangleNodeRef;
typedef std::shared_ptr<class GenTableNode>			GenTableNodeRef;
typedef std::shared_ptr<class GenOscNode>			GenOscNodeRef;
typedef std::shared_ptr<class GenPulseNode>			GenPulseNodeRef;

//! Base class for InputNode's that generate audio samples. Gen's are always mono channel.
class CI_API GenNode : public InputNode {
  public:
	//! Sets the frequency in hertz to a constant value of \a freq.
	void setFreq( float freq )		{ mFreq.setValue( freq ); }
	//! Returns the current frequency in hertz.
	float getFreq() const			{ return mFreq.getValue(); }
	//! Returns a pointer to the Param, which can be used to animate the frequency.
	Param* getParamFreq()			{ return &mFreq; }
	//! Sets the starting phase of the next processed block, in radians.
	void	setPhase( float phase );

  protected:
	GenNode( const Format &format = Format() );
	GenNode( float freq, const Format &format = Format() );

	void initialize() override;
	void initImpl();

	float mSamplePeriod;

	Param mFreq;
	float mPhase;
};

//! Noise generator. \note The frequency parameter is ignored.
class CI_API GenNoiseNode : public GenNode {
  public:
	//! Constructs a GenNoiseNode with optional \a format.
	GenNoiseNode( const Format &format = Format() ) : GenNode( format ) {}

  protected:
	void process( Buffer *buffer ) override;
};

//! Phase generator, i.e. ramping waveform that runs from 0 to 1.
class CI_API GenPhasorNode : public GenNode {
  public:
	//! Constructs a GenPhasorNode with optional \a format.
	GenPhasorNode( const Format &format = Format() ) : GenNode( format ) {}
	GenPhasorNode( float freq, const Format &format = Format() ) : GenNode( freq, format ) {}

  protected:
	void process( Buffer *buffer ) override;
};

//! Sine waveform generator.
class CI_API GenSineNode : public GenNode {
  public:
	GenSineNode( const Format &format = Format() ) : GenNode( format )	{}
	GenSineNode( float freq, const Format &format = Format() ) : GenNode( freq, format ) {}

  protected:
	void process( Buffer *buffer ) override;
};

//! Triangle waveform generator.
class CI_API GenTriangleNode : public GenNode {
  public:
	GenTriangleNode( const Format &format = Format() );
	GenTriangleNode( float freq, const Format &format = Format() );

	void setUpSlope( float up )		{ mUpSlope = up; }
	void setDownSlope( float down )	{ mDownSlope = down; }

	float getUpSlope() const		{ return mUpSlope; }
	float getDownSlope() const		{ return mDownSlope; }

  protected:
	void process( Buffer *buffer ) override;

  private:
	std::atomic<float> mUpSlope, mDownSlope;
};

//! Basic table-lookup oscillator. \note aliasing will occur at higher frequencies, in this case refer to GenOscNode which is more robust.
class CI_API GenTableNode : public GenNode {
  public:
	GenTableNode( const Format &format = Format() )	: GenNode( format )		{}
	GenTableNode( float freq, const Format &format = Format() ) : GenNode( freq, format )	{}

	//! Assigns \a waveTable as the internal wavetable. This allows one to share a WaveTable across multiple Node's.
	void setWaveTable( const WaveTableRef &waveTable )	{ mWaveTable = waveTable; }
	//! Returns a reference to the current wavetable.
	const WaveTableRef&	getWaveTable()	{ return mWaveTable; }

  protected:
	void initialize()				override;
	void process( Buffer *buffer )	override;

	WaveTableRef	mWaveTable;
};

//! General purpose, band-limited oscillator using wavetable lookup.
class CI_API GenOscNode : public GenNode {
  public:
	GenOscNode( const Format &format = Format() );
	GenOscNode( float freq, const Format &format = Format() );
	GenOscNode( WaveformType waveformType, float freq = 0, const Format &format = Format() );

	//! Sets the WaveformType of the internal wavetable. This can be a heavy operation and requires thread synchronization, so be careful not to block the audio thread for too long.
	void setWaveform( WaveformType waveformType );
	//! Assigns \a waveTable as the internal wavetable. This allows one to share a WaveTable2d across multiple Node's.
	void setWaveTable( const WaveTable2dRef &waveTable )	{ mWaveTable = waveTable; }
	//! Returns a reference to the current wavetable.
	const WaveTable2dRef getWaveTable() const				{ return mWaveTable; }

	//! Returns the current WaveformType
	WaveformType	getWaveForm() const			{ return mWaveformType; }
	//! Returns the size of the owned WaveTable2d.
	size_t			getTableSize() const		{ return mWaveTable->getTableSize(); }

  protected:
	void initialize() override;
	void process( Buffer *buffer ) override;

	WaveTable2dRef		mWaveTable;
	WaveformType		mWaveformType;
};

//! Pulse waveform generator with variable pulse width. Based on wavetable lookup of two band-limited sawtooth waveforms, subtracted from each other.
class CI_API GenPulseNode : public GenNode {
  public:
	GenPulseNode( const Format &format = Format() );
	GenPulseNode( float freq, const Format &format = Format() );

	//! Set the pulse width (aka 'duty cycle'). Expected range is between [0:1] (default = 0.5, creating a square wave).
	void			setWidth( float width )	{ mWidth.setValue( width ); }
	//! Get the current pulse width. \see setWidth()
	float			getWidth() const		{ return mWidth.getValue(); }
	//! Returns the Param associated with the width (aka 'duty cycle').  Expected range is between [0:1].
	Param*			getParamWidth()			{ return &mWidth; }

  protected:
	void initialize() override;
	void process( Buffer *buffer ) override;

	WaveTable2dRef			mWaveTable;
	BufferDynamic			mBuffer2;
	Param					mWidth;
};

} } // namespace cinder::audio
