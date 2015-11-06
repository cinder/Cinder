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
using Connection = boost::signals2::connection;

namespace cinder { namespace  tuio {
	
namespace detail {
	
Profile::Profile( const osc::Message &msg )
: mSessionId( msg[0].int32() )
{
}

template<>
Cursor<ci::vec2>::Cursor( const osc::Message &msg )
: Profile( msg ), mPosition( msg[1].flt(), msg[2].flt() ),
	mVelocity( msg[3].flt(), msg[4].flt() ), mAcceleration( msg[5].flt() )
{
}
	
template<>
Cursor<ci::vec3>::Cursor( const osc::Message &msg )
: Profile( msg ), mPosition( msg[1].flt(), msg[2].flt(), msg[3].flt() ),
mVelocity( msg[4].flt(), msg[5].flt(), msg[6].flt() ), mAcceleration( msg[7].flt() )
{
}
	
template<>
Object<ci::vec2, float>::Object( const osc::Message &msg )
: Profile( msg ), mClassId( msg[1].int32() ), mPosition( msg[2].flt(), msg[3].flt() ),
	mAngle( msg[4].flt() ), mVelocity( msg[5].flt(), msg[6].flt() ),
	mRotationVelocity( msg[7].flt() ), mAcceleration( msg[8].flt() ),
	mRotateAccel( msg[9].flt() )
{
}
	
template<>
Object<ci::vec3, float>::Object( const osc::Message &msg )
: Profile( msg ), mClassId( msg[1].int32() ),
	mPosition( msg[2].flt(), msg[3].flt(), msg[4].flt() ),
	mAngle( msg[5].flt() ), mVelocity( msg[6].flt(), msg[7].flt(), msg[8].flt() ),
	mRotationVelocity( msg[9].flt() ), mAcceleration( msg[10].flt() ),
	mRotateAccel( msg[11].flt() )
{
}
	
template<>
Object<ci::vec3, ci::vec3>::Object( const osc::Message &msg )
: Profile( msg ), mClassId( msg[1].int32() ),
	mPosition(	msg[2].flt(), msg[3].flt(), msg[4].flt() ),
	mAngle(		msg[5].flt(), msg[6].flt(), msg[7].flt() ),
	mVelocity(	msg[8].flt(), msg[9].flt(), msg[10].flt() ),
	mRotationVelocity( msg[11].flt(), msg[12].flt(), msg[13].flt() ),
	mAcceleration( msg[14].flt() ), mRotateAccel( msg[15].flt() )
{
}
	
template<>
Blob<ci::vec2, float, ci::vec2>::Blob( const osc::Message &msg )
: Profile( msg ), mPosition( msg[1].flt(), msg[2].flt() ), mAngle( msg[3].flt() ),
	mDimensions( msg[4].flt(), msg[5].flt() ), mGeometry( msg[6].flt() ),
	mVelocity( msg[7].flt(), msg[8].flt() ), mRotationVelocity( msg[9].flt() ),
	mAcceleration( msg[10].flt() ), mRotateAccel( msg[11].flt() )
{
}
	
template<>
Blob<ci::vec3, float, ci::vec2>::Blob( const osc::Message &msg )
: Profile( msg ), mPosition( msg[1].flt(), msg[2].flt(), msg[3].flt() ),
	mAngle( msg[4].flt() ), mDimensions( msg[5].flt(), msg[6].flt() ),
	mGeometry( msg[7].flt() ), mVelocity( msg[8].flt(), msg[9].flt(), msg[10].flt() ),
	mRotationVelocity( msg[11].flt() ), mAcceleration( msg[12].flt() ),
	mRotateAccel( msg[13].flt() )
{
}
	
template<>
Blob<ci::vec3, ci::vec3, ci::vec3>::Blob( const osc::Message &msg )
: Profile( msg ), mPosition( msg[1].flt(), msg[2].flt(), msg[3].flt() ),
	mAngle( msg[4].flt(), msg[5].flt(), msg[6].flt() ),
	mDimensions( msg[7].flt(), msg[8].flt(), msg[9].flt() ),
	mGeometry( msg[10].flt() ), mVelocity( msg[11].flt(), msg[12].flt(), msg[13].flt() ),
	mRotationVelocity( msg[14].flt(), msg[15].flt(), msg[16].flt() ),
	mAcceleration( msg[12].flt() ), mRotateAccel( msg[13].flt() )
{
}
	
} // namespace detail
	
Blob2D::Blob2D( const osc::Message &msg )
: Blob( msg )
{
}

Blob25D::Blob25D( const osc::Message &msg )
: Blob( msg )
{
}

Blob3D::Blob3D( const osc::Message &msg )
: Blob( msg )
{
}
	
	
//app::TouchEvent::Touch ProfileBase::getTouch( double time, const vec2 &posScale ) const
//{
//	return app::TouchEvent::Touch( getPos() * posScale,
//								   getPrevPos() * posScale,
//								   (uint32_t)getSessionId(),
//								   time, NULL );
//}
	
//template<typename T>
//void Client::ProfileHandler<T>::handleMessage( const osc::Message &message )
//{
//	auto messageType = message[0].string();
//	auto currentTime = app::getElapsedSeconds();
//	std::string source;// = message.getRemoteIp();
//	
//	mSources.insert( source );
//	
//	if( messageType == "source" ) {
//		
//	}
//	if( messageType == "set" ) {
//		T inst = T::createFromSetMessage( message );
//		
//		if( mInstances[source].find( inst.getSessionId() ) == mInstances[source].end() )
//			mProfileAdds[source].push_back( inst );
//		else
//			mProfileUpdates[source].push_back( inst );
//	}
//	else if( messageType == "alive" ) {
//		set<int32_t> aliveInstances;
//		for( int i = 1; i < message.getNumArgs(); i++ )
//			aliveInstances.insert( message[i].int32() );
//		
//		// anything not in 'aliveInstances' has been removed
//		
//		// We look at all (and only) the instances owned by the source of the message
//		auto instanceMap = mInstances.find( source );
//		if ( instanceMap != mInstances.end() ) {
//			for( auto & inst : instanceMap->second ) {
//				if( aliveInstances.find( inst.first ) == aliveInstances.end() )
//					mProfileDeletes[source].push_back( inst.second );
//			}
//		}
//	}
//	else if( messageType == "fseq" ) {
//		auto frame = message[1].int32();
//		
//		// due to UDP's unpredictability, it is possible to receive messages from "the past". Don't process these updates if that's true here
//		// note that a frame of -1 implies that this is just an update, but doesn't represent a new time so we'll just process it
//		
//		// If the frame is "too far" in the past, we assume that the source has
//		// been reset/restarted, or it's a different source, and we accept it.
//		int32_t prev_frame = mPreviousFrame[source];
//		int32_t dframe = frame - prev_frame;
//		
//		if( ( frame == -1 ) || ( dframe > 0 ) || ( dframe < -mPastFrameThreshold ) ) {
//			auto windowSize = app::getWindowSize();
//			auto window = app::WindowRef();
//			// propagate the newly added instances
//			vector<app::TouchEvent::Touch> beganTouches;
//			for( auto & added : mProfileAdds[source] ) {
//				mAddedSignal( added );
//				mInstances[source][added.getSessionId()] = added;
//				beganTouches.push_back( added.getTouch( currentTime, windowSize ) );
//			}
//			
//			// send a touchesBegan
//			if( ! beganTouches.empty() )
//				mTouchesBeganSignal( app::TouchEvent( window, beganTouches ) );
//			
//			// propagate the updated instances
//			vector<app::TouchEvent::Touch> movedTouches;
//			for( auto & updated : mProfileUpdates[source] ) {
//				mUpdatedSignal( updated );
//				mInstances[source][updated.getSessionId()] = updated;
//				movedTouches.push_back( updated.getTouch( currentTime, windowSize ) );
//			}
//			
//			// send a touchesMoved
//			if( ! movedTouches.empty() )
//				mTouchesMovedSignal( app::TouchEvent( window, movedTouches ) );
//			
//			// propagate the deleted instances
//			vector<app::TouchEvent::Touch> endedTouches;
//			auto endIt = mProfileDeletes[source].end();
//			for( auto removedIt = mProfileDeletes[source].begin(); removedIt != endIt; ++removedIt ) {
//				mRemovedSignal( *removedIt );
//				auto removedId = removedIt->getSessionId();
//				auto removedTouch = mInstances[source][removedId].getTouch( currentTime, windowSize );
//				endedTouches.push_back( removedTouch );
//				
//				// call this last - we're using it in the callbacks
//				mInstances[source].erase( removedId );
//			}
//			
//			// send a touchesEnded
//			if( ! endedTouches.empty() )
//				mTouchesEndedSignal( app::TouchEvent( app::WindowRef(), endedTouches ) );
//			
//			mPreviousFrame[source] = ( frame == -1 ) ? mPreviousFrame[source] : frame;
//		}
//		
//		mProfileUpdates[source].clear();
//		mProfileAdds[source].clear();
//		mProfileDeletes[source].clear();
//	}
//}
//
//template<typename T>
//vector<T> Client::ProfileHandler<T>::getInstancesAsVector( const std::string &source ) const
//{
//	vector<T> result;
//	
//	if( source == "" ) {
//		// Get instances across all sources
//		for( auto & s : mSources ) {
//			auto instanceMap = mInstances.find(s);
//			if ( instanceMap != mInstances.end() ) {
//				for ( auto & inst : instanceMap->second )
//					result.push_back( inst.second );
//			}
//		}
//	}
//	else {
//		// We collect only the instances owned by the specified source
//		auto instanceMap = mInstances.find(source);
//		if ( instanceMap != mInstances.end() ) {
//			for ( auto & inst : instanceMap->second )
//				result.push_back( inst.second );
//		}
//	}
//	return result;
//}
//
//Client::Client( uint16_t port, asio::io_service &io )
//: mListener( port, udp::v4(), io ),
//mHandlerObject( new ProfileHandler<Object>( DEFAULT_PAST_FRAME_THRESHOLD ) ),
//mHandlerCursor( new ProfileHandler<Cursor>( DEFAULT_PAST_FRAME_THRESHOLD ) ),
//mHandlerCursor25d( new ProfileHandler<Cursor25d>( DEFAULT_PAST_FRAME_THRESHOLD ) )
//{
//}
//
//void Client::connect()
//{
//	mListener.bind();
//	mListener.setListener( "/tuio/2Dobj", std::bind( &ProfileHandler<Object>::handleMessage, mHandlerObject.get(), std::placeholders::_1 ) );
//	mListener.setListener( "/tuio/2Dcur", std::bind( &ProfileHandler<Cursor>::handleMessage, mHandlerCursor.get(), std::placeholders::_1 ) );
//	mListener.setListener( "/tuio/25Dcur", std::bind( &ProfileHandler<Cursor25d>::handleMessage, mHandlerCursor25d.get(), std::placeholders::_1 ) );
//	mListener.listen();
//	mConnected = true;
//}
//
//void Client::disconnect()
//{
//	lock_guard<mutex> lock( mMutex );
//	mListener.close();
//	mConnected = false;
//}
//
//Connection Client::registerCursorAdded( CursorFn callback )
//{
//	return mHandlerCursor->mAddedSignal.connect( callback );
//}
//
//Connection Client::registerCursorUpdated( CursorFn callback )
//{
//	return mHandlerCursor->mUpdatedSignal.connect( callback );
//}
//
//Connection Client::registerCursorRemoved( CursorFn callback )
//{
//	return mHandlerCursor->mRemovedSignal.connect( callback );
//}
//
//Connection Client::registerObjectAdded( ObjectFn callback )
//{
//	return mHandlerObject->mAddedSignal.connect( callback );
//}
//
//Connection Client::registerObjectUpdated( ObjectFn callback )
//{
//	return mHandlerObject->mUpdatedSignal.connect( callback );
//}
//
//Connection Client::registerObjectRemoved( ObjectFn callback )
//{
//	return mHandlerObject->mRemovedSignal.connect( callback );
//}
//
//Connection Client::registerTouchesBegan( TouchesFn callback )
//{
//	return mHandlerCursor->mTouchesBeganSignal.connect( callback );
//}
//
//Connection Client::registerTouchesMoved( TouchesFn callback )
//{
//	return mHandlerCursor->mTouchesMovedSignal.connect( callback );
//}
//
//Connection Client::registerTouchesEnded( TouchesFn callback )
//{
//	return mHandlerCursor->mTouchesEndedSignal.connect( callback );
//}
//
//const std::set<std::string>& Client::getSources() const
//{
//	return mSources;
//}
//
//vector<Cursor> Client::getCursors( const std::string &source ) const
//{
//	return mHandlerCursor->getInstancesAsVector(source);
//}
//
//vector<Cursor25d> Client::getCursors25d( const std::string &source ) const
//{
//	return mHandlerCursor25d->getInstancesAsVector(source);
//}
//
//vector<Object> Client::getObjects( const std::string &source ) const
//{
//	return mHandlerObject->getInstancesAsVector(source);
//}
//
//vector<app::TouchEvent::Touch> Client::getActiveTouches( std::string source ) const
//{
//	lock_guard<mutex> lock( mMutex );
//	
//	double currentTime = app::getElapsedSeconds();
//	vector<app::TouchEvent::Touch> result;
//	if ( source == "" ) {
//		// Get cursors from all sources
//		auto & sources = getSources();
//		int sourcenum = 0;
//		for( auto & source :sources ) {
//			vector<Cursor> cursors = mHandlerCursor->getInstancesAsVector( source );
//			for( auto & inst : cursors ) {
//				result.push_back( inst.getTouch( currentTime, app::getWindowSize() ) );
//			}
//			++sourcenum;
//		}
//	} else {
//		// Get cursors from one source
//		auto cursors = mHandlerCursor->getInstancesAsVector(source);
//		for( auto & inst : cursors ) {
//			result.push_back( inst.getTouch( currentTime, app::getWindowSize() ) );
//		}
//	}
//	
//	return result;	
//}
//
}} // namespace tuio // namespace cinder