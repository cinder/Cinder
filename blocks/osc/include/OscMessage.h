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
#include "cinder/Exception.h"

#include "OscArg.h"
#include <string>
#include <vector>

namespace cinder { namespace osc {
	
	class Message {
	public:
		Message() {}
		~Message();
		Message( const Message& other ){ copy ( other ); }
		Message& operator= ( const Message& other ) { return copy( other ); }

		Message& copy( const Message& other );
		void clear();
		
		std::string getAddress() const { return address; }
		std::string getRemoteIp() const { return remote_host; }
		int getRemotePort() const { return remote_port; }
		void setAddress( std::string _address ) { address = _address; };
		void setRemoteEndpoint( std::string host, int port ) { remote_host = host; remote_port = port; }
		
		int getNumArgs() const;
		ArgType getArgType( int index ) const;
		std::string getArgTypeName( int index ) const;
		
		int32_t getArgAsInt32( int index, bool typeConvert = false ) const;
		float getArgAsFloat( int index, bool typeConvert = false ) const;
		std::string getArgAsString( int index, bool typeConvert = false ) const;
		
		void addIntArg( int32_t argument );
		void addFloatArg( float argument );
		void addStringArg( std::string argument );
		
	protected:
		std::string address;
		std::vector<Arg*> args;
		
		std::string remote_host;
		int remote_port;	
	};
	
	class OscExc : public Exception {
	};
	class OscExcInvalidArgumentType : public OscExc {
	};
	class OscExcOutOfBounds : public OscExc {
	};

} // namespace osc
} // namespace cinder