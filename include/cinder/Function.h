/*
 Copyright (c) 2010, The Cinder Project (http://libcinder.org)
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

#include <algorithm>
#include <map>

#if defined( _MSC_VER ) && ( _MSC_VER >= 1600 )
	#include <boost/tr1/functional.hpp>
	namespace std {
		using std::tr1::function;
	}
#else
	#include <boost/tr1/functional.hpp>
	namespace std {
		using ::boost::function;
	}
#endif

//! Represents a unique identifier for a callback
typedef uint32_t CallbackId;

//! Implements a utility class for maintaining a list of callbacks
template<typename SIG>
class CallbackMgr {
  private:
	std::map<CallbackId,std::function<SIG> >		mCallbacks;	

  public:
	typedef typename std::map<CallbackId,std::function<SIG> >::iterator		iterator;
	
	CallbackId	registerCb( std::function<SIG> cb )
	{
		CallbackId cbId = 0;
		if( ! mCallbacks.empty() )
			cbId = mCallbacks.rbegin()->first + 1;
		mCallbacks[cbId] = cb;
		return cbId;	
	}
	
	void		unregisterCb( CallbackId cbId ) { mCallbacks.erase( cbId ); }
	
	iterator begin() { return mCallbacks.begin(); }
	iterator end() { return mCallbacks.end(); }  
};
