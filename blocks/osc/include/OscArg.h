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

#include <string>

namespace cinder { namespace osc { 
	

typedef enum _ArgType
{
	TYPE_NONE,
	TYPE_INT32,
	TYPE_FLOAT,
	TYPE_STRING,
	TYPE_BLOB,
	TYPE_BUNDLE,
	TYPE_INDEXOUTOFBOUNDS
} ArgType;

	class Arg
	{
	public:
		virtual ~Arg() {};
		
		virtual ArgType getType() const { return TYPE_NONE; }
		virtual std::string getTypeName() const { return "none"; }
		
	private:
	};
	
	class ArgInt32 : public Arg
	{
	public:
		ArgInt32( int32_t _value ) { value = _value; }
		
		/// return the type of this argument
		ArgType getType() const { return TYPE_INT32; }
		std::string getTypeName() const { return "int32"; }
		
		/// return value
		int32_t get() const { return value; }
		/// set value
		void set( int32_t _value ) { value = _value; }
		
	private:
		int32_t value;
	};
	
	class ArgFloat : public Arg
	{
	public:
		ArgFloat( float _value ) { value = _value; }
		
		/// return the type of this argument
		ArgType getType() const { return TYPE_FLOAT; }
		std::string getTypeName() const { return "float"; }
		
		/// return value
		float get() const { return value; }
		/// set value
		void set( float _value ) { value = _value; }
		
	private:
		float value;
	};
	
	class ArgString : public Arg
	{
	public:
		ArgString( std::string _value ) { value = _value; }
		
		/// return the type of this argument
		ArgType getType() const { return TYPE_STRING; }
		std::string getTypeName() const { return "string"; }
		
		/// return value
		std::string get() const { return value; }
		/// set value
		void set( const char* _value ) { value = _value; }
		
	private:
		std::string value;
	};

} // namespace osc
} // namespace cinder