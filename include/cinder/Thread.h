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
	#include "cinder/cocoa/CinderCocoa.h"
#endif
#if defined( CINDER_MAC )
	#include <objc/objc-auto.h>
#endif

#if (defined( _MSC_VER ) && ( _MSC_VER >= 1700 )) || defined( _LIBCPP_VERSION )
	#include <thread>
	#include <mutex>
	#include <condition_variable>
	#include <future>
#else
	#include <boost/thread/mutex.hpp>
	#include <boost/thread/recursive_mutex.hpp>
	#include <boost/thread/thread.hpp>
	#include <boost/thread/condition_variable.hpp>
	#define BOOST_THREAD_PROVIDES_FUTURE
	#include <boost/thread/future.hpp>

	// Promote classes from boost which will be part of std:: in C++1x where necessary
	namespace std {
		using boost::mutex;
		using boost::recursive_mutex;
		using boost::thread;
		using boost::lock_guard;
		using boost::unique_lock;
		using boost::condition_variable;
		namespace this_thread {
			using boost::this_thread::yield;
			using boost::this_thread::get_id;
		}
		using boost::future;
		using boost::packaged_task;
		using boost::promise;
	}
#endif

namespace cinder {
//! Create an instance of this class at the beginning of any multithreaded code that makes use of Cinder functionality
class ThreadSetup {
  public:
	ThreadSetup() {
#if defined( CINDER_MAC )
		objc_registerThreadWithCollector();
#endif
	}
	
	~ThreadSetup() {	
	}
	
  protected:
#if defined( CINDER_COCOA )
	cocoa::SafeNsAutoreleasePool	mAutoreleasePool;
#endif
};

} // namespace cinder