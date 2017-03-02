/*
 Copyright (c) 2015, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

 This signals / slots implementation is adapted from Tim Janik's simplesignal.cc: https://testbit.eu/cpp11-signal-system-performance/
 released under the CC0 Public Domain license: http://creativecommons.org/publicdomain/zero/1.0/
*/

#pragma once

#include "cinder/CinderAssert.h"
#include "cinder/Noncopyable.h"
#include "cinder/Export.h"

#include <functional>
#include <memory>
#include <map>
#include <vector>

namespace cinder { namespace signals {

namespace detail {

//! Base class for signal links, which manages reference counting and provides a concrete type to be passed to Connection
struct SignalLinkBase {
  public:
	SignalLinkBase()
		: mRefCount( 1 ), mEnabled( true )
	{}
	virtual ~SignalLinkBase()
	{
		CI_ASSERT( mRefCount == 0 );
	}

	virtual bool removeSibling( SignalLinkBase *link ) = 0;

	void incrRef()
	{
		mRefCount++;
		CI_ASSERT( mRefCount > 0 );
	}

	void decrRef()
	{
		mRefCount--;
		if( ! mRefCount )
			delete this;
		else
			CI_ASSERT( mRefCount > 0 );
	}

	int getRefCount() const
	{
		return mRefCount;
	}

	void enable()
	{
		mEnabled = true;
	}

	void disable()
	{
		mEnabled = false;
	}

	bool isEnabled() const
	{
		return mEnabled;
	}

  private:
	int		mRefCount;
	bool	mEnabled;
};

//! Base Signal class, which provides a concrete type that can be stored by the Disconnector
struct SignalBase : private Noncopyable {
	//! abstract method to disconnect \a link from the callback chain, which resides in the priority group \a priority.
	virtual bool disconnect( SignalLinkBase *link, int priority ) = 0;

	friend struct Disconnector;
};

//! Helper class for disconnecting Connections.
struct CI_API Disconnector : private Noncopyable {
	//! Constructs a new Disconnector, which is owned by \a signal.
	Disconnector( SignalBase *signal );
	//! Instructs the owning signal to disconnect \a link, which resides within priority group \a priority.
	bool disconnect( SignalLinkBase *link, int priority );

  private:
	SignalBase*	mSignal;
};

} // namespace detail

// ----------------------------------------------------------------------------------------------------
// Connection and ScopedConnection
// ----------------------------------------------------------------------------------------------------

//! Connection is returned from Signal::connect(), which can be used to disconnect the callback slot.
class CI_API Connection {
  public:
	Connection();
	Connection( const std::shared_ptr<detail::Disconnector> &disconnector, detail::SignalLinkBase *link, int priority );
	Connection( const Connection &other );
	Connection( Connection &&other );
	Connection& operator=( const Connection &rhs );
	Connection& operator=( Connection &&rhs );

	//! Disconnects this Connection from the callback chain. \a return true if a disconnection was made, false otherwise.
	bool disconnect();
	//! Returns whether or not this Connection is still connected to the callback chain.
	bool isConnected() const;

	//! Enable calling the function linked to this Connection. Does not affect the callback chain. Default is enabled.
	void enable();
	//! Disable calling the function linked to this Connection. Does not affect the callback chain.
	void disable();
	//! Returns true if the linked function will be called as part of the callback chain. Default is enabled.
	bool isEnabled() const;

  private:
	std::weak_ptr<detail::Disconnector>		mDisconnector;
	detail::SignalLinkBase*					mLink;
	int										mPriority;
};

//! ScopedConnection can be captured from Signal::connect() to limit the connection lifetime to the current scope, after which Connection::disconnect() will be called.
class CI_API ScopedConnection : public Connection, private Noncopyable {
  public:
	ScopedConnection();
	~ScopedConnection();
	ScopedConnection( ScopedConnection &&other );
	ScopedConnection( Connection &&other );
	ScopedConnection& operator=( ScopedConnection &&rhs );
};

// ----------------------------------------------------------------------------------------------------
// ConnectionList
// ----------------------------------------------------------------------------------------------------

//! Maintains a list of Connections and calls disconnect on them when it is destroyed. Non-copyable.
class CI_API ConnectionList : private Noncopyable {
  public:
	~ConnectionList();

