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


#include "OscSender.h"

#include "OscOutboundPacketStream.h"
#include "OscTypes.h"
#include "UdpSocket.h"

#include <assert.h>
namespace cinder { namespace osc {
	
	class OscSender  {
	public:
		OscSender();
		~OscSender();
		
		void setup(std::string hostname, int port);
		
		void sendMessage(Message& message);
		void sendBundle(Bundle& bundle);
		
		void shutdown();
	private:
		
		void appendBundle(Bundle& bundle, ::osc::OutboundPacketStream& p);
		void appendMessage(Message& message, ::osc::OutboundPacketStream& p);
		
		UdpTransmitSocket* socket;
		
	};
	
	

OscSender::OscSender(){
	socket = NULL;
}

OscSender::~OscSender(){
	if (socket)
		shutdown();
}

void OscSender::setup(std::string hostname, int port){
	if (socket)
		shutdown();
	socket = new UdpTransmitSocket( IpEndpointName(hostname.c_str(), port));
}

void OscSender::shutdown(){
	if (socket)
		delete socket;
	socket = NULL;
}

void OscSender::sendBundle(Bundle& bundle){
	static const int OUTPUT_BUFFER_SIZE = 32768;
	char buffer[OUTPUT_BUFFER_SIZE];
	
	::osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);
	
	appendBundle(bundle, p);
	
	socket->Send(p.Data(), p.Size());
}

void OscSender::sendMessage(Message& message){
	static const int OUTPUT_BUFFER_SIZE = 16384;
	char buffer[OUTPUT_BUFFER_SIZE];
	::osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);
	
	p << ::osc::BeginBundleImmediate;
	appendMessage(message, p);
	p << ::osc::EndBundle;
	
	socket->Send(p.Data(), p.Size());
}

void OscSender::appendBundle(Bundle& bundle, ::osc::OutboundPacketStream& p){
	p << ::osc::BeginBundleImmediate;
	for (int i = 0; i < bundle.getBundleCount(); i++){
		appendBundle(bundle.getBundleAt(i), p);
	}
	
	for (int i = 0; i < bundle.getMessageCount(); i++){
		appendMessage(bundle.getMessageAt(i), p);
	}
	p << ::osc::EndBundle;
}

void OscSender::appendMessage(Message& message, ::osc::OutboundPacketStream& p){
	p << ::osc::BeginMessage(message.getAddress().c_str());
	for (int i = 0; i < message.getNumArgs(); ++i) {
		if (message.getArgType(i) == TYPE_INT32){
			p << message.getArgAsInt32(i);
		}else if (message.getArgType(i) == TYPE_FLOAT){
			p << message.getArgAsFloat(i);
		}else if (message.getArgType(i) == TYPE_STRING){
			p << message.getArgAsString(i).c_str();
		}else {
			throw OscExcInvalidArgumentType();
		}
	}
	p << ::osc::EndMessage;
}
	
	
	Sender::Sender(){
		oscSender = shared_ptr<OscSender>( new OscSender );
	}
	
	void Sender::setup(std::string hostname, int port){
		oscSender->setup(hostname, port);
	}
	
	void Sender::sendMessage(Message& message){
		oscSender->sendMessage(message);
	}
	
	void Sender::sendBundle(Bundle& bundle){
		oscSender->sendBundle(bundle);
	}
	
}// namespace cinder
}// namespace osc