/*
 Copyright (c) 2012, The Cinder Project
 All rights reserved.
 
 This code is designed for use with the Cinder C++ library, http://libcinder.org

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

#include <boost/circular_buffer.hpp>
#include "cinder/Noncopyable.h"
#include "cinder/Thread.h"

namespace cinder {

template<typename T>
class ConcurrentCircularBuffer : private Noncopyable {
  public:
	typedef boost::circular_buffer<T> container_type;
	typedef typename container_type::size_type size_type;
	typedef typename container_type::value_type value_type;
	typedef typename boost::call_traits<value_type>::param_type param_type;

	explicit ConcurrentCircularBuffer( size_type capacity )
		: mNumUnread( 0 ), mContainer( capacity ), mCanceled( false )
	{}

	//! Attempts to push \a item to the front of the buffer and waits for an availability. Returns success as true or false.
	bool pushFront( param_type item ) {
		// param_type represents the "best" way to pass a parameter of type value_type to a method
		std::unique_lock<std::mutex> lock( mMutex );
		while( ! is_not_full_impl() && ! mCanceled ) {
			mNotFullCond.wait( lock );
		}
		if( mCanceled )
			return false;
		mContainer.push_front( item );
		++mNumUnread;
		mNotEmptyCond.notify_one();
		return true;
	}

	//! Attempts to pop an item from the back of the buffer and waits for an availability. Returns success as true or false.
	bool popBack(value_type* pItem) {
		std::unique_lock<std::mutex> lock( mMutex );
		while( ! is_not_empty_impl() && ! mCanceled ) {
			mNotEmptyCond.wait( lock );
		}
		if( mCanceled )
			return false;
		*pItem = mContainer[--mNumUnread];
		mNotFullCond.notify_one();
		return true;
	}

	//! Attempts to push \a item to the front of the buffer, but does not wait for an availability. Returns success as true or false.
	bool tryPushFront( param_type item ) {
		// param_type represents the "best" way to pass a parameter of type value_type to a method
		std::lock_guard<std::mutex> lock( mMutex );
		if( ! is_not_full_impl() )
			return false;
		mContainer.push_front( item );
		++mNumUnread;
		mNotEmptyCond.notify_one();	
		return true;
	}

	//! Attempts to pop an item from the back of the buffer, but does not wait for an availability. Returns success as true or false.
	bool tryPopBack( value_type* pItem ) {
		std::lock_guard<std::mutex> lock( mMutex );
		if( ! is_not_empty_impl() )
			return false;
		*pItem = mContainer[--mNumUnread];
		mNotFullCond.notify_one();
		return true;
	}

	bool isNotEmpty() const {
		std::lock_guard<std::mutex> lock( mMutex );
		return is_not_empty_impl();
	}

	bool isNotFull() const {
		std::lock_guard<std::mutex> lock( mMutex );
		return is_not_full_impl();
	}
	
	//! Signals all threads that are waiting to push to or pop from the queue.
	void cancel() {
		std::lock_guard<std::mutex> lock( mMutex );
		mCanceled = true;
		mNotFullCond.notify_all();
		mNotEmptyCond.notify_all();
	}

	//! Allows the buffer to be used again after it was cancelled.
	void reinstate()
	{
		std::lock_guard<std::mutex> lock( mMutex );
		mCanceled = false;
	}
	
	//! Returns the number of items the buffer can hold
	size_t getCapacity() const { return (size_t)mContainer.capacity(); }
	
	//! Returns the number of items the buffer is currently holding
	size_t getSize() const {
		std::lock_guard<std::mutex> lock( mMutex );
		return mNumUnread;
	}

  private:
	bool is_not_empty_impl() const { return mNumUnread > 0; }
	bool is_not_full_impl() const { return mNumUnread < mContainer.capacity(); }

	size_type				mNumUnread;
	container_type			mContainer;
	mutable std::mutex		mMutex;
	std::condition_variable	mNotEmptyCond;
	std::condition_variable	mNotFullCond;
	bool					mCanceled;
};

} // namespace cinder