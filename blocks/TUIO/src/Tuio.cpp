//
//  Tuio.cpp
//  TUIOListener
//
//  Created by ryan bartley on 11/5/15.
//
//

#include "Tuio.h"

using namespace std;
using namespace ci;
using namespace ci::app;
using namespace asio;
using namespace asio::ip;
using Connection = ci::signals::Connection;

namespace cinder { namespace  tuio {
	
ProfileBase::ProfileBase()
: mSessionId( -1 ), mSource( "Unknown" )
{
}
	
ProfileBase::ProfileBase( const std::string &source, int32_t sessionId )
: mSessionId( sessionId ), mSource( source )
{
}
	
app::TouchEvent::Touch ProfileBase::getTouch( double time, const vec2 &posScale ) const
{
	return app::TouchEvent::Touch( getPos() * posScale,
								   getPrevPos() * posScale,
								   (uint32_t)getSessionId(),
								   time, NULL );
}
	
Cursor::Cursor( const std::string &source, int32_t sessionId, vec2 pos, vec2 speed, float motionAccel )
: ProfileBase( source, sessionId ), mPos( pos ), mSpeed( speed ), mPrevPos( pos )
{}

Cursor Cursor::createFromSetMessage( const osc::Message &message ) {
	// TODO: Figure this out.
	std::string source;// = message.getRemoteIp();
	if ( message.getAddress() != "/tuio/2Dcur" ) {
		// should really return a bad cursor or throw an exception
		return Cursor( source, -1, vec2(0.0,0.0), vec2(0.0,0.0), 0.0 );
	}
	auto sessionId = message[1].int32();
	vec2 pos( message[2].flt(), message[3].flt() );
	vec2 speed( message[4].flt(), message[5].flt() );
	auto motionAccel =  message[6].flt();
	return Cursor( source, sessionId, pos, speed, motionAccel );
}
	
Cursor25d::Cursor25d( const std::string &source, int32_t sessionId, vec3 pos, vec3 speed, float motionAccel )
: ProfileBase( source, sessionId ), mPos25( pos ), mSpeed25( speed ), mPrevPos25( pos )
{
}
	
Cursor25d Cursor25d::createFromSetMessage( const osc::Message &message ) {
	// TODO: Figure this out.
	std::string source; // = message.getRemoteIp();
	if ( message.getAddress() != "/tuio/25Dcur" ) {
		// should really return a bad cursor or throw an exception
		return Cursor25d( source, -1, vec3( 0 ), vec3( 0 ), 0.0 );
	}
	auto sessionId = message[1].int32();
	vec3 pos( message[2].flt(), message[3].flt(), message[4].flt() );
	vec3 speed( message[5].flt(), message[6].flt(), message[7].flt() );
	float motionAccel =  message[8].flt();
	return Cursor25d( source, sessionId, pos, speed, motionAccel );
}
	
Object::Object( std::string source, int32_t sessionId, int32_t fiducialId, vec2 pos, float angle, vec2 speed, float rotationSpeed, float motionAccel, float rotationAccel )
: ProfileBase(source,sessionId), mPos( pos ), mSpeed( speed ), mPrevPos( pos ),
	mFiducialId( fiducialId ), mAngle( angle ), mRotationSpeed( rotationSpeed ),
	mRotationAccel( rotationAccel )
{
}
	
Object Object::createFromSetMessage( const osc::Message &message )
{
	// TODO: figure out about getRemoteIp.
	std::string source; // = message.getRemoteIp();
	auto sessionId = message[1].int32();
	auto fiducialId = message[2].int32();
	auto pos = vec2( message[3].flt(), message[4].flt() );
	auto angle = message[5].flt();
	auto speed = vec2( message[6].flt(), message[7].flt() );
	auto rotationSpeed = message[8].flt();
	auto motionAccel = message[9].flt();
	auto rotationAccel = message[10].flt();
	
	return Object( source, sessionId, fiducialId, pos, angle, speed, rotationSpeed, motionAccel, rotationAccel );
}
	
template<typename T>
void Client::ProfileHandler<T>::handleMessage( const osc::Message &message )
{
	lock_guard<mutex> lock( mMutex );
	auto messageType = message[0].string();
	auto currentTime = app::getElapsedSeconds();
	std::string source;// = message.getRemoteIp();
	
	mSources.insert( source );
	
	if( messageType == "set" ) {
		T inst = T::createFromSetMessage( message );
		
		if( mInstances[source].find( inst.getSessionId() ) == mInstances[source].end() )
			mProfileAdds[source].push_back( inst );
		else
			mProfileUpdates[source].push_back( inst );
	}
	else if( messageType == "alive" ) {
		set<int32_t> aliveInstances;
		for( int i = 1; i < message.getNumArgs(); i++ )
			aliveInstances.insert( message[i].int32() );
		
		// anything not in 'aliveInstances' has been removed
		
		// We look at all (and only) the instances owned by the source of the message
		auto instanceMap = mInstances.find( source );
		if ( instanceMap != mInstances.end() ) {
			for( auto & inst : instanceMap->second ) {
				if( aliveInstances.find( inst.first ) == aliveInstances.end() )
					mProfileDeletes[source].push_back( inst.second );
			}
		}
	}
	else if( messageType == "fseq" ) {
		auto frame = message[1].int32();
		
		// due to UDP's unpredictability, it is possible to receive messages from "the past". Don't process these updates if that's true here
		// note that a frame of -1 implies that this is just an update, but doesn't represent a new time so we'll just process it
		
		// If the frame is "too far" in the past, we assume that the source has
		// been reset/restarted, or it's a different source, and we accept it.
		int32_t prev_frame = mPreviousFrame[source];
		int32_t dframe = frame - prev_frame;
		
		if( ( frame == -1 ) || ( dframe > 0 ) || ( dframe < -mPastFrameThreshold ) ) {
			auto windowSize = app::getWindowSize();
			auto window = app::WindowRef();
			// propagate the newly added instances
			vector<app::TouchEvent::Touch> beganTouches;
			for( auto & added : mProfileAdds[source] ) {
				mAddedSignal.emit( added );
				mInstances[source][added.getSessionId()] = added;
				beganTouches.push_back( added.getTouch( currentTime, windowSize ) );
			}
			
			// send a touchesBegan
			if( ! beganTouches.empty() )
				mTouchesBeganSignal.emit( app::TouchEvent( window, beganTouches ) );
			
			// propagate the updated instances
			vector<app::TouchEvent::Touch> movedTouches;
			for( auto & updated : mProfileUpdates[source] ) {
				mUpdatedSignal.emit( updated );
				mInstances[source][updated.getSessionId()] = updated;
				movedTouches.push_back( updated.getTouch( currentTime, windowSize ) );
			}
			
			// send a touchesMoved
			if( ! movedTouches.empty() )
				mTouchesMovedSignal.emit( app::TouchEvent( window, movedTouches ) );
			
			// propagate the deleted instances
			vector<app::TouchEvent::Touch> endedTouches;
			auto endIt = mProfileDeletes[source].end();
			for( auto removedIt = mProfileDeletes[source].begin(); removedIt != endIt; ++removedIt ) {
				mRemovedSignal.emit( *removedIt );
				auto removedId = removedIt->getSessionId();
				auto removedTouch = mInstances[source][removedId].getTouch( currentTime, windowSize );
				endedTouches.push_back( removedTouch );
				
				// call this last - we're using it in the callbacks
				mInstances[source].erase( removedId );
			}
			
			// send a touchesEnded
			if( ! endedTouches.empty() )
				mTouchesEndedSignal.emit( app::TouchEvent( app::WindowRef(), endedTouches ) );
			
			mPreviousFrame[source] = ( frame == -1 ) ? mPreviousFrame[source] : frame;
		}
		
		mProfileUpdates[source].clear();
		mProfileAdds[source].clear();
		mProfileDeletes[source].clear();
	}
}

template<typename T>
vector<T> Client::ProfileHandler<T>::getInstancesAsVector( const std::string &source ) const
{
	lock_guard<mutex> lock( mMutex );
	
	vector<T> result;
	
	if( source == "" ) {
		// Get instances across all sources
		for( auto & s : mSources ) {
			auto instanceMap = mInstances.find(s);
			if ( instanceMap != mInstances.end() ) {
				for ( auto & inst : instanceMap->second )
					result.push_back( inst.second );
			}
		}
	}
	else {
		// We collect only the instances owned by the specified source
		auto instanceMap = mInstances.find(source);
		if ( instanceMap != mInstances.end() ) {
			for ( auto & inst : instanceMap->second )
				result.push_back( inst.second );
		}
	}
	return result;
}

Client::Client( uint16_t port, asio::io_service &io )
: mListener( port, udp::v4(), io ),
mHandlerObject( new ProfileHandler<Object>( DEFAULT_PAST_FRAME_THRESHOLD ) ),
mHandlerCursor( new ProfileHandler<Cursor>( DEFAULT_PAST_FRAME_THRESHOLD ) ),
mHandlerCursor25d( new ProfileHandler<Cursor25d>( DEFAULT_PAST_FRAME_THRESHOLD ) )
{
}

void Client::connect()
{
	mListener.bind();
	mListener.setListener( "/tuio/2Dobj", std::bind( &ProfileHandler<Object>::handleMessage, mHandlerObject.get(), std::placeholders::_1 ) );
	mListener.setListener( "/tuio/2Dcur", std::bind( &ProfileHandler<Cursor>::handleMessage, mHandlerCursor.get(), std::placeholders::_1 ) );
	mListener.setListener( "/tuio/25Dcur", std::bind( &ProfileHandler<Cursor25d>::handleMessage, mHandlerCursor25d.get(), std::placeholders::_1 ) );
	mListener.listen();
	mConnected = true;
}

void Client::disconnect()
{
	lock_guard<mutex> lock( mMutex );
	mListener.close();
	mConnected = false;
}

Connection Client::registerCursorAdded( std::function<void (Cursor)> callback )
{
	std::lock_guard<std::mutex> lock(mMutex);
	return mHandlerCursor->mAddedSignal.connect( callback );
}

Connection Client::registerCursorUpdated( std::function<void (Cursor)> callback )
{
	std::lock_guard<std::mutex> lock(mMutex);
	return mHandlerCursor->mUpdatedSignal.connect( callback );
}

Connection Client::registerCursorRemoved( std::function<void (Cursor)> callback )
{
	std::lock_guard<std::mutex> lock(mMutex);
	return mHandlerCursor->mRemovedSignal.connect( callback );
}

Connection Client::registerObjectAdded( std::function<void (Object)> callback )
{
	std::lock_guard<std::mutex> lock(mMutex);
	return mHandlerObject->mAddedSignal.connect( callback );
}

Connection Client::registerObjectUpdated( std::function<void (Object)> callback )
{
	std::lock_guard<std::mutex> lock(mMutex);
	return mHandlerObject->mUpdatedSignal.connect( callback );
}

Connection Client::registerObjectRemoved( std::function<void (Object)> callback )
{
	std::lock_guard<std::mutex> lock(mMutex);
	return mHandlerObject->mRemovedSignal.connect( callback );
}

Connection Client::registerTouchesBegan( std::function<void (app::TouchEvent)> callback )
{
	std::lock_guard<std::mutex> lock(mMutex);
	return mHandlerCursor->mTouchesBeganSignal.connect( callback );
}

Connection Client::registerTouchesMoved( std::function<void (app::TouchEvent)> callback )
{
	std::lock_guard<std::mutex> lock(mMutex);
	return mHandlerCursor->mTouchesMovedSignal.connect( callback );
}

Connection Client::registerTouchesEnded( std::function<void (app::TouchEvent)> callback )
{
	std::lock_guard<std::mutex> lock(mMutex);
	return mHandlerCursor->mTouchesEndedSignal.connect( callback );
}

const std::set<std::string>& Client::getSources() const
{
	return mSources;
}

vector<Cursor> Client::getCursors( const std::string &source ) const
{
	return mHandlerCursor->getInstancesAsVector(source);
}

vector<Cursor25d> Client::getCursors25d( const std::string &source ) const
{
	return mHandlerCursor25d->getInstancesAsVector(source);
}

vector<Object> Client::getObjects( const std::string &source ) const
{
	return mHandlerObject->getInstancesAsVector(source);
}

vector<app::TouchEvent::Touch> Client::getActiveTouches( std::string source ) const
{
	lock_guard<mutex> lock( mMutex );
	
	double currentTime = app::getElapsedSeconds();
	vector<app::TouchEvent::Touch> result;
	if ( source == "" ) {
		// Get cursors from all sources
		auto & sources = getSources();
		int sourcenum = 0;
		for( auto & source :sources ) {
			vector<Cursor> cursors = mHandlerCursor->getInstancesAsVector( source );
			for( auto & inst : cursors ) {
				result.push_back( inst.getTouch( currentTime, app::getWindowSize() ) );
			}
			++sourcenum;
		}
	} else {
		// Get cursors from one source
		auto cursors = mHandlerCursor->getInstancesAsVector(source);
		for( auto & inst : cursors ) {
			result.push_back( inst.getTouch( currentTime, app::getWindowSize() ) );
		}
	}
	
	return result;	
}

}} // namespace tuio // namespace cinder