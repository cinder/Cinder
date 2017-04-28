//
//  Tuio.cpp
//  TUIOReceiver
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

namespace cinder { namespace  tuio {

const uint16_t Receiver::DEFAULT_TUIO_PORT;
// default threshold for a frame ID being old enough to imply a new source
const uint32_t Receiver::DEFAULT_PAST_FRAME_THRESHOLD;

	
namespace detail {
	
/////////////////////////////////////////////////////////////////////////////////////////////////
///// Type
	
bool Type::operator<(const Type & other) const
{
	return mSource < other.mSource &&
		mSessionId < other.mSessionId;
}

Type::Type( const osc::Message &msg )
: mSessionId( msg[1].int32() )
{
}

Type::Type( Type &&other ) NOEXCEPT
: mSessionId( other.mSessionId ), mSource( std::move( other.mSource ) )
{
}

Type& Type::operator=( Type &&other ) NOEXCEPT
{
	if ( this != &other ) {
		mSessionId = other.mSessionId;
		mSource = std::move( other.mSource );
	}
	return *this;
}
	
/////////////////////////////////////////////////////////////////////////////////////////////////
///// Cursor
	
template class Cursor<vec2>;
template class Cursor<vec3>;

template<>
Cursor<vec2>::Cursor( const osc::Message &msg )
: Type( msg ), mPosition( msg[2].flt(), msg[3].flt() ),
	mVelocity( msg[4].flt(), msg[5].flt() ), mAcceleration( msg[6].flt() )
{
}
	
template<>
Cursor<vec3>::Cursor( const osc::Message &msg )
: Type( msg ), mPosition( msg[2].flt(), msg[3].flt(), msg[4].flt() ),
mVelocity( msg[5].flt(), msg[6].flt(), msg[7].flt() ), mAcceleration( msg[8].flt() )
{
}

template<typename VEC_T>
Cursor<VEC_T>::Cursor( Cursor<VEC_T> &&other ) NOEXCEPT
: Type( std::move( other ) ), mPosition( std::move( other.mPosition ) ),
	mVelocity( std::move( other.mVelocity ) ), mAcceleration( other.mAcceleration )
{
}

template<typename VEC_T>
Cursor<VEC_T>& Cursor<VEC_T>::operator=( Cursor<VEC_T> &&other ) NOEXCEPT
{
	if( this != &other ) {
		Type::operator=( std::move( other ) );
		mPosition = std::move( other.mPosition );
		mVelocity = std::move( other.mVelocity );
		mAcceleration = other.mAcceleration;
	}
	return *this;
}
	
template<typename T>
app::TouchEvent::Touch Cursor<T>::convertToTouch( const ci::app::WindowRef &window ) const
{
	auto windowSize = vec2(window->getSize());
	app::TouchEvent::Touch ret( vec2(getPosition()) * windowSize,
								vec2(getPosition() - getVelocity()) * windowSize,
								getSessionId(),
								app::getElapsedSeconds(),
								nullptr );
	return ret;
}
	
/////////////////////////////////////////////////////////////////////////////////////////////////
///// Object
	
template class Object<ci::vec2, float>;
template class Object<ci::vec3, float>;
template class Object<ci::vec3, ci::vec3>;
	
template<>
Object<ci::vec2, float>::Object( const osc::Message &msg )
: Type( msg ), mClassId( msg[2].int32() ), mPosition( msg[3].flt(), msg[4].flt() ),
	mAngle( msg[5].flt() ), mVelocity( msg[6].flt(), msg[7].flt() ),
	mRotationVelocity( msg[8].flt() ), mAcceleration( msg[9].flt() ),
	mRotateAccel( msg[10].flt() )
{
}
	
template<>
Object<ci::vec3, float>::Object( const osc::Message &msg )
: Type( msg ), mClassId( msg[2].int32() ),
	mPosition( msg[3].flt(), msg[4].flt(), msg[5].flt() ),
	mAngle( msg[6].flt() ), mVelocity( msg[7].flt(), msg[8].flt(), msg[9].flt() ),
	mRotationVelocity( msg[10].flt() ), mAcceleration( msg[11].flt() ),
	mRotateAccel( msg[12].flt() )
{
}
	
template<>
Object<ci::vec3, ci::vec3>::Object( const osc::Message &msg )
: Type( msg ), mClassId( msg[2].int32() ),
	mPosition(	msg[3].flt(), msg[4].flt(), msg[5].flt() ),
	mAngle(		msg[6].flt(), msg[7].flt(), msg[8].flt() ),
	mVelocity(	msg[9].flt(), msg[10].flt(), msg[11].flt() ),
	mRotationVelocity( msg[12].flt(), msg[13].flt(), msg[14].flt() ),
	mAcceleration( msg[15].flt() ), mRotateAccel( msg[16].flt() )
{
}

template<typename VEC_T, typename ROT_T>
Object<VEC_T, ROT_T>::Object( Object<VEC_T, ROT_T> &&other ) NOEXCEPT
: Type( std::move( other ) ), mClassId( other.mClassId ), mPosition( std::move( other.mPosition ) ),
	mVelocity( std::move( other.mVelocity ) ), mAngle( std::move( other.mAngle ) ),
	mRotationVelocity( std::move( other.mRotationVelocity ) ), mAcceleration( other.mAcceleration ),
	mRotateAccel( other.mRotateAccel )
{
}

template<typename VEC_T, typename ROT_T>
Object<VEC_T, ROT_T>& Object<VEC_T, ROT_T>::operator=( Object<VEC_T, ROT_T> &&other ) NOEXCEPT
{
	if( this != &other ) {
		Type::operator=( std::move( other ) );
		mClassId = other.mClassId;
		mPosition = std::move( other.mPosition );
		mVelocity = std::move( other.mVelocity );
		mAngle = std::move( other.mAngle );
		mRotationVelocity = std::move( other.mRotationVelocity );
		mAcceleration = other.mAcceleration;
		mRotateAccel = other.mRotateAccel;
	}
	return *this;
}
	
/////////////////////////////////////////////////////////////////////////////////////////////////
///// Blob
	
template<>
Blob<ci::vec2, float, ci::vec2>::Blob( const osc::Message &msg )
: Type( msg ), mPosition( msg[2].flt(), msg[3].flt() ), mAngle( msg[4].flt() ),
	mDimensions( msg[5].flt(), msg[6].flt() ), mGeometry( msg[7].flt() ),
	mVelocity( msg[8].flt(), msg[9].flt() ), mRotationVelocity( msg[10].flt() ),
	mAcceleration( msg[11].flt() ), mRotateAccel( msg[12].flt() )
{
}
	
template<>
Blob<ci::vec3, float, ci::vec2>::Blob( const osc::Message &msg )
: Type( msg ), mPosition( msg[2].flt(), msg[3].flt(), msg[4].flt() ),
	mAngle( msg[5].flt() ), mDimensions( msg[6].flt(), msg[7].flt() ),
	mGeometry( msg[8].flt() ), mVelocity( msg[9].flt(), msg[10].flt(), msg[11].flt() ),
	mRotationVelocity( msg[12].flt() ), mAcceleration( msg[13].flt() ),
	mRotateAccel( msg[14].flt() )
{
}
	
template<>
Blob<ci::vec3, ci::vec3, ci::vec3>::Blob( const osc::Message &msg )
: Type( msg ), mPosition( msg[2].flt(), msg[3].flt(), msg[4].flt() ),
	mAngle( msg[5].flt(), msg[6].flt(), msg[7].flt() ),
	mDimensions( msg[8].flt(), msg[9].flt(), msg[10].flt() ),
	mGeometry( msg[11].flt() ), mVelocity( msg[12].flt(), msg[13].flt(), msg[14].flt() ),
	mRotationVelocity( msg[15].flt(), msg[16].flt(), msg[17].flt() ),
	mAcceleration( msg[18].flt() ), mRotateAccel( msg[19].flt() )
{
}

template<typename VEC_T, typename ROT_T, typename DIM_T>
Blob<VEC_T, ROT_T, DIM_T>::Blob( Blob<VEC_T, ROT_T, DIM_T> &&other ) NOEXCEPT
: Type( std::move( other ) ), mPosition( std::move( other.mPosition ) ),
	mVelocity( std::move( other.mVelocity ) ), mAngle( std::move( other.mAngle ) ),
	mRotationVelocity( std::move( other.mRotationVelocity ) ), 
	mDimensions( std::move( other.mDimensions ) ), mAcceleration( other.mAcceleration ),
	mRotateAccel( other.mRotateAccel ), mGeometry( other.mGeometry )
{
}

template<typename VEC_T, typename ROT_T, typename DIM_T>
Blob<VEC_T, ROT_T, DIM_T>& Blob<VEC_T, ROT_T, DIM_T>::operator=( Blob<VEC_T, ROT_T, DIM_T> &&other ) NOEXCEPT
{
	if( this != &other ) {
		Type::operator=( std::move( other ) );
		mPosition = std::move( other.mPosition );
		mVelocity = std::move( other.mVelocity );
		mAngle = std::move( other.mAngle );
		mRotationVelocity = std::move( other.mRotationVelocity );
		mDimensions = std::move( other.mDimensions );
		mAcceleration = other.mAcceleration;
		mRotateAccel = other.mRotateAccel;
		mGeometry = other.mGeometry;
	}
	return *this;
}
	
/////////////////////////////////////////////////////////////////////////////////////////////////
///// Blob2d

Blob2d::Blob2d( const osc::Message &msg )
: Blob( msg )
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////
///// Blob25d

Blob25d::Blob25d( const osc::Message &msg )
: Blob( msg )
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////
///// Blob3d

Blob3d::Blob3d( const osc::Message &msg )
: Blob( msg )
{
}

template<typename T>
using TypeFn = Receiver::TypeFn<T>;
	
/////////////////////////////////////////////////////////////////////////////////////////////////
///// Blob3D

template<typename TuioType>
class TypeHandler : public TypeHandlerBase {
public:
	//! TODO: Need to figure out about "PastFrameThreshold", got rid of it.
	TypeHandler() : mSourceIsCurrent( false ) {}
	virtual ~TypeHandler() = default;

