/*
 Copyright (c) 2010, Hector Sanchez-Pajares
 Aer Studio http://www.aerstudio.com
 All rights reserved.
 
 
 This is a block for OSC Integration for the Cinder framework (http://libcinder.org)
 
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
#include "cinder/Function.h"

#include "OscMessage.h"
#include "OscArg.h"


namespace cinder { namespace osc {
	
class Listener {	
  public:
	Listener();
	
	void setup(int listen_port);
	void shutdown();
	
	// Callback methods
	//! Registers an asynchronous callback which fires whenever a new message is received.
	CallbackId	registerMessageReceived( std::function<void (const osc::Message*)> callback );
	//! Registers an asynchronous callback which fires whenever a new message is received.
	template<typename T>
	CallbackId	registerMessageReceived( T *obj, void (T::*cb)(const osc::Message*) ) { return registerMessageReceived( std::bind1st( std::mem_fun( cb ), obj ) ); }
	//! Unregisters an asynchronous callback previously registered with registerMessageReceived()
	void		unregisterMessageReceived( CallbackId id );

	//! Returns whether the are messages waiting to be processed via getNextMessage(). Always \c false if callbacks have been registered using registerMessageReceived().
	bool hasWaitingMessages() const;
	//! Gets the next message to be processed and puts it in \a resultMessage. Returns whether there was a message to process or not. Always \c false if callbacks have been registered using registerMessageReceived().
	bool getNextMessage( Message *resultMessage );
	
  private:
	std::shared_ptr<class OscListener>   oscListener;
};

} } // namespace cinder::osc