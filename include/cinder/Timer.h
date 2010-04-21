/*
 Copyright (c) 2010, The Barbarian Group
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

#if defined( CINDER_COCOA )
	#include <CoreFoundation/CoreFoundation.h>
#endif

namespace cinder {

/** \brief A high-resolution timer class **/
class Timer {
  public:
	//! Constructs a default timer which is initialized as stopped
	Timer();
	//! Constructs a default timer which is initialized as running unless \a startOnConstruction is false
	Timer( bool startOnConstruction );
	
	//! Begins timing
	void	start();
	//! Returns the elapsed seconds if the timer is running, or the total time between calls to start() and stop() if it is stopped.
	double	getSeconds() const;
	//! Ends timing
	void	stop();

	//! Returns whether the timer is currently running
	bool	isStopped() const { return mIsStopped; }
	
  private:
	bool	mIsStopped;
#if defined( CINDER_COCOA )
	::CFAbsoluteTime	mStartTime, mEndTime;
#elif defined( CINDER_MSW )
	double				mStartTime, mEndTime, mInvNativeFreq;
#endif
};

} // namespace cinder