/*
 Copyright (c) 2009, The Barbarian Group
 All rights reserved.

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
#include "cinder/audio/Io.h"
#include "cinder/Exception.h"

namespace cinder { namespace audio {

typedef int32_t					TrackId;
typedef shared_ptr<class Track> TrackRef;

class OutputImpl;

//TODO: add a track 'controller' that talks to tracks through their Output owner
/*class TrackController {
  public:
	~TrackController() {}
  private:
	TrackController( OutputImpl * output, TrackId track ) : mOutput( output ), mTrackId( trackId ) {}
	
	TrackId		mTrackId;
	OutputImpl	* mOutput;
}*/

class Track {
  public:
	virtual ~Track() {}
	virtual void play() = 0;
	virtual void stop() = 0;
	virtual bool isPlaying() const = 0;
	
	virtual void setVolume( float aVolume ) = 0;
	virtual float getVolume() const = 0;
	
	virtual double getTime() const = 0;
	virtual void setTime( double aTime ) = 0;
	
	virtual void setLooping( bool isLooping ) = 0;
	virtual bool isLooping() const = 0;
	
	virtual void enablePcmBuffering( bool isBuffering ) = 0;
	virtual bool isPcmBuffering() = 0;
	virtual PcmBuffer32fRef getPcmBuffer() = 0;
  protected:
	Track() {}
};

class OutputImpl {
  public:
	virtual ~OutputImpl() {}
	
	virtual void play( SourceRef aSource ) { addTrack( aSource, true ); }
	virtual TrackRef addTrack( SourceRef aSource, bool autoplay ) = 0;
	virtual	void removeTrack( TrackId trackId ) = 0;
	
	virtual float getVolume() const = 0;
	virtual void setVolume( float aVolume ) = 0;
	
	//virtual TargetRef getTarget() = 0;
  protected:
	OutputImpl();
	virtual TrackId					availableTrackId() { return mNextTrackId++; }
	
	/*void retainTrack( TrackId trackId ) {}
	void releaseTrack( TrackId trackId ) {}
	void playTrack( TrackId trackId ) {}
	void stopTrack( TrackId trackId ) {}
	void setTrackVolume( TrackId trackId, float aVolume ) {}
	float getTrackVolume( TrackId trackId ) {}
	void setTrackTime( TrackId trackId, double aTime ) {}
	float getTrackTime( TrackId trackId ) {}*/
	

	TrackId							mNextTrackId;

	friend class Track;
	//friend class TrackController;
};

class Output {
  public:
	static void play( SourceRef aSource ) { instance()->play( aSource ); }
	
	static TrackRef addTrack( SourceRef aSource, bool autoplay = true ) { return instance()->addTrack( aSource, autoplay ); }
	static void	removeTrack( TrackId trackId ) { instance()->removeTrack( trackId ); }
	
	static float getVolume() { return instance()->getVolume(); }
	static void setVolume( float aVolume ) { instance()->setVolume( aVolume ); }
	
	//static TargetRef getTarget() { return instance()->getTarget(); }
  private:
	static OutputImpl* instance();
};

class OutputException : public Exception {};
class OutOfTracksException : public Exception {};



}} //namespace