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

#include "cinder/Thread.h" 
#include "cinder/Utilities.h"
#include "OscListener.h"
#include "osc/OscTypes.h"
#include "osc/OscPacketListener.h"
#include "osc/OscReceivedElements.h"
#include "ip/UdpSocket.h"

#include <iostream>
#include <assert.h>
#include <deque>
#include <map>
using namespace std;

namespace cinder { namespace osc {
	
class OscListener : public ::osc::OscPacketListener {	
  public:
	OscListener();
	~OscListener();
	
	void setup(int listen_port);
	
	bool hasWaitingMessages() const;
	bool getNextMessage( Message * );

	CallbackId	registerMessageReceived( std::function<void (const osc::Message*)> callback );
	void		unregisterMessageReceived( CallbackId id );
	
	void shutdown();
	
  protected:
	virtual void ProcessMessage( const ::osc::ReceivedMessage &m, const IpEndpointName& remoteEndpoint );
	
  private:
	void threadSocket();
	
	deque<Message*> mMessages;
	
	UdpListeningReceiveSocket* mListen_socket;
	
	mutable std::mutex mMutex;
	std::shared_ptr<std::thread> mThread;
	
	CallbackMgr<void (const Message*)>	mMessageReceivedCbs;
	bool mSocketHasShutdown;
};

OscListener::OscListener()
{
	mListen_socket = NULL;
}

void OscListener::setup(int listen_port)
{
	if (mListen_socket) {
		shutdown();
	}
	
	mSocketHasShutdown = false;
	
	mListen_socket = new UdpListeningReceiveSocket(IpEndpointName(IpEndpointName::ANY_ADDRESS, listen_port), this);

	mThread = std::shared_ptr<std::thread>( new std::thread( &OscListener::threadSocket, this ) );
}

void OscListener::shutdown() {
	if (mListen_socket) {
		mListen_socket->AsynchronousBreak();
		
		while( ! mSocketHasShutdown ) {
			ci::sleep( 1 );
		}
		
		mThread->join();
		
		delete mListen_socket;
		mListen_socket = NULL;
	}
}

OscListener::~OscListener() {
	shutdown();
}

void OscListener::threadSocket() {
	
	mListen_socket->Run();
	mSocketHasShutdown = true;
	
}

void OscListener::ProcessMessage( const ::osc::ReceivedMessage &m, const IpEndpointName& remoteEndpoint ) {
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
	
	lock_guard<mutex> lock(mMutex);
	
	if( mMessageReceivedCbs.empty() ){
		mMessages.push_back( message );
	}else{
		mMessageReceivedCbs.call( message );
		delete message;
	}
}

bool OscListener::hasWaitingMessages() const
{
	std::lock_guard<mutex> lock( mMutex );
	return ! mMessages.empty();
}

bool OscListener::getNextMessage( Message* message )
{
	lock_guard<mutex> lock( mMutex );
	
	if( mMessages.empty() )
		return false;
	
	Message* src_message = mMessages.front();
	message->copy( *src_message );
	
	delete src_message;	
	mMessages.pop_front();
	
	return true;
}

CallbackId OscListener::registerMessageReceived( std::function<void (const osc::Message*)> callback )
{
	lock_guard<mutex> lock( mMutex );
	return mMessageReceivedCbs.registerCb( callback );
}

void OscListener::unregisterMessageReceived( CallbackId id )
{
	lock_guard<mutex> lock(mMutex);
	return mMessageReceivedCbs.unregisterCb( id );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Listener
Listener::Listener() {
	oscListener = std::shared_ptr<OscListener>( new OscListener );
}

void Listener::setup(int listen_port){
	oscListener->setup(listen_port);
}

void Listener::shutdown(){
	oscListener->shutdown();
}

bool Listener::hasWaitingMessages() const {
	return oscListener->hasWaitingMessages();
}

bool Listener::getNextMessage(Message* message) {
	return oscListener->getNextMessage(message);
}

CallbackId Listener::registerMessageReceived( std::function<void (const osc::Message*)> callback )
{
	return oscListener->registerMessageReceived( callback );
}

void Listener::unregisterMessageReceived( CallbackId id )
{
	return oscListener->unregisterMessageReceived( id );
}
	
} } // namespace cinder::osc