	//! Handles incoming osc::Messages from the osc::Receiver.
	void			handleMessage( const osc::Message &message ) override;
	//! Abstract function that represents calling the function handlers.
	virtual void	handleFseq( int32_t frame ) = 0;
	void setSource( const std::string &source );
	
	
protected:
	typename map<string, vector<TuioType>>::iterator	mCurrentIt;
	map<string, vector<TuioType>>						mSetOfCurrentTouches;
	vector<int32_t>					mAdded, mUpdated;
	vector<TuioType>				mRemovedTouches;
	map<std::string, int32_t>		mSourceFrameNums;
	bool							mSourceIsCurrent;
};
	
//! Represents a handler with seperated callbacks
template<typename TuioType>
class SeparatedCallbackHandler : public TypeHandler<TuioType> {
public:
	SeparatedCallbackHandler() = default;
	~SeparatedCallbackHandler() = default;
	
	//! Sets the underlying TuioType Added Callback function.
	void setAddedHandler( TypeFn<TuioType> callback );
	//! Sets the underlying TuioType Updated Callback function.
	void setUpdatedHandler( TypeFn<TuioType> callback );
	//! Sets the underlying TuioType Removed Callback function.
	void setRemovedHandler( TypeFn<TuioType> callback );
	//! Overriden function which processes the Added, Updated, and Removed
	//! TuioTypes for this frame.
	void handleFseq( int32_t frame ) override;
	
protected:
	TypeFn<TuioType>	mAddCallback, mUpdateCallback, mRemoveCallback;
	std::mutex			mCallbackMutex;
};

//! Represents a specialized handler for ci::app::Windows
class WindowCursorHandler : public TypeHandler<Cursor2d> {
public:
	//! callback for window
	using FseqFn = std::function<void( const std::vector<Cursor2d> &/*added*/,
									   const std::vector<Cursor2d> &/*updated*/,
									   const std::vector<Cursor2d> &/*removed*/)>;
	WindowCursorHandler() = default;
	~WindowCursorHandler() = default;
	