	//! Add a Connection to the list
	void add( Connection &&target );
	//! Disconnects and clears all Connections.
	void clear();
	//! Same as add()
	void operator+=( Connection &&target ) { add( std::move( target ) ); }

  private:
	std::vector<Connection>	mConnections;
};

// ----------------------------------------------------------------------------------------------------
// Collectors
// ----------------------------------------------------------------------------------------------------

namespace detail {

//! The template implementation for callback list.
template<typename, typename> class	SignalProto;   // undefined

//! Invokes signal handlers differently depending on return type.
template<typename, typename> struct	CollectorInvocation;

//! Returns the result of the last signal handler from a signal emission.
template<typename ResultT>
struct CollectorLast {
	typedef ResultT CollectorResult;

	explicit CollectorLast() : mLast()	{}

	inline bool operator()( ResultT r )    { mLast = r; return true; }

	CollectorResult getResult() const        { return mLast; }
private:
	ResultT mLast;
};

//! Implements the default signal handler collection behaviour.
template<typename ResultT>
struct CollectorDefault : CollectorLast<ResultT> {
};

//! CollectorDefault specialisation for signals with void return type.
template<>
struct CollectorDefault<void> {
	typedef void CollectorResult;

	void getResult() const		{}
	inline bool operator()()	{ return true; }
};

//! CollectorInvocation specialisation for regular signals.
template<class Collector, class R, class... Args>
struct CollectorInvocation<Collector, R ( Args... )> : public SignalBase {

	bool invoke( Collector &collector, const std::function<R ( Args... )> &callback, Args... args )
	{
		return collector( callback( args... ) );
	}
};

//! CollectorInvocation specialisation for signals with void return type.
template<class Collector, class... Args>
struct CollectorInvocation<Collector, void( Args... )> : public SignalBase {

	bool invoke( Collector &collector, const std::function<void( Args... )> &callback, Args... args )
	{
		callback( args... );
		return collector();
	}
};

// ----------------------------------------------------------------------------------------------------
// SignalProto
// ----------------------------------------------------------------------------------------------------

//! SignalProto template, the parent class of Signal, specialised for the callback signature and collector.
template<class Collector, class R, class... Args>
class SignalProto<R ( Args... ), Collector> : private CollectorInvocation<Collector, R ( Args... )> {
  protected:
	typedef std::function<R ( Args... )>		CallbackFn;
	typedef typename CallbackFn::result_type	Result;
	typedef typename Collector::CollectorResult	CollectorResult;

  public:
	//! Constructs an empty SignalProto
	SignalProto()
		: mDisconnector( new Disconnector( this ) )
	{}

	//! Destructor releases all resources associated with this signal.
	~SignalProto()
	{
		for( auto &lp : mLinks ) {
			SignalLink *link = lp.second;
			if( link ) {
				while( link->mNext != link )
					link->mNext->unlink();

				CI_ASSERT( link->getRefCount() >= 2 );

				link->decrRef();
				link->decrRef();
			}
		}
	}

	//! Connects \a callback to the signal, assigned to the default priority group (priority = 0). \return a Connection, which can be used to disconnect this callback slot.
	Connection connect( const CallbackFn &callback )
	{
		return connect( 0, callback );
	}

	//! Connects \a callback to the signal, assigned to the priority group \a priority. \return a Connection, which can be used to disconnect this callback slot.
	Connection connect( int priority, const CallbackFn &callback )
	{
		auto head = ensureLinkGroup( priority );
		auto link = head->addBefore( callback );
		return Connection( mDisconnector, link, priority );
	}

	//! Emit a signal, i.e. invoke all its callbacks and collect return types with Collector. \return the CollectorResult from the collector.
	CollectorResult	emit( Args... args )
	{
		Collector collector;
		emit( collector, args... );
		return collector.getResult();
	}

