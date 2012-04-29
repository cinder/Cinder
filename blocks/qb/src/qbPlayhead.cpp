//
//  qbPlayhead.h
//
//  Created by Roger Sodre on 01/07/2011
//  Copyright 2011 Studio Avante. All rights reserved.
//
#include "qbPlayhead.h"
#include "qb.h"

namespace cinder { namespace qb {
	
	
	//////////////////////////////////////////////////////////////////
	//
	// QB MANAGER WRAPPERS
	//
	// Play / Pause
	void qbMain::playSwitch()
	{
		this->play( ! mPlayhead.isPlaying() );
	}
	void qbMain::stop()
	{
		this->play(false);
	}
	void qbMain::play( bool p )
	{
		if (p)
			mPlayhead.resume();
		else
			mPlayhead.stop();
	}
	void qbMain::rewind()
	{
		mPlayhead.rewind();
		// Rewind Sources
		std::map<int,qbSourceSelector>::const_iterator it;
		for ( it = mSources.begin() ; it != mSources.end(); it++ )
		{
			int i = it->first;
			if (this->source(i))
				this->source(i).rewind();
		}
		// TODO:: Rewind AUDIO
	}
	
	
	
	
	//////////////////////////////////////////////////////////////////
	//
	// PLAYHEAD
	//
	qbPlayhead::qbPlayhead()
	{
		bIsPlaying = false;
		bShouldRewind = false;
		mCurrentFrame = 0;
		mSeconds = 0.0;
	}
	
	//
	// Rewind + Start
	void qbPlayhead::start()
	{
		bIsPlaying = true;
		mLastTime = CFAbsoluteTimeGetCurrent();
		this->rewind();
	}
	
	//
	// Rewind
	void qbPlayhead::rewind()
	{
		bShouldRewind = true;
		//this->update();
	}
	
	//
	// Play = Rewind
	void qbPlayhead::stop()
	{
		if ( bIsPlaying )
			bIsPlaying = false;
	}
	
	//
	// Play = Rewind
	void qbPlayhead::resume()
	{
		if ( ! bIsPlaying )
		{
			bIsPlaying = true;
			mLastTime = CFAbsoluteTimeGetCurrent();
		}
	}
	
	//
	// Update
	void qbPlayhead::update()
	{
		if ( bIsPlaying )
		{
			int dir = ( _qb.isPlayingBackwards() ? -1.0 : 1.0 );
			double now = CFAbsoluteTimeGetCurrent();
			// Realtime = Play continuously, may lose frames
			if ( bShouldRewind )
			{
				mSeconds = 0.0;
				mCurrentFrame = 0;
				bShouldRewind = false;
			}
			else if ( _qb.isPreviewRealtime() )
			{
				mSeconds += (now - mLastTime) * dir;
				mCurrentFrame = (int) (mSeconds * _qb.getFrameRate());
			}
			// Not Realtime = Play frame by frame
			else
			{
				mSeconds += QB_FRAME_DURATION * dir;
				mCurrentFrame++;
			}
			// clamp animation
			if (_qb.getRenderSeconds() > 0 && mSeconds > _qb.getRenderSeconds())
				_qb.rewind();
			mLastTime = now;
			//printf("PLAYHEAD real[%d] secs[%.2f] fr[%d]\n",_qb.isPreviewRealtime(),mSeconds,mCurrentFrame);
		}
		// Update GUI
		_cfg.set(QBCFG_PLAYING, bIsPlaying);
		_cfg.set(QBCFG_CURRENT_TIME, mSeconds);
		_cfg.set(DUMMY_CURRENT_FRAME, (int)mCurrentFrame);
	}
	void qbPlayhead::seekToTime( double _s )
	{
		mSeconds = _s;
		mCurrentFrame = (int) (mSeconds * _qb.getFrameRate());
		mLastTime = CFAbsoluteTimeGetCurrent();
	}

		
	
} } // cinder::qb







