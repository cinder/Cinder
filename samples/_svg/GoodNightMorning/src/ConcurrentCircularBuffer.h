#pragma once

#include <boost/circular_buffer.hpp>
#include "cinder/Thread.h"

namespace cinder {

template<typename T>
class ConcurrentCircularBuffer {
  public:
	typedef boost::circular_buffer<T> container_type;
	typedef typename container_type::size_type size_type;
	typedef typename container_type::value_type value_type;
	typedef typename boost::call_traits<value_type>::param_type param_type;

	explicit ConcurrentCircularBuffer( size_type capacity )
		: mNumUnread( 0 ), mContainer( capacity ), mCanceled( false )
	{}

	void pushFront( param_type item ) {
		// param_type represents the "best" way to pass a parameter of type value_type to a method
		std::unique_lock<std::mutex> lock( mMutex );
		while( ! is_not_full_impl() && ! mCanceled ) {
			mNotFullCond.wait( lock );
		}
		if( mCanceled )
			return;
		mContainer.push_front( item );
		++mNumUnread;
		mNotEmptyCond.notify_one();
	}

	void popBack(value_type* pItem) {
		std::unique_lock<std::mutex> lock( mMutex );
		while( ! is_not_empty_impl() && ! mCanceled ) {
			mNotEmptyCond.wait( lock );
		}
		if( mCanceled )
			return;
		*pItem = mContainer[--mNumUnread];
		mNotFullCond.notify_one();
	}

	bool isNotEmpty() {
		std::unique_lock<std::mutex> lock( mMutex );
		return is_not_empty_impl();
	}

	bool isNotFull() {
		std::unique_lock<std::mutex> lock( mMutex );
		return is_not_full_impl();
	}
	
	void cancel() {
		std::lock_guard<std::mutex> lock( mMutex );
		mCanceled = true;
		mNotFullCond.notify_all();
		mNotEmptyCond.notify_all();
	}

  private:
	ConcurrentCircularBuffer( const ConcurrentCircularBuffer& );              // Disabled copy constructor
	ConcurrentCircularBuffer& operator=( const ConcurrentCircularBuffer& ); // Disabled assign operator

	bool is_not_empty_impl() const { return mNumUnread > 0; }
	bool is_not_full_impl() const { return mNumUnread < mContainer.capacity(); }

	size_type				mNumUnread;
	container_type			mContainer;
	std::mutex				mMutex;
	std::condition_variable	mNotEmptyCond;
	std::condition_variable	mNotFullCond;
	bool					mCanceled;
};

} // namespace cinder