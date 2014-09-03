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

#include "cinder/Vector.h"
#include "cinder/PolyLine.h"
#include "cinder/gl/VboMesh.h"

#include <vector>

void drawAudioBuffer( const ci::audio::Buffer &buffer, const ci::Rectf &bounds, bool drawFrame = false, const ci::ColorA &color = ci::ColorA( 0, 0.9f, 0, 1 ) );

class Waveform {
  public:
	enum CalcMode { MIN_MAX, AVERAGE };
    Waveform() {}
    Waveform( const std::vector<float> &samples, const ci::ivec2 &waveSize, size_t pixelsPerVertex = 2, CalcMode mode = MIN_MAX )	{ load( samples.data(), samples.size(), waveSize, pixelsPerVertex, mode ); }
    Waveform( const float *samples, size_t numSamples, const ci::ivec2 &waveSize, size_t pixelsPerVertex = 2, CalcMode mode = MIN_MAX )	{ load( samples, numSamples, waveSize, pixelsPerVertex, mode ); }

	void load( const float *samples, size_t numSamples, const ci::ivec2 &waveSize, size_t pixelsPerVertex = 2, CalcMode mode = MIN_MAX );

    const ci::PolyLine2f&	getOutline() const	{ return mOutline; }
	ci::gl::VboMeshRef		getMesh() const		{ return mMesh; };

    bool loaded() { return mOutline.getPoints().size() > 0; }
    
  private:
    ci::PolyLine2f		mOutline;
	ci::gl::VboMeshRef	mMesh;
};

class WaveformPlot {
  public:
	WaveformPlot( const ci::ColorA &colorMinMax = ci::ColorA::gray( 0.5f ), const ci::ColorA &colorAverage = ci::ColorA::gray( 0.75f ) ) : mColorMinMax( colorMinMax ), mColorAverage( colorAverage )	{}

	void load( const std::vector<float> &samples, const ci::Rectf &bounds, size_t pixelsPerVertex = 2 );

	void load( const ci::audio::BufferRef &buffer, const ci::Rectf &bounds, size_t pixelsPerVertex = 2 );

	const std::vector<Waveform>& getWaveforms() const	{ return mWaveforms; }
	const ci::Rectf& getBounds() const					{ return mBounds; }

	void draw();

	void clear()	{ 	mWaveforms.clear(); }

  private:
	std::vector<Waveform> mWaveforms;
	ci::Rectf mBounds;
	ci::ColorA mColorMinMax, mColorAverage;
};

class SpectrumPlot {
  public:
	SpectrumPlot();
	
	void setBounds( const ci::Rectf &bounds )	{ mBounds = bounds; }
	const ci::Rectf& getBounds() const			{ return mBounds; }

	void enableScaleDecibels( bool b = true )	{ mScaleDecibels = b; }
	bool getScaleDecibels() const				{ return mScaleDecibels; }

	void enableBorder( bool b = true )			{ mBorderEnabled = b; }
	bool getBorderEnabled() const				{ return mBorderEnabled; }

	void setBorderColor( const ci::ColorA &color )	{ mBorderColor = color; }
	const ci::ColorA& getBorderColor() const		{ return mBorderColor; }

	void draw( const std::vector<float> &magSpectrum );

  private:
	ci::Rectf				mBounds;
	bool					mScaleDecibels, mBorderEnabled;
	ci::ColorA				mBorderColor;
};
