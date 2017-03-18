/*
 Copyright (c) 2015, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#include "cinder/Signals.h"

using namespace std;

namespace cinder { namespace signals {

Connection::Connection()
	: mLink( nullptr ), mPriority( 0 )
{
}

Connection::Connection( const std::shared_ptr<detail::Disconnector> &disconnector, detail::SignalLinkBase *link, int priority )
	: mDisconnector( disconnector ), mLink( link ), mPriority( priority )
{
}

Connection::Connection( const Connection &other )
	: mDisconnector( other.mDisconnector ), mLink( other.mLink ), mPriority( other.mPriority )
{
}

Connection::Connection( Connection &&other )
	: mDisconnector( move( other.mDisconnector ) ), mLink( move( other.mLink ) ), mPriority( move( other.mPriority ) )
{
}

Connection& Connection::operator=( const Connection &rhs )
{
	mDisconnector = rhs.mDisconnector;
	mLink = rhs.mLink;
	mPriority = rhs.mPriority;

	return *this;
}

Connection& Connection::operator=( Connection &&rhs )
{	
	mDisconnector = move( rhs.mDisconnector );
	mLink = move( rhs.mLink );
	mPriority = move( rhs.mPriority );

	// Note: on pre-C++14 compilers, we still need to reset the rhs' weak_ptr<Disconnector>, as it doesn't yet support move semantics
	// TODO: remove once all compilers support C++14
	rhs.mDisconnector.reset();

	return *this;
}

bool Connection::disconnect()
{
	auto disconnector = mDisconnector.lock();
	if( disconnector ) {
		auto link = mLink;
		mLink = nullptr;
		return disconnector->disconnect( link, mPriority );
	}

	return false;
}

bool Connection::isConnected() const
{
	return ! mDisconnector.expired() && mLink;
}

void Connection::disable()
{
	if( mLink ) {
		mLink->disable();
	}
}

void Connection::enable()
{
	if( mLink ) {
		mLink->enable();
	}
}

bool Connection::isEnabled() const
{
	if( mLink ) {
		return mLink->isEnabled();
	}
	return false;
}

ScopedConnection::ScopedConnection()
{
}

ScopedConnection::~ScopedConnection()
{
	disconnect();
}

ScopedConnection::ScopedConnection( ScopedConnection &&other )
	: Connection( std::move( other ) )
{
}

ScopedConnection::ScopedConnection( Connection &&other )
	: Connection( std::move( other ) )
{
}

ScopedConnection& ScopedConnection::operator=( ScopedConnection &&rhs )
{
	disconnect(); // first disconnect from existing
	Connection::operator=( std::move( rhs ) );
	return *this;
}

ConnectionList::~ConnectionList()
{
	clear();
}

void ConnectionList::add( Connection &&target )
{
	mConnections.emplace_back( std::move( target ) );
}

void ConnectionList::clear()
{
	for( auto &conn : mConnections )
		conn.disconnect();

	mConnections.clear();
}

namespace detail {

Disconnector::Disconnector( SignalBase *signal )
	: mSignal( signal )
{
}

bool Disconnector::disconnect( SignalLinkBase *link, int priority )
{
	return mSignal->disconnect( link, priority );
}

} } } // namespace cinder::signals::detail