	//! Emit a signal, i.e. invoke all its callbacks and collect return types with \a collector.
	void emit( Collector &collector, Args... args )
	{
		bool continueEmission = true;
		for( auto &lp : mLinks ) {
			SignalLink *link = lp.second;
			link->incrRef();
			do {
				if( link->mCallbackFn && link->isEnabled() ) {
					continueEmission = this->invoke( collector, link->mCallbackFn, args... );
					if( ! continueEmission ) {
						break;
					}
				}

				SignalLink *old = link;
				link = old->mNext;

				link->incrRef();
				old->decrRef();
			}
			while( link != lp.second );
			
			link->decrRef();

			if( ! continueEmission )
				break;
		}
	}

	//! Returns the number of connected slots.
	size_t getNumSlots() const
	{
		size_t count = 0;

		for( auto &lp : mLinks ) {
			SignalLink *link = lp.second;
			do {
				if( link->mCallbackFn )
					count++;

				SignalLink *old = link;
				link = old->mNext;
			}
			while( link != lp.second );
		}

		return count;
	}

  private:

	//! implements a doubly-linked ring with ref-counted nodes containing the signal handlers.
	struct SignalLink : public SignalLinkBase {
		explicit SignalLink( const CallbackFn &callback )
			: mNext( nullptr ), mPrev( nullptr ), mCallbackFn( callback )
		{}

		void unlink()
		{
			mCallbackFn = nullptr;
			if( mNext )
				mNext->mPrev = mPrev;
			if( mPrev )
				mPrev->mNext = mNext;

			decrRef();
			// leave intact ->mNext, ->mPrev for stale iterators
		}

		SignalLink* addBefore( const CallbackFn &callback )
		{
			SignalLink *link = new SignalLink( callback );

			link->mPrev = mPrev; // link to last
			link->mNext = this;
			mPrev->mNext = link; // link from last
			mPrev = link;

			return link;
		}

		bool deactivate( const CallbackFn &callback )
		{
			if( callback == mCallbackFn ) {
				mCallbackFn = nullptr;	// deactivate static head
				return true;
			}

			for( SignalLink *link = this->mNext ? this->mNext : this; link != this; link = link->mNext ) {
				if( callback == link->function ) {
					link->unlink();		// deactivate and unlink sibling
					return true;
				}
			}
			return false;
		}

		bool removeSibling( SignalLinkBase *sibling ) override
		{
			for( SignalLink *link = this->mNext ? this->mNext : this; link != this; link = link->mNext ) {
				if( sibling == link ) {
					link->unlink();		// deactivate and unlink sibling
					return true;
				}
			}
			return false;
		}

		SignalLink*		mNext;
		SignalLink*		mPrev;
		CallbackFn		mCallbackFn;
	};

	bool disconnect( SignalLinkBase *link, int priority ) override
	{
		auto it = mLinks.find( priority );
		if( it == mLinks.end() )
			return false;

		return it->second->removeSibling( link );
	}

	//! returns the head link for this priority group
	SignalLink* ensureLinkGroup( int priority )
	{
		auto it = mLinks.find( priority );
		if( it != mLinks.end() )
			return it->second;
		else {
			auto head = new SignalLink( CallbackFn() ); // ref count = 1
			head->incrRef(); // ref count = 2, head of ring, can be deactivated but not removed
			head->mNext = head; // ring head initialization
			head->mPrev = head; // ring tail initialization

			mLinks.insert( { priority, head } );
			return head;
		}
	}

	std::map<int, SignalLink*, std::greater<int> >	mLinks;			// groups of callback nodes, which are stored in a reference counted linked list. Each group represents priority, greater int means fires first.
	std::shared_ptr<Disconnector>					mDisconnector;	// Connection holds a weak_ptr to this to make disconnections.
};

} // cinder::detail

// namespace cinder

// ----------------------------------------------------------------------------------------------------
// Signal
// ----------------------------------------------------------------------------------------------------

//! \brief Signal is a template type providing an interface for arbitrary callback lists.
//!
//! A signal type is declared with the function signature of its callbacks, and optionally a return
//! result collector class type. The callbacks of a signal are invoked with the emit() method and
//! arguments according to the signature.
//!
//! The result returned by emit() depends on the signal collector class. By default, the result of
//! the last callback is returned from emit(). Collectors can be implemented to accumulate callback
//! results or to halt the running emissions in correspondance to callback results.
//!
//! You can control the order in which callbacks are connected by providing a `priority` int as the
//! first argument to connect(). A higher priority means the callback will happen sooner in the
//! emission chain. Callbacks within the same emission group will be called according to the order they
//! were connected, first in first out. By default all connected slots will be assigned to the priority
//! group `0`, e.g. you call connect() with no `priority` argument. This also means by default, when no
//! priority groups are used then whatever callback is connected first to a signal will be called first.
//!
//! To disconnect a slot, you use the Connection class returned from the connect() methods. You can
//! also capture a ScopedConnection from connect(), which will automatically disconnect when it goes out
//! of scope.
//!
//! The signal implementation is safe against recursion, so callbacks may be connected and disconnected
//! during a signal emission. Recursive emit() calls are also safe.
//!
//! \note Signals are non-copyable.
template <typename Signature, class Collector = detail::CollectorDefault<typename std::function<Signature>::result_type> >
struct Signal : detail::SignalProto<Signature, Collector> {

