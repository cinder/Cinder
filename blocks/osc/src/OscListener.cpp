/*
 Copyright (c) 2010, Hector Sanchez-Pajares
 Aer Studio http://www.aerstudio.com
 All rights reserved.
 
 
 This is a block for OSC Integration for Cinder framework developed by The Barbarian Group, 2010
 
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

#include "cinder/Thread.h" 
#include "cinder/Utilities.h"
#include "OscListener.h"
#include "osc/OscTypes.h"
#include "osc/OscPacketListener.h"
#include "osc/OscReceivedElements.h"
#include "ip/UdpSocket.h"

#include <boost/bind.hpp>

#include <iostream>
#include <assert.h>
#include <deque>
using std::deque;

namespace cinder { namespace osc {
	
	class OscListener : public ::osc::OscPacketListener{
		
	public:
		OscListener();
		~OscListener();
		
		void setup(int listen_port);
		
		bool hasWaitingMessages();
		
		bool getNextMessage(Message*);
		
		void shutdown();
		
	protected:
		virtual void ProcessMessage( const ::osc::ReceivedMessage &m, const IpEndpointName& remoteEndpoint );
		
	private:
		
		
		void threadSocket();
		
	    deque< Message* > mMessages;
		
		UdpListeningReceiveSocket* mListen_socket;
		
		std::mutex mMutex;		
		shared_ptr< std::thread > mThread;
		
		
		bool mSocketHasShutdown;
		
	};

OscListener::OscListener(){
	mListen_socket = NULL;
}

void OscListener::setup(int listen_port){
	if (mListen_socket) {
		shutdown();
	}
	
	mSocketHasShutdown = false;
	
	mListen_socket = new UdpListeningReceiveSocket(IpEndpointName(IpEndpointName::ANY_ADDRESS, listen_port), this);

	mThread = shared_ptr< std::thread >(new std::thread(boost::bind(&OscListener::threadSocket, this)));	
	
}

void OscListener::shutdown(){
	if (mListen_socket) {
		mListen_socket->AsynchronousBreak();
		
		while (!mSocketHasShutdown) {
			ci::sleep( 1 );
		}
		
		mThread->join();
		
		delete mListen_socket;
		mListen_socket = NULL;
	}
}

OscListener::~OscListener(){
	shutdown();
}

void OscListener::threadSocket(){
	
	mListen_socket->Run();
	mSocketHasShutdown = true;
	
}

void OscListener::ProcessMessage( const ::osc::ReceivedMessage &m, const IpEndpointName& remoteEndpoint ){
	Message* message = new Message();
	
	message->setAddress(m.AddressPattern());
	
	char endpoint_host[IpEndpointName::ADDRESS_STRING_LENGTH];
	remoteEndpoint.AddressAsString(endpoint_host);
	message->setRemoteEndpoint(endpoint_host, remoteEndpoint.port);
	
	for (::osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin(); arg != m.ArgumentsEnd(); ++arg){
		if (arg->IsInt32())
			message->addIntArg( arg->AsInt32Unchecked());
		else if (arg->IsFloat())
			message->addFloatArg(arg->AsFloatUnchecked());
		else if (arg->IsString())
			message->addStringArg(arg->AsStringUnchecked());
		else {
			assert(false && "message argument type unknown");
		}
	}
	
	boost::mutex::scoped_lock lock(mMutex);
	
	mMessages.push_back(message);
	
	

}

bool OscListener::hasWaitingMessages(){
	boost::mutex::scoped_lock lock(mMutex);
	
	int queue_length = (int)mMessages.size();
	
	
	
	return queue_length > 0;
}

bool OscListener::getNextMessage(Message* message){
	boost::mutex::scoped_lock lock(mMutex);
	
	if (mMessages.size() == 0) {
		
		return false;
	}
	
	Message* src_message = mMessages.front();
	message->copy(*src_message);
	
	delete src_message;
	
	mMessages.pop_front();
	
	
	
	return true;
}
	

	
	Listener::Listener(){
		oscListener = shared_ptr<OscListener>( new OscListener );
	}
	
	void Listener::setup(int listen_port){
		oscListener->setup(listen_port);
	}
	
	bool Listener::hasWaitingMessages(){
		return oscListener->hasWaitingMessages();
	}
	
	bool Listener::getNextMessage(Message* message){
		return oscListener->getNextMessage(message);
	}
	
	
	
	
} // namespace osc
} // namespace cinder
