/*
 Copyright (c) 2010, The Cinder Project: http://libcinder.org
 All rights reserved.

 Portions Copyright (c) 2010, Hector Sanchez-Pajares
 Aer Studio http://www.aerstudio.com
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


#include "TuioClient.h"
#include "cinder/app/App.h"

#include <set>
#include <map>

using namespace std;

namespace cinder { namespace tuio {


// This class handles each of the profile types, currently Object: '2Dobj' and Cursor: '2Dcur'
template<typename T>
struct ProfileHandler {
	ProfileHandler() : mPreviousFrame( -1 ) {}

	void			handleMessage( const osc::Message &message );
	std::vector<T>	getInstancesAsVector() const;

	// current instances of this profile
	std::map<int32_t,T>		mInstances;

	// containers for changes which will be propagated upon receipt of 'fseq'
	std::vector<T>			mUpdates, mAdds;
	std::vector<int32_t>	mDeletes;

	// last frame we processed per the 'fseq' message
	int32_t		mPreviousFrame;

	CallbackMgr<void (T)>					mAddedCallbacks, mUpdatedCallbacks, mRemovedCallbacks;
	CallbackMgr<void (app::TouchEvent)>		mTouchesBeganCb, mTouchesMovedCb, mTouchesEndedCb;
	mutable std::mutex			mMutex;
};

template<typename T>
void ProfileHandler<T>::handleMessage( const osc::Message &message )
{
	lock_guard<mutex> lock( mMutex );
	const std::string messageType = message.getArgAsString( 0 );
	double currentTime = app::getElapsedSeconds();

	if( messageType == "set" ) {
		T inst = T::createFromSetMessage( message );

		if( mInstances.find( inst.getSessionId() ) == mInstances.end() )
			mAdds.push_back( inst );
		else
			mUpdates.push_back( inst );					
	}
	else if( messageType == "alive" ) {
		set<int32_t> aliveInstances;
		for( int i = 1; i < message.getNumArgs(); i++ )
			aliveInstances.insert( message.getArgAsInt32( i ) );

		// anything not in 'aliveInstances' has been removed
		for( typename map<int32_t,T>::iterator instIt = mInstances.begin(); instIt != mInstances.end(); ++instIt ) {
			if( aliveInstances.find( instIt->first ) == aliveInstances.end() )
				mDeletes.push_back( instIt->first );
		}
	}
	else if( messageType == "fseq" ) {
		int32_t frame = message.getArgAsInt32( 1 );

		// due to UDP's unpredictability, it is possible to receive messages from "the past". Don't process these updates if that's true here
		// note that a frame of -1 implies that this is just an update, but doesn't represent a new time so we'll just process it
		if( ( frame == -1 ) || ( frame > mPreviousFrame ) ) {
			// propagate the newly added instances
			vector<app::TouchEvent::Touch> beganTouches;
			for( typename vector<T>::const_iterator addIt = mAdds.begin(); addIt != mAdds.end(); ++addIt ) {
				mInstances[addIt->getSessionId()] = *addIt;
				beganTouches.push_back( addIt->getTouch( currentTime, app::getWindowSize() ) );
				mAddedCallbacks.call( *addIt );
			}
		
			// send a touchesBegan
			if( ! beganTouches.empty() )
				mTouchesBeganCb.call( app::TouchEvent( beganTouches ) );		

			// propagate the updated instances
			vector<app::TouchEvent::Touch> movedTouches;
			for( typename vector<T>::const_iterator updateIt = mUpdates.begin(); updateIt != mUpdates.end(); ++updateIt ) {
				mInstances[updateIt->getSessionId()] = *updateIt;
				movedTouches.push_back( updateIt->getTouch( currentTime, app::getWindowSize() ) );
				mUpdatedCallbacks.call( *updateIt );
			}

			// send a touchesMoved
			if( ! movedTouches.empty() )
				mTouchesMovedCb.call( app::TouchEvent( movedTouches ) );

			// propagate the deleted instances
			vector<app::TouchEvent::Touch> endedTouches;
			for( vector<int32_t>::const_iterator deleteIt = mDeletes.begin(); deleteIt != mDeletes.end(); ++deleteIt ) {
				mRemovedCallbacks.call( mInstances[*deleteIt] );

				endedTouches.push_back( mInstances[*deleteIt].getTouch( currentTime, app::getWindowSize() ) );

				// call this last - we're using it in the callbacks
				mInstances.erase( *deleteIt );
			}

			// send a touchesEnded
			if( ! endedTouches.empty() )
				mTouchesEndedCb.call( app::TouchEvent( endedTouches ) );

			mPreviousFrame = ( frame == -1 ) ? mPreviousFrame : frame;
		}

		mUpdates.clear();
		mAdds.clear();
		mDeletes.clear();
	}
}
	
template<typename T>
vector<T> ProfileHandler<T>::getInstancesAsVector() const
{
	lock_guard<mutex> lock( mMutex );
	
	vector<T> result;
	for( typename map<int32_t,T>::const_iterator instIt = mInstances.begin(); instIt != mInstances.end(); ++instIt )
		result.push_back( instIt->second );

	return result;
}

Client::Client()
	: mHandlerObject( new ProfileHandler<Object>() ), mHandlerCursor( new ProfileHandler<Cursor>() )
{
}

void Client::connect( int port )
{
	mListener.setup( port );
	mListener.registerMessageReceived( this, &Client::oscMessageReceived );
	mConnected = true;
}
	
void Client::disconnect() {
	lock_guard<mutex> lock( mMutex );

	mListener.shutdown();
	mConnected = false;
}

CallbackId	Client::registerCursorAdded( std::function<void (Cursor)> callback ) { std::lock_guard<std::mutex> lock(mMutex); return mHandlerCursor->mAddedCallbacks.registerCb( callback ); }
void		Client::unregisterCursorAdded( CallbackId id ) { std::lock_guard<std::mutex> lock(mMutex); return mHandlerCursor->mAddedCallbacks.unregisterCb( id ); }

CallbackId	Client::registerCursorUpdated( std::function<void (Cursor)> callback ) { std::lock_guard<std::mutex> lock(mMutex); return mHandlerCursor->mUpdatedCallbacks.registerCb( callback ); }
void		Client::unregisterCursorUpdated( CallbackId id ) { std::lock_guard<std::mutex> lock(mMutex); return mHandlerCursor->mUpdatedCallbacks.unregisterCb( id ); }

CallbackId	Client::registerCursorRemoved( std::function<void (Cursor)> callback ) { std::lock_guard<std::mutex> lock(mMutex); return mHandlerCursor->mRemovedCallbacks.registerCb( callback ); }
void		Client::unregisterCursorRemoved( CallbackId id ) { std::lock_guard<std::mutex> lock(mMutex); return mHandlerCursor->mRemovedCallbacks.unregisterCb( id ); }

CallbackId	Client::registerObjectAdded( std::function<void (Object)> callback ) { std::lock_guard<std::mutex> lock(mMutex); return mHandlerObject->mAddedCallbacks.registerCb( callback ); }
void		Client::unregisterObjectAdded( CallbackId id ) { std::lock_guard<std::mutex> lock(mMutex); return mHandlerObject->mAddedCallbacks.unregisterCb( id ); }

CallbackId	Client::registerObjectUpdated( std::function<void (Object)> callback ) { std::lock_guard<std::mutex> lock(mMutex); return mHandlerObject->mUpdatedCallbacks.registerCb( callback ); }
void		Client::unregisterObjectUpdated( CallbackId id ) { std::lock_guard<std::mutex> lock(mMutex); return mHandlerObject->mUpdatedCallbacks.unregisterCb( id ); }

CallbackId	Client::registerObjectRemoved( std::function<void (Object)> callback ) { std::lock_guard<std::mutex> lock(mMutex); return mHandlerObject->mRemovedCallbacks.registerCb( callback ); }
void		Client::unregisterObjectRemoved( CallbackId id ) { std::lock_guard<std::mutex> lock(mMutex); return mHandlerObject->mRemovedCallbacks.unregisterCb( id ); }

CallbackId	Client::registerOscMessageReceived( std::function<void (const osc::Message*)> callback ) { std::lock_guard<std::mutex> lock(mMutex); return mOscMessageCallbacks.registerCb( callback ); }
void		Client::unregisterOscMessageReceived( CallbackId id ) { std::lock_guard<std::mutex> lock(mMutex); return mOscMessageCallbacks.unregisterCb( id ); }

CallbackId	Client::registerTouchesBegan( std::function<void (app::TouchEvent)> callback ) { return mHandlerCursor->mTouchesBeganCb.registerCb( callback ); }
void		Client::unregisterTouchesBegan( CallbackId id ) { mHandlerCursor->mTouchesBeganCb.unregisterCb( id ); }

CallbackId	Client::registerTouchesMoved( std::function<void (app::TouchEvent)> callback ) { return mHandlerCursor->mTouchesMovedCb.registerCb( callback ); }
void		Client::unregisterTouchesMoved( CallbackId id ) { mHandlerCursor->mTouchesMovedCb.unregisterCb( id ); }

CallbackId	Client::registerTouchesEnded( std::function<void (app::TouchEvent)> callback ) { return mHandlerCursor->mTouchesEndedCb.registerCb( callback ); }
void		Client::unregisterTouchesEnded( CallbackId id ) { mHandlerCursor->mTouchesEndedCb.unregisterCb( id ); }


vector<Cursor> Client::getCursors() const
{
	return mHandlerCursor->getInstancesAsVector();
}

vector<Object> Client::getObjects() const
{
	return mHandlerObject->getInstancesAsVector();
}

vector<app::TouchEvent::Touch> Client::getActiveTouches() const
{
	lock_guard<mutex> lock( mMutex );
	
	double currentTime = app::getElapsedSeconds();
	vector<app::TouchEvent::Touch> result;
	vector<Cursor> cursors = mHandlerCursor->getInstancesAsVector();
	for( vector<Cursor>::const_iterator instIt = cursors.begin(); instIt != cursors.end(); ++instIt )
		result.push_back( instIt->getTouch( currentTime, app::getWindowSize() ) );

	return result;	
}

void Client::oscMessageReceived( const osc::Message *message )
{
	if( message->getAddress() == "/tuio/2Dobj" )
		mHandlerObject->handleMessage( *message );
	else if( message->getAddress() == "/tuio/2Dcur" )
		mHandlerCursor->handleMessage( *message );
	else { // send the raw OSC message since it's one we don't know about
		for( CallbackMgr<void (const osc::Message*)>::iterator cbIt = mOscMessageCallbacks.begin(); cbIt != mOscMessageCallbacks.end(); ++cbIt )
			cbIt->second( message );
	}
}

} } // namespace cinder::tuio