	typedef detail::SignalProto<Signature, Collector>	SignalProto;
	typedef typename SignalProto::CallbackFn			CallbackFn;
};

// ----------------------------------------------------------------------------------------------------
// slot
// ----------------------------------------------------------------------------------------------------

//! Creates a std::function by binding \a object to the member function pointer \a method.
template<class Instance, class Class, class R, class... Args>
std::function<R ( Args... )> slot( Instance &object, R (Class::*method)( Args... ) )
{
	return [&object, method] ( Args... args )	{ return ( object .* method )( args... ); };
}

//! Creates a std::function by binding \a object to the member function pointer \a method.
template<class Class, class R, class... Args>
std::function<R ( Args... )> slot( Class *object, R ( Class::*method )( Args... ) )
{
	return [object, method] ( Args... args )	{ return ( object ->* method )( args... ); };
}

// ----------------------------------------------------------------------------------------------------
// Collectors
// ----------------------------------------------------------------------------------------------------

//! Keep signal emissions going until any handler returns false.
template<typename ResultT>
struct CollectorUntil0 {
	typedef ResultT	CollectorResult;

	explicit CollectorUntil0() : mResult() {}

	const CollectorResult& getResult() const	{ return mResult; }

	inline bool	operator()( ResultT r )
	{
		mResult = r;
		return mResult ? true : false;
	}

private:
	CollectorResult mResult;
};

//! Keep signal emissions going while all handlers return 0 false.
template<typename ResultT>
struct CollectorWhile0 {
	typedef ResultT CollectorResult;

	explicit CollectorWhile0() : mResult() {}

	const CollectorResult& getResult() const	{ return mResult; }

	inline bool	operator()( ResultT r )
	{
		mResult = r;
		return ( mResult ? false : true );
	}

private:
	CollectorResult mResult;
};

//! Returns true if all slots return true, else false. Does not short-circuit. Returns true if there are no slots.
struct CollectorBooleanAnd {
	typedef bool CollectorResult;

	explicit CollectorBooleanAnd() : mResult( true ) {}

	const CollectorResult& getResult() const	{ return mResult; }

	inline bool	operator()( bool r )
	{
		mResult = ( mResult && r );
		return true;
	}

private:
	CollectorResult mResult;
};

//! Returns a bitmask where in order for the bit in type T to be be 1, it has to be 1 from all slots. Returns 0 if there are no slots.
template<typename ResultT>
struct CollectorBitwiseAnd {
	typedef ResultT	CollectorResult;

	explicit CollectorBitwiseAnd() : mResult( 0 ), mFirst( true ) {}

	const CollectorResult& getResult() const	{ return mResult; }

	inline bool	operator()( ResultT r )
	{
		if( mFirst ) {
			mFirst = false;
			mResult = r;
		}
		else
			mResult &= r;

		return true;
	}

private:
	CollectorResult mResult;
	bool			mFirst;
};

//! CollectorVector returns the result of all signal handlers from a signal emission in a std::vector.
template<typename ResultT>
struct CollectorVector {
	typedef std::vector<ResultT> CollectorResult;

	const CollectorResult& getResult() const	{ return mResult; }

	inline bool	operator()( ResultT r )
	{
		mResult.push_back( r );
		return true;
	}
	
private:
	CollectorResult mResult;
};

} } // namespace cinder::signals