	//! Sets the underlying FseqFn for this handler.
	void setFseqFn( FseqFn fseqFn );
	//! Overriden function which processes the Added, Updated, and Removed
	//! TuioTypes for this frame.
	void handleFseq( int32_t frame ) override;
	
protected:
	std::mutex	mFseqMutex;
	FseqFn		mFseqFn;
};
	
} // namespace detail
	
/////////////////////////////////////////////////////////////////////////////////////////////////
///// Receiver
	
Receiver::Receiver( osc::ReceiverBase *ptr )
: mReceiver(  ptr )
{
}
	
Receiver::Receiver( const app::WindowRef &window, osc::ReceiverBase *ptr )
: mReceiver( ptr )
{
	setupWindowReceiver( window );
}
	
Receiver::~Receiver()
{
	for( auto & handler : mHandlers ) {
		mReceiver->removeListener( handler.first );
	}
	mHandlers.clear();
}
	
template<typename TuioType>
void Receiver::setAddedFn( TypeFn<TuioType> callback )
{
	auto address = getOscAddressFromType<TuioType>();
	auto found = mHandlers.find( address );
	if( found != mHandlers.end() ) {
		auto typeHandler = dynamic_cast<detail::SeparatedCallbackHandler<TuioType>*>(found->second.get());
		typeHandler->setAddedHandler( callback );
	}
	else {
		auto inserted = mHandlers.emplace( address, std::unique_ptr<detail::SeparatedCallbackHandler<TuioType>>( new detail::SeparatedCallbackHandler<TuioType>() ) );
		auto created = dynamic_cast<detail::SeparatedCallbackHandler<TuioType>*>(inserted.first->second.get());
		created->setAddedHandler( callback );
		mReceiver->setListener( address, std::bind( &detail::TypeHandlerBase::handleMessage,
												   created, std::placeholders::_1 ) );
	}
}


template<typename TuioType>
void Receiver::setUpdatedFn( TypeFn<TuioType> callback )
{
	auto address = getOscAddressFromType<TuioType>();
	auto found = mHandlers.find( address );
	if( found != mHandlers.end() ) {
		auto typeHandler = dynamic_cast<detail::SeparatedCallbackHandler<TuioType>*>(found->second.get());
		typeHandler->setUpdatedHandler( callback );
	}
	else {
		auto inserted = mHandlers.emplace( address, std::unique_ptr<detail::SeparatedCallbackHandler<TuioType>>( new detail::SeparatedCallbackHandler<TuioType>() ) );
		auto created = dynamic_cast<detail::SeparatedCallbackHandler<TuioType>*>(inserted.first->second.get());
		created->setUpdatedHandler( callback );
		mReceiver->setListener( address, std::bind( &detail::TypeHandlerBase::handleMessage,
												   created, std::placeholders::_1 ) );
	}
}

template<typename TuioType>
void Receiver::setRemovedFn( TypeFn<TuioType> callback )
{
	auto address = getOscAddressFromType<TuioType>();
	auto found = mHandlers.find( address );
	if( found != mHandlers.end() ) {
		auto typeHandler = dynamic_cast<detail::SeparatedCallbackHandler<TuioType>*>(found->second.get());
		typeHandler->setRemovedHandler( callback );
	}
	else {
		auto inserted = mHandlers.emplace( address, std::unique_ptr<detail::SeparatedCallbackHandler<TuioType>>( new detail::SeparatedCallbackHandler<TuioType>() ) );
		auto created = dynamic_cast<detail::SeparatedCallbackHandler<TuioType>*>(inserted.first->second.get());
		created->setRemovedHandler( callback );
		mReceiver->setListener( address, std::bind( &detail::TypeHandlerBase::handleMessage,
												   created, std::placeholders::_1 ) );
	}
}
	
template<>
void Receiver::setAddedFn( TypeFn<Cursor2d> callback )
{
	auto address = getOscAddressFromType<Cursor2d>();
	auto found = mHandlers.find( address );
	if( found != mHandlers.end() ) {
		auto typeHandler = dynamic_cast<detail::SeparatedCallbackHandler<Cursor2d>*>(found->second.get());
		// could be WindowCursorHandler
		if( typeHandler ) {
			typeHandler->setAddedHandler( callback );
		}
		else {
			CI_LOG_W("app::Window is already capturing this event");
		}
	}
	else {
		auto inserted = mHandlers.emplace( address, std::unique_ptr<detail::SeparatedCallbackHandler<Cursor2d>>( new detail::SeparatedCallbackHandler<Cursor2d>() ) );
		auto created = dynamic_cast<detail::SeparatedCallbackHandler<Cursor2d>*>(inserted.first->second.get());
		created->setAddedHandler( callback );
		mReceiver->setListener( address, std::bind( &detail::TypeHandlerBase::handleMessage,
												   created, std::placeholders::_1 ) );
	}
}
template<>
void Receiver::setUpdatedFn( TypeFn<Cursor2d> callback )
{
	auto address = getOscAddressFromType<Cursor2d>();
	auto found = mHandlers.find( address );
	if( found != mHandlers.end() ) {
		auto typeHandler = dynamic_cast<detail::SeparatedCallbackHandler<Cursor2d>*>(found->second.get());
		// could be WindowCursorHandler
		if( typeHandler ) {
			typeHandler->setUpdatedHandler( callback );
		}
		else {
			CI_LOG_W("app::Window is already capturing this event");
		}
	}
	else {
		auto inserted = mHandlers.emplace( address, std::unique_ptr<detail::SeparatedCallbackHandler<Cursor2d>>( new detail::SeparatedCallbackHandler<Cursor2d>() ) );
		auto created = dynamic_cast<detail::SeparatedCallbackHandler<Cursor2d>*>(inserted.first->second.get());
		created->setUpdatedHandler( callback );
		mReceiver->setListener( address, std::bind( &detail::TypeHandlerBase::handleMessage,
												   created, std::placeholders::_1 ) );
	}
}
template<>
void Receiver::setRemovedFn( TypeFn<Cursor2d> callback )
{
	auto address = getOscAddressFromType<Cursor2d>();
	auto found = mHandlers.find( address );
	if( found != mHandlers.end() ) {
		auto typeHandler = dynamic_cast<detail::SeparatedCallbackHandler<Cursor2d>*>(found->second.get());
		// could be WindowCursorHandler
		if( typeHandler ) {
			typeHandler->setRemovedHandler( callback );
		}
		else {
			CI_LOG_W("app::Window is already capturing this event");
		}
	}
	else {
		auto inserted = mHandlers.emplace( address, std::unique_ptr<detail::SeparatedCallbackHandler<Cursor2d>>( new detail::SeparatedCallbackHandler<Cursor2d>() ) );
		auto created = dynamic_cast<detail::SeparatedCallbackHandler<Cursor2d>*>(inserted.first->second.get());
		created->setRemovedHandler( callback );
		mReceiver->setListener( address, std::bind( &detail::TypeHandlerBase::handleMessage,
												   created, std::placeholders::_1 ) );
	}
}

template<typename TuioType>
void Receiver::clear()
{
	auto address = getOscAddressFromType<TuioType>();
	auto found = mHandlers.find( address );
	if( found != mHandlers.end() ) {
		mReceiver->removeListener( address );
		mHandlers.erase( found );
	}
}
	
void Receiver::setupWindowReceiver( ci::app::WindowRef window )
{
	auto address = getOscAddressFromType<Cursor2d>();
	auto inserted = mHandlers.emplace( address, std::unique_ptr<detail::WindowCursorHandler>( new detail::WindowCursorHandler() ) );
	auto handler = dynamic_cast<detail::WindowCursorHandler*>(inserted.first->second.get());
	mReceiver->setListener( address, std::bind( &detail::TypeHandlerBase::handleMessage,
												   handler, std::placeholders::_1 ) );
	
	// Take a weak pointer, so that we don't try to access it after it's been destroyed.
	auto weakWindow = std::weak_ptr<app::Window>( window );
	// Set the Fseq lambda
	handler->setFseqFn(
	[weakWindow]( const std::vector<Cursor2d> &added,
					const std::vector<Cursor2d> &updated,
					const std::vector<Cursor2d> &removed ) {
		auto shared = weakWindow.lock();
		if( shared ) {
			auto transformFn = [shared]( const Cursor2d &cursor ) { return cursor.convertToTouch( shared ); };
			if( ! added.empty() ) {
				std::vector<TouchEvent::Touch> touches( added.size() );
				std::transform( begin( added ), end( added ), touches.begin(), transformFn );
				ci::app::TouchEvent event( shared, touches );
				shared->emitTouchesBegan( &event );
			}
			if( ! updated.empty() ) {
				std::vector<TouchEvent::Touch> touches( updated.size() );
				std::transform( begin( updated ), end( updated ), touches.begin(), transformFn );
				ci::app::TouchEvent event( shared, touches );
				shared->emitTouchesMoved( &event );
			}
			if( ! removed.empty() ) {
				std::vector<TouchEvent::Touch> touches( removed.size() );
				std::transform( begin( removed ), end( removed ), touches.begin(), transformFn );
				ci::app::TouchEvent event( shared, touches );
				shared->emitTouchesEnded( &event );
			}
		}
	});
}

template<typename T>
const char* Receiver::getOscAddressFromType()
{
	if( std::is_same<T, Cursor2d>::value ) return "/tuio/2Dcur";
	else if( std::is_same<T, Cursor25d>::value ) return "/tuio/25Dcur";
	else if( std::is_same<T, Cursor3d>::value ) return "/tuio/3Dcur";
	else if( std::is_same<T, Object2d>::value ) return "/tuio/2Dobj";
	else if( std::is_same<T, Object25d>::value ) return "/tuio/25Dobj";
	else if( std::is_same<T, Object3d>::value ) return "/tuio/3Dobj";
	else if( std::is_same<T, Blob2d>::value ) return "/tuio/2Dblb";
	else if( std::is_same<T, Blob25d>::value ) return "/tuio/25Dblb";
	else if( std::is_same<T, Blob3d>::value ) return "/tuio/3Dblb";
	else return "Unknown Target";
}
	
/////////////////////////////////////////////////////////////////////////////////////////////////
///// Detail
	
namespace detail {
	
/////////////////////////////////////////////////////////////////////////////////////////////////
///// TypeHandler templated
	
template<typename TuioType>
void TypeHandler<TuioType>::setSource( const std::string &source )
{
	mCurrentIt = mSetOfCurrentTouches.find( source );
	if( mCurrentIt == mSetOfCurrentTouches.end() ) {
		auto emplaced = mSetOfCurrentTouches.emplace( move( source ), std::vector<TuioType>() );
		mCurrentIt = emplaced.first;
	}
	mSourceIsCurrent = true;
}
	
template<typename TuioType>
void TypeHandler<TuioType>::handleMessage( const osc::Message &message )
{
	auto messageType = message[0].string();
	
	using namespace std;
	if( messageType == "source" ) {
		setSource( message[1].string() );
	}
	else if( messageType == "set" ) {
		if( ! mSourceIsCurrent )
			setSource( message.getSenderIpAddress().to_string() );
		
		auto sessionId = message[1].int32();
		auto &currentSet = mCurrentIt->second;
		auto currentBegin = begin( currentSet );
		auto currentEnd = end( currentSet );
		auto it = find_if( currentBegin, currentEnd,
		[sessionId]( const TuioType &type ){
			  return sessionId == type.getSessionId();
		});
		if( it == currentEnd ) {
			TuioType type( message );
			auto insert = lower_bound( currentBegin,
									   currentEnd,
									   type,
			[]( const TuioType &lhs, const TuioType &value ){
				  return lhs < value;
			});
			type.setSource( mCurrentIt->first );
			currentSet.emplace( insert, std::move( type ) );
			mAdded.push_back( sessionId );
		}
		else {
			TuioType type( message );
			type.setSource( mCurrentIt->first );
			*it = std::move( type );
			mUpdated.push_back( sessionId );
		}
	}
	else if( messageType == "alive" ) {
		if( ! mSourceIsCurrent )
			setSource( message.getSenderIpAddress().to_string() );
		
		std::vector<int32_t> aliveIds( message.getNumArgs() - 1 );
		int i = 1;
		for( auto & aliveId : aliveIds ) {
			aliveId = message[i++].int32();
		}
		std::sort( begin( aliveIds ), end( aliveIds ) );
		auto & aliveIdsRef = aliveIds;
		std::function<bool(const TuioType &)> aliveIdsRemoveIf = [aliveIdsRef]( const TuioType &type ) {
			return binary_search( begin( aliveIdsRef ), end( aliveIdsRef ), type.getSessionId() ); // search for the sessionId
		};
		auto &currentSet = mCurrentIt->second;
		auto currentEnd = end( currentSet );
		auto remove = stable_partition( begin( currentSet ), currentEnd, aliveIdsRemoveIf );
		if( remove != currentEnd ) {
			std::move( remove, currentEnd, back_inserter( mRemovedTouches ) );
			currentSet.erase( remove, currentEnd );
		}
	}
	else if( messageType == "fseq" ) {
		if( ! mSourceIsCurrent )
			setSource( message.getSenderIpAddress().to_string() );
		
		handleFseq( message[1].int32() );
		
		mSourceIsCurrent = false;
	}
}
	
/////////////////////////////////////////////////////////////////////////////////////////////////
///// SeparatedCallbackHandler templated
	
template<typename TuioType>
void SeparatedCallbackHandler<TuioType>::setAddedHandler( TypeFn<TuioType> callback )
{
	std::lock_guard<std::mutex> lock( mCallbackMutex );
	mAddCallback = callback;
}

template<typename TuioType>
void SeparatedCallbackHandler<TuioType>::setUpdatedHandler( TypeFn<TuioType> callback )
{
	std::lock_guard<std::mutex> lock( mCallbackMutex );
	mUpdateCallback = callback;
}

template<typename TuioType>
void SeparatedCallbackHandler<TuioType>::setRemovedHandler( TypeFn<TuioType> callback )
{
	std::lock_guard<std::mutex> lock( mCallbackMutex );
	mRemoveCallback = callback;
}
	
template<typename TuioType>
void SeparatedCallbackHandler<TuioType>::handleFseq( int32_t frame )
{
	int32_t prev_frame = this->mSourceFrameNums[this->mCurrentIt->first];
	int32_t delta_frame = frame - prev_frame;
	// TODO: figure out about past frame threshold updating, this was also in the if condition ( dframe < -mPastFrameThreshold )
	if( frame == -1 || delta_frame > 0 ) {
		std::lock_guard<std::mutex> lock( mCallbackMutex );
		auto & currentSet = this->mCurrentIt->second;
		auto currentBegin = begin( currentSet );
		auto currentEnd = end( currentSet );
		if( ! this->mAdded.empty() ) {
			if( mAddCallback ) {
				for( auto & added : this->mAdded ) {
					auto found = find_if( currentBegin, currentEnd,
					[added]( const TuioType &type ) {
						 return added == type.getSessionId();
					});
					if( found != currentEnd )
						mAddCallback( *found );
				}
			}
			this->mAdded.clear();
		}
		if ( ! this->mUpdated.empty() ) {
			if( mUpdateCallback ) {
				for( auto & updated : this->mUpdated ) {
					auto found = find_if( currentBegin, currentEnd,
					[updated]( const TuioType &type ) {
						 return updated == type.getSessionId();
					});
					if( found != currentEnd )
						mUpdateCallback( *found );
				}
			}
			this->mUpdated.clear();
		}
		if( ! this->mRemovedTouches.empty() ){
			if( mRemoveCallback )
				for( auto & removed : this->mRemovedTouches ) {
					mRemoveCallback( removed );
				}
			this->mRemovedTouches.clear();
		}
		this->mSourceFrameNums[this->mCurrentIt->first] = ( frame == -1 ) ? this->mSourceFrameNums[this->mCurrentIt->first] : frame;
	}
}
	
/////////////////////////////////////////////////////////////////////////////////////////////////
///// WindowCursorHandler
	
void WindowCursorHandler::setFseqFn( FseqFn fseqFn )
{
	std::lock_guard<std::mutex> lock( mFseqMutex );
	mFseqFn = fseqFn;
}
	
void WindowCursorHandler::handleFseq( int32_t frame )
{
	int32_t prev_frame = this->mSourceFrameNums[this->mCurrentIt->first];
	int32_t delta_frame = frame - prev_frame;
	// TODO: figure out about past frame threshold updating, this was also in the if condition ( dframe < -mPastFrameThreshold )
	if( frame == -1 || delta_frame > 0 ) {
		auto & currentSet = this->mCurrentIt->second;
		auto currentBegin = begin( currentSet );
		auto currentEnd = end( currentSet );
		std::vector<Cursor2d> addedCursors, updatedCursors;
		if( ! this->mAdded.empty() ) {
			for( auto & added : this->mAdded ) {
				auto found = find_if( currentBegin, currentEnd,
				[added]( const Cursor2d &type ) {
					 return added == type.getSessionId();
				});
				if( found != currentEnd )
					addedCursors.push_back( *found );
			}
			this->mAdded.clear();
		}
		if ( ! this->mUpdated.empty() ) {
			for( auto & updated : this->mUpdated ) {
				auto found = find_if( currentBegin, currentEnd,
				[updated]( const Cursor2d &type ) {
					 return updated == type.getSessionId();
				});
				if( found != currentEnd )
					updatedCursors.push_back( *found );
			}
			this->mUpdated.clear();
		}
		this->mSourceFrameNums[this->mCurrentIt->first] = ( frame == -1 ) ? this->mSourceFrameNums[this->mCurrentIt->first] : frame;
		{
			std::lock_guard<std::mutex> lock( mFseqMutex );
			mFseqFn( addedCursors, updatedCursors, this->mRemovedTouches );
		}
		this->mRemovedTouches.clear();
	}
}

} // namespace detail

//! Specializations per type.
	
template void Receiver::setAddedFn( TypeFn<tuio::detail::Cursor<ci::vec2>> );
template void Receiver::setUpdatedFn( TypeFn<tuio::detail::Cursor<ci::vec2>> );
template void Receiver::setRemovedFn( TypeFn<tuio::detail::Cursor<ci::vec2>> );
template void Receiver::clear<tuio::detail::Cursor<ci::vec2>>();
	
template void Receiver::setAddedFn( TypeFn<tuio::detail::Cursor<ci::vec3>> );
template void Receiver::setUpdatedFn( TypeFn<tuio::detail::Cursor<ci::vec3>> );
template void Receiver::setRemovedFn( TypeFn<tuio::detail::Cursor<ci::vec3>> );
template void Receiver::clear<tuio::detail::Cursor<ci::vec3>>();
	
template void Receiver::setAddedFn( TypeFn<tuio::Object2d> );
template void Receiver::setUpdatedFn( TypeFn<tuio::Object2d> );
template void Receiver::setRemovedFn( TypeFn<tuio::Object2d> );
template void Receiver::clear<tuio::Object2d>();
	
template void Receiver::setAddedFn( TypeFn<tuio::Object25d> );
template void Receiver::setUpdatedFn( TypeFn<tuio::Object25d> );
template void Receiver::setRemovedFn( TypeFn<tuio::Object25d> );
template void Receiver::clear<tuio::Object25d>();
	
template void Receiver::setAddedFn( TypeFn<tuio::Object3d> );
template void Receiver::setUpdatedFn( TypeFn<tuio::Object3d> );
template void Receiver::setRemovedFn( TypeFn<tuio::Object3d> );
template void Receiver::clear<tuio::Object3d>();
	
template void Receiver::setAddedFn( TypeFn<tuio::Blob2d> );
template void Receiver::setUpdatedFn( TypeFn<tuio::Blob2d> );
template void Receiver::setRemovedFn( TypeFn<tuio::Blob2d> );
template void Receiver::clear<tuio::Blob2d>();
	
template void Receiver::setAddedFn( TypeFn<tuio::Blob25d> );
template void Receiver::setUpdatedFn( TypeFn<tuio::Blob25d> );
template void Receiver::setRemovedFn( TypeFn<tuio::Blob25d> );
template void Receiver::clear<tuio::Blob25d>();
	
template void Receiver::setAddedFn( TypeFn<tuio::Blob3d> );
template void Receiver::setUpdatedFn( TypeFn<tuio::Blob3d> );
template void Receiver::setRemovedFn( TypeFn<tuio::Blob3d> );
template void Receiver::clear<tuio::Blob3d>();

}}  // namespace tuio // namespace cinder
