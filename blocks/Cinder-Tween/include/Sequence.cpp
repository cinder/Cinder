/*
 *  Sequence.cpp
 *  BasicTween
 *
 *  Created by David Wicks on 5/27/10.
 *  Copyright 2010 David Wicks. All rights reserved.
 *
 */

#include "Sequence.h"
#include "cinder/app/App.h"

using namespace cinder;
using namespace cinder::tween;
typedef std::vector< SeqRef >::iterator s_iter;

Sequence::Sequence()
{
	mCurrentTime = 0;
}

void Sequence::step()
{	// would like to use getAverageFps, but it doesn't work statically (yet)
	step( 1.0 / app::getFrameRate() );
}

void Sequence::step( double timestep )
{
	mCurrentTime += timestep;
	
	for( s_iter iter = mActions.begin(); iter != mActions.end(); ++iter )
	{
		(**iter).stepTo( mCurrentTime );
	}
}

void Sequence::stepTo( double time )
{	
	mCurrentTime = time;
	
	for( s_iter iter = mActions.begin(); iter != mActions.end(); ++iter )
	{
		(**iter).stepTo( time );
	}
}

void Sequence::clearSequence()
{
	mActions.clear();	
}

void Sequence::clearFinishedTweens()
{
	s_iter iter = mActions.begin();
	
	while (iter != mActions.end()) {		
		if( (**iter).isComplete() )
		{
			iter = mActions.erase(iter);
		} else {
			++iter;
		}
	}
}

void Sequence::add( SeqRef action )
{
	mActions.push_back( action );
}

SeqRef Sequence::find( void *target )
{
	s_iter iter = mActions.begin();
	while( iter != mActions.end() ) {
		if( (*iter)->getTargetVoid() == target )
			return *iter;
		++iter;
	}
	
	return SeqRef(); // failed returns null tween
}

void Sequence::remove( SeqRef action )
{
	s_iter iter = std::find( mActions.begin(), mActions.end(), action );
	if( iter != mActions.end() )
		mActions.erase( iter );
}


