//
//  qbPlayhead.h
//
//  Created by Roger Sodre on 01/07/2011
//  Copyright 2011 Studio Avante. All rights reserved.
//
#pragma once

#include "cinder/Cinder.h"
#include "cinder/Timer.h"

namespace cinder { namespace qb {
	
	class qbPlayhead {
	public:
		qbPlayhead();
		
		void			update();
		void			seekToTime( double _s );

		void			start();
		void			rewind();
		void			stop();
		void			resume();
		
		bool			isPlaying()			{ return bIsPlaying; }
		double			getSeconds()		{ return mSeconds; }
		int				getCurrentFrame()	{ return mCurrentFrame; }
		
	private:
		
		bool			bIsPlaying;
		bool			bShouldRewind;
		double			mLastTime;
		double			mSeconds;
		int				mCurrentFrame;
		
	};
	
	
} } // cinder::qb


