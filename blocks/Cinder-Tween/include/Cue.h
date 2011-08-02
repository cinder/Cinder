/*
 *  Cue.h
 *  BasicTween
 *
 *  Created by David Wicks on 2/24/11.
 *  Copyright 2011 David Wicks. All rights reserved.
 *
 */

#pragma once

#include "Sequenceable.hpp"
#include <boost/signals2.hpp>

namespace cinder
{

	namespace tween
	{
		class Cue : public Sequenceable
		{
		public:
			Cue( boost::function<void ()> action, double atTime=0.0 );
			~Cue();
			void addReceiver( boost::function<void ()> action );
			void stepTo( double time );
			
		private:
			boost::signals2::signal<void ()> mSignal;
			double mTime;
			double mLastTime;
		};
		
		typedef boost::shared_ptr<Cue> CueRef;
	}
	
	
}